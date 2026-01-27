#include <stdint.h>
#include <stdio.h>

#define COPS_IMPLEMENTATION
#define COPS_ASSERT
#include "../src/06_pool.h"

typedef struct LongStruct {
        double x, y, z;
        long a, b, c;
} LongStruct;

init_pool(LongStruct, pool_struct);

int main(void)
{
        pool_struct pool;
        pool_struct_new(&pool, 65);
        uintptr_t old = (uintptr_t)NULL;
        int mod_eq = 6;
        for (int i = 0; i < 200; i++) {
                LongStruct *x = pool_struct_alloc(&pool);
                x->c = 250;
                if ((uintptr_t)x - old > 0x30) {
                        printf("\n\x1b[34m~~~~~\x1b[0m\n");
                        mod_eq = (mod_eq + 1) % 8;
                }
                old = (uintptr_t)x;
                printf("[\x1b[32m%p\x1b[0m]", x);
                if ((i % 8) == mod_eq)
                        printf("\n");
                else
                        printf(" ");
        }
        printf("\n");
        pool_struct_reset(&pool);
        for (int i = 0; i < 200; i++) {
                LongStruct *x = pool_struct_alloc(&pool);
                COPS_ASSERT(x->c != 250);
        }
        pool_struct_free(&pool);
        return 0;
}
