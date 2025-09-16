#ifndef COPS_VEC_H
#define COPS_VEC_H

#ifdef __cplusplus
extern "C" {
#endif

#include "core.h"

#define __init_cops_vec(name, T)                                                                   \
        typedef struct name {                                                                      \
                uint32_t rc;                                                                       \
                uint32_t nelem;                                                                    \
                uint8_t log_cap;                                                                   \
                T *data;                                                                           \
        } name;                                                                                    \
                                                                                                   \
        static inline name *name##_new()                                                           \
        {                                                                                          \
                name *self = cops_default_allocator.alloc(sizeof(name));                           \
                if (!self)                                                                         \
                        return NULL;                                                               \
                self->rc = 1;                                                                      \
                self->nelem = 0;                                                                   \
                self->log_cap = 3;                                                                 \
                if ((1 << self->log_cap) >= SIZE_MAX / sizeof(T)) {                                \
                        cops_default_allocator.free(self);                                         \
                        return NULL;                                                               \
                }                                                                                  \
                self->data =                                                                       \
                    cops_default_allocator.alloc(sizeof(T) * ((size_t)1 << self->log_cap));        \
                if (!self->data) {                                                                 \
                        cops_default_allocator.free(self);                                         \
                        return NULL;                                                               \
                }                                                                                  \
                return self;                                                                       \
        }                                                                                          \
                                                                                                   \
        static inline name *name##_dup(name *self)                                                 \
        {                                                                                          \
                if (self)                                                                          \
                        self->rc++;                                                                \
                return self;                                                                       \
        }                                                                                          \
                                                                                                   \
        static inline name *name##_free(name *self)                                                \
        {                                                                                          \
                if (self && self->rc > 0 && !(--self->rc)) {                                       \
                        cops_default_allocator.free(self->data);                                   \
                        cops_default_allocator.free(self);                                         \
                }                                                                                  \
                return NULL;                                                                       \
        }                                                                                          \
                                                                                                   \
        static inline int name##_push(name *self, T val)                                           \
        {                                                                                          \
                if (!self || !self->data)                                                          \
                        return -1;                                                                 \
                if (self->nelem == 1 << self->log_cap) {                                           \
                        if ((2 << self->log_cap) > SIZE_MAX / sizeof(T)) {                         \
                                return -2;                                                         \
                        }                                                                          \
                        T *old = self->data;                                                       \
                        T *new = cops_default_allocator.alloc(sizeof(T) *                          \
                                                              ((size_t)2 << self->log_cap));       \
                        if (!new)                                                                  \
                                return -1;                                                         \
                        memset(new, 0, sizeof(T) * ((size_t)2 << self->log_cap));                  \
                        memcpy(new, old, sizeof(T) * ((size_t)1 << self->log_cap));                \
                        memset(old, 0, sizeof(T) * ((size_t)1 << self->log_cap));                  \
                        self->log_cap++;                                                           \
                        self->data = new;                                                          \
                        cops_default_allocator.free(old);                                          \
                }                                                                                  \
                T *trg = self->data + self->nelem++;                                               \
                memcpy(trg, &val, sizeof(T));                                                      \
                return 0;                                                                          \
        }                                                                                          \
                                                                                                   \
        static inline int name##_pop(name *self, T *res)                                           \
        {                                                                                          \
                if (!self || !self->data)                                                          \
                        return -1;                                                                 \
                if (!self->nelem)                                                                  \
                        return -2;                                                                 \
                T *trg = self->data + --self->nelem;                                               \
                if (res)                                                                           \
                        memcpy(res, trg, sizeof(T));                                               \
                memset(trg, 0, sizeof(T));                                                         \
                return 0;                                                                          \
        }                                                                                          \
                                                                                                   \
        static inline int name##_set(name *self, uint32_t pos, T val)                              \
        {                                                                                          \
                if (!self || !self->data)                                                          \
                        return -1;                                                                 \
                if (pos == self->nelem)                                                            \
                        return name##_push(self, val);                                             \
                if (pos > self->nelem)                                                             \
                        return -2;                                                                 \
                T *trg = self->data + pos;                                                         \
                memcpy(trg, &val, sizeof(T));                                                      \
                return 0;                                                                          \
        }                                                                                          \
                                                                                                   \
        static inline int name##_get(name *self, uint32_t pos, T *res)                             \
        {                                                                                          \
                if (!self || !res || !self->data)                                                  \
                        return -1;                                                                 \
                if (pos >= self->nelem)                                                            \
                        return -2;                                                                 \
                T *trg = self->data + pos;                                                         \
                memcpy(res, trg, sizeof(T));                                                       \
                return 0;                                                                          \
        }

#define init_cops_vec(T) __init_cops_vec(cops_##T##_vec, T)

#ifdef __cplusplus
}
#endif

#endif /* ifndef COPS_VEC_H */
