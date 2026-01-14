#include <stdio.h>
#include <string.h>

#define COPS_ASSERT_ENABLE
#include "../src/list.h"

#include "test.h"

// Entity declaration
init_slice(Entity, slice_ent);
init_list(Entity, ll_ent, slice_ent);

// Generic declaration
init_slice(Ptr, slice);
init_list(Ptr, ll, slice);
static void PtrFree(Ptr);
static Ptr PtrDup(Ptr);
static Ptr PtrNew(int, const char *);

// Vec3 declaration
init_slice(Vec3, slice_vec);
init_list(Vec3, ll_vec3, slice_vec);

int main(void)
{
        //
        // Test Entity
        //
        ll_ent *ent_list = ll_ent_new();
        for (int i = 0; i < 10; i++) {
                COPS_ASSERT(
                    ll_ent_push_front(ent_list, (Entity){i, "stack string"}) ==
                    COPS_OK);
        }
        for (int i = 0; i < 10; i++) {
                COPS_ASSERT(ll_ent_push_back(
                                ent_list, (Entity){i + 100, "stack string"}) ==
                            COPS_OK);
        }
        COPS_ASSERT(ent_list->len == 20);
        ll_ent_list_node *iter = ent_list->head;
        printf("~~~ push front and back ~~~\n");
        for (int i = 0; i < ent_list->len; i++) {
                printf("[%02d]: %d\n", i, iter->val.id);
                iter = iter->next;
        }
        for (int i = 0; i < 5; i++) {
                COPS_ASSERT(ll_ent_pop_front(ent_list, NULL) == COPS_OK);
        }
        for (int i = 0; i < 5; i++) {
                COPS_ASSERT(ll_ent_pop_back(ent_list, NULL) == COPS_OK);
        }
        COPS_ASSERT(ent_list->len == 10);
        iter = ent_list->head;
        printf("~~~ pop front and back ~~~\n");
        for (int i = 0; i < ent_list->len; i++) {
                printf("[%02d]: %d\n", i, iter->val.id);
                iter = iter->next;
        }
        iter = ent_list->head->next;
        COPS_ASSERT(COPS_OK == ll_ent_remove(ent_list, iter));
        iter = ent_list->head;
        COPS_ASSERT(COPS_OK == ll_ent_insert_after(ent_list, iter,
                                                   (Entity){420, "after"}));
        COPS_ASSERT(COPS_OK == ll_ent_insert_before(ent_list, iter,
                                                    (Entity){420, "before"}));
        iter = ent_list->head;
        printf("~~~ insert and remove ~~~\n");
        for (int i = 0; i < ent_list->len; i++) {
                printf("[%02d]: %d\n", i, iter->val.id);
                iter = iter->next;
        }
        ll_ent_free(ent_list);
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
