#ifndef COPS_CORE_ARRAY_H
#define COPS_CORE_ARRAY_H

#include <cops/core/container.h>
#include <cops/core/interface.h>
#include <cops/core/iterator.h>
#include <stddef.h>

typedef struct cops_array cops_array_t;

extern const cops_interface_t *const Array;

struct cops_array {
        cops_container_t super;
        void *data;
        size_t len;
};

void array_init(cops_array_t *self, cops_interface_t *type, size_t len);
void array_clean(cops_array_t *self);
cops_iterator_t *array_iterator(cops_array_t *self);
void array_merge(cops_array_t *self, cops_container_t *input);

void array_insert(cops_array_t *self, size_t pos, void *val);
void *array_remove(cops_array_t *self, size_t pos);
void array_set(cops_array_t *self, size_t pos, void *val);
void *array_peek(cops_array_t *self, size_t pos);

#endif /* end of include guard: COPS_CORE_ARRAY_H */
