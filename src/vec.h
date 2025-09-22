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
                uint32_t cap;                                                                      \
                void (*free)(T);                                                                   \
                T (*dup)(T);                                                                       \
                T *data;                                                                           \
        } name;                                                                                    \
                                                                                                   \
        static inline name *name##_new()                                                           \
        {                                                                                          \
                name *self = (name *)cops_default_allocator.alloc(sizeof(*self));                  \
                if (!self)                                                                         \
                        return NULL;                                                               \
                self->rc = 1;                                                                      \
                self->nelem = 0;                                                                   \
                self->cap = 1 << 3;                                                                \
                self->free = NULL;                                                                 \
                if ((unsigned long)(self->cap) >= SIZE_MAX / sizeof(T)) {                          \
                        cops_default_allocator.free(self);                                         \
                        return NULL;                                                               \
                }                                                                                  \
                self->data =                                                                       \
                    (T *)cops_default_allocator.alloc(sizeof(*self->data) * (size_t)self->cap);    \
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
                        if (self->free) {                                                          \
                                for (size_t i = 0; i < self->nelem; i++) {                         \
                                        self->free(self->data[i]);                                 \
                                }                                                                  \
                        }                                                                          \
                        cops_default_allocator.free(self->data);                                   \
                        cops_default_allocator.free(self);                                         \
                }                                                                                  \
                return NULL;                                                                       \
        }                                                                                          \
                                                                                                   \
        static inline int name##_push(name *self, T val)                                           \
        {                                                                                          \
                if (!self || !self->data)                                                          \
                        return COPS_INVALID;                                                       \
                if (self->nelem == (uint32_t)self->cap) {                                          \
                        if ((unsigned long)(2 * self->cap) > (SIZE_MAX / sizeof(T))) {             \
                                return -2;                                                         \
                        }                                                                          \
                        T *old = self->data;                                                       \
                        T *new = cops_default_allocator.realloc(old, sizeof(T) *                   \
                                                                         (size_t)(2 * self->cap)); \
                        if (!new)                                                                  \
                                return COPS_MEMERR;                                                \
                        memset(new + self->cap, 0, sizeof(T) * (size_t)self->cap);                 \
                        self->cap *= 2;                                                            \
                        self->data = new;                                                          \
                }                                                                                  \
                T *trg = self->data + self->nelem++;                                               \
                memcpy(trg, &val, sizeof(T));                                                      \
                return COPS_OK;                                                                    \
        }                                                                                          \
                                                                                                   \
        static inline int name##_pop(name *self, T *res)                                           \
        {                                                                                          \
                if (!self || !self->data || !self->nelem)                                          \
                        return COPS_INVALID;                                                       \
                T *trg = self->data + --self->nelem;                                               \
                if (res)                                                                           \
                        memcpy(res, trg, sizeof(T));                                               \
                if (self->free)                                                                    \
                        self->free(*trg);                                                          \
                memset(trg, 0, sizeof(T));                                                         \
                return COPS_OK;                                                                    \
        }                                                                                          \
                                                                                                   \
        static inline int name##_set(name *self, uint32_t pos, T val)                              \
        {                                                                                          \
                if (!self || !self->data || pos > self->nelem)                                     \
                        return COPS_INVALID;                                                       \
                if (pos == self->nelem)                                                            \
                        return name##_push(self, val);                                             \
                T *trg = self->data + pos;                                                         \
                if (self->free)                                                                    \
                        self->free(*trg);                                                          \
                *trg = val;                                                                        \
                return COPS_OK;                                                                    \
        }                                                                                          \
                                                                                                   \
        static inline int name##_get(name *self, uint32_t pos, T *res)                             \
        {                                                                                          \
                if (!self || !self->data || pos >= self->nelem)                                    \
                        return COPS_INVALID;                                                       \
                T *trg = self->data + pos;                                                         \
                if (res)                                                                           \
                        *res = *trg;                                                               \
                return COPS_OK;                                                                    \
        }                                                                                          \
                                                                                                   \
        static inline int name##_import(name *self, const name *oth)                               \
        {                                                                                          \
                if (!self || !self->data || !oth || !oth->data)                                    \
                        return COPS_INVALID;                                                       \
                if (self->nelem > UINT32_MAX - oth->nelem)                                         \
                        return COPS_INVALID;                                                       \
                uint32_t new_cap = self->cap;                                                      \
                while (self->nelem + oth->nelem > new_cap)                                         \
                        new_cap *= 2;                                                              \
                if (new_cap != self->cap) {                                                        \
                        T *new = cops_default_allocator.realloc(self->data, new_cap);              \
                        if (!new)                                                                  \
                                return COPS_MEMERR;                                                \
                        memset(new + self->cap, 0, (size_t)(new_cap - self->cap) * sizeof(T));     \
                        self->data = new;                                                          \
                        self->cap = new_cap;                                                       \
                }                                                                                  \
                if (self->dup) {                                                                   \
                        for (size_t i = 0; i < oth->nelem; i++) {                                  \
                                self->data[self->nelem + i] = self->dup(oth->data[i]);             \
                        }                                                                          \
                } else {                                                                           \
                        memcpy(self->data + self->nelem, oth->data,                                \
                               sizeof(T) * (size_t)oth->nelem);                                    \
                }                                                                                  \
                self->nelem += oth->nelem;                                                         \
                return COPS_OK;                                                                    \
        }

#define init_cops_vec(T) __init_cops_vec(cops_##T##_vec, T)

#ifdef __cplusplus
}
#endif

#endif /* ifndef COPS_VEC_H */
