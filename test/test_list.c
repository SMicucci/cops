#include <stdio.h>
#include <string.h>

#define COPS_ASSERT_ENABLE
#include "../src/list.h"

#include "test.h"

// Entity declaration
init_slice(Entity, slice_ent);
init_list(Entity, ent_ll, slice_ent);

// Generic declaration
init_slice(Ptr, slice);
init_list(Ptr, ll, slice);

// Vec3 declaration
init_slice(Vec3, slice_vec);
init_list(Vec3, vec3_ll, slice_vec);

int main(void)
{
        //
        // Test Entity
        //
        ent_ll *ent_list = ent_ll_new();
        for (int i = 0; i < 10; i++) {
                COPS_ASSERT(
                    ent_ll_push_front(ent_list, (Entity){i, "stack string"}) ==
                    COPS_OK);
        }
        for (int i = 0; i < 10; i++) {
                COPS_ASSERT(ent_ll_push_back(
                                ent_list, (Entity){i + 100, "stack string"}) ==
                            COPS_OK);
        }
        COPS_ASSERT(ent_list->len == 20);
        ent_ll_list_node *iter = ent_list->head;
        printf("~~~ push front and back ~~~\n");
        for (int i = 0; i < ent_list->len; i++) { // print list
                printf("[%02d]: (%d) '%s'\n", i, iter->val.id, iter->val.str);
                iter = iter->next;
        }
        for (int i = 0; i < 5; i++) {
                COPS_ASSERT(ent_ll_pop_front(ent_list, NULL) == COPS_OK);
        }
        for (int i = 0; i < 5; i++) {
                COPS_ASSERT(ent_ll_pop_back(ent_list, NULL) == COPS_OK);
        }
        COPS_ASSERT(ent_list->len == 10);
        iter = ent_list->head;
        printf("~~~ pop front and back ~~~\n");
        for (int i = 0; i < ent_list->len; i++) { // print list
                printf("[%02d]: (%d) '%s'\n", i, iter->val.id, iter->val.str);
                iter = iter->next;
        }
        iter = ent_list->head->next;
        COPS_ASSERT(COPS_OK == ent_ll_remove(ent_list, iter));
        iter = ent_list->head;
        COPS_ASSERT(COPS_OK == ent_ll_insert_after(ent_list, iter,
                                                   (Entity){420, "after"}));
        COPS_ASSERT(COPS_OK == ent_ll_insert_before(ent_list, iter,
                                                    (Entity){420, "before"}));
        iter = ent_list->head;
        printf("~~~ insert and remove ~~~\n");
        for (int i = 0; i < ent_list->len; i++) { // print list
                printf("[%02d]: (%d) '%s'\n", i, iter->val.id, iter->val.str);
                iter = iter->next;
        }
        ent_ll_free(ent_list);

        //
        // Test Generic
        //
        printf("\n-------------------------------\n\n");
        ll *gen_list = ll_new();
        gen_list->free = PtrFree;
        gen_list->dup = PtrDup;
        char *arr_str[12] = {"alpha",   "bravo", "charlie",  "delta",
                             "epsilon", "flare", "geronimo", "hammoc",
                             "iqos",    "jack",  "kirk",     "larry"};
        for (int i = 0; i < 12; i++) {
                if (i % 2)
                        COPS_ASSERT(
                            ll_push_front(gen_list, PtrNew(arr_str[i])) ==
                            COPS_OK);
                else
                        COPS_ASSERT(
                            ll_push_back(gen_list, PtrNew(arr_str[i])) ==
                            COPS_OK);
        }
        COPS_ASSERT(gen_list->len == 12);
        ll_list_node *g_tmp;
        g_tmp = gen_list->tail;
        printf("~~~ insert pointer in it ~~~\n");
        for (int i = gen_list->len - 1; i >= 0; i--) { // print
                Entity *v = g_tmp->val;
                printf("[%02d]: (%d) '%s'\n", i, v->id, v->str);
                g_tmp = g_tmp->prev;
        }
        for (int i = 0; i < 6; i++) {
                COPS_ASSERT(ll_pop_front(gen_list, NULL) == COPS_OK);
        }
        g_tmp = gen_list->tail;
        printf("~~~ remove some pointer in it ~~~\n");
        for (int i = gen_list->len - 1; i >= 0; i--) { // print
                Entity *v = g_tmp->val;
                printf("[%02d]: (%d) '%s'\n", i, v->id, v->str);
                g_tmp = g_tmp->prev;
        }
        // test import-export
        slice *gen_arr = ll_export(gen_list);
        ll_import(gen_list, gen_arr);
        g_tmp = gen_list->tail;
        printf("~~~ export and import ~~~\n");
        for (int i = gen_list->len - 1; i >= 0; i--) { // print
                Entity *v = g_tmp->val;
                printf("[%02d]: (%d) '%s'\n", i, v->id, v->str);
                g_tmp = g_tmp->prev;
        }
        ll_free(gen_list);
        return 0;
}
