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
        static inline int NAME##_push_front(NAME *self, T val) {}              \
        static inline int NAME##_push_back(NAME *self, T val) {}               \
        static inline int NAME##_pop_front(NAME *self, T *res) {}              \
        static inline int NAME##_pop_back(NAME *self, T *res) {}

#define __cops_init_list_3(T, NAME, SLICE_T) __cops_init_list_2(T, NAME);
