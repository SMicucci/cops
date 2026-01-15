#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#define COPS_ASSERT_ENABLE
#include "../src/set.h"

#include "test.h"

init_slice(Entity, ent_slice);
init_set(Entity, ent_set, ent_slice);
uint64_t ent_hash(Entity self);
int ent_cmp(Entity self, Entity oth);

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

int main(void)
{
        ent_set *e_set = ent_set_new(ent_hash, ent_cmp);
        for (int i = 0; i < 12; i++) {
                Entity *ent = PtrNew("test");
                ent_set_add(e_set, *ent);
                PtrFree(ent);
        }
        ent_set_free(e_set);
        return 0;
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
