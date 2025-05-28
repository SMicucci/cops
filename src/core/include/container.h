#ifndef COPS_CORE_CONTAINER_H
#define COPS_CORE_CONTAINER_H

#include <stddef.h>

#include <cops/core/interface.h>
#include <cops/core/iterator.h>

typedef struct cops_container cops_container_t;
typedef enum cops_type cops_e_type_t, *CopsEType;

typedef cops_iterator_t *(*cops_iterator_FnPtr)(cops_container_t *);
typedef void (*cops_insert_FnPtr)(cops_container_t *, const void *);
typedef void *(*cops_remove_FnPtr)(cops_container_t *);
typedef void (*cops_merge_FnPtr)(cops_container_t *, cops_container_t *);

enum cops_type {
        COPS_STRING = 0,
        COPS_NUMBER = 1,
        COPS_OBJECT = 2,
        COPS_VOIDPTR = 3
};

struct cops_container {
        cops_interface_t super;
        cops_iterator_FnPtr iter;
        cops_insert_FnPtr insert;
        cops_remove_FnPtr remove;
        cops_merge_FnPtr merge;
        cops_e_type_t v_type;
        size_t v_size;
};

#endif /* end of include guard: COPS_CORE_CONTAINER_H */
