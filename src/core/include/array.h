#ifndef COPS_CORE_ARRAY_H
#define COPS_CORE_ARRAY_H

#include <cops/core/container.h>
#include <cops/core/interface.h>
#include <cstddef>
#include <stddef.h>

typedef struct cops_array cops_array_t;

struct cops_array {
        cops_container_t super;
        void *data;
        size_t len;
};

void array_init(cops_array_t *self, cops_interface_t type, size_t len);
void array_clean(cops_array_t *self);

#endif /* end of include guard: COPS_CORE_ARRAY_H */
