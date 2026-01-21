#include "01_core.h"

#if defined(COPS_IMPLEMENTATION)
#define __cops_init_pool(T, NAME)                                              \
        __cops_init_pool_decl(T, NAME) __cops_init_pool_impl(T, NAME)
#else
#define __cops_init_pool(T, NAME) __cops_init_pool_decl(T, NAME)
#endif /* if defined(COPS_IMPLEMENTATION) */

#define __cops_pool_slab_vla(len)                                              \
        ((uint64_t)((uint64_t)((uint64_t)len + 7) / (uint64_t)8))

#define __cops_init_pool_decl(T, NAME)                                         \
        typedef struct NAME##_slab NAME##_slab;                                \
        typedef struct NAME {                                                  \
                NAME##_slab *head;                                             \
                NAME##_slab *tail;                                             \
        } NAME;                                                                \
        void NAME##_new(NAME *self, uint64_t pool_len);                        \
        void NAME##_free(NAME *self);                                          \
        void NAME##_reset(NAME *self);                                         \
        T *NAME##_alloc(NAME *self);                                           \
        void NAME##_release(NAME *self, T *mem);

#define __cops_init_pool_impl(T, NAME)                                         \
        typedef struct NAME##_bitmap {                                         \
                uint64_t len;                                                  \
                uint8_t map[];                                                 \
        } NAME##_bitmap;                                                       \
                                                                               \
        struct NAME##_slab {                                                   \
                NAME##_slab *next;                                             \
                NAME##_bitmap *map;                                            \
                T data;                                                        \
        };                                                                     \
                                                                               \
        static inline __##NAME##_slab *NAME##_slab_new(uint64_t len)           \
        {                                                                      \
                NAME##_slab *res = COPS_ALLOC(sizeof(T) * len + sizeof(*res)); \
                COPS_ASSERT(res);                                              \
                if (!res)                                                      \
                        return NULL;                                           \
                uint64_t map_len =                                             \
                    sizeof(NAME##_bitmap) + __cops_pool_slab_vla(len);         \
                res->map = COPS_ALLOC(map_len);                                \
                COPS_ASSERT(res->map);                                         \
                if (!res->map) {                                               \
                        COPS_FREE(res);                                        \
                        return NULL;                                           \
                }                                                              \
                res->next = NULL;                                              \
                return res;                                                    \
        }                                                                      \
                                                                               \
        static inline void __##NAME##_slab_reset(NAME##_slab *self)            \
        {                                                                      \
                uint64_t bites = __cops_pool_slab_vla(len);                    \
                for (uint64_t i = 0; i < bites; i++) {                         \
                        self->map[i] = 0x00;                                   \
                }                                                              \
        }                                                                      \
                                                                               \
        static inline T *__##NAME##__slab_alloc(NAME##_slab *self) {}          \
                                                                               \
        static inline int __##NAME##__slab_release(NAME##_slab *self) {}       \
                                                                               \
        void NAME##_new(NAME *self, uint64_t pool_len)                         \
        {                                                                      \
                COPS_ASSERT(res);                                              \
                if (!res)                                                      \
                        return NULL;                                           \
                res->head = NAME##_slab_new(pool_len);                         \
                COPS_ASSERT(res->head);                                        \
                if (!res->head)                                                \
                        return NULL;                                           \
                res->tail = res->head;                                         \
                return res;                                                    \
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
                        __##NAME##_slab_reset(trg);                            \
                        trg = trg->next;                                       \
                }                                                              \
        }                                                                      \
                                                                               \
        T *NAME##_alloc(NAME *self)                                            \
        {                                                                      \
                COPS_ASSERT(self);                                             \
                if (!self)                                                     \
                        return;                                                \
                T *res = NULL;                                                 \
                NAME##_slab *trg = self->head;                                 \
                while (trg) {                                                  \
                        res = __##NAME##_slab_alloc(trg);                      \
                        if (res)                                               \
                                return res;                                    \
                        trg = trg->next;                                       \
                }                                                              \
                /* update tail and alloc */                                    \
                self->tail->next = NAME##_slab_new(self->head->map->len);      \
                self->tail = self->tail->next;                                 \
                res = __##NAME##_slab_alloc(self->tail);                       \
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
                        int res = __##NAME##_slab_release(trg);                \
                        if (res)                                               \
                                return;                                        \
                        trg = trg->next;                                       \
                }                                                              \
        }                                                                      \
        \
