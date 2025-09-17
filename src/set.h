#ifndef COPS_SET_H
#define COPS_SET_H

#ifdef __cplusplus
extern "C" {
#endif

#include "core.h"

/**
 * hashset with robin hood probing
 *
 * pow2 size is the simplest and easy to handle size growth
 * chaining is too complex to handle and too expensive
 * linear probing would lead to chuncking and long jumps
 * quadratic probing would lead to untouched entries in pow2 size
 * triangular robinhood probing fit perfectly
 *
 * add will override previous value
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

#define __init_cops_set(name, T)                                                                   \
        typedef struct name##_node {                                                               \
                uint8_t flag;                                                                      \
                T val;                                                                             \
        } name##_node;                                                                             \
                                                                                                   \
        typedef struct name {                                                                      \
                uint32_t rc;                                                                       \
                uint32_t cap;                                                                      \
                uint32_t nelem;                                                                    \
                name##_node *data;                                                                 \
                size_t (*hash)(T);                                                                 \
                int (*cmp)(T, T);                                                                  \
                void (*free)(T);                                                                   \
                T (*dup)(T);                                                                       \
        } name;                                                                                    \
                                                                                                   \
        static inline name *name##_new(size_t (*hash)(T), int (*cmp)(T, T))                        \
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
                        if (self->free) {                                                          \
                                for (size_t i = 0; i < self->cap; i++) {                           \
                                        name##_node d = self->data[i];                             \
                                        if (d.flag >= 0x40)                                        \
                                                continue;                                          \
                                        self->free(d.val);                                         \
                                }                                                                  \
                        }                                                                          \
                        cops_default_allocator.free(self->data);                                   \
                        cops_default_allocator.free(self);                                         \
                }                                                                                  \
                return NULL;                                                                       \
        }                                                                                          \
                                                                                                   \
        static inline int name##_set(name *self, T val)                                            \
        {                                                                                          \
                if (!self || !self->data || !self->hash || !self->cmp)                             \
                        return -1;                                                                 \
                size_t pos, entry = self->hash(val) % self->cap;                                   \
                uint8_t i = 0;                                                                     \
                while (i < 0x40) {                                                                 \
                        pos = (entry + (i * i + i) / 2) % self->cap;                               \
                        name##_node *n = self->data + pos;                                         \
                        if (n->flag & 0x80) {                                                      \
                                return -3;                                                         \
                        }                                                                          \
                        if (n->flag < 0x40 && !self->cmp(val, n->val)) {                           \
                                if (self->free)                                                    \
                                        self->free(n->val);                                        \
                                n->val = val;                                                      \
                                return 0;                                                          \
                        }                                                                          \
                        i++;                                                                       \
                }                                                                                  \
                return -2;                                                                         \
        }                                                                                          \
                                                                                                   \
        static inline int name##_add(name *self, T val)                                            \
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
                                        res = name##_add(self, n->val);                            \
                                }                                                                  \
                        }                                                                          \
                        cops_default_allocator.free(old);                                          \
                }                                                                                  \
                size_t pos, entry = self->hash(val) % self->cap;                                   \
                uint8_t i = 0;                                                                     \
                while (i < 0x40) {                                                                 \
                        pos = (entry + (i * i + i) / 2) % self->cap;                               \
                        name##_node *n = self->data + pos;                                         \
                        if (n->flag >= 0x40) {                                                     \
                                *n = (name##_node){i, val};                                        \
                                self->nelem++;                                                     \
                                return 0;                                                          \
                        }                                                                          \
                        if (n->flag < 0x40 && !self->cmp(val, n->val)) {                           \
                                return -3;                                                         \
                        }                                                                          \
                        if (n->flag < i) {                                                         \
                                name##_node next = *n;                                             \
                                *n = (name##_node){i, val};                                        \
                                i = next.flag;                                                     \
                                val = next.val;                                                    \
                                entry = self->hash(val) % self->cap;                               \
                        }                                                                          \
                        i++;                                                                       \
                }                                                                                  \
                return -2;                                                                         \
        }                                                                                          \
                                                                                                   \
        static inline int name##_has(name *self, T val)                                            \
        {                                                                                          \
                if (!self || !self->data || !self->hash || !self->cmp)                             \
                        return -1;                                                                 \
                size_t pos, entry = self->hash(val) % self->cap;                                   \
                uint8_t i = 0;                                                                     \
                while (i < 0x40) {                                                                 \
                        pos = (entry + (i * i + i) / 2) % self->cap;                               \
                        name##_node *n = self->data + pos;                                         \
                        if (n->flag & 0x80)                                                        \
                                return 0;                                                          \
                        if (n->flag < 0x40 && !self->cmp(val, n->val))                             \
                                return 1;                                                          \
                        i++;                                                                       \
                }                                                                                  \
                return 0;                                                                          \
        }                                                                                          \
                                                                                                   \
        static inline int name##_del(name *self, T val)                                            \
        {                                                                                          \
                if (!self || !self->data || !self->hash || !self->cmp)                             \
                        return -1;                                                                 \
                size_t pos, entry = self->hash(val) % self->cap;                                   \
                uint8_t i = 0;                                                                     \
                while (i < 0x40) {                                                                 \
                        pos = (entry + (i * i + i) / 2) % self->cap;                               \
                        name##_node *n = self->data + pos;                                         \
                        if (n->flag & 0x80)                                                        \
                                return -1;                                                         \
                        if (n->flag < 0x40 && !self->cmp(val, n->val)) {                           \
                                n->flag = 0x40;                                                    \
                                if (self->free)                                                    \
                                        self->free(n->val);                                        \
                                self->nelem--;                                                     \
                                return 0;                                                          \
                        }                                                                          \
                        i++;                                                                       \
                }                                                                                  \
                return -3;                                                                         \
        }                                                                                          \
                                                                                                   \
        static inline int name##_get(name *self, T val, T *old)                                    \
        {                                                                                          \
                if (!self || !self->data || !self->hash || !self->cmp)                             \
                        return -1;                                                                 \
                size_t pos, entry = self->hash(val) % self->cap;                                   \
                uint8_t i = 0;                                                                     \
                while (i < 0x40) {                                                                 \
                        pos = (entry + (i * i + i) / 2) % self->cap;                               \
                        name##_node *n = self->data + pos;                                         \
                        if (n->flag < 0x40 && !self->cmp(val, n->val)) {                           \
                                if (old)                                                           \
                                        *old = n->val;                                             \
                                return 0;                                                          \
                        }                                                                          \
                        i++;                                                                       \
                }                                                                                  \
                return -3;                                                                         \
        }                                                                                          \
                                                                                                   \
        static inline int name##_import(name *self, name *oth)                                     \
        {                                                                                          \
                if (self == oth)                                                                   \
                        return -1;                                                                 \
                if (!self || !self->data || !self->hash || !self->cmp)                             \
                        return -1;                                                                 \
                if (!oth || !oth->data || !oth->hash || !oth->cmp)                                 \
                        return -1;                                                                 \
                uint32_t cap = self->cap;                                                          \
                uint32_t len = self->nelem;                                                        \
                for (size_t i = 0; i < oth->cap; i++) {                                            \
                        name##_node *n = oth->data + i;                                            \
                        if (n->flag < 0x40) {                                                      \
                                if (!name##_has(self, n->val))                                     \
                                        len++;                                                     \
                        }                                                                          \
                }                                                                                  \
                while (((100 * (len)) / cap) > 90)                                                 \
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
                        self->nelem = 0;                                                           \
                        for (size_t i = 0; i < o_cap; i++) {                                       \
                                int res;                                                           \
                                name##_node *n = old + i;                                          \
                                if (n->flag < 0x40) {                                              \
                                        res = name##_add(self, n->val);                            \
                                }                                                                  \
                                if (res)                                                           \
                                        return res;                                                \
                        }                                                                          \
                        cops_default_allocator.free(old);                                          \
                }                                                                                  \
                for (size_t i = 0; i < oth->cap; i++) {                                            \
                        name##_node *n = oth->data + i;                                            \
                        if (n->flag < 0x40) {                                                      \
                                T val = n->val;                                                    \
                                if (self->dup)                                                     \
                                        val = self->dup(n->val);                                   \
                                if (name##_add(self, val))                                         \
                                        name##_set(self, n->val);                                  \
                        }                                                                          \
                }                                                                                  \
                return 0;                                                                          \
        }

#define init_cops_set(T) __init_cops_set(cops_##T##_set, T)

#ifdef __cplusplus
}
#endif

#endif /* ifndef COPS_SET_H */
