#ifndef COPS_CORE_CONTAINER_H
#define COPS_CORE_CONTAINER_H

#include <stddef.h>

#include <cops/core/interface.h>
#include <cops/core/iterator.h>

typedef struct cops_container cops_container_t;

typedef cops_iterator_t *(*cops_iterator_FnPtr)(cops_container_t *);
typedef void (*cops_insert_FnPtr)(cops_container_t *, const void *);
typedef void *(*cops_remove_FnPtr)(cops_container_t *);
typedef cops_container_t *(*cops_toarray_FnPtr)(cops_container_t *,
                                                cops_container_t *);
typedef void (*cops_merge_FnPtr)(cops_container_t *, cops_container_t *);

struct cops_container {
        cops_interface_t super;
        cops_iterator_FnPtr iter;
        cops_insert_FnPtr insert;
        cops_remove_FnPtr remove;
        cops_toarray_FnPtr toarr;
        cops_merge_FnPtr merge;
        cops_interface_t *value;
};

#endif /* end of include guard: COPS_CORE_CONTAINER_H */
