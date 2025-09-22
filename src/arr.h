#ifndef COPS_ARR_H
#define COPS_ARR_H

#ifdef __cplusplus
extern "C" {
#endif

#include "core.h"

#define __init_cops_arr(name, T)                                                                   \
        typedef struct name {                                                                      \
                uint32_t len;                                                                      \
                uint32_t rc;                                                                       \
                T data[];                                                                          \
        } name;                                                                                    \
        static inline name *name##_new(uint32_t len)                                               \
        {                                                                                          \
                if (len > (uint32_t)((SIZE_MAX - sizeof(name)) / sizeof(T)))                       \
                        return NULL;                                                               \
                name *self =                                                                       \
                    (name *)cops_default_allocator.alloc(sizeof(*self) + sizeof(T) * len);         \
                if (!self)                                                                         \
                        return NULL;                                                               \
                self->len = len;                                                                   \
                self->rc = 1;                                                                      \
                memset(self->data, 0, sizeof(T) * len);                                            \
                return self;                                                                       \
        }                                                                                          \
        static inline name *name##_dup(name *self)                                                 \
        {                                                                                          \
                if (self)                                                                          \
                        self->rc++;                                                                \
                return self;                                                                       \
        }                                                                                          \
        static inline name *name##_free(name *self)                                                \
        {                                                                                          \
                if (self && self->rc > 0 && !(--self->rc)) {                                       \
                        cops_default_allocator.free(self);                                         \
                }                                                                                  \
                return NULL;                                                                       \
        }

#define init_cops_arr(T) __init_cops_arr(cops_##T##_arr, T)

// string
__init_cops_arr(string, char)

#ifdef __cplusplus
}
#endif

#endif /* ifndef COPS_ARR_H */
