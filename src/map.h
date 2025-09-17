#ifndef COPS_MAP_H
#define COPS_MAP_H

#include "core.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * basic string hash function
 */
static inline size_t djb2(char *s)
{
        size_t hash = 5381;
        int c;
        while ((c = *s++))
                hash = ((hash << 5) + hash) + c;
        return hash;
}

/**
 * hashmap with robin hood probing
 *
 * pow2 size is the simplest and easy to handle size growth
 * chaining is too complex to handle and too expensive
 * linear probing would lead to chuncking and long jumps
 * quadratic probing would lead to untouched entries in pow2 size
 * triangular robinhood probing fit perfectly
 *
 * set will override previous value
 *
 *  0 success
 * -1 invalid parameters
 * -2 memory error
 * -3 logic error
 *
 * flags:
 *   0x80 -> free
 *   0x40 -> tombstone
 *   0x3f -> jumps
 */

#define __init_cops_map(name, K, V)                                                                \
        typedef struct name##_node {                                                               \
                uint8_t flag;                                                                      \
                K key;                                                                             \
                V val;                                                                             \
        } name##_node;                                                                             \
                                                                                                   \
        typedef struct name {                                                                      \
                uint32_t rc;                                                                       \
                uint32_t cap;                                                                      \
                uint32_t nelem;                                                                    \
                name##_node *data;                                                                 \
                size_t (*hash)(K);                                                                 \
                int (*cmp)(K, K);                                                                  \
                void (*free_key)(K);                                                               \
                void (*free_val)(V);                                                               \
        } name;                                                                                    \
                                                                                                   \
        static inline name *name##_new(size_t (*hash)(K), int (*cmp)(K, K))                        \
        {                                                                                          \
                if (!hash || !cmp)                                                                 \
                        return NULL;                                                               \
                if (16 > SIZE_MAX / sizeof(name##_node))                                           \
                        return NULL;                                                               \
                name *self = (name *)cops_default_allocator.alloc(sizeof(*self));                  \
                if (!self)                                                                         \
                        return NULL;                                                               \
                self->cap = 16;                                                                    \
                self->rc = 1;                                                                      \
                self->data =                                                                       \
                    (name##_node *)cops_default_allocator.alloc(sizeof(name##_node) * self->cap);  \
                if (!self->data) {                                                                 \
                        cops_default_allocator.free(self);                                         \
                        return NULL;                                                               \
                }                                                                                  \
                memset(self->data, 0, sizeof(*self->data) * self->cap);                            \
                for (size_t i = 0; i < self->cap; i++) {                                           \
                        self->data[i].flag = 0x80;                                                 \
                }                                                                                  \
                self->hash = hash;                                                                 \
                self->cmp = cmp;                                                                   \
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
                        if (self->free_key || self->free_val) {                                    \
                                for (size_t i = 0; i < self->cap; i++) {                           \
                                        name##_node d = self->data[i];                             \
                                        if (d.flag >= 0x40)                                        \
                                                continue;                                          \
                                        if (self->free_key)                                        \
                                                self->free_key(d.key);                             \
                                        if (self->free_val)                                        \
                                                self->free_val(d.val);                             \
                                }                                                                  \
                        }                                                                          \
                        cops_default_allocator.free(self->data);                                   \
                        cops_default_allocator.free(self);                                         \
                }                                                                                  \
                return NULL;                                                                       \
        }                                                                                          \
                                                                                                   \
        static inline int name##_set(name *self, K key, V val)                                     \
        {                                                                                          \
                if (!self || !self->data || !self->hash || !self->cmp)                             \
                        return -1;                                                                 \
                if (((100 * self->nelem) / self->cap) > 90) {                                      \
                        int res = 0;                                                               \
                        size_t cap = self->cap * 2;                                                \
                        name##_node *old = self->data;                                             \
                        name##_node *data =                                                        \
                            (name##_node *)cops_default_allocator.alloc(sizeof(*data) * cap);      \
                        if (!data)                                                                 \
                                return -2;                                                         \
                        memset(data, 0, sizeof(*data) * cap);                                      \
                        for (size_t i = 0; i < cap; i++) {                                         \
                                data[i].flag = 0x80;                                               \
                        }                                                                          \
                        self->data = data;                                                         \
                        self->cap = cap;                                                           \
                        self->nelem = 0;                                                           \
                        for (size_t i = 0; i < cap / 2; i++) {                                     \
                                if (res)                                                           \
                                        return res;                                                \
                                name##_node *n = old + i;                                          \
                                if (n->flag < 0x40) {                                              \
                                        res = name##_set(self, n->key, n->val);                    \
                                }                                                                  \
                        }                                                                          \
                        cops_default_allocator.free(old);                                          \
                }                                                                                  \
                size_t pos, entry = self->hash(key) % self->cap;                                   \
                uint8_t i = 0;                                                                     \
                while (i < 0x40) {                                                                 \
                        pos = (entry + (i * i + i) / 2) % self->cap;                               \
                        name##_node *n = self->data + pos;                                         \
                        if (n->flag & 0x80) {                                                      \
                                return -3;                                                         \
                        }                                                                          \
                        if (n->flag < 0x40 && !self->cmp(key, n->key)) {                           \
                                if (self->free_val)                                                \
                                        self->free_val(n->val);                                    \
                                *n = (name##_node){i, n->key, val};                                \
                                return 0;                                                          \
                        }                                                                          \
                        i++;                                                                       \
                }                                                                                  \
                return -2;                                                                         \
        }                                                                                          \
                                                                                                   \
        static inline int name##_add(name *self, K key, V val)                                     \
        {                                                                                          \
                if (!self || !self->data || !self->hash || !self->cmp)                             \
                        return -1;                                                                 \
                if (((100 * self->nelem) / self->cap) > 90) {                                      \
                        int res = 0;                                                               \
                        size_t cap = self->cap * 2;                                                \
                        name##_node *old = self->data;                                             \
                        name##_node *data =                                                        \
                            (name##_node *)cops_default_allocator.alloc(sizeof(*data) * cap);      \
                        if (!data)                                                                 \
                                return -2;                                                         \
                        memset(data, 0, sizeof(*data) * cap);                                      \
                        for (size_t i = 0; i < cap; i++) {                                         \
                                data[i].flag = 0x80;                                               \
                        }                                                                          \
                        self->data = data;                                                         \
                        self->cap = cap;                                                           \
                        self->nelem = 0;                                                           \
                        for (size_t i = 0; i < cap / 2; i++) {                                     \
                                if (res)                                                           \
                                        return res;                                                \
                                name##_node *n = old + i;                                          \
                                if (n->flag != 0x80) {                                             \
                                        res = name##_add(self, n->key, n->val);                    \
                                }                                                                  \
                        }                                                                          \
                        cops_default_allocator.free(old);                                          \
                }                                                                                  \
                size_t pos, entry = self->hash(key) % self->cap;                                   \
                uint8_t i = 0;                                                                     \
                while (i < 0x40) {                                                                 \
                        pos = (entry + (i * i + i) / 2) % self->cap;                               \
                        name##_node *n = self->data + pos;                                         \
                        if (n->flag >= 0x40) {                                                     \
                                *n = (name##_node){i, key, val};                                   \
                                self->nelem++;                                                     \
                                return 0;                                                          \
                        }                                                                          \
                        if (n->flag < 0x40 && !self->cmp(key, n->key)) {                           \
                                return -3;                                                         \
                        }                                                                          \
                        if (n->flag < i) {                                                         \
                                name##_node next = *n;                                             \
                                *n = (name##_node){i, key, val};                                   \
                                i = next.flag;                                                     \
                                val = next.val;                                                    \
                                key = next.key;                                                    \
                                entry = self->hash(key) % self->cap;                               \
                        }                                                                          \
                        i++;                                                                       \
                }                                                                                  \
                return -2;                                                                         \
        }                                                                                          \
                                                                                                   \
        static inline int name##_has(name *self, K key)                                            \
        {                                                                                          \
                if (!self || !self->data || !self->hash || !self->cmp)                             \
                        return -1;                                                                 \
                size_t pos, entry = self->hash(key) % self->cap;                                   \
                uint8_t i = 0;                                                                     \
                while (i < 0x40) {                                                                 \
                        pos = (entry + (i * i + i) / 2) % self->cap;                               \
                        name##_node *n = self->data + pos;                                         \
                        if (n->flag & 0x80)                                                        \
                                return 0;                                                          \
                        if (n->flag < 0x40 && self->cmp(key, n->key) == 0)                         \
                                return 1;                                                          \
                        i++;                                                                       \
                }                                                                                  \
                return 0;                                                                          \
        }                                                                                          \
                                                                                                   \
        static inline int name##_del(name *self, K key, V *val)                                    \
        {                                                                                          \
                if (!self || !self->data || !self->hash || !self->cmp)                             \
                        return -1;                                                                 \
                size_t pos, entry = self->hash(key) % self->cap;                                   \
                uint8_t i = 0;                                                                     \
                while (i < 0x40) {                                                                 \
                        pos = (entry + (i * i + i) / 2) % self->cap;                               \
                        name##_node *n = self->data + pos;                                         \
                        if (n->flag & 0x80)                                                        \
                                return -1;                                                         \
                        if (n->flag < 0x40 && !self->cmp(key, n->key)) {                           \
                                n->flag = 0x40;                                                    \
                                if (val)                                                           \
                                        *val = n->val;                                             \
                                if (self->free_key)                                                \
                                        self->free_key(n->key);                                    \
                                if (self->free_val)                                                \
                                        self->free_val(n->val);                                    \
                                self->nelem--;                                                     \
                                return 0;                                                          \
                        }                                                                          \
                        i++;                                                                       \
                }                                                                                  \
                return -3;                                                                         \
        }                                                                                          \
                                                                                                   \
        static inline int name##_get(name *self, K key, V *val)                                    \
        {                                                                                          \
                if (!self || !self->data || !self->hash || !self->cmp)                             \
                        return -1;                                                                 \
                size_t pos, entry = self->hash(key) % self->cap;                                   \
                uint8_t i = 0;                                                                     \
                while (i < 0x40) {                                                                 \
                        pos = (entry + (i * i + i) / 2) % self->cap;                               \
                        name##_node *n = self->data + pos;                                         \
                        if (n->flag < 0x40 && !self->cmp(key, n->key)) {                           \
                                if (val)                                                           \
                                        *val = n->val;                                             \
                                return 0;                                                          \
                        }                                                                          \
                        i++;                                                                       \
                }                                                                                  \
                return -3;                                                                         \
        }                                                                                          \
                                                                                                   \
        static inline int name##_import(name *self, name *oth)                                     \
        {                                                                                          \
                if (!self || !self->data || !self->hash || !self->cmp)                             \
                        return -1;                                                                 \
                if (!oth || !oth->data || !oth->hash || !oth->cmp)                                 \
                        return -1;                                                                 \
                uint32_t cap = self->cap;                                                          \
                while (((100 * (self->nelem + oth->nelem)) / cap) > 90)                            \
                        cap *= 2;                                                                  \
                if (cap != self->cap) {                                                            \
                        uint32_t o_cap = self->cap;                                                \
                        name##_node *old = self->data;                                             \
                        name##_node *new =                                                         \
                            (name##_node *)cops_default_allocator.alloc(sizeof(*new) * cap);       \
                        if (!new)                                                                  \
                                return -2;                                                         \
                        memset(new, 0, sizeof(*new) * cap);                                        \
                        for (size_t i = 0; i < cap; i++) {                                         \
                                new[i].flag = 0x80;                                                \
                        }                                                                          \
                        self->data = new;                                                          \
                        self->cap = cap;                                                           \
                        for (size_t i = 0; i < o_cap; i++) {                                       \
                                int res;                                                           \
                                name##_node *n = old + i;                                          \
                                if (n->flag < 0x40) {                                              \
                                        res = name##_set(self, n->key, n->val);                    \
                                }                                                                  \
                                if (res)                                                           \
                                        return res;                                                \
                        }                                                                          \
                        cops_default_allocator.free(old);                                          \
                }                                                                                  \
                for (size_t i = 0; i < oth->cap; i++) {                                            \
                        int res;                                                                   \
                        name##_node *n = oth->data + i;                                            \
                        if (n->flag < 0x40) {                                                      \
                                res = name##_set(self, n->key, n->val);                            \
                        }                                                                          \
                        if (res)                                                                   \
                                return res;                                                        \
                }                                                                                  \
                return 0;                                                                          \
        }

#define init_cops_map(K, V) __init_cops_map(cops_##K##_##V##_map, K, V)

#ifdef __cplusplus
}
#endif

#endif /* ifndef COPS_MAP_H */
