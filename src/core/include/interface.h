#ifndef COPS_CORE_INTERFACE_H
#define COPS_CORE_INTERFACE_H

#include <stddef.h>
typedef struct cops_interface cops_interface_t;

typedef void (*cops_init_FnPtr)(void *);
typedef void (*cops_clean_FnPtr)(cops_interface_t *);
typedef int (*cops_compare_FnPtr)(const cops_interface_t *,
                                  const cops_interface_t *);
typedef unsigned long (*cops_hash_FnPtr)(const cops_interface_t *);
typedef void *(*cops_clone_FnPtr)(const cops_interface_t *);

struct cops_interface {
        cops_init_FnPtr init;
        cops_clean_FnPtr clean;
        cops_compare_FnPtr compare;
        cops_hash_FnPtr hash;
        cops_clone_FnPtr clone;
        size_t size;
};

#endif /* end of include guard: COPS_CORE_INTERFACE_H */
