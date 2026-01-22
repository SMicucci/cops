#define COPS_IMPLEMENTATION
// #define COPS_ASSERT
#include "../src/06_pool.h"

typedef struct LongStruct {
        double x, y, z;
        long a, b, c;
} LongStruct;

init_pool(LongStruct, StructPool)
