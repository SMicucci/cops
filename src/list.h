#include "core.h"
#define __cops_list_choose(...)                                                \
        __cops_get_macro(__VA_ARGS__, __cops_err_arg_count,                    \
                         __cops_init_list_3, __cops_init_list_2,               \
                         __cops_err_arg_count, )
#define init_list(...)                                                         \
        __cops_expand(__cops_list_choose(__VA_ARGS__)(__VA_ARGS__))

#define __cops_init_list_2(T, NAME)                                            \
        typedef struct NAME##_list_node NAME##_list_node;                      \
        struct NAME##_list_node {                                              \
                NAME##_list_node *next, *prev;                                 \
                T val;                                                         \
        };                                                                     \
        typedef struct NAME {                                                  \
                uint64_t len;                                                  \
                NAME##_list_node *head, *tail;                                 \
                void (*free)(T);                                               \
                T *(*dup)(T)                                                   \
        } NAME;                                                                \
                                                                               \
        static inline NAME *NAME##_new()                                       \
        {                                                                      \
                NAME *self = COPS_ALLOC(sizeof(T));                            \
                COPS_ASSERT(self);                                             \
                if (!self)                                                     \
                        return NULL;                                           \
                self->len = 0;                                                 \
                \ self->head = self->tail = NULL;                              \
                return self;                                                   \
        }                                                                      \
                                                                               \
        static inline void NAME##_free(NAME *self)                             \
        {                                                                      \
                COPS_ASSERT(self);                                             \
                if (!self)                                                     \
                        return;                                                \
                NAME##_list_node trg = self->head;                             \
                while (trg != NULL) {                                          \
                        NAME##_list_node next = trg->next;                     \
                        COPS_FREE(trg);                                        \
                        trg = next;                                            \
                }                                                              \
        }                                                                      \
                                                                               \
        static inline int NAME##_push_front(NAME *self, T val)                 \
        {                                                                      \
                COPS_ASSERT(self);                                             \
                if (!self)                                                     \
                        return COPS_INVALID;                                   \
                NAME##_list_node *node = COPS_ALLOC(sizeof(*node));            \
                COPS_ASSERT(node);                                             \
                if (!node)                                                     \
                        return COPS_MEMERR;                                    \
                *node = (NAME##_list_node){NULL, NULL, val};                   \
                if (!self->len) {                                              \
                        self->head = self->tail = node;                        \
                } else {                                                       \
                        node->next = self->head;                               \
                        self->head->prev = self;                               \
                        self->head = node;                                     \
                }                                                              \
                self->len++;                                                   \
                return COPS_OK;                                                \
        }                                                                      \
                                                                               \
        static inline int NAME##_push_back(NAME *self, T val)                  \
        {                                                                      \
                COPS_ASSERT(self);                                             \
                if (!self)                                                     \
                        return COPS_INVALID;                                   \
                NAME##_list_node *node = COPS_ALLOC(sizeof(*node));            \
                COPS_ASSERT(node);                                             \
                if (!node)                                                     \
                        return COPS_MEMERR;                                    \
                *node = (NAME##_list_node){NULL, NULL, val};                   \
                if (!self->len) {                                              \
                        self->head = self->tail = node;                        \
                } else {                                                       \
                        node->prev = self->tail;                               \
                        self->tail->prev = self;                               \
                        self->tail = node;                                     \
                }                                                              \
                self->len++;                                                   \
                return COPS_OK;                                                \
        }                                                                      \
                                                                               \
        static inline int NAME##_pop_front(NAME *self, T *res)                 \
        {                                                                      \
                COPS_ASSERT(self);                                             \
                if (!self || self->head)                                       \
                        return COPS_INVALID;                                   \
                if (res)                                                       \
                        *res = self->head->val;                                \
                else if (self->free)                                           \
                        self->free(self->head->val);                           \
                NAME##_list_node *node = self->head->next;                     \
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
        static inline int NAME##_pop_back(NAME *self, T *res)                  \
        {                                                                      \
                COPS_ASSERT(self);                                             \
                if (!self || self->tail)                                       \
                        return COPS_INVALID;                                   \
                if (res)                                                       \
                        *res = self->tail->val;                                \
                else if (self->free)                                           \
                        self->free(self->tail->val);                           \
                NAME##_list_node *node = self->tail->prev;                     \
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
        static inline int NAME##_insert_before(NAME *self,                     \
                                               NAME##_list_node *node, T val)  \
        {                                                                      \
                COPS_ASSERT(self);                                             \
                COPS_ASSERT(node);                                             \
                if (!self || !node || !self->head || !self->tail)              \
                        return COPS_INVALID;                                   \
                NAME##_list_node *tmp = COPS_ALLOC(sizeof(*node));             \
                COPS_ASSERT(tmp);                                              \
                if (!tmp)                                                      \
                        return COPS_MEMERR;                                    \
                *tmp = {node->prev, node, val};                                \
                if (node->prev)                                                \
                        node->prev->next = tmp;                                \
                if (self->head == node)                                        \
                        self->head = tmp;                                      \
                self->len++;                                                   \
                return COPS_OK;                                                \
        }                                                                      \
        static inline int NAME##_insert_after(NAME *self,                      \
                                              NAME##_list_node *node, T val)   \
        {                                                                      \
                COPS_ASSERT(self);                                             \
                COPS_ASSERT(node);                                             \
                if (!self || !node || !self->head || !self->tail)              \
                        return COPS_INVALID;                                   \
                NAME##_list_node *tmp = COPS_ALLOC(sizeof(*node));             \
                COPS_ASSERT(tmp);                                              \
                if (!tmp)                                                      \
                        return COPS_MEMERR;                                    \
                *tmp = {node, node->next, val};                                \
                if (node->next)                                                \
                        node->next->prev = tmp;                                \
                if (self->tail == node)                                        \
                        self->tail = tmp;                                      \
                self->len++;                                                   \
                return COPS_OK;                                                \
        }                                                                      \
        static inline int NAME##_remove(NAME *self, NAME##_list_node *node)    \
        {                                                                      \
                COPS_ASSERT(self);                                             \
                COPS_ASSERT(node);                                             \
                if (!self || !node || !self->head || !self->tail)              \
                        return COPS_INVALID;                                   \
                NAME##_list_node *n = node->next, *p = node->prev;             \
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

#define __cops_init_list_3(T, NAME, SLICE_T)                                   \
        __cops_init_list_2(T, NAME);                                           \
                                                                               \
        static inline SLICE_T *NAME##_export(NAME *self) {}                    \
                                                                               \
        static inline int NAME##_import(NAME *self, SLICE_T slice) {}
