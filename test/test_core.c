#include <stdio.h>

#define COPS_ASSERT_ENABLE
#include "../src/core.h"

/*
 * structs examples
 * */
typedef struct Entity {
        int id;
        char *str;
} Entity;
typedef struct Vect3 {
        long x;
        long y;
        long z;
} Vect3;
typedef void *Ptr;

init_slice(int, int_vla);
init_slice(char, sds);

int main(void)
{
        int_vla *arr = int_vla_new(10);
        COPS_ASSERT(arr->len == 10);
        printf("test passed.\n");
        return 0;
}
