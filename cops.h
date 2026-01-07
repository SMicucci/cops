#ifndef COPS_H
#define COPS_H
/* start import ./src/core.h */
/* optional assert */
#if defined(COPS_ASSERT_ENABLE)
#include <assert.h>
#define COPS_ASSERT(x) assert(x)
#else
#undef COPS_ASSERT
#define COPS_ASSERT(x) ((void)0)
#endif

/* allocate with libc by default */
#if !defined(COPS_ALLOC)
#include <stdlib.h>
#define COPS_ALLOC(sz) malloc(sz)
#define COPS_REALLOC(ptr, sz) realloc(ptr, sz)
#define COPS_FREE(ptr) free(ptr)
#endif

/* default return value to check */
#define COPS_OK ((int)0)
#define COPS_INVALID ((int)-1)
#define COPS_MEMERR ((int)-2)

/* slice declaration for interop between collections/prototypes */
#include <stdint.h>
#define init_slice(T, name)                                                    \
        typedef struct name {                                                  \
                uint64_t len;                                                  \
                T data[];                                                      \
        } name;                                                                \
        static inline name *name##_new(uint64_t len)                           \
        {                                                                      \
                name *self = COPS_ALLOC(sizeof(*self) + sizeof(T) * len);      \
                COPS_ASSERT(self);                                             \
                self->len = len;                                               \
                return self;                                                   \
        }                                                                      \
        static inline void name##_free(name *self) { COPS_FREE(self); }

/* variadic macro utility */
#define __cops_get_macro(_1, _2, _3, _4, macro, ...) macro
#define __cops_expand(x) x
#define __cops_err_arg_count(...) char __cops_err_arg_count[-1]
#define __cops_err_var_macro(...) char
/* end import ./src/core.h */
/* start import ./src/vec.h */
#define __cops_vec_choose(...)                                                 \
        __cops_get_macro(__VA_ARGS__, __cops_err_arg_count, __cops_init_vec_3, \
                         __cops_init_vec_2, __cops_err_arg_count, )
#define init_vec(...) __cops_expand(__cops_vec_choose(__VA_ARGS__)(__VA_ARGS__))

#define __cops_init_vec_2(T, NAME)                                             \
        typedef struct NAME {                                                  \
                uint64_t len;                                                  \
                uint64_t cap;                                                  \
                T *(*dup)(T);                                                  \
                void (*free)(T);                                               \
                T *data;                                                       \
        } NAME;                                                                \
                                                                               \
        static inline NAME *NAME##_new()                                       \
        {                                                                      \
                NAME *self = COPS_ALLOC(sizeof(*self));                        \
                COPS_ASSERT(self);                                             \
                if (!self) {                                                   \
                        return NULL;                                           \
                }                                                              \
                self->cap = 8;                                                 \
                self->data = COPS_ALLOC(sizeof(T) * self->cap);                \
                COPS_ASSERT(self->data);                                       \
                if (!self->data) {                                             \
                        COPS_FREE(self);                                       \
                        return NULL;                                           \
                }                                                              \
                return self;                                                   \
        }                                                                      \
                                                                               \
        static inline void NAME##_free(NAME *self)                             \
        {                                                                      \
                COPS_ASSERT(self);                                             \
                if (!self) {                                                   \
                        return;                                                \
                }                                                              \
                if (self->free) {                                              \
                        for (uint64_t i = 0; i < self->len; i++) {             \
                                self->free(self->data[i]);                     \
                        }                                                      \
                }                                                              \
                COPS_FREE(self->data);                                         \
                COPS_FREE(self);                                               \
        }                                                                      \
                                                                               \
        static inline int NAME##_push(NAME *self, T val)                       \
        {                                                                      \
                COPS_ASSERT(self);                                             \
                if (!self) {                                                   \
                        return COPS_INVALID;                                   \
                }                                                              \
                if (self->len == self->cap) {                                  \
                        self->cap *= 2;                                        \
                        self->data =                                           \
                            COPS_REALLOC(self->data, self->cap * sizeof(T));   \
                        COPS_ASSERT(self->data);                               \
                        if (!self->data) {                                     \
                                return COPS_MEMERR;                            \
                        }                                                      \
                }                                                              \
                self->data[self->len++] = val;                                 \
                return COPS_OK;                                                \
        }                                                                      \
                                                                               \
        static inline int NAME##_pop(NAME *self, T *res)                       \
        {                                                                      \
                COPS_ASSERT(self);                                             \
                if (!self) {                                                   \
                        return COPS_INVALID;                                   \
                }                                                              \
                if (res) {                                                     \
                        *res = self->data[self->len];                          \
                }                                                              \
                self->len--;                                                   \
                return COPS_OK;                                                \
        }                                                                      \
                                                                               \
        static inline int NAME##_set(NAME *self, uint64_t pos, T val, T *old)  \
        {                                                                      \
                COPS_ASSERT(self);                                             \
                COPS_ASSERT(pos < self->len);                                  \
                if (!self || pos >= self->len) {                               \
                        return COPS_INVALID;                                   \
                }                                                              \
                if (old) {                                                     \
                        *old = self->data[pos];                                \
                }                                                              \
                self->data[pos] = val;                                         \
                return COPS_OK;                                                \
        }                                                                      \
                                                                               \
        static inline int NAME##_get(NAME *self, uint64_t pos, T *val)         \
        {                                                                      \
                COPS_ASSERT(self);                                             \
                COPS_ASSERT(pos < self->len);                                  \
                if (!self || pos >= self->len) {                               \
                        return COPS_INVALID;                                   \
                }                                                              \
                if (val) {                                                     \
                        *val = self->data[pos];                                \
                }                                                              \
                return COPS_OK;                                                \
        }

#define __cops_init_vec_3(T, NAME, SLICE_T)                                    \
        __cops_init_vec_2(T, NAME);                                            \
                                                                               \
        static inline SLICE_T *NAME##_export(NAME *self)                       \
        {                                                                      \
                SLICE_T *slice = SLICE_T##_new(self->len);                     \
                COPS_ASSERT(slice);                                            \
                if (!slice) {                                                  \
                        return (SLICE_T *)NULL;                                \
                }                                                              \
                memcpy(slice->data, self->data, self->len * sizeof(T));        \
                return slice;                                                  \
        }                                                                      \
        static inline int NAME##_import(NAME *self, SLICE_T *slice)            \
        {                                                                      \
                COPS_ASSERT(self);                                             \
                COPS_ASSERT(slice);                                            \
                if (!self || !slice) {                                         \
                        return COPS_INVALID;                                   \
                }                                                              \
                uint64_t old_cap = self->cap;                                  \
                while (self->cap < self->len + slice->len) {                   \
                        self->cap *= 2;                                        \
                }                                                              \
                if (self->cap != old_cap) { /*realloc*/                        \
                        self->data =                                           \
                            COPS_REALLOC(self->data, self->cap * sizeof(T));   \
                        COPS_ASSERT(self->data);                               \
                        if (!self->data) {                                     \
                                return COPS_MEMERR;                            \
                        }                                                      \
                }                                                              \
                memcpy((self->data + self->len), slice->data,                  \
                       slice->len * sizeof(T));                                \
                self->len += slice->len;                                       \
                SLICE_T##_free(slice);                                         \
                return COPS_OK;                                                \
        }
/* end import ./src/vec.h */
#endif /* #ifndef COPS_H guard */
