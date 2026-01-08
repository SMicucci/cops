#include "core.h"
#define __cops_vec_choose(...)                                                 \
        __cops_get_macro(__VA_ARGS__, __cops_err_arg_count, __cops_init_vec_3, \
                         __cops_init_vec_2, __cops_err_arg_count, )
#define init_vec(...) __cops_expand(__cops_vec_choose(__VA_ARGS__)(__VA_ARGS__))

#define __cops_init_vec_2(T, NAME)                                             \
        typedef struct NAME {                                                  \
                uint64_t len;                                                  \
                uint64_t cap;                                                  \
                T *data;                                                       \
                void (*free)(T);                                               \
                T (*dup)(T);                                                   \
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
                self->len = 0;                                                 \
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
        static inline int NAME##_reset(NAME *self)                             \
        {                                                                      \
                COPS_ASSERT(self);                                             \
                if (!self)                                                     \
                        return COPS_INVALID;                                   \
                self->len = 0;                                                 \
                return COPS_OK;                                                \
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
                } else if (self->free) {                                       \
                        self->free(self->data[self->len]);                     \
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
                } else if (self->free) {                                       \
                        self->free(self->data[pos]);                           \
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
        }                                                                      \
                                                                               \
        static inline int NAME##_insert(NAME *self, uint64_t pos, T val)       \
        {                                                                      \
                COPS_ASSERT(self);                                             \
                COPS_ASSERT(pos <= self->len);                                 \
                if (!self || pos > self->len) {                                \
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
                self->data[self->len++] = self->data[pos];                     \
                self->data[pos] = val;                                         \
                return COPS_OK;                                                \
        }                                                                      \
                                                                               \
        static inline int NAME##_remove(NAME *self, uint64_t pos, T *res)      \
        {                                                                      \
                COPS_ASSERT(self);                                             \
                COPS_ASSERT(pos < self->len);                                  \
                if (!self || pos >= self->len) {                               \
                        return COPS_INVALID;                                   \
                }                                                              \
                if (res) {                                                     \
                        *res = self->data[pos];                                \
                } else if (self->free) {                                       \
                        self->free(self->data[pos]);                           \
                }                                                              \
                self->data[pos] = self->data[self->len-- - 1];                 \
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
                slice->free = self->free;                                      \
                slice->dup = self->dup;                                        \
                if (self->dup) {                                               \
                        for (uint64_t i = 0; i < self->len; i++) {             \
                                slice->data[i] = self->dup(slice->data[i]);    \
                        }                                                      \
                } else                                                         \
                        memcpy(slice->data, self->data,                        \
                               self->len * sizeof(T));                         \
                return slice;                                                  \
        }                                                                      \
                                                                               \
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
