#include <stdio.h>
#include <stdlib.h>

#define COPS_ASSERT_ENABLE
#include "../src/core.h"

#include "test.h"

// Entity declaration
init_slice(Entity, slice_ent);

// Generic declaration
init_slice(Ptr, slice);

// Vec3 delcaration
init_slice(Vec3 *, vec_slice);

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
                Vec3 *tmp = malloc(sizeof(Vec3));
                *tmp = (Vec3){i + 1, 2 * i, -i + 5};
                nested_slice->data[i] = tmp;
        }
        for (uint64_t i = 0; i < nested_slice->len; i++) {
                free(nested_slice->data[i]);
        }
        vec_slice_free(nested_slice);
        printf("Nested slice test passed\n");
        printf("All slice tests passed!\n");
        return 0;
}
