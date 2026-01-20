#include "01_core.h"
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
