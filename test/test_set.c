#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define COPS_ASSERT_ENABLE
#include "../src/set.h"

#include "test.h"

init_slice(Entity, ent_slice);
init_set(Entity, ent_set, ent_slice);
uint64_t ent_hash(Entity self);
int ent_cmp(Entity self, Entity oth);
void entr_print(ent_set *e_set);

typedef struct KV {
        char *key;
        uint64_t val;
} *KV;
init_slice(KV, slice);
init_set(KV, set, slice);
uint64_t KV_hash(KV self);
int KV_cmp(KV self, KV oth);
void KV_free(KV self);
KV KV_dup(KV self);
KV KV_new(char *key, uint64_t val);

void kvset_print(set *kvset);

int main(void)
{
        //
        // Test Entity
        //
        ent_set *e_set = ent_set_new(ent_hash, ent_cmp);
        // populate [add]
        for (int i = 0; i < 20; i++) {
                Entity *ent = PtrNew("test");
                printf("%d \n", i);
                ent_set_add(e_set, *ent);
                free(ent);
        }
        printf("~~~ populate entity ~~~\n"); // print routine
        entr_print(e_set);
        // delete [del]
        for (int i = 0; i < e_set->len; i++) {
                if (i % 4)
                        continue;
                ent_set_del(e_set, (Entity){i, ""});
        }
        printf("~~~ trim some entity ~~~\n"); // print routine
        entr_print(e_set);
        // alter [has, get, set]
        for (int i = 0; i < e_set->cap; i++) {
                if (i % 3)
                        continue;
                int has = ent_set_has(e_set, (Entity){i, ""});
                assert(has != COPS_INVALID);
                if (!has)
                        continue;
                Entity node;
                ent_set_get(e_set, (Entity){i, ""}, &node);
                free(node.str);
                node.str = malloc(9);
                strcpy(node.str, "modified");
                node.str[9] = 0;
                ent_set_set(e_set, node);
        }
        printf("~~~ trim some entity ~~~\n"); // print routine
        entr_print(e_set);
        ent_set_free(e_set);

        //
        // Test KV
        //
        set *kvset = set_new(KV_hash, KV_cmp);
        kvset->free = KV_free;
        kvset->dup = KV_dup;
        char *arr_str[20] = {"alpha",  "bravo",    "charlie", "delta",
                             "echo",   "foxtrot",  "golf",    "hammoc",
                             "iqos",   "juliet",   "kirk",    "lima",
                             "mike",   "november", "oscar",   "papa",
                             "quebec", "romeo",    "sierra",  "tango"};
        for (int i = 0; i < 20; i++) {
                set_add(kvset, KV_new(arr_str[i], 100 + i * 4));
        }
        printf("~~~ create generic set ~~~\n"); // print routine
        kvset_print(kvset);
        slice *kvslice = set_export(kvset);
        for (int i = 0; i < kvslice->len; i++) {
                kvslice->data[i]->key[0] = 'x';
        }
        set_import(kvset, kvslice);
        printf("~~~ create generic set ~~~\n"); // print routine
        kvset_print(kvset);
        set_free(kvset);
        return 0;
}

void entr_print(ent_set *e_set)
{
        for (int i = 0; i < e_set->cap; i++) {
                ent_set_set_node node = e_set->data[i];
                printf("> [\x1b[32m%02d\x1b[0m]", i);
                if (node.free) {
                        printf(" \x1b[36mF\x1b[0m\n");
                } else if (node.tomb) {
                        printf(" \x1b[34mt\x1b[0m\n");
                } else {
                        printf("\x1b[35m%2d\x1b[0m => ", node.jumps);
                        printf("%02d '%s'\n", node.val.id, node.val.str);
                }
        }
}

void kvset_print(set *kvset)
{
        for (int i = 0; i < kvset->cap; i++) {
                set_set_node node = kvset->data[i];
                printf("> [\x1b[32m%02d\x1b[0m]", i);
                if (node.free) {
                        printf(" \x1b[36mF\x1b[0m\n");
                } else if (node.tomb) {
                        printf(" \x1b[34mt\x1b[0m\n");
                } else {
                        printf("\x1b[35m%2d\x1b[0m => ", node.jumps);
                        printf("'%s' %02lu\n", node.val->key, node.val->val);
                }
        }
}

uint64_t ent_hash(Entity self) { return hash64shift(self.id); }
int ent_cmp(Entity self, Entity oth)
{
        return (self.id > oth.id) - (self.id < oth.id);
}

uint64_t KV_hash(KV self) { return djb2(self->key, strlen(self->key)); }
int KV_cmp(KV self, KV oth) { return strcmp(self->key, oth->key); }
void KV_free(KV self)
{
        if (!self)
                return;
        free(self->key);
        free(self);
}
KV KV_dup(KV self)
{
        if (!self)
                return self;
        KV res = malloc(sizeof(*self));
        COPS_ASSERT(res);
        uint64_t _len = strlen(self->key) + 1;
        res->key = malloc(_len);
        memcpy(res->key, self->key, _len);
        return res;
}
KV KV_new(char *key, uint64_t val)
{
        KV resp = malloc(sizeof(*resp));
        COPS_ASSERT(resp);
        uint64_t _len = strlen(key) + 1;
        resp->key = malloc(_len);
        COPS_ASSERT(resp->key);
        memcpy(resp->key, key, _len);
        resp->val = val;
        return resp;
}
