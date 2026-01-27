#include "01_core.h"

#define init_pool(T, NAME) __cops_init_pool(T, NAME)

#if defined(COPS_IMPLEMENTATION)
#define __cops_init_pool(T, NAME)                                              \
        __cops_init_pool_decl(T, NAME) __cops_init_pool_impl(T, NAME)
#else
#define __cops_init_pool(T, NAME) __cops_init_pool_decl(T, NAME)
#endif /* if defined(COPS_IMPLEMENTATION) */

#define __cops_bitmap_size(len)                                                \
        ((uint64_t)((uint64_t)((uint64_t)len + 7) / (uint64_t)8))

#define __cops_first_zero(x)                                                   \
        !(~(uint64_t)(x)) +                                                    \
            (const unsigned char                                               \
                 []){63, 0,  1,  52, 2,  6,  53, 26, 3,  37, 40, 7,  33,       \
                     54, 47, 27, 61, 4,  38, 45, 43, 41, 21, 8,  23, 34,       \
                     58, 55, 48, 17, 28, 10, 62, 51, 5,  25, 36, 39, 32,       \
                     46, 60, 44, 42, 20, 22, 57, 16, 9,  50, 24, 35, 31,       \
                     59, 19, 56, 15, 49, 30, 18, 14, 29, 13, 12, 11}           \
                [((~(uint64_t)(x) & -~(uint64_t)(x)) * 0x045FBAC7992A70DA) >>  \
                 58]

#define __cops_init_pool_decl(T, NAME)                                         \
        typedef struct NAME##_slab NAME##_slab;                                \
        typedef struct NAME {                                                  \
                NAME##_slab *head;                                             \
                NAME##_slab *tail;                                             \
                uint64_t len;                                                  \
        } NAME;                                                                \
        void NAME##_new(NAME *self, uint64_t len);                             \
        void NAME##_free(NAME *self);                                          \
        void NAME##_reset(NAME *self);                                         \
        T *NAME##_alloc(NAME *self);                                           \
        void NAME##_release(NAME *self, T *mem);

