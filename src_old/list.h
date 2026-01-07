#ifndef COPS_LIST_H
#define COPS_LIST_H

#ifdef __cplusplus
extern "C" {
#endif

#include "core.h"

#define __init_cops_list(name, T)                                                                  \
        typedef struct name##_node {                                                               \
                struct name##_node *next, *prev;                                                   \
                T val;                                                                             \
        } name##_node;                                                                             \
                                                                                                   \
        typedef struct name {                                                                      \
                uint32_t nelem;                                                                    \
                uint32_t rc;                                                                       \
                name##_node *head, *tail;                                                          \
                void (*free)(T);                                                                   \
        } name;                                                                                    \
                                                                                                   \
        static inline name *name##_new()                                                           \
        {                                                                                          \
                name *self = (name *)cops_default_allocator.alloc(sizeof(*self));                  \
                self->nelem = 0;                                                                   \
                self->rc = 1;                                                                      \
                self->head = NULL;                                                                 \
                self->tail = NULL;                                                                 \
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
                        name##_node *n = self->head;                                               \
                        while (n) {                                                                \
                                name##_node *next = n->next;                                       \
                                if (self->free)                                                    \
                                        self->free(n->val);                                        \
                                cops_default_allocator.free((void *)n);                            \
                                n = next;                                                          \
                        }                                                                          \
                        cops_default_allocator.free((void *)self);                                 \
                }                                                                                  \
                return NULL;                                                                       \
        }                                                                                          \
                                                                                                   \
        static inline int name##_push_front(name *self, T val)                                     \
        {                                                                                          \
                if (!self)                                                                         \
                        return COPS_INVALID;                                                       \
                name##_node *n = (name##_node *)cops_default_allocator.alloc(sizeof(*n));          \
                if (!n)                                                                            \
                        return COPS_MEMERR;                                                        \
                *n = (name##_node){NULL, NULL, val};                                               \
                if (!self->head) {                                                                 \
                        self->head = self->tail = n;                                               \
                } else {                                                                           \
                        self->head->prev = n;                                                      \
                        n->next = self->head;                                                      \
                        self->head = n;                                                            \
                }                                                                                  \
                self->nelem++;                                                                     \
                return COPS_OK;                                                                    \
        }                                                                                          \
                                                                                                   \
        static inline int name##_push_back(name *self, T val)                                      \
        {                                                                                          \
                if (!self)                                                                         \
                        return COPS_INVALID;                                                       \
                name##_node *n = (name##_node *)cops_default_allocator.alloc(sizeof(*n));          \
                if (!n)                                                                            \
                        return COPS_MEMERR;                                                        \
                *n = (name##_node){NULL, NULL, val};                                               \
                if (!self->tail) {                                                                 \
                        self->head = self->tail = n;                                               \
                } else {                                                                           \
                        self->tail->next = n;                                                      \
                        n->prev = self->tail;                                                      \
                        self->tail = n;                                                            \
                }                                                                                  \
                self->nelem++;                                                                     \
                return COPS_OK;                                                                    \
        }                                                                                          \
                                                                                                   \
        static inline int name##_pop_front(name *self, T *val)                                     \
        {                                                                                          \
                if (!self || !self->head)                                                          \
                        return COPS_INVALID;                                                       \
                if (val)                                                                           \
                        *val = self->head->val;                                                    \
                else if (self->free)                                                               \
                        self->free(self->head->val);                                               \
                name##_node *n = self->head->next;                                                 \
                if (n)                                                                             \
                        n->prev = NULL;                                                            \
                else                                                                               \
                        self->tail = NULL;                                                         \
                cops_default_allocator.free((void *)self->head);                                   \
                self->head = n;                                                                    \
                self->nelem--;                                                                     \
                return COPS_OK;                                                                    \
        }                                                                                          \
                                                                                                   \
        static inline int name##_pop_back(name *self, T *val)                                      \
        {                                                                                          \
                if (!self || !self->tail)                                                          \
                        return COPS_INVALID;                                                       \
                if (val) {                                                                         \
                        *val = self->tail->val;                                                    \
                } else if (self->free)                                                             \
                        self->free(self->tail->val);                                               \
                name##_node *n = self->tail->prev;                                                 \
                if (n)                                                                             \
                        n->next = NULL;                                                            \
                else                                                                               \
                        self->head = NULL;                                                         \
                cops_default_allocator.free((void *)self->tail);                                   \
                self->tail = n;                                                                    \
                self->nelem--;                                                                     \
                return COPS_OK;                                                                    \
        }                                                                                          \
                                                                                                   \
        static inline int name##_add_next(name *self, name##_node *node, T val)                    \
        {                                                                                          \
                if (!self || !node || !self->head || !self->tail)                                  \
                        return COPS_INVALID;                                                       \
                /*name##_node *t=self->head;int v=0;while(t &&                                     \
                 * !v){if(t==node)v=1;t=t->next;}if(!v)return -1;*/                                \
                name##_node *n = (name##_node *)cops_default_allocator.alloc(sizeof(*n));          \
                if (!n)                                                                            \
                        return COPS_MEMERR;                                                        \
                /* (node) <-> (n) <-> (node->next) */                                              \
                *n = (name##_node){node, node->next, val};                                         \
                if (node->next)                                                                    \
                        node->next->prev = n;                                                      \
                node->next = n;                                                                    \
                if (self->tail == node)                                                            \
                        self->tail = n;                                                            \
                self->nelem++;                                                                     \
                return COPS_OK;                                                                    \
        }                                                                                          \
                                                                                                   \
        static inline int name##_add_prev(name *self, name##_node *node, T val)                    \
        {                                                                                          \
                if (!self || !node || !self->head || !self->tail)                                  \
                        return COPS_INVALID;                                                       \
                /*name##_node *t=self->head;int v=0;while(t &&                                     \
                 * !v){if(t==node)v=1;t=t->next;}if(!v)return -1;*/                                \
                name##_node *n = (name##_node *)cops_default_allocator.alloc(sizeof(*n));          \
                if (!n)                                                                            \
                        return COPS_MEMERR;                                                        \
                /* (node->prev) <-> (n) <-> (node) */                                              \
                *n = (name##_node){node->prev, node, val};                                         \
                if (node->prev)                                                                    \
                        node->prev->next = n;                                                      \
                node->prev = n;                                                                    \
                if (self->head == node)                                                            \
                        self->head = n;                                                            \
                self->nelem++;                                                                     \
                return COPS_OK;                                                                    \
        }                                                                                          \
                                                                                                   \
        static inline int name##_del(name *self, name##_node *node)                                \
        {                                                                                          \
                if (!self || !node || !self->head || !self->tail)                                  \
                        return COPS_INVALID;                                                       \
                /*name##_node *t=self->head;int v=0;while(t &&                                     \
                 * !v){if(t==node)v=1;t=t->next;}if(!v)return -1;*/                                \
                name##_node *n = node->next, *p = node->prev;                                      \
                if (n)                                                                             \
                        n->prev = p;                                                               \
                else                                                                               \
                        self->tail = p;                                                            \
                if (p)                                                                             \
                        p->next = n;                                                               \
                else                                                                               \
                        self->head = n;                                                            \
                if (self->free)                                                                    \
                        self->free(node->val);                                                     \
                cops_default_allocator.free((void *)node);                                         \
                self->nelem--;                                                                     \
                return COPS_OK;                                                                    \
        }

#define init_cops_list(T) __init_cops_list(cops_##T##_list, T)

#ifdef __cplusplus
}
#endif

#endif /* end of include guard: COPS_LIST_H */
