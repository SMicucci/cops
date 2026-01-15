#include "core.h"
#include <stdint.h>
#define __cops_set_choose(...)                                                 \
        __cops_get_macro(__VA_ARGS__, __cops_err_arg_count, __cops_init_set_3, \
                         __cops_init_set_2, __cops_err_arg_count, )
#define init_set(...) __cops_expand(__cops_set_choose(__VA_ARGS__)(__VA_ARGS__))

/* RH HASHSET magic number */
#define __cops_set_probe(pos, jmp, cap) ((pos + (jmp * jmp + jmp) / 2) % cap)
#define __cops_set_max_cap(len, cap) (((100 * len) / cap) > 90)
#define __cops_set_max_jump (1 << 6)

#define __cops_init_set_2(T, NAME)                                             \
        typedef struct NAME##_set_node {                                       \
                uint8_t free : 1;                                              \
                uint8_t tomb : 1;                                              \
                uint8_t jumps : 6;                                             \
                T val;                                                         \
        } NAME##_set_node;                                                     \
                                                                               \
        typedef struct NAME {                                                  \
                uint64_t cap;                                                  \
                uint64_t len;                                                  \
                NAME##_set_node *data;                                         \
                uint64_t (*hash)(T);                                           \
                int (*cmp)(T, T);                                              \
                void (*free)(T);                                               \
                T (*dup)(T);                                                   \
        } NAME;                                                                \
                                                                               \
        static inline NAME *NAME##_new(uint64_t (*hash)(T), int (*cmp)(T, T))  \
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
                self->data = (NAME##_set_node *)COPS_ALLOC(                    \
                    sizeof(NAME##_set_node) * self->cap);                      \
                COPS_ASSERT(self->data);                                       \
                if (!self->data) {                                             \
                        COPS_FREE(self);                                       \
                        return NULL;                                           \
                }                                                              \
                for (uint64_t i = 0; i < self->cap; i++) {                     \
                        self->data[i].free = 1;                                \
                }                                                              \
                self->hash = hash;                                             \
                self->cmp = cmp;                                               \
                self->free = NULL;                                             \
                self->dup = NULL;                                              \
                return self;                                                   \
        }                                                                      \
                                                                               \
        static inline void NAME##_free(NAME *self)                             \
        {                                                                      \
                COPS_ASSERT(self);                                             \
                if (self->free) {                                              \
                        for (uint64_t i = 0; i < self->cap; i++) {             \
                                NAME##_set_node d = self->data[i];             \
                                if (d.free || d.tomb)                          \
                                        continue;                              \
                                self->free(d.val);                             \
                        }                                                      \
                }                                                              \
                COPS_FREE(self->data);                                         \
                COPS_FREE(self);                                               \
        }                                                                      \
                                                                               \
        static inline int NAME##_set(NAME *self, T val)                        \
        {                                                                      \
                COPS_ASSERT(self);                                             \
                if (!self)                                                     \
                        return COPS_INVALID;                                   \
                uint64_t pos, entry = self->hash(val) % self->cap;             \
                uint8_t i = 0;                                                 \
                while (i < __cops_set_max_jump) {                              \
                        pos = __cops_set_probe(entry, i, self->cap);           \
                        NAME##_set_node *n = self->data + pos;                 \
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
        static inline int __##NAME##_insert(NAME *self, T val)                 \
        {                                                                      \
                uint64_t pos, entry = self->hash(val) % self->cap;             \
                uint8_t jmp = 0;                                               \
                /* max jump supported */                                       \
                while (jmp < __cops_set_max_jump) {                            \
                        pos = __cops_set_probe(entry, jmp, self->cap);         \
                        NAME##_set_node *n = self->data + pos;                 \
                        if (n->free || n->tomb) {                              \
                                *n = (NAME##_set_node){0, 0, jmp, val};        \
                                self->len++;                                   \
                                return COPS_OK;                                \
                        }                                                      \
                        /* fail on duplicate */                                \
                        COPS_ASSERT(self->cmp(val, n->val));                   \
                        if (!self->cmp(val, n->val))                           \
                                return COPS_INVALID;                           \
                        /* RH swap */                                          \
                        if (n->jumps < jmp) {                                  \
                                NAME##_set_node lazy = *n;                     \
                                *n = (NAME##_set_node){0, 0, jmp, val};        \
                                jmp = lazy.jumps;                              \
                                val = lazy.val;                                \
                                entry = self->hash(val) % self->cap;           \
                        }                                                      \
                        jmp++;                                                 \
                }                                                              \
                return COPS_INVALID;                                           \
        }                                                                      \
                                                                               \
        static inline int NAME##_add(NAME *self, T val)                        \
        {                                                                      \
                COPS_ASSERT(self);                                             \
                if (!self)                                                     \
                        return COPS_INVALID;                                   \
                /* resize hashset */                                           \
                if (__cops_set_max_cap(self->len, self->cap)) {                \
                        self->cap *= 2;                                        \
                        NAME##_set_node *old = self->data;                     \
                        self->data = (NAME##_set_node *)COPS_ALLOC(sizeof(T) * \
                                                                   self->cap); \
                        COPS_ASSERT(self->data);                               \
                        if (!self->data)                                       \
                                return COPS_MEMERR;                            \
                        for (uint64_t i = 0; i < self->cap; i++) {             \
                                self->data[i].free = 1;                        \
                        }                                                      \
                        self->len = 0;                                         \
                        /* iterate truh old storage */                         \
                        for (uint64_t i = 0; i < self->cap / 2; i++) {         \
                                NAME##_set_node *n = old + i;                  \
                                if (n->free || n->tomb)                        \
                                        continue;                              \
                                int res = __##NAME##_insert(self, val);        \
                                if (res == COPS_OK)                            \
                                        continue;                              \
                                return res;                                    \
                        }                                                      \
                        COPS_FREE(old);                                        \
                }                                                              \
                return __##NAME##_insert(self, val);                           \
        }                                                                      \
                                                                               \
        static inline int NAME##_has(NAME *self, T val)                        \
        {                                                                      \
                COPS_ASSERT(self);                                             \
                if (!self)                                                     \
                        return COPS_INVALID;                                   \
                uint64_t pos, entry = self->hash(val) % self->cap;             \
                uint8_t jmp = 0;                                               \
                while (jmp < __cops_set_max_jump) {                            \
                        pos = __cops_set_probe(entry, jmp, self->cap);         \
                        NAME##_set_node *n = self->data + pos;                 \
                        if (n->free)                                           \
                                return 0;                                      \
                        if (!n->tomb && !self->cmp(val, n->val))               \
                                return 1;                                      \
                        jmp++;                                                 \
                }                                                              \
                return COPS_INVALID;                                           \
        }                                                                      \
                                                                               \
        static inline int NAME##_del(NAME *self, T val)                        \
        {                                                                      \
                COPS_ASSERT(self);                                             \
                if (!self)                                                     \
                        return COPS_INVALID;                                   \
                uint64_t pos, entry = self->hash(val) % self->cap;             \
                uint8_t jmp = 0;                                               \
                while (jmp < __cops_set_max_jump) {                            \
                        pos = __cops_set_probe(entry, jmp, self->cap);         \
                        NAME##_set_node *n = self->data + pos;                 \
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
        static inline int NAME##_get(NAME *self, T val, T *old)                \
        {                                                                      \
                COPS_ASSERT(self);                                             \
                if (!self)                                                     \
                        return COPS_INVALID;                                   \
                uint64_t pos, entry = self->hash(val) % self->cap;             \
                uint8_t jmp = 0;                                               \
                while (jmp < __cops_set_max_jump) {                            \
                        pos = __cops_set_probe(entry, jmp, self->cap);         \
                        NAME##_set_node *n = self->data + pos;                 \
                        if (n->free)                                           \
                                return COPS_INVALID;                           \
                        if (!n->tomb && !self->cmp(val, n->val)) {             \
                                if (old)                                       \
                                        *old = n->val;                         \
                                return COPS_OK;                                \
                        }                                                      \
                        jmp++;                                                 \
                }                                                              \
                return COPS_INVALID;                                           \
        }

#define __cops_init_set_3(T, NAME, SLICE_T)                                    \
        __cops_init_set_2(T, NAME);                                            \
                                                                               \
        static inline SLICE_T *NAME##_export(NAME *self)                       \
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
                                NAME##_set_node *n = self->data + i;           \
                                if (n->free || n->tomb)                        \
                                        continue;                              \
                                slice->data[j++] = self->dup(n->val);          \
                        }                                                      \
                } else {                                                       \
                        for (uint64_t i = 0; i < self->cap; i++) {             \
                                NAME##_set_node *n = self->data + i;           \
                                if (n->free || n->tomb)                        \
                                        continue;                              \
                                slice->data[j++] = n->val;                     \
                        }                                                      \
                }                                                              \
                return slice;                                                  \
        }                                                                      \
                                                                               \
        static inline int NAME##_import(NAME *self, SLICE_T *slice)            \
        {                                                                      \
                COPS_ASSERT(self);                                             \
                COPS_ASSERT(slice);                                            \
                if (!self || !slice)                                           \
                        return COPS_INVALID;                                   \
                /* increase data size */                                       \
                uint64_t old_cap = self->cap;                                  \
                while (__cops_set_max_cap(self->len + slice->len, self->cap))  \
                        self->cap *= 2;                                        \
                NAME##_set_node *old = self->data;                             \
                self->data =                                                   \
                    (NAME##_set_node *)COPS_ALLOC(sizeof(T) * old_cap);        \
                COPS_ASSERT(self->data);                                       \
                if (!self->data)                                               \
                        return COPS_MEMERR;                                    \
                for (uint64_t i = 0; i < self->cap; i++) {                     \
                        self->data[i].free = 1;                                \
                }                                                              \
                self->len = 0;                                                 \
                for (uint64_t i = 0; i < old_cap; i++) {                       \
                        NAME##_set_node *n = old + i;                          \
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

// mixin string bounded
static uint64_t djb2(char *str, uint64_t len)
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

// mixin integer (and casted float/double)
static uint64_t hash64shift(uint64_t key)
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
