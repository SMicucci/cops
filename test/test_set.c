#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define COPS_IMPLEMENTATION
#define COPS_ASSERT_ENABLE
#include "../src/04_hset.h"

#include "test.h"

init_slice(Entity, ent_slice);
init_hset(Entity, ent_hset, ent_slice);
uint64_t ent_hash(Entity self);
int ent_cmp(Entity self, Entity oth);
void entr_print(ent_hset *e_hset);

typedef struct KV {
        char *key;
        uint64_t val;
} *KV;
init_slice(KV, slice);
init_hset(KV, hset, slice);
uint64_t KV_hash(KV self);
int KV_cmp(KV self, KV oth);
void KV_free(KV self);
KV KV_dup(KV self);
KV KV_new(char *key, uint64_t val);

void kvhset_print(hset *kvhset);

int main(void)
{
        //
        // Test Entity
        //
        ent_hset *e_hset = ent_hset_new(ent_hash, ent_cmp);
        // populate [add]
        for (int i = 0; i < 20; i++) {
                Entity *ent = PtrNew("test");
                printf("%d \n", i);
                ent_hset_add(e_hset, *ent);
                free(ent);
        }
        printf("~~~ populate entity ~~~\n"); // print routine
        entr_print(e_hset);
        // delete [del]
        for (int i = 0; i < e_hset->len; i++) {
                if (i % 4)
                        continue;
                ent_hset_del(e_hset, (Entity){i, ""});
        }
        printf("~~~ trim some entity ~~~\n"); // print routine
        entr_print(e_hset);
        // alter [has, get, hset]
        for (int i = 0; i < e_hset->cap; i++) {
                if (i % 3)
                        continue;
                int has = ent_hset_has(e_hset, (Entity){i, ""});
                assert(has != COPS_INVALID);
                if (!has)
                        continue;
                Entity node;
                ent_hset_get(e_hset, (Entity){i, ""}, &node);
                free(node.str);
                node.str = malloc(9);
                strcpy(node.str, "modified");
                node.str[9] = 0;
                ent_hset_hset(e_hset, node);
        }
        printf("~~~ trim some entity ~~~\n"); // print routine
        entr_print(e_hset);
        ent_hset_free(e_hset);

        //
        // Test KV
        //
        hset *kvhset = hset_new(KV_hash, KV_cmp);
        kvhset->free = KV_free;
        kvhset->dup = KV_dup;
        char *arr_str[20] = {"alpha",  "bravo",    "charlie", "delta",
                             "echo",   "foxtrot",  "golf",    "hammoc",
                             "iqos",   "juliet",   "kirk",    "lima",
                             "mike",   "november", "oscar",   "papa",
                             "quebec", "romeo",    "sierra",  "tango"};
        for (int i = 0; i < 20; i++) {
                hset_add(kvhset, KV_new(arr_str[i], 100 + i * 4));
        }
        printf("~~~ create generic hset ~~~\n"); // print routine
        kvhset_print(kvhset);
        slice *kvslice = hset_export(kvhset);
        for (int i = 0; i < kvslice->len; i++) {
                kvslice->data[i]->key[0] = 'x';
        }
        hset_import(kvhset, kvslice);
        printf("~~~ create generic hset ~~~\n"); // print routine
        kvhset_print(kvhset);
        hset_free(kvhset);
        return 0;
}

void entr_print(ent_hset *e_hset)
{
        for (int i = 0; i < e_hset->cap; i++) {
                ent_hset_rh_node node = e_hset->data[i];
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

void kvhset_print(hset *kvhset)
{
        for (int i = 0; i < kvhset->cap; i++) {
                hset_rh_node node = kvhset->data[i];
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
        res->val = self->val;
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
