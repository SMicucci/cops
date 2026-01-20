#ifndef COPS_H
#define COPS_H
/* start import ./src/01_core.h */
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
#if defined(COPS_IMPLEMENTATION)
#define init_slice(T, NAME)                                                    \
        __cops_slice_decl(T, NAME) __cops_slice_impl(T, NAME)
#else
#define init_slice(T, NAME) __cops_slice_decl(T, NAME);
#endif /* if defined(COPS_IMPLEMENTATION) */

#define __cops_slice_decl(T, NAME)                                             \
        typedef struct NAME {                                                  \
                uint64_t len;                                                  \
                T data[];                                                      \
        } NAME;                                                                \
        NAME *NAME##_new(uint64_t len);                                        \
        void NAME##_free(NAME *self);

#define __cops_slice_impl(T, NAME)                                             \
        NAME *NAME##_new(uint64_t len)                                         \
        {                                                                      \
                NAME *self = COPS_ALLOC(sizeof(*self) + sizeof(T) * len);      \
                COPS_ASSERT(self && "failed allocation");                      \
                self->len = len;                                               \
                return self;                                                   \
        }                                                                      \
        void NAME##_free(NAME *self)                                           \
        {                                                                      \
                COPS_ASSERT(self && "invalid reference");                      \
                if (!self)                                                     \
                        return;                                                \
                COPS_FREE(self);                                               \
        }

/* variadic macro utility */
#define __cops_get_macro(_1, _2, _3, _4, macro, ...) macro
#define __cops_expand(x) x
#define __cops_err_arg_count(...) char __cops_err_arg_count[-1]
#define __cops_err_var_macro(...) char
/* end import ./src/01_core.h */
/* start import ./src/02_vec.h */
#define __cops_vec_choose(...)                                                 \
        __cops_get_macro(__VA_ARGS__, __cops_err_arg_count, __cops_init_vec_3, \
                         __cops_init_vec_2, __cops_err_arg_count, )
#define init_vec(...) __cops_expand(__cops_vec_choose(__VA_ARGS__)(__VA_ARGS__))

#if defined(COPS_IMPLEMENTATION)
#define __cops_init_vec_2(T, NAME)                                             \
        __cops_init_vec_2_decl(T, NAME) __cops_init_vec_2_impl(T, NAME)
#define __cops_init_vec_3(T, NAME, SLICE_T)                                    \
        __cops_init_vec_3_decl(T, NAME, SLICE_T)                               \
            __cops_init_vec_3_impl(T, NAME, SLICE_T)
#else
#define __cops_init_vec_2(T, NAME) __cops_init_vec_2_decl(T, NAME)
#define __cops_init_vec_3(T, NAME, SLICE_T)                                    \
        __cops_init_vec_3_decl(T, NAME, SLICE_T)
#endif /* if defined(COPS_IMPLEMENTATION) */

#define __cops_init_vec_2_decl(T, NAME)                                        \
        typedef struct NAME {                                                  \
                uint64_t len;                                                  \
                uint64_t cap;                                                  \
                T *data;                                                       \
                void (*free)(T);                                               \
                T (*dup)(T);                                                   \
        } NAME;                                                                \
        NAME *NAME##_new();                                                    \
        void NAME##_free(NAME *self);                                          \
        int NAME##_reset(NAME *self);                                          \
        int NAME##_push(NAME *self, T val);                                    \
        int NAME##_pop(NAME *self, T *res);                                    \
        int NAME##_set(NAME *self, uint64_t pos, T val, T *res);               \
        int NAME##_get(NAME *self, uint64_t pos, T *res);                      \
        int NAME##_insert(NAME *self, uint64_t pos, T val);                    \
        int NAME##_remove(NAME *self, uint64_t pos, T *res);

