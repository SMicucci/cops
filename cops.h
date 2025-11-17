#ifndef COPS_H
#define COPS_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 *
 * COPS CORE
 *
 */

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define COPS_OK ((int)0)
#define COPS_INVALID ((int)-1)
#define COPS_ABORT ((int)-2)
#define COPS_MEMERR ((int)-3)

/**
 * basic string hash function
 */
static inline size_t djb2(const char *s)
{
        size_t hash = 5381;
        int c;
        while ((c = *s++))
                hash = ((hash << 5) + hash) + c;
        return hash;
}

static inline size_t longhash(unsigned long l)
{
        size_t x = (size_t)l;
        x = (~x) + (x << 21);
        x = x ^ (x >> 24);
        x = (x + (x << 3)) + (x << 8);
        x = x ^ (x >> 14);
        x = (x + (x << 2)) + (x << 4);
        x = x + (x << 28);
        x = x + (x << 31);
        return x;
}

static inline void *cops_alloc(size_t size) { return calloc(1, size); }

/**
 * rewritable allocation in macros:
 *
 * - alloc:   zeroed new memory slot
 * - free:    release memory
 */
typedef struct cops_allocator {
        void *(*alloc)(size_t);
        void (*free)(void *);
} cops_allocator;

// default allocator used in all the macros
static cops_allocator cops_default_allocator = {cops_alloc, free};

#ifdef __init_cops_list
#error "'__init_cops_list' is a reserved word for cops library"
#endif
#ifdef init_cops_list
#error "'init_cops_list' is a reserved word for cops library"
#endif
#ifdef __init_cops_arr
#error "'__init_cops_arr' is a reserved word for cops library"
#endif
#ifdef init_cops_arr
#error "'init_cops_arr' is a reserved word for cops library"
#endif
#ifdef __init_cops_vec
#error "'__init_cops_vec' is a reserved word for cops library"
#endif
#ifdef init_cops_vec
#error "'init_cops_vec' is a reserved word for cops library"
#endif
#ifdef __init_cops_map
#error "'__init_cops_map' is a reserved word for cops library"
#endif
#ifdef init_cops_map
#error "'init_cops_map' is a reserved word for cops library"
#endif
#ifdef __init_cops_set
#error "'__init_cops_set' is a reserved word for cops library"
#endif
#ifdef init_cops_set
#error "'init_cops_set' is a reserved word for cops library"
#endif
#ifdef __init_cops_omap
#error "'__init_cops_omap' is a reserved word for cops library"
#endif
#ifdef init_cops_omap
#error "'init_cops_omap' is a reserved word for cops library"
#endif
#ifdef __init_cops_oset
#error "'__init_cops_oset' is a reserved word for cops library"
#endif
#ifdef init_cops_oset
#error "'init_cops_oset' is a reserved word for cops library"
#endif

/**
 *
 * COPS_ARR
 *
 */
#define __init_cops_arr(name, T)                                               \
        typedef struct name {                                                  \
                uint32_t len;                                                  \
                uint32_t rc;                                                   \
                T data[];                                                      \
        } name;                                                                \
        static inline name *name##_new(uint32_t len)                           \
        {                                                                      \
                if (len > (uint32_t)((SIZE_MAX - sizeof(name)) / sizeof(T)))   \
                        return NULL;                                           \
                name *self = (name *)cops_default_allocator.alloc(             \
                    sizeof(*self) + sizeof(T) * len);                          \
                if (!self)                                                     \
                        return NULL;                                           \
                self->len = len;                                               \
                self->rc = 1;                                                  \
                memset(self->data, 0, sizeof(T) * len);                        \
                return self;                                                   \
        }                                                                      \
        static inline name *name##_dup(name *self)                             \
        {                                                                      \
                if (self)                                                      \
                        self->rc++;                                            \
                return self;                                                   \
        }                                                                      \
        static inline name *name##_free(name *self)                            \
        {                                                                      \
                if (self && self->rc > 0 && !(--self->rc)) {                   \
                        cops_default_allocator.free(self);                     \
                }                                                              \
                return NULL;                                                   \
        }

#define init_cops_arr(T) __init_cops_arr(cops_##T##_arr, T)

/**
 *
 * COPS_VEC
 *
 */

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

/**
 *
 * COPS_LIST
 *
 */

