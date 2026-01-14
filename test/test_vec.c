#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define COPS_ASSERT_ENABLE
#include "../src/vec.h"

#include "test.h"

// Entity declaration
init_slice(Entity, slice_ent);
init_vec(Entity, ent_vec, slice_ent);

// Generic declaration
init_slice(Ptr, slice);
init_vec(Ptr, vec_t, slice);
static void PtrFree(Ptr);
static Ptr PtrDup(Ptr);
static Ptr PtrNew(int, const char *);

// Vec3 declaration
init_slice(Vec3, slice_vec);
init_vec(Vec3, Vec3DynArr, slice_vec);

int main(void)
{
        //
        // Test Entity
        //
        ent_vec *ent_v = ent_vec_new();
        // vector push
        for (int i = 0; i < 20; i++) {
                COPS_ASSERT(ent_vec_push(ent_v, (Entity){i, "stack string"}) ==
                            COPS_OK);
        }
        COPS_ASSERT(ent_v->len == 20 && "unmatched size");
        // vector remove
        for (int i = 19; i >= 0; i--) {
                if (i % 4)
                        continue;
                COPS_ASSERT(ent_vec_remove(ent_v, i, NULL) == COPS_OK);
        }
        // vector insert
        for (int i = 0; i < 5; i++) {
                COPS_ASSERT(
                    ent_vec_insert(ent_v, 0, (Entity){420, "inserted"}) ==
                    COPS_OK);
        }
        // vector pop
        for (int i = 0; i < 5; i++) {
                Entity tmp;
                COPS_ASSERT(ent_vec_pop(ent_v, &tmp) == COPS_OK);
        }
        // vector reset
        uint64_t old_cap = ent_v->cap;
        ent_vec_reset(ent_v);
        COPS_ASSERT(ent_v->len == 0 && "unmatched size");
        COPS_ASSERT(ent_v->cap == old_cap && "unmatched size");
        ent_vec_free(ent_v);

        //
        // Test Generic
        //
        vec_t *v = vec_t_new(); /* will use Entity as a generic kind */
        v->free = PtrFree;
        v->dup = PtrDup;
        char *arr_str[12] = {"alpha",   "bravo", "charlie",  "delta",
                             "epsilon", "flare", "geronimo", "hammoc",
                             "iqos",    "jack",  "kirk",     "larry"};
        for (int i = 0; i < 12; i++) {
                vec_t_push(v, PtrNew(i, arr_str[i]));
        }
        slice *slice_ = vec_t_export(v);
        slice *s_tmp = slice_new(slice_->len);
        for (int i = 0; i < slice_->len; i++) {
                s_tmp->data[i] = slice_->data[i];
        }
        vec_t_import(v, s_tmp);
        for (int i = 0; i < slice_->len; i++) {
                PtrFree(slice_->data[i]);
        }
        assert(v->len > 12);
        vec_t_free(v);

        //
        // Test Vec3
        //
        Vec3DynArr *da = Vec3DynArr_new();
        for (int i = 0; i < 12; i++) {
                Vec3DynArr_push(da, (Vec3){1 + 1, 3 * i - 5, 20 - i});
        }
        for (int i = 3; i < 6; i++) {
                Vec3 tmp;
                Vec3DynArr_get(da, i, &tmp);
                tmp.z = 0;
                Vec3DynArr_set(da, i, tmp, NULL);
        }
        Vec3DynArr_free(da);
        return 0;
}

//
// Generic implementation of function pointer
//
static void PtrFree(Ptr ptr)
{
        Entity *in = ptr;
        if (in && in->str)
                free(in->str);
        if (in)
                free(in);
}
static Ptr PtrDup(Ptr ptr)
{
        assert(ptr && "invalid reference");
        Entity *in = ptr;
        Entity *res = malloc(sizeof(Entity));
        assert(res && "failed allocation");
        res->id = in->id;
        res->str = malloc(strlen(in->str) + 1);
        assert(res->str && "failed allocation");
        strcpy(res->str, in->str);
        res->str[strlen(in->str)] = 0;
        return res;
}
static Ptr PtrNew(int id, const char *str)
{
        Entity *res = malloc(sizeof(Entity));
        assert(res && "failed allocation");
        res->id = id;
        res->str = malloc(strlen(str) + 1);
        assert(res->str && "failed allocation");
        strcpy(res->str, str);
        res->str[strlen(str)] = 0;
        return res;
}
