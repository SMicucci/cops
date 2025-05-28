#ifndef COPS_CORE_ARRAY_H
#define COPS_CORE_ARRAY_H

#include <cops/core/container.h>
#include <cops/core/interface.h>
#include <stddef.h>

typedef struct cops_array cops_array_t;

struct cops_array {
        cops_container_t super;
        void *data;
        size_t len;
};

#endif /* end of include guard: COPS_CORE_ARRAY_H */