#define __init_cops_list(name, T)                                              \
        typedef struct name##_node {                                           \
                struct name##_node *next, *prev;                               \
                T val;                                                         \
        } name##_node;                                                         \
                                                                               \
        typedef struct name {                                                  \
                uint32_t nelem;                                                \
                uint32_t rc;                                                   \
                name##_node *head, *tail;                                      \
                void (*free)(T);                                               \
        } name;                                                                \
                                                                               \
        static inline name *name##_new()                                       \
        {                                                                      \
                name *self =                                                   \
                    (name *)cops_default_allocator.alloc(sizeof(*self));       \
                self->nelem = 0;                                               \
                self->rc = 1;                                                  \
                self->head = NULL;                                             \
                self->tail = NULL;                                             \
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
                        name##_node *n = self->head;                           \
                        while (n) {                                            \
                                name##_node *next = n->next;                   \
                                if (self->free)                                \
                                        self->free(n->val);                    \
                                cops_default_allocator.free((void *)n);        \
                                n = next;                                      \
                        }                                                      \
                        cops_default_allocator.free((void *)self);             \
                }                                                              \
                return NULL;                                                   \
        }                                                                      \
                                                                               \
        static inline int name##_push_front(name *self, T val)                 \
        {                                                                      \
                if (!self)                                                     \
                        return COPS_INVALID;                                   \
                name##_node *n =                                               \
                    (name##_node *)cops_default_allocator.alloc(sizeof(*n));   \
                if (!n)                                                        \
                        return COPS_MEMERR;                                    \
                *n = (name##_node){NULL, NULL, val};                           \
                if (!self->head) {                                             \
                        self->head = self->tail = n;                           \
                } else {                                                       \
                        self->head->prev = n;                                  \
                        n->next = self->head;                                  \
                        self->head = n;                                        \
                }                                                              \
                self->nelem++;                                                 \
                return COPS_OK;                                                \
        }                                                                      \
                                                                               \
        static inline int name##_push_back(name *self, T val)                  \
        {                                                                      \
                if (!self)                                                     \
                        return COPS_INVALID;                                   \
                name##_node *n =                                               \
                    (name##_node *)cops_default_allocator.alloc(sizeof(*n));   \
                if (!n)                                                        \
                        return COPS_MEMERR;                                    \
                *n = (name##_node){NULL, NULL, val};                           \
                if (!self->tail) {                                             \
                        self->head = self->tail = n;                           \
                } else {                                                       \
                        self->tail->next = n;                                  \
                        n->prev = self->tail;                                  \
                        self->tail = n;                                        \
                }                                                              \
                self->nelem++;                                                 \
                return COPS_OK;                                                \
        }                                                                      \
                                                                               \
        static inline int name##_pop_front(name *self, T *val)                 \
        {                                                                      \
                if (!self || !self->head)                                      \
                        return COPS_INVALID;                                   \
                if (val)                                                       \
                        *val = self->head->val;                                \
                else if (self->free)                                           \
                        self->free(self->head->val);                           \
                name##_node *n = self->head->next;                             \
                if (n)                                                         \
                        n->prev = NULL;                                        \
                else                                                           \
                        self->tail = NULL;                                     \
                cops_default_allocator.free((void *)self->head);               \
                self->head = n;                                                \
                self->nelem--;                                                 \
                return COPS_OK;                                                \
        }                                                                      \
                                                                               \
        static inline int name##_pop_back(name *self, T *val)                  \
        {                                                                      \
                if (!self || !self->tail)                                      \
                        return COPS_INVALID;                                   \
                if (val) {                                                     \
                        *val = self->tail->val;                                \
                } else if (self->free)                                         \
                        self->free(self->tail->val);                           \
                name##_node *n = self->tail->prev;                             \
                if (n)                                                         \
                        n->next = NULL;                                        \
                else                                                           \
                        self->head = NULL;                                     \
                cops_default_allocator.free((void *)self->tail);               \
                self->tail = n;                                                \
                self->nelem--;                                                 \
                return COPS_OK;                                                \
        }                                                                      \
                                                                               \
        static inline int name##_add_next(name *self, name##_node *node,       \
                                          T val)                               \
        {                                                                      \
                if (!self || !node || !self->head || !self->tail)              \
                        return COPS_INVALID;                                   \
                /*name##_node *t=self->head;int v=0;while(t &&                 \
                 * !v){if(t==node)v=1;t=t->next;}if(!v)return -1;*/            \
                name##_node *n =                                               \
                    (name##_node *)cops_default_allocator.alloc(sizeof(*n));   \
                if (!n)                                                        \
                        return COPS_MEMERR;                                    \
                /* (node) <-> (n) <-> (node->next) */                          \
                *n = (name##_node){node, node->next, val};                     \
                if (node->next)                                                \
                        node->next->prev = n;                                  \
                node->next = n;                                                \
                if (self->tail == node)                                        \
                        self->tail = n;                                        \
                self->nelem++;                                                 \
                return COPS_OK;                                                \
        }                                                                      \
                                                                               \
        static inline int name##_add_prev(name *self, name##_node *node,       \
                                          T val)                               \
        {                                                                      \
                if (!self || !node || !self->head || !self->tail)              \
                        return COPS_INVALID;                                   \
                /*name##_node *t=self->head;int v=0;while(t &&                 \
                 * !v){if(t==node)v=1;t=t->next;}if(!v)return -1;*/            \
                name##_node *n =                                               \
                    (name##_node *)cops_default_allocator.alloc(sizeof(*n));   \
                if (!n)                                                        \
                        return COPS_MEMERR;                                    \
                /* (node->prev) <-> (n) <-> (node) */                          \
                *n = (name##_node){node->prev, node, val};                     \
                if (node->prev)                                                \
                        node->prev->next = n;                                  \
                node->prev = n;                                                \
                if (self->head == node)                                        \
                        self->head = n;                                        \
                self->nelem++;                                                 \
                return COPS_OK;                                                \
        }                                                                      \
                                                                               \
        static inline int name##_del(name *self, name##_node *node)            \
        {                                                                      \
                if (!self || !node || !self->head || !self->tail)              \
                        return COPS_INVALID;                                   \
                /*name##_node *t=self->head;int v=0;while(t &&                 \
                 * !v){if(t==node)v=1;t=t->next;}if(!v)return -1;*/            \
                name##_node *n = node->next, *p = node->prev;                  \
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
                cops_default_allocator.free((void *)node);                     \
                self->nelem--;                                                 \
                return COPS_OK;                                                \
        }

#define init_cops_list(T) __init_cops_list(cops_##T##_list, T)

/**
 *
 * COPS_MAP
 *
 */

#define __init_cops_map(name, K, V)                                            \
        typedef struct name##_node {                                           \
                uint8_t flag;                                                  \
                K key;                                                         \
                V val;                                                         \
        } name##_node;                                                         \
                                                                               \
        typedef struct name {                                                  \
                uint32_t rc;                                                   \
                uint32_t cap;                                                  \
                uint32_t nelem;                                                \
                name##_node *data;                                             \
                size_t (*hash)(K);                                             \
                int (*cmp)(K, K);                                              \
                void (*free_key)(K);                                           \
                void (*free_val)(V);                                           \
                K (*dup_key)(K);                                               \
                V (*dup_val)(V);                                               \
        } name;                                                                \
                                                                               \
        static inline name *name##_new(size_t (*hash)(K), int (*cmp)(K, K))    \
        {                                                                      \
                if (!hash || !cmp)                                             \
                        return NULL;                                           \
                if (16 > SIZE_MAX / sizeof(name##_node))                       \
                        return NULL;                                           \
                name *self =                                                   \
                    (name *)cops_default_allocator.alloc(sizeof(*self));       \
                if (!self)                                                     \
                        return NULL;                                           \
                self->cap = 16;                                                \
                self->rc = 1;                                                  \
                self->data = (name##_node *)cops_default_allocator.alloc(      \
                    sizeof(*self->data) * self->cap);                          \
                if (!self->data) {                                             \
                        cops_default_allocator.free(self);                     \
                        return NULL;                                           \
                }                                                              \
                memset(self->data, 0, sizeof(*self->data) * self->cap);        \
                for (size_t i = 0; i < self->cap; i++) {                       \
                        self->data[i].flag = 0x80;                             \
                }                                                              \
                self->hash = hash;                                             \
                self->cmp = cmp;                                               \
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
                        if (self->free_key || self->free_val) {                \
                                for (size_t i = 0; i < self->cap; i++) {       \
                                        name##_node d = self->data[i];         \
                                        if (d.flag >= 0x40)                    \
                                                continue;                      \
                                        if (self->free_key)                    \
                                                self->free_key(d.key);         \
                                        if (self->free_val)                    \
                                                self->free_val(d.val);         \
                                }                                              \
                        }                                                      \
                        cops_default_allocator.free(self->data);               \
                        cops_default_allocator.free(self);                     \
                }                                                              \
                return NULL;                                                   \
        }                                                                      \
                                                                               \
        static inline int name##_set(name *self, K key, V val)                 \
        {                                                                      \
                if (!self || !self->data || !self->hash || !self->cmp)         \
                        return COPS_INVALID;                                   \
                size_t pos, entry = self->hash(key) % self->cap;               \
                uint8_t i = 0;                                                 \
                while (i < 0x40) {                                             \
                        pos = (entry + (i * i + i) / 2) % self->cap;           \
                        name##_node *n = self->data + pos;                     \
                        if (n->flag & 0x80) {                                  \
                                return COPS_INVALID;                           \
                        }                                                      \
                        if (n->flag < 0x40 && !self->cmp(key, n->key)) {       \
                                if (self->free_val)                            \
                                        self->free_val(n->val);                \
                                n->val = val;                                  \
                                return COPS_OK;                                \
                        }                                                      \
                        i++;                                                   \
                }                                                              \
                return COPS_INVALID;                                           \
        }                                                                      \
                                                                               \
        static inline int name##_add(name *self, K key, V val)                 \
        {                                                                      \
                if (!self || !self->data || !self->hash || !self->cmp)         \
                        return COPS_INVALID;                                   \
                if (((100 * self->nelem) / self->cap) > 90) {                  \
                        int res = 0;                                           \
                        size_t cap = self->cap * 2;                            \
                        name##_node *old = self->data;                         \
                        name##_node *data =                                    \
                            (name##_node *)cops_default_allocator.alloc(       \
                                sizeof(*data) * cap);                          \
                        if (!data)                                             \
                                return COPS_MEMERR;                            \
                        memset(data, 0, sizeof(*data) * cap);                  \
                        for (size_t i = 0; i < cap; i++) {                     \
                                data[i].flag = 0x80;                           \
                        }                                                      \
                        self->data = data;                                     \
                        self->cap = cap;                                       \
                        self->nelem = 0;                                       \
                        for (size_t i = 0; i < cap / 2; i++) {                 \
                                if (res)                                       \
                                        return res;                            \
                                name##_node *n = old + i;                      \
                                if (n->flag != 0x80) {                         \
                                        res =                                  \
                                            name##_add(self, n->key, n->val);  \
                                }                                              \
                        }                                                      \
                        cops_default_allocator.free(old);                      \
                }                                                              \
                size_t pos, entry = self->hash(key) % self->cap;               \
                uint8_t i = 0;                                                 \
                while (i < 0x40) {                                             \
                        pos = (entry + (i * i + i) / 2) % self->cap;           \
                        name##_node *n = self->data + pos;                     \
                        if (n->flag >= 0x40) {                                 \
                                *n = (name##_node){i, key, val};               \
                                self->nelem++;                                 \
                                return COPS_OK;                                \
                        }                                                      \
                        if (n->flag < 0x40 && !self->cmp(key, n->key)) {       \
                                return COPS_INVALID;                           \
                        }                                                      \
                        if (n->flag < i) {                                     \
                                name##_node next = *n;                         \
                                *n = (name##_node){i, key, val};               \
                                i = next.flag;                                 \
                                val = next.val;                                \
                                key = next.key;                                \
                                entry = self->hash(key) % self->cap;           \
                        }                                                      \
                        i++;                                                   \
                }                                                              \
                return COPS_INVALID;                                           \
        }                                                                      \
                                                                               \
        static inline int name##_has(name *self, K key)                        \
        {                                                                      \
                if (!self || !self->data || !self->hash || !self->cmp)         \
                        return COPS_INVALID;                                   \
                size_t pos, entry = self->hash(key) % self->cap;               \
                uint8_t i = 0;                                                 \
                while (i < 0x40) {                                             \
                        pos = (entry + (i * i + i) / 2) % self->cap;           \
                        name##_node *n = self->data + pos;                     \
                        if (n->flag & 0x80)                                    \
                                return 0;                                      \
                        if (n->flag < 0x40 && !self->cmp(key, n->key))         \
                                return 1;                                      \
                        i++;                                                   \
                }                                                              \
                return COPS_INVALID;                                           \
        }                                                                      \
                                                                               \
        static inline int name##_del(name *self, K key, V *val)                \
        {                                                                      \
                if (!self || !self->data || !self->hash || !self->cmp)         \
                        return COPS_INVALID;                                   \
                size_t pos, entry = self->hash(key) % self->cap;               \
                uint8_t i = 0;                                                 \
                while (i < 0x40) {                                             \
                        pos = (entry + (i * i + i) / 2) % self->cap;           \
                        name##_node *n = self->data + pos;                     \
                        if (n->flag & 0x80)                                    \
                                return COPS_INVALID;                           \
                        if (n->flag < 0x40 && !self->cmp(key, n->key)) {       \
                                n->flag = 0x40;                                \
                                if (val)                                       \
                                        *val = n->val;                         \
                                if (self->free_key)                            \
                                        self->free_key(n->key);                \
                                if (self->free_val)                            \
                                        self->free_val(n->val);                \
                                self->nelem--;                                 \
                                return COPS_OK;                                \
                        }                                                      \
                        i++;                                                   \
                }                                                              \
                return COPS_INVALID;                                           \
        }                                                                      \
                                                                               \
        static inline int name##_get(name *self, K key, V *val)                \
        {                                                                      \
                if (!self || !self->data || !self->hash || !self->cmp)         \
                        return COPS_INVALID;                                   \
                size_t pos, entry = self->hash(key) % self->cap;               \
                uint8_t i = 0;                                                 \
                while (i < 0x40) {                                             \
                        pos = (entry + (i * i + i) / 2) % self->cap;           \
                        name##_node *n = self->data + pos;                     \
                        if (n->flag < 0x40 && !self->cmp(key, n->key)) {       \
                                if (val)                                       \
                                        *val = n->val;                         \
                                return COPS_OK;                                \
                        }                                                      \
                        i++;                                                   \
                }                                                              \
                return COPS_INVALID;                                           \
        }                                                                      \
                                                                               \
        static inline int name##_import(name *self, name *oth)                 \
        {                                                                      \
                if (self == oth)                                               \
                        return COPS_INVALID;                                   \
                if (!self || !self->data || !self->hash || !self->cmp)         \
                        return COPS_INVALID;                                   \
                if (!oth || !oth->data || !oth->hash || !oth->cmp)             \
                        return COPS_INVALID;                                   \
                uint32_t cap = self->cap;                                      \
                uint32_t len = self->nelem;                                    \
                for (size_t i = 0; i < oth->cap; i++) {                        \
                        name##_node *n = oth->data + i;                        \
                        if (n->flag < 0x40) {                                  \
                                if (!name##_has(self, n->key))                 \
                                        len++;                                 \
                        }                                                      \
                }                                                              \
                while (((100 * (len)) / cap) > 90)                             \
                        cap *= 2;                                              \
                if (cap != self->cap) {                                        \
                        uint32_t o_cap = self->cap;                            \
                        name##_node *old = self->data;                         \
                        name##_node *new =                                     \
                            (name##_node *)cops_default_allocator.alloc(       \
                                sizeof(*new) * cap);                           \
                        if (!new)                                              \
                                return COPS_MEMERR;                            \
                        memset(new, 0, sizeof(*new) * cap);                    \
                        for (size_t i = 0; i < cap; i++) {                     \
                                new[i].flag = 0x80;                            \
                        }                                                      \
                        self->data = new;                                      \
                        self->cap = cap;                                       \
                        self->nelem = 0;                                       \
                        for (size_t i = 0; i < o_cap; i++) {                   \
                                int res;                                       \
                                name##_node *n = old + i;                      \
                                if (n->flag < 0x40) {                          \
                                        res =                                  \
                                            name##_add(self, n->key, n->val);  \
                                }                                              \
                                if (res)                                       \
                                        return res;                            \
                        }                                                      \
                        cops_default_allocator.free(old);                      \
                }                                                              \
                for (size_t i = 0; i < oth->cap; i++) {                        \
                        name##_node *n = oth->data + i;                        \
                        if (n->flag < 0x40) {                                  \
                                K key = n->key;                                \
                                V val = n->val;                                \
                                if (self->dup_val)                             \
                                        val = self->dup_val(n->val);           \
                                if (name##_has(self, key) == 1) {              \
                                        name##_set(self, key, val);            \
                                } else {                                       \
                                        if (self->dup_key)                     \
                                                key = self->dup_key(n->key);   \
                                        name##_add(self, key, val);            \
                                }                                              \
                        }                                                      \
                }                                                              \
                return COPS_OK;                                                \
        }

#define init_cops_map(K, V) __init_cops_map(cops_##K##_##V##_map, K, V)

/**
 *
 * COPS_SET
 *
 */

#define __init_cops_set(name, T)                                               \
        typedef struct name##_node {                                           \
                uint8_t flag;                                                  \
                T val;                                                         \
        } name##_node;                                                         \
                                                                               \
        typedef struct name {                                                  \
                uint32_t rc;                                                   \
                uint32_t cap;                                                  \
                uint32_t nelem;                                                \
                name##_node *data;                                             \
                size_t (*hash)(T);                                             \
                int (*cmp)(T, T);                                              \
                void (*free)(T);                                               \
                T (*dup)(T);                                                   \
        } name;                                                                \
                                                                               \
        static inline name *name##_new(size_t (*hash)(T), int (*cmp)(T, T))    \
        {                                                                      \
                if (!hash || !cmp)                                             \
                        return NULL;                                           \
                if (16 > SIZE_MAX / sizeof(name##_node))                       \
                        return NULL;                                           \
                name *self =                                                   \
                    (name *)cops_default_allocator.alloc(sizeof(*self));       \
                if (!self)                                                     \
                        return NULL;                                           \
                self->cap = 16;                                                \
                self->rc = 1;                                                  \
                self->data = (name##_node *)cops_default_allocator.alloc(      \
                    sizeof(name##_node) * self->cap);                          \
                if (!self->data) {                                             \
                        cops_default_allocator.free(self);                     \
                        return NULL;                                           \
                }                                                              \
                memset(self->data, 0, sizeof(*self->data) * self->cap);        \
                for (size_t i = 0; i < self->cap; i++) {                       \
                        self->data[i].flag = 0x80;                             \
                }                                                              \
                self->hash = hash;                                             \
                self->cmp = cmp;                                               \
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
                                for (size_t i = 0; i < self->cap; i++) {       \
                                        name##_node d = self->data[i];         \
                                        if (d.flag >= 0x40)                    \
                                                continue;                      \
                                        self->free(d.val);                     \
                                }                                              \
                        }                                                      \
                        cops_default_allocator.free(self->data);               \
                        cops_default_allocator.free(self);                     \
                }                                                              \
                return NULL;                                                   \
        }                                                                      \
                                                                               \
        static inline int name##_set(name *self, T val)                        \
        {                                                                      \
                if (!self || !self->data || !self->hash || !self->cmp)         \
                        return COPS_INVALID;                                   \
                size_t pos, entry = self->hash(val) % self->cap;               \
                uint8_t i = 0;                                                 \
                while (i < 0x40) {                                             \
                        pos = (entry + (i * i + i) / 2) % self->cap;           \
                        name##_node *n = self->data + pos;                     \
                        if (n->flag & 0x80) {                                  \
                                return COPS_INVALID;                           \
                        }                                                      \
                        if (n->flag < 0x40 && !self->cmp(val, n->val)) {       \
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
        static inline int name##_add(name *self, T val)                        \
        {                                                                      \
                if (!self || !self->data || !self->hash || !self->cmp)         \
                        return COPS_INVALID;                                   \
                if (((100 * self->nelem) / self->cap) > 90) {                  \
                        int res = 0;                                           \
                        size_t cap = self->cap * 2;                            \
                        name##_node *old = self->data;                         \
                        name##_node *data =                                    \
                            (name##_node *)cops_default_allocator.alloc(       \
                                sizeof(*data) * cap);                          \
                        if (!data)                                             \
                                return COPS_MEMERR;                            \
                        memset(data, 0, sizeof(*data) * cap);                  \
                        for (size_t i = 0; i < cap; i++) {                     \
                                data[i].flag = 0x80;                           \
                        }                                                      \
                        self->data = data;                                     \
                        self->cap = cap;                                       \
                        self->nelem = 0;                                       \
                        for (size_t i = 0; i < cap / 2; i++) {                 \
                                if (res)                                       \
                                        return res;                            \
                                name##_node *n = old + i;                      \
                                if (n->flag != 0x80) {                         \
                                        res = name##_add(self, n->val);        \
                                }                                              \
                        }                                                      \
                        cops_default_allocator.free(old);                      \
                }                                                              \
                size_t pos, entry = self->hash(val) % self->cap;               \
                uint8_t i = 0;                                                 \
                while (i < 0x40) {                                             \
                        pos = (entry + (i * i + i) / 2) % self->cap;           \
                        name##_node *n = self->data + pos;                     \
                        if (n->flag >= 0x40) {                                 \
                                *n = (name##_node){i, val};                    \
                                self->nelem++;                                 \
                                return COPS_OK;                                \
                        }                                                      \
                        if (n->flag < 0x40 && !self->cmp(val, n->val)) {       \
                                return COPS_INVALID;                           \
                        }                                                      \
                        if (n->flag < i) {                                     \
                                name##_node next = *n;                         \
                                *n = (name##_node){i, val};                    \
                                i = next.flag;                                 \
                                val = next.val;                                \
                                entry = self->hash(val) % self->cap;           \
                        }                                                      \
                        i++;                                                   \
                }                                                              \
                return COPS_INVALID;                                           \
        }                                                                      \
                                                                               \
        static inline int name##_has(name *self, T val)                        \
        {                                                                      \
                if (!self || !self->data || !self->hash || !self->cmp)         \
                        return COPS_INVALID;                                   \
                size_t pos, entry = self->hash(val) % self->cap;               \
                uint8_t i = 0;                                                 \
                while (i < 0x40) {                                             \
                        pos = (entry + (i * i + i) / 2) % self->cap;           \
                        name##_node *n = self->data + pos;                     \
                        if (n->flag & 0x80)                                    \
                                return 0;                                      \
                        if (n->flag < 0x40 && !self->cmp(val, n->val))         \
                                return 1;                                      \
                        i++;                                                   \
                }                                                              \
                return COPS_INVALID;                                           \
        }                                                                      \
                                                                               \
        static inline int name##_del(name *self, T val)                        \
        {                                                                      \
                if (!self || !self->data || !self->hash || !self->cmp)         \
                        return COPS_INVALID;                                   \
                size_t pos, entry = self->hash(val) % self->cap;               \
                uint8_t i = 0;                                                 \
                while (i < 0x40) {                                             \
                        pos = (entry + (i * i + i) / 2) % self->cap;           \
                        name##_node *n = self->data + pos;                     \
                        if (n->flag & 0x80)                                    \
                                return COPS_INVALID;                           \
                        if (n->flag < 0x40 && !self->cmp(val, n->val)) {       \
                                n->flag = 0x40;                                \
                                if (self->free)                                \
                                        self->free(n->val);                    \
                                self->nelem--;                                 \
                                return COPS_OK;                                \
                        }                                                      \
                        i++;                                                   \
                }                                                              \
                return COPS_INVALID;                                           \
        }                                                                      \
                                                                               \
        static inline int name##_get(name *self, T val, T *old)                \
        {                                                                      \
                if (!self || !self->data || !self->hash || !self->cmp)         \
                        return COPS_INVALID;                                   \
                size_t pos, entry = self->hash(val) % self->cap;               \
                uint8_t i = 0;                                                 \
                while (i < 0x40) {                                             \
                        pos = (entry + (i * i + i) / 2) % self->cap;           \
                        name##_node *n = self->data + pos;                     \
                        if (n->flag < 0x40 && !self->cmp(val, n->val)) {       \
                                if (old)                                       \
                                        *old = n->val;                         \
                                return COPS_OK;                                \
                        }                                                      \
                        i++;                                                   \
                }                                                              \
                return COPS_INVALID;                                           \
        }                                                                      \
                                                                               \
        static inline int name##_import(name *self, name *oth)                 \
        {                                                                      \
                if (self == oth)                                               \
                        return COPS_INVALID;                                   \
                if (!self || !self->data || !self->hash || !self->cmp)         \
                        return COPS_INVALID;                                   \
                if (!oth || !oth->data || !oth->hash || !oth->cmp)             \
                        return COPS_INVALID;                                   \
                uint32_t cap = self->cap;                                      \
                uint32_t len = self->nelem;                                    \
                for (size_t i = 0; i < oth->cap; i++) {                        \
                        name##_node *n = oth->data + i;                        \
                        if (n->flag < 0x40) {                                  \
                                if (!name##_has(self, n->val))                 \
                                        len++;                                 \
                        }                                                      \
                }                                                              \
                while (((100 * (len)) / cap) > 90)                             \
                        cap *= 2;                                              \
                if (cap != self->cap) {                                        \
                        uint32_t o_cap = self->cap;                            \
                        name##_node *old = self->data;                         \
                        name##_node *new =                                     \
                            (name##_node *)cops_default_allocator.alloc(       \
                                sizeof(*new) * cap);                           \
                        if (!new)                                              \
                                return COPS_MEMERR;                            \
                        memset(new, 0, sizeof(*new) * cap);                    \
                        for (size_t i = 0; i < cap; i++) {                     \
                                new[i].flag = 0x80;                            \
                        }                                                      \
                        self->data = new;                                      \
                        self->cap = cap;                                       \
                        self->nelem = 0;                                       \
                        for (size_t i = 0; i < o_cap; i++) {                   \
                                int res;                                       \
                                name##_node *n = old + i;                      \
                                if (n->flag < 0x40) {                          \
                                        res = name##_add(self, n->val);        \
                                }                                              \
                                if (res)                                       \
                                        return res;                            \
                        }                                                      \
                        cops_default_allocator.free(old);                      \
                }                                                              \
                for (size_t i = 0; i < oth->cap; i++) {                        \
                        name##_node *n = oth->data + i;                        \
                        if (n->flag < 0x40) {                                  \
                                T val = n->val;                                \
                                if (self->dup)                                 \
                                        val = self->dup(n->val);               \
                                if (name##_add(self, val))                     \
                                        name##_set(self, n->val);              \
                        }                                                      \
                }                                                              \
                return COPS_OK;                                                \
        }

#define init_cops_set(T) __init_cops_set(cops_##T##_set, T)

/**
 *
 * COPS_OMAP
 *
 */

#define __init_cops_omap(name, K, V)                                           \
                                                                               \
        typedef struct name##_node {                                           \
                struct name##_node *parent, *left, *right;                     \
                uint8_t isred;                                                 \
                K key;                                                         \
                V val;                                                         \
        } name##_node;                                                         \
                                                                               \
        typedef struct name {                                                  \
                uint32_t nelem;                                                \
                uint32_t rc;                                                   \
                name##_node *root;                                             \
                int (*cmp)(K, K);                                              \
        } name;                                                                \
                                                                               \
        static inline name *name##_new(int (*cmp)(K, K))                       \
        {                                                                      \
                name *self =                                                   \
                    (name *)cops_default_allocator.alloc(sizeof(*self));       \
                if (!self)                                                     \
                        return NULL;                                           \
                self->cmp = cmp;                                               \
                self->nelem = 0;                                               \
                self->rc = 1;                                                  \
                self->root = NULL;                                             \
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
                        if (self->nelem) {                                     \
                                name##_node *n = self->root;                   \
                                while (n) {                                    \
                                        if (n->left) {                         \
                                                n = n->left;                   \
                                        } else if (n->right) {                 \
                                                n = n->right;                  \
                                        } else {                               \
                                                name##_node *p = n->parent;    \
                                                if (p && p->left == n)         \
                                                        p->left = NULL;        \
                                                if (p && p->right == n)        \
                                                        p->right = NULL;       \
                                                cops_default_allocator.free(   \
                                                    n);                        \
                                                n = p;                         \
                                        }                                      \
                                }                                              \
                        }                                                      \
                        self->root = NULL;                                     \
                        cops_default_allocator.free(self);                     \
                }                                                              \
                return NULL;                                                   \
        }                                                                      \
                                                                               \
        /* rotate node to became right child of his left child*/               \
        static inline int name##_node_rotate_right(name *self, name##_node *p) \
        {                                                                      \
                if (!self || !p)                                               \
                        return COPS_INVALID;                                   \
                name##_node *x = p->left, *g = p->parent;                      \
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
        static inline int name##_node_rotate_left(name *self, name##_node *p)  \
        {                                                                      \
                if (!self || !p)                                               \
                        return COPS_INVALID;                                   \
                name##_node *x = p->right, *g = p->parent;                     \
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
        static inline int name##_add(name *self, K key, V val)                 \
        {                                                                      \
                if (!self)                                                     \
                        return COPS_INVALID;                                   \
                if (!self->cmp)                                                \
                        return COPS_INVALID; /* init node*/                    \
                name##_node *e =                                               \
                    (name##_node *)cops_default_allocator.alloc(sizeof(*e));   \
                if (!e)                                                        \
                        return COPS_MEMERR;                                    \
                *e = (name##_node){NULL, NULL, NULL, 1, key, val};             \
                /* insert*/                                                    \
                if (!self->root) {                                             \
                        self->root = e;                                        \
                        e->isred = 0;                                          \
                        self->nelem++;                                         \
                        return COPS_OK;                                        \
                }                                                              \
                name##_node *n = self->root;                                   \
                while (1) {                                                    \
                        int diff = self->cmp(key, n->key);                     \
                        if (!diff) {                                           \
                                cops_default_allocator.free(e);                \
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
                self->nelem++;                                                 \
                n = NULL;                                                      \
                /* balance*/                                                   \
                name##_node *p, *u, *g, *x = e;                                \
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
                                /* reposition names*/                          \
                                x = g;                                         \
                                continue;                                      \
                        }                                                      \
                        /* from here we assume u is black or null*/            \
                        if (g->left == p) {                                    \
                                /* LR rot.*/                                   \
                                if (p->right == x) {                           \
                                        name##_node_rotate_left(self, p);      \
                                        x = p;                                 \
                                        p = x->parent;                         \
                                }                                              \
                                /* LL rot.*/                                   \
                                name##_node_rotate_right(self, g);             \
                                p->isred = 0;                                  \
                                g->isred = 1;                                  \
                                break;                                         \
                        } else {                                               \
                                /* RL rot.*/                                   \
                                if (p->left == x) {                            \
                                        name##_node_rotate_right(self, p);     \
                                        x = p;                                 \
                                        p = x->parent;                         \
                                }                                              \
                                /* RR rot.*/                                   \
                                name##_node_rotate_left(self, g);              \
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
        static inline int name##_has(name *self, K key, V *val)                \
        {                                                                      \
                if (!self)                                                     \
                        return COPS_INVALID;                                   \
                if (!self->cmp)                                                \
                        return COPS_INVALID;                                   \
                name##_node *n = self->root;                                   \
                while (n) {                                                    \
                        int cmp = self->cmp(key, n->key);                      \
                        if (!cmp) {                                            \
                                if (val)                                       \
                                        *val = n->val;                         \
                                return COPS_OK;                                \
                        } else if (cmp > 0) {                                  \
                                n = n->right;                                  \
                        } else {                                               \
                                n = n->left;                                   \
                        }                                                      \
                }                                                              \
                return COPS_INVALID;                                           \
        }                                                                      \
                                                                               \
        static inline int name##_mod(name *self, K key, V val)                 \
        {                                                                      \
                if (!self)                                                     \
                        return COPS_INVALID;                                   \
                if (!self->cmp)                                                \
                        return COPS_INVALID;                                   \
                name##_node *n = self->root;                                   \
                while (n) {                                                    \
                        int cmp = self->cmp(key, n->key);                      \
                        if (!cmp) {                                            \
                                n->val = val;                                  \
                                return COPS_OK;                                \
                        } else if (cmp > 0) {                                  \
                                n = n->right;                                  \
                        } else {                                               \
                                n = n->left;                                   \
                        }                                                      \
                }                                                              \
                return COPS_INVALID;                                           \
        }                                                                      \
                                                                               \
        static inline int name##_del(name *self, K key, V *val)                \
        {                                                                      \
                if (!self)                                                     \
                        return COPS_INVALID;                                   \
                if (!self->cmp)                                                \
                        return COPS_INVALID; /* valid tree*/                   \
                name##_node *x = self->root;                                   \
                while (x) {                                                    \
                        int cmp = self->cmp(key, x->key);                      \
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
                name##_node *n = x;        /* *n -> double black or target*/   \
                if (x->left && x->right) { /* leftmost right children*/        \
                        n = n->right;                                          \
                        while (n->left) {                                      \
                                n = n->left;                                   \
                        }                                                      \
                        K k = x->key;                                          \
                        V v = x->val;                                          \
                        x->key = n->key;                                       \
                        x->val = n->val;                                       \
                        n->key = k;                                            \
                        n->val = v;                                            \
                }                                                              \
                x = NULL;                   /* only one children (n)*/         \
                name##_node *p = n->parent; /* *p -> n parent*/                \
                if (n->left || n->right) {                                     \
                        name##_node *oc =                                      \
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
                        cops_default_allocator.free(n);                        \
                        self->nelem--;                                         \
                        return COPS_OK;                                        \
                } /* no children*/                                             \
                if (!p) {                                                      \
                        self->nelem--;                                         \
                        self->root = NULL;                                     \
                        cops_default_allocator.free(n);                        \
                        return COPS_OK;                                        \
                }                                                              \
                name##_node *s = p->left == n ? p->right : p->left;            \
                name##_node *cn, *dn;                                          \
                if (p->left == n)                                              \
                        p->left = NULL;                                        \
                else                                                           \
                        p->right = NULL;                                       \
                volatile int n_is_red = n->isred;                              \
                cops_default_allocator.free(n);                                \
                n = NULL;                                                      \
                self->nelem--;                                                 \
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
                                        name##_node_rotate_right(self, p);     \
                                else                                           \
                                        name##_node_rotate_left(self, p);      \
                                s->isred = 0;                                  \
                                p->isred = 1;                                  \
                                continue;                                      \
                        } /* sibling is black*/                                \
                        if (cn && cn->isred) {                                 \
                                if (n_is_right)                                \
                                        name##_node_rotate_left(self, s);      \
                                else                                           \
                                        name##_node_rotate_right(self, s);     \
                                cn->isred = 0;                                 \
                                s->isred = 1;                                  \
                                continue;                                      \
                        } /* close nephew is black*/                           \
                        if (dn && dn->isred) {                                 \
                                if (n_is_right)                                \
                                        name##_node_rotate_right(self, p);     \
                                else                                           \
                                        name##_node_rotate_left(self, p);      \
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

#define init_cops_omap(K, V) __init_cops_omap(cops_##K##_##V##_omap, K, V)

/**
 *
 * COPS_OSET
 *
 */

#define __init_cops_oset(name, T)                                              \
                                                                               \
        typedef struct name##_node {                                           \
                struct name##_node *parent, *left, *right;                     \
                uint8_t isred;                                                 \
                T val;                                                         \
        } name##_node;                                                         \
                                                                               \
        typedef struct name {                                                  \
                uint32_t nelem;                                                \
                uint32_t rc;                                                   \
                name##_node *root;                                             \
                int (*cmp)(T, T);                                              \
        } name;                                                                \
                                                                               \
        static inline name *name##_new(int (*cmp)(T, T))                       \
        {                                                                      \
                name *self =                                                   \
                    (name *)cops_default_allocator.alloc(sizeof(*self));       \
                if (!self)                                                     \
                        return NULL;                                           \
                self->cmp = cmp;                                               \
                self->nelem = 0;                                               \
                self->rc = 1;                                                  \
                self->root = NULL;                                             \
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
                        if (self->nelem) {                                     \
                                name##_node *n = self->root;                   \
                                while (n) {                                    \
                                        if (n->left) {                         \
                                                n = n->left;                   \
                                        } else if (n->right) {                 \
                                                n = n->right;                  \
                                        } else {                               \
                                                name##_node *p = n->parent;    \
                                                if (p && p->left == n)         \
                                                        p->left = NULL;        \
                                                if (p && p->right == n)        \
                                                        p->right = NULL;       \
                                                cops_default_allocator.free(   \
                                                    n);                        \
                                                n = p;                         \
                                        }                                      \
                                }                                              \
                        }                                                      \
                        self->root = NULL;                                     \
                        cops_default_allocator.free(self);                     \
                }                                                              \
                return NULL;                                                   \
        }                                                                      \
                                                                               \
        /* rotate node to became right child of his left child*/               \
        static inline int name##_node_rotate_right(name *self, name##_node *p) \
        {                                                                      \
                if (!self || !p)                                               \
                        return COPS_INVALID;                                   \
                name##_node *x = p->left, *g = p->parent;                      \
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
        static inline int name##_node_rotate_left(name *self, name##_node *p)  \
        {                                                                      \
                if (!self || !p)                                               \
                        return COPS_INVALID;                                   \
                name##_node *x = p->right, *g = p->parent;                     \
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
        static inline int name##_add(name *self, T val)                        \
        {                                                                      \
                if (!self)                                                     \
                        return COPS_INVALID;                                   \
                if (!self->cmp)                                                \
                        return COPS_INVALID; /* init node*/                    \
                name##_node *e =                                               \
                    (name##_node *)cops_default_allocator.alloc(sizeof(*e));   \
                if (!e)                                                        \
                        return COPS_MEMERR;                                    \
                *e = (name##_node){NULL, NULL, NULL, 1, val};                  \
                /* insert*/                                                    \
                if (!self->root) {                                             \
                        self->root = e;                                        \
                        e->isred = 0;                                          \
                        self->nelem++;                                         \
                        return COPS_OK;                                        \
                }                                                              \
                name##_node *n = self->root;                                   \
                while (1) {                                                    \
                        int diff = self->cmp(key, n->key);                     \
                        if (!diff) {                                           \
                                cops_default_allocator.free(e);                \
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
                self->nelem++;                                                 \
                n = NULL;                                                      \
                /* balance*/                                                   \
                name##_node *p, *u, *g, *x = e;                                \
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
                                /* reposition names*/                          \
                                x = g;                                         \
                                continue;                                      \
                        }                                                      \
                        /* from here we assume u is black or null*/            \
                        if (g->left == p) {                                    \
                                /* LR rot.*/                                   \
                                if (p->right == x) {                           \
                                        name##_node_rotate_left(self, p);      \
                                        x = p;                                 \
                                        p = x->parent;                         \
                                }                                              \
                                /* LL rot.*/                                   \
                                name##_node_rotate_right(self, g);             \
                                p->isred = 0;                                  \
                                g->isred = 1;                                  \
                                break;                                         \
                        } else {                                               \
                                /* RL rot.*/                                   \
                                if (p->left == x) {                            \
                                        name##_node_rotate_right(self, p);     \
                                        x = p;                                 \
                                        p = x->parent;                         \
                                }                                              \
                                /* RR rot.*/                                   \
                                name##_node_rotate_left(self, g);              \
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
        static inline int name##_has(name *self, T val)                        \
        {                                                                      \
                if (!self)                                                     \
                        return COPS_INVALID;                                   \
                if (!self->cmp)                                                \
                        return COPS_INVALID;                                   \
                name##_node *n = self->root;                                   \
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
        static inline int name##_del(name *self, T *val)                       \
        {                                                                      \
                if (!self)                                                     \
                        return COPS_INVALID;                                   \
                if (!self->cmp)                                                \
                        return COPS_INVALID; /* valid tree*/                   \
                name##_node *x = self->root;                                   \
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
                name##_node *n = x;        /* *n -> double black or target*/   \
                if (x->left && x->right) { /* leftmost right children*/        \
                        n = n->right;                                          \
                        while (n->left) {                                      \
                                n = n->left;                                   \
                        }                                                      \
                        T v = x->val;                                          \
                        x->val = n->val;                                       \
                        n->val = v;                                            \
                }                                                              \
                x = NULL;                   /* only one children (n)*/         \
                name##_node *p = n->parent; /* *p -> n parent*/                \
                if (n->left || n->right) {                                     \
                        name##_node *oc =                                      \
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
                        cops_default_allocator.free(n);                        \
                        self->nelem--;                                         \
                        return COPS_OK;                                        \
                } /* no children*/                                             \
                if (!p) {                                                      \
                        self->nelem--;                                         \
                        self->root = NULL;                                     \
                        cops_default_allocator.free(n);                        \
                        return COPS_OK;                                        \
                }                                                              \
                name##_node *s = p->left == n ? p->right : p->left;            \
                name##_node *cn, *dn;                                          \
                if (p->left == n)                                              \
                        p->left = NULL;                                        \
                else                                                           \
                        p->right = NULL;                                       \
                volatile int n_is_red = n->isred;                              \
                cops_default_allocator.free(n);                                \
                n = NULL;                                                      \
                self->nelem--;                                                 \
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
                                        name##_node_rotate_right(self, p);     \
                                else                                           \
                                        name##_node_rotate_left(self, p);      \
                                s->isred = 0;                                  \
                                p->isred = 1;                                  \
                                continue;                                      \
                        } /* sibling is black*/                                \
                        if (cn && cn->isred) {                                 \
                                if (n_is_right)                                \
                                        name##_node_rotate_left(self, s);      \
                                else                                           \
                                        name##_node_rotate_right(self, s);     \
                                cn->isred = 0;                                 \
                                s->isred = 1;                                  \
                                continue;                                      \
                        } /* close nephew is black*/                           \
                        if (dn && dn->isred) {                                 \
                                if (n_is_right)                                \
                                        name##_node_rotate_right(self, p);     \
                                else                                           \
                                        name##_node_rotate_left(self, p);      \
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

#define init_cops_oset(T) __init_cops_oset(cops_##K##_##V##_oset, K, V)

#ifdef __cplusplus
}
#endif

#endif /* ifndef COPS_H */
