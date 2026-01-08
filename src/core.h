#pragma once
/* optional assert */
#if defined(COPS_ASSERT_ENABLE)
#include <assert.h>
#define COPS_ASSERT(x) assert(x)
#else
#undef COPS_ASSERT
#define COPS_ASSERT(x) ((void)0)
#endif

/* allocate with libc by default */
#if !defined(COPS_ALLOC)
#include <stdlib.h>
#define COPS_ALLOC(sz) malloc(sz)
#define COPS_REALLOC(ptr, sz) realloc(ptr, sz)
#define COPS_FREE(ptr) free(ptr)
#endif

/* default return value to check */
#define COPS_OK ((int)0)
#define COPS_INVALID ((int)-1)
#define COPS_MEMERR ((int)-2)

/* slice declaration for interop between collections/prototypes */
#include <stdint.h>
#define init_slice(T, name)                                                    \
        typedef struct name {                                                  \
                uint64_t len;                                                  \
                void (*free)(T);                                               \
                T (*dup)(T);                                                   \
                T data[];                                                      \
        } name;                                                                \
        static inline name *name##_new(uint64_t len)                           \
        {                                                                      \
                name *self = COPS_ALLOC(sizeof(*self) + sizeof(T) * len);      \
                COPS_ASSERT(self);                                             \
                self->len = len;                                               \
                return self;                                                   \
        }                                                                      \
        static inline void name##_free(name *self) { COPS_FREE(self); }

/* variadic macro utility */
#define __cops_get_macro(_1, _2, _3, _4, macro, ...) macro
#define __cops_expand(x) x
#define __cops_err_arg_count(...) char __cops_err_arg_count[-1]
#define __cops_err_var_macro(...) char
