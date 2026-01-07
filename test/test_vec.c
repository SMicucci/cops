#include <stdio.h>
#include <string.h>

#define COPS_ASSERT_ENABLE
#include "../src/vec.h"

typedef struct compl_struct {
        int a;
        int b;
        int c;
        int d;
} complex_t;

init_slice(complex_t, int_vla);
init_vec(complex_t, int_vec, int_vla);

int main(void)
{
        int_vec *v1 = int_vec_new();
        int_vec *v2 = int_vec_new();
        for (int i = 0; i < 6; i++) {
                COPS_ASSERT(int_vec_push(v1, (complex_t){i, i, i, i}) ==
                            COPS_OK);
        }
        printf("vec1[%lu]:\n", v1->len);
        for (int i = 0; i < v1->len; i++) {
                printf("[%02d]: %d\n", i, v1->data[i].a);
        }
        for (int i = 0; i < 14; i++) {
                COPS_ASSERT(int_vec_insert(v2, 0,
                                           (complex_t){100 + i, 100 + i,
                                                       100 + i, 100 + i}) ==
                            COPS_OK);
                printf("- insert(%d) [len: %lu, last: %d]\n", i + 1, v2->len,
                       v2->data[v2->len - 1].a);
        }
        // for (int i = 10; i > 0; i--) {
        //         if (i % 2)
        //                 COPS_ASSERT(int_vec_remove(v2, i, NULL) == COPS_OK);
        // }
        printf("vec2[%lu]:\n", v2->len);
        for (int i = 0; i < v2->len; i++) {
                printf("[%02d]: %d\n", i, v2->data[i].a);
        }
        int_vla *vla1 = int_vec_export(v2);
        COPS_ASSERT(vla1);
        COPS_ASSERT(int_vec_import(v1, vla1) == COPS_OK);
        printf("vec1[%lu] (merged with 2):\n", v1->len);
        for (int i = 0; i < v1->len; i++) {
                printf("[%02d]: %d\n", i, v1->data[i].a);
        }
        return 0;
}
