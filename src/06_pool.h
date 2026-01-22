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
                for (uint64_t i = 0; i < bites; i++) {                         \
                        self->map[i] = 0x00;                                   \
                }                                                              \
        }                                                                      \
                                                                               \
        static inline NAME##_slab *__##NAME##_slab_new(uint64_t len)           \
        {                                                                      \
                NAME##_slab *res = COPS_ALLOC(sizeof(T) * len + sizeof(*res)); \
                COPS_ASSERT(res);                                              \
                if (!res)                                                      \
                        return NULL;                                           \
                res->map = COPS_ALLOC(__cops_bitmap_size(len));                \
                COPS_ASSERT(res->map);                                         \
                if (!res->map) {                                               \
                        COPS_FREE(res);                                        \
                        return NULL;                                           \
                }                                                              \
                __##NAME##_slab_reset(res, len);                               \
                res->next = NULL;                                              \
                return res;                                                    \
        }                                                                      \
                                                                               \
        static inline T *__##NAME##_slab_alloc(NAME##_slab *self,              \
                                               uint64_t len)                   \
        {                                                                      \
                uint64_t bites = __cops_bitmap_size(len);                      \
                int64_t trg_byte = -1;                                         \
                for (uint64_t i = 0; i < bites; i++) {                         \
                        if (self->map[i] == 0xFF)                              \
                                continue;                                      \
                        trg_byte = i;                                          \
                        break;                                                 \
                }                                                              \
                if (trg_byte == -1)                                            \
                        return NULL;                                           \
                for (uint8_t j = 0; j < 8; j++) {                              \
                        if (((1 << j) & self->map[trg_byte]))                  \
                                continue;                                      \
                        int64_t pos = trg_byte * 8 + j;                        \
                        if (pos >= len)                                        \
                                return NULL;                                   \
                        self->map[trg_byte] |= (1 << j);                       \
                        return self->data + pos;                               \
                }                                                              \
        }                                                                      \
                                                                               \
        static inline int __##NAME##_slab_release(NAME##_slab *self, T *ptr)   \
        {                                                                      \
                return 0;                                                      \
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
                        int res = __##NAME##_slab_release(trg, mem);           \
                        if (res)                                               \
                                return;                                        \
                        trg = trg->next;                                       \
                }                                                              \
        }                                                                      \
        \
