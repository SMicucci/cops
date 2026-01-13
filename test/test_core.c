#include <stdio.h>
#include <stdlib.h>

#define COPS_ASSERT_ENABLE
#include "../src/core.h"

typedef struct Entity {
        int id;
        char *str;
} Entity;
init_slice(Entity, slice_ent);

typedef void *Ptr;
init_slice(Ptr, slice);

init_slice(double, vec);
init_slice(vec *, vec_slice);

int main(void)
{
        //
        // Test Entity
        //
        slice_ent *ent_slice = slice_ent_new(5);
        if (!ent_slice) {
                printf("Failed to allocate Entity slice\n");
                return 1;
        }
        for (uint64_t i = 0; i < ent_slice->len; i++) {
                ent_slice->data[i].id = (int)i;
                ent_slice->data[i].str = "test";
        }
        printf("Entity slice test passed\n");
        slice_ent_free(ent_slice);
        //
        // Test generic
        //
        slice *ptr_slice = slice_new(6);
        if (!ptr_slice) {
                printf("Failed to allocate generic slice\n");
                return 1;
        }
        for (uint64_t i = 0; i < ptr_slice->len; i++) {
                ptr_slice->data[i] = malloc(8 * (1 + i));
        }
        printf("Generic slice test passed\n");
        for (uint64_t i = 0; i < ptr_slice->len; i++) {
                free(ptr_slice->data[i]);
        }
        slice_free(ptr_slice);
        //
        // Test vec
        //
        vec_slice *nested_slice = vec_slice_new(9);
        if (!nested_slice) {
                printf("Failed to allocate nested slice\n");
                return 1;
        }
        for (uint64_t i = 0; i < nested_slice->len; i++) {
                nested_slice->data[i] = vec_new(i + 1);
        }
        for (uint64_t i = 0; i < nested_slice->len; i++) {
                vec_free(nested_slice->data[i]);
        }
        printf("Nested slice test passed (outer len=%lu)\n", nested_slice->len);
        vec_slice_free(nested_slice);
        printf("All slice tests passed!\n");
        return 0;
}