#define __cops_init_vec_2_impl(T, NAME)                                        \
                                                                               \
        NAME *NAME##_new()                                                     \
        {                                                                      \
                NAME *self = COPS_ALLOC(sizeof(*self));                        \
                COPS_ASSERT(self && "failed allocation");                      \
                if (!self) {                                                   \
                        return NULL;                                           \
                }                                                              \
                self->free = NULL;                                             \
                self->dup = NULL;                                              \
                self->cap = 8;                                                 \
                self->len = 0;                                                 \
                self->data = COPS_ALLOC(sizeof(T) * self->cap);                \
                COPS_ASSERT(self->data && "failed allocation");                \
                if (!self->data) {                                             \
                        COPS_FREE(self);                                       \
                        return NULL;                                           \
                }                                                              \
                return self;                                                   \
        }                                                                      \
                                                                               \
        void NAME##_free(NAME *self)                                           \
        {                                                                      \
                COPS_ASSERT(self && "invalid reference");                      \
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
        int NAME##_reset(NAME *self)                                           \
        {                                                                      \
                COPS_ASSERT(self && "invalid reference");                      \
                if (!self)                                                     \
                        return COPS_INVALID;                                   \
                if (self->free) {                                              \
                        for (uint64_t i = 0; i < self->len; i++)               \
                                self->free(self->data[i]);                     \
                }                                                              \
                self->len = 0;                                                 \
                return COPS_OK;                                                \
        }                                                                      \
                                                                               \
        int NAME##_push(NAME *self, T val)                                     \
        {                                                                      \
                COPS_ASSERT(self && "invalid reference");                      \
                if (!self) {                                                   \
                        return COPS_INVALID;                                   \
                }                                                              \
                if (self->len == self->cap) {                                  \
                        self->cap *= 2;                                        \
                        self->data =                                           \
                            COPS_REALLOC(self->data, self->cap * sizeof(T));   \
                        COPS_ASSERT(self->data && "failed allocation");        \
                        if (!self->data) {                                     \
                                return COPS_MEMERR;                            \
                        }                                                      \
                }                                                              \
                self->data[self->len++] = val;                                 \
                return COPS_OK;                                                \
        }                                                                      \
                                                                               \
        int NAME##_pop(NAME *self, T *res)                                     \
        {                                                                      \
                COPS_ASSERT(self && "invalid reference");                      \
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
        int NAME##_set(NAME *self, uint64_t pos, T val, T *res)                \
        {                                                                      \
                COPS_ASSERT(self && "invalid reference");                      \
                COPS_ASSERT(pos < self->len ||                                 \
                            pos >= 0 && "access out of bound");                \
                if (!self || pos >= self->len || pos < 0) {                    \
                        return COPS_INVALID;                                   \
                }                                                              \
                if (res) {                                                     \
                        *res = self->data[pos];                                \
                } else if (self->free) {                                       \
                        self->free(self->data[pos]);                           \
                }                                                              \
                self->data[pos] = val;                                         \
                return COPS_OK;                                                \
        }                                                                      \
                                                                               \
        int NAME##_get(NAME *self, uint64_t pos, T *res)                       \
        {                                                                      \
                COPS_ASSERT(self && "invalid reference");                      \
                COPS_ASSERT(pos < self->len ||                                 \
                            pos >= 0 && "access out of bound");                \
                if (!self || pos >= self->len || pos < 0) {                    \
                        return COPS_INVALID;                                   \
                }                                                              \
                if (res) {                                                     \
                        *res = self->data[pos];                                \
                }                                                              \
                return COPS_OK;                                                \
        }                                                                      \
                                                                               \
        int NAME##_insert(NAME *self, uint64_t pos, T val)                     \
        {                                                                      \
                COPS_ASSERT(self && "invalid reference");                      \
                COPS_ASSERT(pos <= self->len ||                                \
                            pos >= 0 && "access out of bound");                \
                if (!self || pos > self->len || pos < 0) {                     \
                        return COPS_INVALID;                                   \
                }                                                              \
                if (self->len == self->cap) {                                  \
                        self->cap *= 2;                                        \
                        self->data =                                           \
                            COPS_REALLOC(self->data, self->cap * sizeof(T));   \
                        COPS_ASSERT(self->data && "failed allocation");        \
                        if (!self->data) {                                     \
                                return COPS_MEMERR;                            \
                        }                                                      \
                }                                                              \
                self->data[self->len++] = self->data[pos];                     \
                self->data[pos] = val;                                         \
                return COPS_OK;                                                \
        }                                                                      \
                                                                               \
        int NAME##_remove(NAME *self, uint64_t pos, T *res)                    \
        {                                                                      \
                COPS_ASSERT(self && "invalid reference");                      \
                COPS_ASSERT(pos < self->len ||                                 \
                            pos >= 0 && "access out of bound");                \
                if (!self || pos >= self->len || pos < 0) {                    \
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

#define __cops_init_vec_3_decl(T, NAME, SLICE_T)                               \
        __cops_init_vec_2(T, NAME);                                            \
        SLICE_T *NAME##_export(NAME *self);                                    \
        int NAME##_import(NAME *self, SLICE_T *slice);

#define __cops_init_vec_3_impl(T, NAME, SLICE_T)                               \
        SLICE_T *NAME##_export(NAME *self)                                     \
        {                                                                      \
                COPS_ASSERT(self && "invalid reference");                      \
                if (!self)                                                     \
                        return (SLICE_T *)NULL;                                \
                SLICE_T *slice = SLICE_T##_new(self->len);                     \
                COPS_ASSERT(slice && "failed allocation");                     \
                if (!slice) {                                                  \
                        return NULL;                                           \
                }                                                              \
                if (self->dup) {                                               \
                        for (uint64_t i = 0; i < self->len; i++) {             \
                                slice->data[i] = self->dup(self->data[i]);     \
                        }                                                      \
                } else                                                         \
                        memcpy(slice->data, self->data,                        \
                               self->len * sizeof(T));                         \
                return slice;                                                  \
        }                                                                      \
                                                                               \
        int NAME##_import(NAME *self, SLICE_T *slice)                          \
        {                                                                      \
                COPS_ASSERT(self && "invalid reference");                      \
                COPS_ASSERT(slice && "invalid reference");                     \
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
                        COPS_ASSERT(self->data && "failed allocation");        \
                        if (!self->data) {                                     \
                                return COPS_MEMERR;                            \
                        }                                                      \
                }                                                              \
                if (self->dup) {                                               \
                        for (uint64_t i = 0; i < slice->len; i++) {            \
                                self->data[i + self->len] =                    \
                                    self->dup(slice->data[i]);                 \
                        }                                                      \
                } else                                                         \
                        memcpy((self->data + self->len), slice->data,          \
                               slice->len * sizeof(T));                        \
                self->len += slice->len;                                       \
                SLICE_T##_free(slice);                                         \
                return COPS_OK;                                                \
        }
/* end import ./src/02_vec.h */
/* start import ./src/03_list.h */
#define __cops_list_choose(...)                                                \
        __cops_get_macro(__VA_ARGS__, __cops_err_arg_count,                    \
                         __cops_init_list_3, __cops_init_list_2,               \
                         __cops_err_arg_count, )
#define init_list(...)                                                         \
        __cops_expand(__cops_list_choose(__VA_ARGS__)(__VA_ARGS__))

#if defined(COPS_IMPLEMENTATION)
#define __cops_init_list_2(T, NAME)                                            \
        __cops_init_list_2_decl(T, NAME) __cops_init_list_2_impl(T, NAME)
#define __cops_init_list_3(T, NAME, SLICE_T)                                   \
        __cops_init_list_3_decl(T, NAME, SLICE_T)                              \
            __cops_init_list_3_impl(T, NAME, SLICE_T)
#else
#define __cops_init_list_2(T, NAME) __cops_init_list_2_decl(T, NAME)
#define __cops_init_list_3(T, NAME, SLICE_T)                                   \
        __cops_init_list_3_decl(T, NAME, SLICE_T)
#endif /* if defined(COPS_IMPLEMENTATION) */

#define __cops_init_list_2_decl(T, NAME)                                       \
        typedef struct NAME##_ll_node NAME##_ll_node;                          \
        struct NAME##_ll_node {                                                \
                NAME##_ll_node *next, *prev;                                   \
                T val;                                                         \
        };                                                                     \
        typedef struct NAME {                                                  \
                uint64_t len;                                                  \
                NAME##_ll_node *head, *tail;                                   \
                void (*free)(T);                                               \
                T (*dup)(T);                                                   \
        } NAME;                                                                \
        NAME *NAME##_new();                                                    \
        void NAME##_free(NAME *self);                                          \
        int NAME##_push_front(NAME *self, T val);                              \
        int NAME##_push_back(NAME *self, T val);                               \
        int NAME##_pop_front(NAME *self, T *res);                              \
        int NAME##_pop_back(NAME *self, T *res);                               \
        int NAME##_insert_before(NAME *self, NAME##_ll_node *node, T val);     \
        int NAME##_insert_after(NAME *self, NAME##_ll_node *node, T val);      \
        int NAME##_remove(NAME *self, NAME##_ll_node *node);

#define __cops_init_list_2_impl(T, NAME)                                       \
        NAME *NAME##_new()                                                     \
        {                                                                      \
                NAME *self = COPS_ALLOC(sizeof(*self));                        \
                COPS_ASSERT(self && "failed allocation");                      \
                if (!self)                                                     \
                        return NULL;                                           \
                self->len = 0;                                                 \
                self->head = self->tail = NULL;                                \
                self->free = NULL;                                             \
                self->dup = NULL;                                              \
                return self;                                                   \
        }                                                                      \
                                                                               \
        void NAME##_free(NAME *self)                                           \
        {                                                                      \
                COPS_ASSERT(self && "invalid reference");                      \
                if (!self)                                                     \
                        return;                                                \
                NAME##_ll_node *trg = self->head;                              \
                if (self->free) {                                              \
                        while (trg != NULL) {                                  \
                                NAME##_ll_node *next = trg->next;              \
                                self->free(trg->val);                          \
                                COPS_FREE(trg);                                \
                                trg = next;                                    \
                        }                                                      \
                } else {                                                       \
                        while (trg != NULL) {                                  \
                                NAME##_ll_node *next = trg->next;              \
                                COPS_FREE(trg);                                \
                                trg = next;                                    \
                        }                                                      \
                }                                                              \
                COPS_FREE(self);                                               \
        }                                                                      \
                                                                               \
        int NAME##_push_front(NAME *self, T val)                               \
        {                                                                      \
                COPS_ASSERT(self && "invalid reference");                      \
                if (!self)                                                     \
                        return COPS_INVALID;                                   \
                NAME##_ll_node *node = COPS_ALLOC(sizeof(*node));              \
                COPS_ASSERT(node && "failed allocation");                      \
                if (!node)                                                     \
                        return COPS_MEMERR;                                    \
                *node = (NAME##_ll_node){self->head, NULL, val};               \
                if (!self->len) {                                              \
                        self->head = self->tail = node;                        \
                } else {                                                       \
                        self->head->prev = node;                               \
                        self->head = node;                                     \
                }                                                              \
                self->len++;                                                   \
                return COPS_OK;                                                \
        }                                                                      \
                                                                               \
        int NAME##_push_back(NAME *self, T val)                                \
        {                                                                      \
                COPS_ASSERT(self && "invalid reference");                      \
                if (!self)                                                     \
                        return COPS_INVALID;                                   \
                NAME##_ll_node *node = COPS_ALLOC(sizeof(*node));              \
                COPS_ASSERT(node && "failed allocation");                      \
                if (!node)                                                     \
                        return COPS_MEMERR;                                    \
                *node = (NAME##_ll_node){NULL, self->tail, val};               \
                if (!self->len) {                                              \
                        self->head = self->tail = node;                        \
                } else {                                                       \
                        self->tail->next = node;                               \
                        self->tail = node;                                     \
                }                                                              \
                self->len++;                                                   \
                return COPS_OK;                                                \
        }                                                                      \
                                                                               \
        int NAME##_pop_front(NAME *self, T *res)                               \
        {                                                                      \
                COPS_ASSERT(self && "invalid reference");                      \
                COPS_ASSERT(self->head && "empty reference");                  \
                if (!self || !self->head)                                      \
                        return COPS_INVALID;                                   \
                if (res)                                                       \
                        *res = self->head->val;                                \
                else if (self->free)                                           \
                        self->free(self->head->val);                           \
                NAME##_ll_node *node = self->head->next;                       \
                if (node)                                                      \
                        node->prev = NULL;                                     \
                else                                                           \
                        self->tail = NULL;                                     \
                COPS_FREE(self->head);                                         \
                self->head = node;                                             \
                self->len--;                                                   \
                return COPS_OK;                                                \
        }                                                                      \
                                                                               \
        int NAME##_pop_back(NAME *self, T *res)                                \
        {                                                                      \
                COPS_ASSERT(self && "invalid reference");                      \
                COPS_ASSERT(self->tail && "empty reference");                  \
                if (!self || !self->tail)                                      \
                        return COPS_INVALID;                                   \
                if (res)                                                       \
                        *res = self->tail->val;                                \
                else if (self->free)                                           \
                        self->free(self->tail->val);                           \
                NAME##_ll_node *node = self->tail->prev;                       \
                if (node)                                                      \
                        node->next = NULL;                                     \
                else                                                           \
                        self->head = NULL;                                     \
                COPS_FREE(self->tail);                                         \
                self->tail = node;                                             \
                self->len--;                                                   \
                return COPS_OK;                                                \
        }                                                                      \
                                                                               \
        int NAME##_insert_before(NAME *self, NAME##_ll_node *node, T val)      \
        {                                                                      \
                COPS_ASSERT(self && "invalid reference");                      \
                COPS_ASSERT(node && "invalid reference");                      \
                COPS_ASSERT(self->head && "empty reference");                  \
                if (!self || !node || !self->head)                             \
                        return COPS_INVALID;                                   \
                NAME##_ll_node *tmp = COPS_ALLOC(sizeof(*tmp));                \
                COPS_ASSERT(tmp && "failed allocation");                       \
                if (!tmp)                                                      \
                        return COPS_MEMERR;                                    \
                *tmp = (NAME##_ll_node){node, node->prev, val};                \
                if (tmp->prev) {                                               \
                        tmp->prev->next = tmp;                                 \
                } else {                                                       \
                        self->head = tmp;                                      \
                }                                                              \
                node->prev = tmp;                                              \
                self->len++;                                                   \
                return COPS_OK;                                                \
        }                                                                      \
                                                                               \
        int NAME##_insert_after(NAME *self, NAME##_ll_node *node, T val)       \
        {                                                                      \
                COPS_ASSERT(self && "invalid reference");                      \
                COPS_ASSERT(node && "invalid reference");                      \
                COPS_ASSERT(self->tail && "empty reference");                  \
                if (!self || !node || !self->tail)                             \
                        return COPS_INVALID;                                   \
                NAME##_ll_node *tmp = COPS_ALLOC(sizeof(*tmp));                \
                COPS_ASSERT(tmp && "failed allocation");                       \
                if (!tmp)                                                      \
                        return COPS_MEMERR;                                    \
                *tmp = (NAME##_ll_node){node->next, node, val};                \
                if (tmp->next) {                                               \
                        tmp->next->prev = tmp;                                 \
                } else {                                                       \
                        self->tail = tmp;                                      \
                }                                                              \
                node->next = tmp;                                              \
                self->len++;                                                   \
                return COPS_OK;                                                \
        }                                                                      \
                                                                               \
        int NAME##_remove(NAME *self, NAME##_ll_node *node)                    \
        {                                                                      \
                COPS_ASSERT(self && "invalid reference");                      \
                COPS_ASSERT(node && "invalid reference");                      \
                if (!self || !node || !self->head || !self->tail)              \
                        return COPS_INVALID;                                   \
                NAME##_ll_node *n = node->next, *p = node->prev;               \
                if (n)                                                         \
                        n->prev = p;                                           \
                else                                                           \
                        self->tail = p;                                        \
                if (p)                                                         \
                        p->next = n;                                           \
                else                                                           \
                        self->head = n;                                        \
                if (self->free)                                                \
                        self->free(node->val);                                 \
                self->len--;                                                   \
                return COPS_OK;                                                \
        }

#define __cops_init_list_3_decl(T, NAME, SLICE_T)                              \
        __cops_init_list_2(T, NAME);                                           \
        SLICE_T *NAME##_export(NAME *self);                                    \
        int NAME##_import(NAME *self, SLICE_T *slice);

#define __cops_init_list_3_impl(T, NAME, SLICE_T)                              \
        SLICE_T *NAME##_export(NAME *self)                                     \
        {                                                                      \
                COPS_ASSERT(self && "invalid reference");                      \
                if (!self)                                                     \
                        return NULL;                                           \
                SLICE_T *slice = SLICE_T##_new(self->len);                     \
                COPS_ASSERT(slice && "failed allocation");                     \
                if (!slice)                                                    \
                        return NULL;                                           \
                NAME##_ll_node *trg = self->head;                              \
                if (self->dup) {                                               \
                        for (uint64_t i = 0; i < self->len; i++) {             \
                                COPS_ASSERT(trg && "invalid reference");       \
                                if (!trg) {                                    \
                                        SLICE_T##_free(slice);                 \
                                        return NULL;                           \
                                }                                              \
                                slice->data[i] = self->dup(trg->val);          \
                                trg = trg->next;                               \
                        }                                                      \
                } else {                                                       \
                        for (uint64_t i = 0; i < self->len; i++) {             \
                                COPS_ASSERT(trg && "invalid reference");       \
                                if (!trg) {                                    \
                                        SLICE_T##_free(slice);                 \
                                        return NULL;                           \
                                }                                              \
                                slice->data[i] = trg->val;                     \
                                trg = trg->next;                               \
                        }                                                      \
                }                                                              \
                return slice;                                                  \
        }                                                                      \
                                                                               \
        int NAME##_import(NAME *self, SLICE_T *slice)                          \
        {                                                                      \
                COPS_ASSERT(self && "invalid reference");                      \
                COPS_ASSERT(slice && "invalid reference");                     \
                if (!self || !slice)                                           \
                        return COPS_INVALID;                                   \
                if (self->dup) {                                               \
                        for (uint64_t i = 0; i < slice->len; i++) {            \
                                NAME##_ll_node *tmp =                          \
                                    COPS_ALLOC(sizeof(*tmp));                  \
                                COPS_ASSERT(tmp && "failed allocation");       \
                                if (!tmp)                                      \
                                        return COPS_MEMERR;                    \
                                *tmp = (NAME##_ll_node){                       \
                                    NULL, self->tail,                          \
                                    self->dup(slice->data[i])};                \
                                self->tail->next = tmp;                        \
                                self->tail = tmp;                              \
                                self->len++;                                   \
                        }                                                      \
                } else {                                                       \
                        for (uint64_t i = 0; i < slice->len; i++) {            \
                                NAME##_ll_node *tmp =                          \
                                    COPS_ALLOC(sizeof(*tmp));                  \
                                COPS_ASSERT(tmp && "failed allocation");       \
                                if (!tmp)                                      \
                                        return COPS_MEMERR;                    \
                                *tmp = (NAME##_ll_node){NULL, self->tail,      \
                                                        slice->data[i]};       \
                                self->tail->next = tmp;                        \
                                self->tail = tmp;                              \
                                self->len++;                                   \
                        }                                                      \
                }                                                              \
                return COPS_OK;                                                \
        }
/* end import ./src/03_list.h */
/* start import ./src/04_hset.h */
#include <stdint.h>
#define __cops_hset_choose(...)                                                \
        __cops_get_macro(__VA_ARGS__, __cops_err_arg_count,                    \
                         __cops_init_hset_3, __cops_init_hset_2,               \
                         __cops_err_arg_count, )
#define init_hset(...)                                                         \
        __cops_expand(__cops_hset_choose(__VA_ARGS__)(__VA_ARGS__))

/* RH HASHSET magic number */
#define __cops_hset_probe(pos, jmp, cap) ((pos + (jmp * jmp + jmp) / 2) % cap)
#define __cops_hset_max_cap(len, cap) (((100 * (len)) / cap) > 90)
#define __cops_hset_max_jump (1 << 6)

#if defined(COPS_IMPLEMENTATION)
#define __cops_init_hset_2(T, NAME)                                            \
        __cops_init_hset_2_decl(T, NAME) __cops_init_hset_2_impl(T, NAME)
#define __cops_init_hset_3(T, NAME, SLICE_T)                                   \
        __cops_init_hset_3_decl(T, NAME, SLICE_T)                              \
            __cops_init_hset_3_impl(T, NAME, SLICE_T)
#else
#define __cops_init_hset_2(T, NAME) __cops_init_hset_2_decl(T, NAME)
#define __cops_init_hset_3(T, NAME, SLICE_T)                                   \
        __cops_init_hset_3_decl(T, NAME, SLICE_T)
#endif /* if defined(COPS_IMPLEMENTATION) */

#define __cops_init_hset_2_decl(T, NAME)                                       \
        typedef struct NAME##_rh_node {                                        \
                uint8_t free : 1;                                              \
                uint8_t tomb : 1;                                              \
                uint8_t jumps : 6;                                             \
                T val;                                                         \
        } NAME##_rh_node;                                                      \
        typedef struct NAME {                                                  \
                uint64_t cap;                                                  \
                uint64_t len;                                                  \
                NAME##_rh_node *data;                                          \
                uint64_t (*hash)(T);                                           \
                int (*cmp)(T, T);                                              \
                void (*free)(T);                                               \
                T (*dup)(T);                                                   \
        } NAME;                                                                \
        NAME *NAME##_new(uint64_t (*hash)(T), int (*cmp)(T, T));               \
        void NAME##_free(NAME *self);                                          \
        int NAME##_hset(NAME *self, T val);                                    \
        int NAME##_add(NAME *self, T val);                                     \
        int NAME##_has(NAME *self, T val);                                     \
        int NAME##_del(NAME *self, T val);                                     \
        int NAME##_get(NAME *self, T val, T *res);

#define __cops_init_hset_2_impl(T, NAME)                                       \
                                                                               \
        NAME *NAME##_new(uint64_t (*hash)(T), int (*cmp)(T, T))                \
        {                                                                      \
                COPS_ASSERT(hash);                                             \
                COPS_ASSERT(cmp);                                              \
                if (!hash || !cmp)                                             \
                        return NULL;                                           \
                NAME *self = (NAME *)COPS_ALLOC(sizeof(*self));                \
                COPS_ASSERT(self);                                             \
                if (!self)                                                     \
                        return NULL;                                           \
                self->cap = 16;                                                \
                self->data = (NAME##_rh_node *)COPS_ALLOC(                     \
                    sizeof(NAME##_rh_node) * self->cap);                       \
                COPS_ASSERT(self->data);                                       \
                if (!self->data) {                                             \
                        COPS_FREE(self);                                       \
                        return NULL;                                           \
                }                                                              \
                for (uint64_t i = 0; i < self->cap; i++) {                     \
                        self->data[i].free = 1;                                \
                        self->data[i].tomb = 0;                                \
                        self->data[i].jumps = 0;                               \
                }                                                              \
                self->hash = hash;                                             \
                self->cmp = cmp;                                               \
                self->free = NULL;                                             \
                self->dup = NULL;                                              \
                return self;                                                   \
        }                                                                      \
                                                                               \
        void NAME##_free(NAME *self)                                           \
        {                                                                      \
                COPS_ASSERT(self);                                             \
                if (self->free) {                                              \
                        for (uint64_t i = 0; i < self->cap; i++) {             \
                                NAME##_rh_node d = self->data[i];              \
                                if (d.free || d.tomb)                          \
                                        continue;                              \
                                self->free(d.val);                             \
                        }                                                      \
                }                                                              \
                COPS_FREE(self->data);                                         \
                COPS_FREE(self);                                               \
        }                                                                      \
                                                                               \
        int NAME##_hset(NAME *self, T val)                                     \
        {                                                                      \
                COPS_ASSERT(self);                                             \
                if (!self)                                                     \
                        return COPS_INVALID;                                   \
                uint64_t pos, entry = self->hash(val) % self->cap;             \
                uint8_t i = 0;                                                 \
                while (i < __cops_hset_max_jump) {                             \
                        pos = __cops_hset_probe(entry, i, self->cap);          \
                        NAME##_rh_node *n = self->data + pos;                  \
                        COPS_ASSERT(!n->free);                                 \
                        if (n->free) {                                         \
                                return COPS_INVALID;                           \
                        }                                                      \
                        if (!n->tomb && !self->cmp(val, n->val)) {             \
                                if (self->free)                                \
                                        self->free(n->val);                    \
                                n->val = val;                                  \
                                return COPS_OK;                                \
                        }                                                      \
                        i++;                                                   \
                }                                                              \
                return COPS_INVALID;                                           \
        }                                                                      \
                                                                               \
        /* private function for simplification of reallocation*/               \
        static inline int __##NAME##_insert(NAME *self, T val)                 \
        {                                                                      \
                uint64_t pos, entry = self->hash(val) % self->cap;             \
                uint8_t jmp = 0;                                               \
                /* max jump supported */                                       \
                while (jmp < __cops_hset_max_jump) {                           \
                        pos = __cops_hset_probe(entry, jmp, self->cap);        \
                        NAME##_rh_node *n = self->data + pos;                  \
                        if (n->free || n->tomb) {                              \
                                *n = (NAME##_rh_node){0, 0, jmp, val};         \
                                self->len++;                                   \
                                return COPS_OK;                                \
                        }                                                      \
                        /* fail on duplicate */                                \
                        COPS_ASSERT(self->cmp(val, n->val));                   \
                        if (!self->cmp(val, n->val))                           \
                                return COPS_INVALID;                           \
                        /* RH swap */                                          \
                        if (n->jumps < jmp) {                                  \
                                NAME##_rh_node lazy = *n;                      \
                                *n = (NAME##_rh_node){0, 0, jmp, val};         \
                                jmp = lazy.jumps;                              \
                                val = lazy.val;                                \
                                entry = self->hash(val) % self->cap;           \
                        }                                                      \
                        jmp++;                                                 \
                }                                                              \
                return COPS_INVALID;                                           \
        }                                                                      \
                                                                               \
        int NAME##_add(NAME *self, T val)                                      \
        {                                                                      \
                COPS_ASSERT(self);                                             \
                if (!self)                                                     \
                        return COPS_INVALID;                                   \
                /* resize hashhset */                                          \
                if (__cops_hset_max_cap(self->len, self->cap)) {               \
                        self->cap *= 2;                                        \
                        NAME##_rh_node *old = self->data;                      \
                        self->data = (NAME##_rh_node *)COPS_ALLOC(             \
                            sizeof(*old) * self->cap);                         \
                        COPS_ASSERT(self->data);                               \
                        if (!self->data)                                       \
                                return COPS_MEMERR;                            \
                        for (uint64_t i = 0; i < self->cap; i++) {             \
                                self->data[i].free = 1;                        \
                                self->data[i].tomb = 0;                        \
                                self->data[i].jumps = 0;                       \
                        }                                                      \
                        self->len = 0;                                         \
                        /* iterate truh old storage */                         \
                        for (uint64_t i = 0; i < self->cap / 2; i++) {         \
                                NAME##_rh_node *n = old + i;                   \
                                if (n->free || n->tomb)                        \
                                        continue;                              \
                                int res = __##NAME##_insert(self, n->val);     \
                                if (res == COPS_OK)                            \
                                        continue;                              \
                                return res;                                    \
                        }                                                      \
                        COPS_FREE(old);                                        \
                }                                                              \
                return __##NAME##_insert(self, val);                           \
        }                                                                      \
                                                                               \
        int NAME##_has(NAME *self, T val)                                      \
        {                                                                      \
                COPS_ASSERT(self);                                             \
                if (!self)                                                     \
                        return COPS_INVALID;                                   \
                uint64_t pos, entry = self->hash(val) % self->cap;             \
                uint8_t jmp = 0;                                               \
                while (jmp < __cops_hset_max_jump) {                           \
                        pos = __cops_hset_probe(entry, jmp, self->cap);        \
                        NAME##_rh_node *n = self->data + pos;                  \
                        if (n->free)                                           \
                                return 0;                                      \
                        if (!n->tomb && !self->cmp(val, n->val))               \
                                return 1;                                      \
                        jmp++;                                                 \
                }                                                              \
                return COPS_INVALID;                                           \
        }                                                                      \
                                                                               \
        int NAME##_del(NAME *self, T val)                                      \
        {                                                                      \
                COPS_ASSERT(self);                                             \
                if (!self)                                                     \
                        return COPS_INVALID;                                   \
                uint64_t pos, entry = self->hash(val) % self->cap;             \
                uint8_t jmp = 0;                                               \
                while (jmp < __cops_hset_max_jump) {                           \
                        pos = __cops_hset_probe(entry, jmp, self->cap);        \
                        NAME##_rh_node *n = self->data + pos;                  \
                        if (n->free)                                           \
                                return COPS_INVALID;                           \
                        if (!n->tomb && !self->cmp(val, n->val)) {             \
                                n->tomb = 1;                                   \
                                if (self->free)                                \
                                        self->free(n->val);                    \
                                self->len--;                                   \
                                return COPS_OK;                                \
                        }                                                      \
                        jmp++;                                                 \
                }                                                              \
                return COPS_INVALID;                                           \
        }                                                                      \
                                                                               \
        int NAME##_get(NAME *self, T val, T *res)                              \
        {                                                                      \
                COPS_ASSERT(self);                                             \
                if (!self)                                                     \
                        return COPS_INVALID;                                   \
                uint64_t pos, entry = self->hash(val) % self->cap;             \
                uint8_t jmp = 0;                                               \
                while (jmp < __cops_hset_max_jump) {                           \
                        pos = __cops_hset_probe(entry, jmp, self->cap);        \
                        NAME##_rh_node *n = self->data + pos;                  \
                        if (n->free)                                           \
                                return COPS_INVALID;                           \
                        if (!n->tomb && !self->cmp(val, n->val)) {             \
                                if (res)                                       \
                                        *res = n->val;                         \
                                return COPS_OK;                                \
                        }                                                      \
                        jmp++;                                                 \
                }                                                              \
                return COPS_INVALID;                                           \
        }

#define __cops_init_hset_3_decl(T, NAME, SLICE_T)                              \
        __cops_init_hset_2(T, NAME);                                           \
        SLICE_T *NAME##_export(NAME *self);                                    \
        int NAME##_import(NAME *self, SLICE_T *slice);

#define __cops_init_hset_3_impl(T, NAME, SLICE_T)                              \
        SLICE_T *NAME##_export(NAME *self)                                     \
        {                                                                      \
                COPS_ASSERT(self);                                             \
                if (!self)                                                     \
                        return (SLICE_T *)NULL;                                \
                SLICE_T *slice = SLICE_T##_new(self->len);                     \
                COPS_ASSERT(slice);                                            \
                if (!slice)                                                    \
                        return slice;                                          \
                uint64_t j = 0;                                                \
                if (self->dup) {                                               \
                        for (uint64_t i = 0; i < self->cap; i++) {             \
                                NAME##_rh_node *n = self->data + i;            \
                                if (n->free || n->tomb)                        \
                                        continue;                              \
                                slice->data[j++] = self->dup(n->val);          \
                        }                                                      \
                } else {                                                       \
                        for (uint64_t i = 0; i < self->cap; i++) {             \
                                NAME##_rh_node *n = self->data + i;            \
                                if (n->free || n->tomb)                        \
                                        continue;                              \
                                slice->data[j++] = n->val;                     \
                        }                                                      \
                }                                                              \
                return slice;                                                  \
        }                                                                      \
                                                                               \
        int NAME##_import(NAME *self, SLICE_T *slice)                          \
        {                                                                      \
                COPS_ASSERT(self);                                             \
                COPS_ASSERT(slice);                                            \
                if (!self || !slice)                                           \
                        return COPS_INVALID;                                   \
                /* increase data size */                                       \
                uint64_t old_cap = self->cap;                                  \
                while (__cops_hset_max_cap(self->len + slice->len, self->cap)) \
                        self->cap *= 2;                                        \
                NAME##_rh_node *old = self->data;                              \
                self->data =                                                   \
                    (NAME##_rh_node *)COPS_ALLOC(sizeof(*old) * self->cap);    \
                COPS_ASSERT(self->data);                                       \
                if (!self->data)                                               \
                        return COPS_MEMERR;                                    \
                for (uint64_t i = 0; i < self->cap; i++) {                     \
                        self->data[i].free = 1;                                \
                        self->data[i].tomb = 0;                                \
                        self->data[i].jumps = 0;                               \
                }                                                              \
                self->len = 0;                                                 \
                for (uint64_t i = 0; i < old_cap; i++) {                       \
                        NAME##_rh_node *n = old + i;                           \
                        if (n->free || n->tomb)                                \
                                continue;                                      \
                        int res = __##NAME##_insert(self, n->val);             \
                        if (res == COPS_OK)                                    \
                                continue;                                      \
                        return res;                                            \
                }                                                              \
                COPS_FREE(old);                                                \
                if (self->dup) {                                               \
                        for (uint64_t i = 0; i < slice->len; i++) {            \
                                T val = self->dup(slice->data[i]);             \
                                int res = __##NAME##_insert(self, val);        \
                                if (res == COPS_OK)                            \
                                        continue;                              \
                        }                                                      \
                } else {                                                       \
                        for (uint64_t i = 0; i < slice->len; i++) {            \
                                T val = slice->data[i];                        \
                                int res = __##NAME##_insert(self, val);        \
                                if (res == COPS_OK)                            \
                                        continue;                              \
                        }                                                      \
                }                                                              \
                SLICE_T##_free(slice);                                         \
                return COPS_OK;                                                \
        }

/* utility hash function to pick */

#if defined(COPS_IMPLEMENTATION)
uint64_t djb2(char *str, uint64_t len)
{
        uint64_t hash = 5381;
        int c;
        uint64_t i = 0;
        while ((c = *str++) && i < len) {
                hash = ((hash << 5) + hash) + c;
                i++;
        }
        return hash;
}
uint64_t hash64shift(uint64_t key)
{
        key = (~key) + (key << 21);
        key = key ^ (key >> 24);
        key = (key + (key << 3)) + (key << 8);
        key = key ^ (key >> 14);
        key = (key + (key << 2)) + (key << 4);
        key = key ^ (key >> 28);
        key = key + (key << 31);
        return key;
}
#else
// mixin string bounded
uint64_t djb2(char *str, uint64_t len);
// mixin integer (and casted float/double)
uint64_t hash64shift(uint64_t key);
#endif /* if defined(COPS_IMPLEMENTATION) */
/* end import ./src/04_hset.h */
/* start import ./src/05_tset.h */
#define __cops_tset_choose(...)                                                \
        __cops_get_macro(__VA_ARGS__, __cops_err_arg_count,                    \
                         __cops_init_tset_3, __cops_init_tset_2,               \
                         __cops_err_arg_count, )
#define init_tset(...)                                                         \
        __cops_expand(__cops_tset_choose(__VA_ARGS__)(__VA_ARGS__))

#if defined(COPS_IMPLEMENTATION)
#define __cops_init_tset_2(T, NAME)                                            \
        __cops_init_tset_2_decl(T, NAME) __cops_init_tset_2_impl(T, NAME)
#define __cops_init_tset_3(T, NAME, SLICE_T)                                   \
        __cops_init_tset_3_decl(T, NAME, SLICE_T)                              \
            __cops_init_tset_3_impl(T, NAME, SLICE_T)
#else
#define __cops_init_tset_2(T, NAME) __cops_init_tset_2_decl(T, NAME)
#define __cops_init_tset_3(T, NAME, SLICE_T)                                   \
        __cops_init_tset_3_decl(T, NAME, SLICE_T)
#endif /* if defined(COPS_IMPLEMENTATION) */

#define __cops_init_tset_2_impl(NAME, T)                                       \
        typedef struct NAME##_rb_node NAME##_rb_node;                          \
        struct NAME##_rb_node {                                                \
                struct NAME##_rb_node *parent, *left, *right;                  \
                uint8_t isred;                                                 \
                T val;                                                         \
        };                                                                     \
        typedef struct NAME {                                                  \
                uint32_t len;                                                  \
                NAME##_rb_node *root;                                          \
                int (*cmp)(T, T);                                              \
                void (*free)(T);                                               \
                T (*dup)(T);                                                   \
        } NAME;                                                                \
        NAME *NAME##_new(int (*cmp)(T, T));                                    \
        void NAME##_free(NAME *self);                                          \
        int NAME##_add(NAME *self, T val);                                     \
        int NAME##_has(NAME *self, T val);                                     \
        int NAME##_del(NAME *self, T *val);

#define __cops_init_tset_2_decl(NAME, T)                                       \
                                                                               \
        NAME *NAME##_new(int (*cmp)(T, T))                                     \
        {                                                                      \
                NAME *self = COPS_ALLOC(sizeof(*self));                        \
                COPS_ASSERT(self && "failed allocation");                      \
                if (!self)                                                     \
                        return NULL;                                           \
                self->cmp = cmp;                                               \
                self->len = 0;                                                 \
                self->root = NULL;                                             \
                self->free = NULL;                                             \
                self->dup = NULL;                                              \
                return self;                                                   \
        }                                                                      \
                                                                               \
        void NAME##_free(NAME *self)                                           \
        {                                                                      \
                COPS_ASSERT(self && "invalid reference");                      \
                if (self->len) {                                               \
                        NAME##_rb_node *n = self->root;                        \
                        while (n) {                                            \
                                if (n->left) {                                 \
                                        n = n->left;                           \
                                } else if (n->right) {                         \
                                        n = n->right;                          \
                                } else {                                       \
                                        NAME##_rb_node *p = n->parent;         \
                                        if (p && p->left == n)                 \
                                                p->left = NULL;                \
                                        if (p && p->right == n)                \
                                                p->right = NULL;               \
                                        if (self->free)                        \
                                                self->free(p->val);            \
                                        COPS_FREE(n);                          \
                                        n = p;                                 \
                                }                                              \
                        }                                                      \
                }                                                              \
                self->root = NULL;                                             \
                COPS_FREE(self);                                               \
                return;                                                        \
        }                                                                      \
                                                                               \
        /* rotate node to became right child of his left child*/               \
        static inline int NAME##_node_rotate_right(NAME *self,                 \
                                                   NAME##_rb_node *p)          \
        {                                                                      \
                COPS_ASSERT(self && "invalid reference");                      \
                COPS_ASSERT(p && "invalid reference");                         \
                COPS_ASSERT(p->left && "empty reference");                     \
                if (!self || !p)                                               \
                        return COPS_INVALID;                                   \
                NAME##_rb_node *x = p->left, *g = p->parent;                   \
                if (!x)                                                        \
                        return COPS_INVALID; /* link parent and child*/        \
                x->parent = g;                                                 \
                if (!g)                                                        \
                        self->root = x;                                        \
                else if (g->left == p)                                         \
                        g->left = x;                                           \
                else if (g->right == p)                                        \
                        g->right = x;                                          \
                /* link child.right and node*/                                 \
                p->left = x->right;                                            \
                if (x->right)                                                  \
                        x->right->parent = p;                                  \
                /* link parent and child*/                                     \
                p->parent = x;                                                 \
                x->right = p;                                                  \
                return COPS_OK;                                                \
        }                                                                      \
                                                                               \
        /* rotate node to became left child of his right child*/               \
        static inline int NAME##_node_rotate_left(NAME *self,                  \
                                                  NAME##_rb_node *p)           \
        {                                                                      \
                COPS_ASSERT(self && "invalid reference");                      \
                COPS_ASSERT(p && "invalid reference");                         \
                COPS_ASSERT(p->right && "empty reference");                    \
                if (!self || !p)                                               \
                        return COPS_INVALID;                                   \
                NAME##_rb_node *x = p->right, *g = p->parent;                  \
                if (!x)                                                        \
                        return COPS_INVALID;                                   \
                /* link parent and child*/                                     \
                x->parent = g;                                                 \
                if (!g)                                                        \
                        self->root = x;                                        \
                else if (g->left == p)                                         \
                        g->left = x;                                           \
                else if (g->right == p)                                        \
                        g->right = x;                                          \
                /* link child.left and node*/                                  \
                p->right = x->left;                                            \
                if (x->left)                                                   \
                        x->left->parent = p;                                   \
                /* link parent and child*/                                     \
                p->parent = x;                                                 \
                x->left = p;                                                   \
                return COPS_OK;                                                \
        }                                                                      \
                                                                               \
        int NAME##_add(NAME *self, T val)                                      \
        {                                                                      \
                COPS_ASSERT(self && "invalid reference");                      \
                if (!self)                                                     \
                        return COPS_INVALID;                                   \
                NAME##_rb_node *e = COPS_ALLOC(sizeof(*e));                    \
                COPS_ASSERT(e && "failed allocation");                         \
                if (!e)                                                        \
                        return COPS_MEMERR;                                    \
                *e = (NAME##_rb_node){NULL, NULL, NULL, 1, val};               \
                /* insert*/                                                    \
                if (!self->root) {                                             \
                        self->root = e;                                        \
                        e->isred = 0;                                          \
                        self->len++;                                           \
                        return COPS_OK;                                        \
                }                                                              \
                NAME##_rb_node *n = self->root;                                \
                while (1) {                                                    \
                        int diff = self->cmp(key, n->key);                     \
                        if (!diff) {                                           \
                                COPS_FREE(e);                                  \
                                return COPS_INVALID;                           \
                        } else if (diff > 0) {                                 \
                                if (!n->right) {                               \
                                        e->parent = n;                         \
                                        n->right = e;                          \
                                        break;                                 \
                                }                                              \
                                n = n->right;                                  \
                        } else {                                               \
                                if (!n->left) {                                \
                                        e->parent = n;                         \
                                        n->left = e;                           \
                                        break;                                 \
                                }                                              \
                                n = n->left;                                   \
                        }                                                      \
                }                                                              \
                self->len++;                                                   \
                n = NULL;                                                      \
                /* balance*/                                                   \
                NAME##_rb_node *p, *u, *g, *x = e;                             \
                while (1) {                                                    \
                        /* init variables*/                                    \
                        p = x->parent;                                         \
                        g = u = NULL;                                          \
                        if (p)                                                 \
                                g = p->parent;                                 \
                        if (g)                                                 \
                                u = g->left == p ? g->right : g->left;         \
                        /* check cases*/                                       \
                        if (!p || !p->isred || !g)                             \
                                break;                                         \
                        /* from here we assume g is black and p is red*/       \
                        if (u && u->isred) {                                   \
                                /* repaint chunk*/                             \
                                p->isred = u->isred = 0;                       \
                                g->isred = 1;                                  \
                                /* reposition NAMEs*/                          \
                                x = g;                                         \
                                continue;                                      \
                        }                                                      \
                        /* from here we assume u is black or null*/            \
                        if (g->left == p) {                                    \
                                /* LR rot.*/                                   \
                                if (p->right == x) {                           \
                                        NAME##_node_rotate_left(self, p);      \
                                        x = p;                                 \
                                        p = x->parent;                         \
                                }                                              \
                                /* LL rot.*/                                   \
                                NAME##_node_rotate_right(self, g);             \
                                p->isred = 0;                                  \
                                g->isred = 1;                                  \
                                break;                                         \
                        } else {                                               \
                                /* RL rot.*/                                   \
                                if (p->left == x) {                            \
                                        NAME##_node_rotate_right(self, p);     \
                                        x = p;                                 \
                                        p = x->parent;                         \
                                }                                              \
                                /* RR rot.*/                                   \
                                NAME##_node_rotate_left(self, g);              \
                                p->isred = 0;                                  \
                                g->isred = 1;                                  \
                                break;                                         \
                        }                                                      \
                }                                                              \
                /* repaint root*/                                              \
                self->root->isred = 0;                                         \
                return COPS_OK;                                                \
        }                                                                      \
                                                                               \
        int NAME##_has(NAME *self, T val)                                      \
        {                                                                      \
                COPS_ASSERT(self && "invalid reference");                      \
                if (!self)                                                     \
                        return COPS_INVALID;                                   \
                if (!self->cmp)                                                \
                        return COPS_INVALID;                                   \
                NAME##_rb_node *n = self->root;                                \
                while (n) {                                                    \
                        int cmp = self->cmp(val, n->val);                      \
                        if (!cmp)                                              \
                                return COPS_OK;                                \
                        else if (cmp > 0)                                      \
                                n = n->right;                                  \
                        else                                                   \
                                n = n->left;                                   \
                }                                                              \
                return COPS_INVALID;                                           \
        }                                                                      \
                                                                               \
        int NAME##_del(NAME *self, T *val)                                     \
        {                                                                      \
                COPS_ASSERT(self && "invalid reference");                      \
                if (!self)                                                     \
                        return COPS_INVALID;                                   \
                NAME##_rb_node *x = self->root;                                \
                while (x) {                                                    \
                        int cmp = self->cmp(*val, x->val);                     \
                        if (!cmp)                                              \
                                break;                                         \
                        else if (cmp > 0)                                      \
                                x = x->right;                                  \
                        else                                                   \
                                x = x->left;                                   \
                }                                                              \
                if (!x)                                                        \
                        return COPS_INVALID;                                   \
                if (val)                                                       \
                        *val = x->val;     /* target found*/                   \
                NAME##_rb_node *n = x;     /* *n -> double black or target*/   \
                if (x->left && x->right) { /* leftmost right children*/        \
                        n = n->right;                                          \
                        while (n->left) {                                      \
                                n = n->left;                                   \
                        }                                                      \
                        T v = x->val;                                          \
                        x->val = n->val;                                       \
                        n->val = v;                                            \
                }                                                              \
                x = NULL;                      /* only one children (n)*/      \
                NAME##_rb_node *p = n->parent; /* *p -> n parent*/             \
                if (n->left || n->right) {                                     \
                        NAME##_rb_node *oc =                                   \
                            n->left ? n->left                                  \
                                    : n->right; /* *oc -> only child*/         \
                        if (!oc->isred) {                                      \
                                return COPS_ABORT;                             \
                        } else if (!p) {                                       \
                                self->root = oc;                               \
                        } else if (p->left == n) {                             \
                                p->left = oc;                                  \
                        } else if (p->right == n) {                            \
                                p->right = oc;                                 \
                        } /* parent (n) children is only child*/               \
                        oc->parent = p;                                        \
                        oc->isred = 0;                                         \
                        COPS_FREE(n);                                          \
                        self->len--;                                           \
                        return COPS_OK;                                        \
                } /* no children*/                                             \
                if (!p) {                                                      \
                        self->len--;                                           \
                        self->root = NULL;                                     \
                        COPS_FREE(n);                                          \
                        return COPS_OK;                                        \
                }                                                              \
                NAME##_rb_node *s = p->left == n ? p->right : p->left;         \
                NAME##_rb_node *cn, *dn;                                       \
                if (p->left == n)                                              \
                        p->left = NULL;                                        \
                else                                                           \
                        p->right = NULL;                                       \
                volatile int n_is_red = n->isred;                              \
                COPS_FREE(n);                                                  \
                n = NULL;                                                      \
                self->len--;                                                   \
                if (n_is_red)                                                  \
                        return COPS_OK; /* black node*/                        \
                while (1) {                                                    \
                        if (n)                                                 \
                                p = n->parent;                                 \
                        if (!p)                                                \
                                break; /* no root*/                            \
                        int n_is_right = p->right == n ? 1 : 0;                \
                        if (n_is_right) {                                      \
                                s = p->left;                                   \
                                cn = s->right;                                 \
                                dn = s->left;                                  \
                        } else {                                               \
                                s = p->right;                                  \
                                cn = s->left;                                  \
                                dn = s->right;                                 \
                        } /* resetted variable*/                               \
                        if (s && s->isred) {                                   \
                                if (n_is_right)                                \
                                        NAME##_node_rotate_right(self, p);     \
                                else                                           \
                                        NAME##_node_rotate_left(self, p);      \
                                s->isred = 0;                                  \
                                p->isred = 1;                                  \
                                continue;                                      \
                        } /* sibling is black*/                                \
                        if (cn && cn->isred) {                                 \
                                if (n_is_right)                                \
                                        NAME##_node_rotate_left(self, s);      \
                                else                                           \
                                        NAME##_node_rotate_right(self, s);     \
                                cn->isred = 0;                                 \
                                s->isred = 1;                                  \
                                continue;                                      \
                        } /* close nephew is black*/                           \
                        if (dn && dn->isred) {                                 \
                                if (n_is_right)                                \
                                        NAME##_node_rotate_right(self, p);     \
                                else                                           \
                                        NAME##_node_rotate_left(self, p);      \
                                s->isred = p->isred;                           \
                                p->isred = 0;                                  \
                                dn->isred = 0;                                 \
                                break;                                         \
                        } /* distant nephew is black*/                         \
                        if (p && p->isred) {                                   \
                                p->isred = 0;                                  \
                                s->isred = 1;                                  \
                                break;                                         \
                        } /* parent is black*/                                 \
                        s->isred = 1;                                          \
                        n = p;                                                 \
                }                                                              \
                return COPS_OK;                                                \
        }
/* end import ./src/05_tset.h */
/* start import ./src/06_pool.h */

#if defined(COPS_IMPLEMENTATION)
#define __cops_init_pool(T, NAME)                                              \
        __cops_init_pool_decl(T, NAME) __cops_init_pool_impl(T, NAME)
#else
#define __cops_init_pool(T, NAME) __cops_init_pool_decl(T, NAME)
#endif /* if defined(COPS_IMPLEMENTATION) */

#define __cops_init_pool_decl(T, NAME)
#define __cops_init_pool_impl(T, NAME)
/* end import ./src/06_pool.h */
/* start import ./src/vec.h */
#define COPS_VEC_H

#ifdef __cplusplus
extern "C" {
#endif

#include "core.h"

#define __init_cops_vec(name, T)                                               \
        typedef struct name {                                                  \
                uint32_t rc;                                                   \
                uint32_t nelem;                                                \
                uint32_t cap;                                                  \
                void (*free)(T);                                               \
                T (*dup)(T);                                                   \
                T *data;                                                       \
        } name;                                                                \
                                                                               \
        static inline name *name##_new()                                       \
        {                                                                      \
                name *self =                                                   \
                    (name *)cops_default_allocator.alloc(sizeof(*self));       \
                if (!self)                                                     \
                        return NULL;                                           \
                self->rc = 1;                                                  \
                self->nelem = 0;                                               \
                self->cap = 1 << 3;                                            \
                self->free = NULL;                                             \
                if ((unsigned long)(self->cap) >= SIZE_MAX / sizeof(T)) {      \
                        cops_default_allocator.free(self);                     \
                        return NULL;                                           \
                }                                                              \
                self->data = (T *)cops_default_allocator.alloc(                \
                    sizeof(*self->data) * (size_t)self->cap);                  \
                if (!self->data) {                                             \
                        cops_default_allocator.free(self);                     \
                        return NULL;                                           \
                }                                                              \
                return self;                                                   \
        }                                                                      \
                                                                               \
        static inline name *name##_dup(name *self)                             \
        {                                                                      \
                if (self)                                                      \
                        self->rc++;                                            \
                return self;                                                   \
        }                                                                      \
                                                                               \
        static inline name *name##_free(name *self)                            \
        {                                                                      \
                if (self && self->rc > 0 && !(--self->rc)) {                   \
                        if (self->free) {                                      \
                                for (size_t i = 0; i < self->nelem; i++) {     \
                                        self->free(self->data[i]);             \
                                }                                              \
                        }                                                      \
                        cops_default_allocator.free(self->data);               \
                        cops_default_allocator.free(self);                     \
                }                                                              \
                return NULL;                                                   \
        }                                                                      \
                                                                               \
        static inline int name##_push(name *self, T val)                       \
        {                                                                      \
                if (!self || !self->data)                                      \
                        return COPS_INVALID;                                   \
                if (self->nelem == (uint32_t)self->cap) {                      \
                        if ((unsigned long)(2 * self->cap) >                   \
                            (SIZE_MAX / sizeof(T))) {                          \
                                return -2;                                     \
                        }                                                      \
                        T *old = self->data;                                   \
                        T *new = cops_default_allocator.alloc(                 \
                            sizeof(T) * (size_t)(2 * self->cap));              \
                        if (!new)                                              \
                                return COPS_MEMERR;                            \
                        memcpy(new, old, (size_t)self->cap);                   \
                        cops_default_allocator.free(old);                      \
                        memset(new + self->cap, 0,                             \
                               sizeof(T) * (size_t)self->cap);                 \
                        self->cap *= 2;                                        \
                        self->data = new;                                      \
                }                                                              \
                T *trg = self->data + self->nelem++;                           \
                memcpy(trg, &val, sizeof(T));                                  \
                return COPS_OK;                                                \
        }                                                                      \
                                                                               \
        static inline int name##_pop(name *self, T *res)                       \
        {                                                                      \
                if (!self || !self->data || !self->nelem)                      \
                        return COPS_INVALID;                                   \
                T *trg = self->data + --self->nelem;                           \
                if (res)                                                       \
                        memcpy(res, trg, sizeof(T));                           \
                if (self->free)                                                \
                        self->free(*trg);                                      \
                memset(trg, 0, sizeof(T));                                     \
                return COPS_OK;                                                \
        }                                                                      \
                                                                               \
        static inline int name##_del(name *self, size_t pos, T *res)           \
        {                                                                      \
                if (!self || !self->data || !self->nelem)                      \
                        return COPS_INVALID;                                   \
                if (pos == self->nelem - 1)                                    \
                        return name##_pop(self, res);                          \
                if (pos >= self->nelem)                                        \
                        return COPS_INVALID;                                   \
                T *trg = &(self->data[pos]);                                   \
                if (res)                                                       \
                        memcpy(res, trg, sizeof(T));                           \
                if (self->free)                                                \
                        self->free(*trg);                                      \
                self->data[pos] = self->data[--self->nelem];                   \
                trg = &(self->data[self->nelem]);                              \
                memset(trg, 0, sizeof(T));                                     \
                return COPS_OK;                                                \
        }                                                                      \
                                                                               \
        static inline int name##_set(name *self, uint32_t pos, T val)          \
        {                                                                      \
                if (!self || !self->data || pos > self->nelem)                 \
                        return COPS_INVALID;                                   \
                if (pos == self->nelem)                                        \
                        return name##_push(self, val);                         \
                T *trg = self->data + pos;                                     \
                if (self->free)                                                \
                        self->free(*trg);                                      \
                *trg = val;                                                    \
                return COPS_OK;                                                \
        }                                                                      \
                                                                               \
        static inline int name##_get(name *self, uint32_t pos, T *res)         \
        {                                                                      \
                if (!self || !self->data || pos >= self->nelem)                \
                        return COPS_INVALID;                                   \
                T *trg = self->data + pos;                                     \
                if (res)                                                       \
                        *res = *trg;                                           \
                return COPS_OK;                                                \
        }                                                                      \
                                                                               \
        static inline int name##_import(name *self, const name *oth)           \
        {                                                                      \
                if (!self || !self->data || !oth || !oth->data)                \
                        return COPS_INVALID;                                   \
                if (self->nelem > UINT32_MAX - oth->nelem)                     \
                        return COPS_INVALID;                                   \
                uint32_t new_cap = self->cap;                                  \
                while (self->nelem + oth->nelem > new_cap)                     \
                        new_cap *= 2;                                          \
                if (new_cap != self->cap) {                                    \
                        T *old = self->data;                                   \
                        T *new = cops_default_allocator.alloc(new_cap);        \
                        if (!new)                                              \
                                return COPS_MEMERR;                            \
                        memcpy(new, old, self->cap);                           \
                        cops_default_allocator.free(old);                      \
                        memset(new + self->cap, 0,                             \
                               (size_t)(new_cap - self->cap) * sizeof(T));     \
                        self->data = new;                                      \
                        self->cap = new_cap;                                   \
                }                                                              \
                if (self->dup) {                                               \
                        for (size_t i = 0; i < oth->nelem; i++) {              \
                                self->data[self->nelem + i] =                  \
                                    self->dup(oth->data[i]);                   \
                        }                                                      \
                } else {                                                       \
                        memcpy(self->data + self->nelem, oth->data,            \
                               sizeof(T) * (size_t)oth->nelem);                \
                }                                                              \
                self->nelem += oth->nelem;                                     \
                return COPS_OK;                                                \
        }

#define init_cops_vec(T) __init_cops_vec(cops_##T##_vec, T)

#ifdef __cplusplus
}
#endif

#endif /* ifndef COPS_VEC_H */
/* end import ./src/vec.h */
#endif /* #ifndef COPS_H guard */