#define __cops_init_pool_impl(T, NAME)                                         \
        struct NAME##_slab {                                                   \
                NAME##_slab *next;                                             \
                uint8_t *map;                                                  \
                T data[];                                                      \
        };                                                                     \
                                                                               \
        static inline void __##NAME##_slab_reset(NAME##_slab *self,            \
                                                 uint64_t len)                 \
        {                                                                      \
                uint64_t bites = __cops_bitmap_size(len);                      \
                /* TODO: speed up */                                           \
                for (uint64_t i = 0; i < bites; i++) {                         \
                        self->map[i] = 0x00;                                   \
                }                                                              \
        }                                                                      \
                                                                               \
        static inline NAME##_slab *__##NAME##_slab_new(uint64_t len)           \
        {                                                                      \
                /* allocate slab */                                            \
                NAME##_slab *res = COPS_ALLOC(sizeof(T) * len + sizeof(*res)); \
                COPS_ASSERT(res);                                              \
                if (!res)                                                      \
                        return NULL;                                           \
                /* allocate map */                                             \
                res->map = COPS_ALLOC(__cops_bitmap_size(len));                \
                COPS_ASSERT(res->map);                                         \
                if (!res->map) {                                               \
                        COPS_FREE(res);                                        \
                        return NULL;                                           \
                }                                                              \
                /* zero data */                                                \
                __##NAME##_slab_reset(res, len);                               \
                res->next = NULL;                                              \
                return res;                                                    \
        }                                                                      \
                                                                               \
        /* slab free is handled by pool_free */                                \
                                                                               \
        static inline T *__##NAME##_slab_alloc(NAME##_slab *self,              \
                                               uint64_t len)                   \
        {                                                                      \
                uint64_t bites = __cops_bitmap_size(len);                      \
                for (uint64_t i = 0; i < bites; i += 8) {                      \
                        /* populate proxy value (avoid dirty read) */          \
                        uint64_t map_word = 0;                                 \
                        for (uint64_t j = 0; j < 8 && (i + j) < bites; j++) {  \
                                map_word |= (uint64_t)self->map[i + j]         \
                                            << (j * 8);                        \
                        }                                                      \
                        /* scan and try allocate */                            \
                        uint64_t first_zero = __cops_first_zero(map_word);     \
                        if (first_zero < 64) {                                 \
                                uint64_t pos = i * 8 + first_zero;             \
                                if (pos >= len)                                \
                                        return NULL;                           \
                                /* bookkeeping */                              \
                                self->map[pos / 8] |= (1 << (pos % 8));        \
                                return self->data + pos;                       \
                        }                                                      \
                }                                                              \
                return NULL;                                                   \
        }                                                                      \
                                                                               \
        /* 1: is released, 0: not owned */                                     \
        static inline int __##NAME##_slab_release(NAME##_slab *self,           \
                                                  uint64_t len, T *ptr)        \
        {                                                                      \
                uintptr_t start = (uintptr_t)self->data;                       \
                uintptr_t end = start + sizeof(T) * len;                       \
                if ((uintptr_t)ptr < start || (uintptr_t)ptr >= end)           \
                        return 0;                                              \
                uint64_t pos = ((uintptr_t)ptr - start) / sizeof(T);           \
                self->map[pos / 8] &= ~(1 << (pos % 8));                       \
                return 1;                                                      \
        }                                                                      \
                                                                               \
        void NAME##_new(NAME *self, uint64_t len)                              \
        {                                                                      \
                COPS_ASSERT(self);                                             \
                if (!self)                                                     \
                        return;                                                \
                self->head = __##NAME##_slab_new(len);                         \
                COPS_ASSERT(self->head);                                       \
                if (!self->head)                                               \
                        return;                                                \
                self->len = len;                                               \
                self->tail = self->head;                                       \
                return;                                                        \
        }                                                                      \
                                                                               \
        void NAME##_free(NAME *self)                                           \
        {                                                                      \
                COPS_ASSERT(self);                                             \
                if (!self)                                                     \
                        return;                                                \
                NAME##_slab *trg = self->head;                                 \
                while (trg) {                                                  \
                        NAME##_slab *next = trg->next;                         \
                        COPS_FREE(trg->map);                                   \
                        COPS_FREE(trg);                                        \
                        trg = next;                                            \
                }                                                              \
        }                                                                      \
                                                                               \
        void NAME##_reset(NAME *self)                                          \
        {                                                                      \
                COPS_ASSERT(self);                                             \
                if (!self)                                                     \
                        return;                                                \
                NAME##_slab *trg = self->head;                                 \
                while (trg) {                                                  \
                        __##NAME##_slab_reset(trg, self->len);                 \
                        trg = trg->next;                                       \
                }                                                              \
        }                                                                      \
                                                                               \
        T *NAME##_alloc(NAME *self)                                            \
        {                                                                      \
                COPS_ASSERT(self);                                             \
                if (!self)                                                     \
                        return NULL;                                           \
                T *res = NULL;                                                 \
                NAME##_slab *trg = self->head;                                 \
                while (trg) {                                                  \
                        res = __##NAME##_slab_alloc(trg, self->len);           \
                        if (res)                                               \
                                return res;                                    \
                        trg = trg->next;                                       \
                }                                                              \
                /* update tail and alloc */                                    \
                self->tail->next = __##NAME##_slab_new(self->len);             \
                self->tail = self->tail->next;                                 \
                res = __##NAME##_slab_alloc(self->tail, self->len);            \
                return res;                                                    \
        }                                                                      \
                                                                               \
        void NAME##_release(NAME *self, T *mem)                                \
        {                                                                      \
                COPS_ASSERT(self);                                             \
                if (!self)                                                     \
                        return;                                                \
                NAME##_slab *trg = self->head;                                 \
                while (trg) {                                                  \
                        int res =                                              \
                            __##NAME##_slab_release(trg, self->len, mem);      \
                        if (res)                                               \
                                return;                                        \
                        trg = trg->next;                                       \
                }                                                              \
        }
