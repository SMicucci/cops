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
#if defined(COPS_IMPLEMENTATION)
#define init_slice(T, NAME)                                                    \
        __cops_slice_decl(T, NAME) __cops_slice_impl(T, NAME)
#else
#define init_slice(T, NAME) __cops_slice_decl(T, NAME);
#endif /* if defined(COPS_IMPLEMENTATION) */

#define __cops_slice_decl(T, NAME)                                             \
        typedef struct NAME {                                                  \
                uint64_t len;                                                  \
                T data[];                                                      \
        } NAME;                                                                \
        NAME *NAME##_new(uint64_t len);                                        \
        void NAME##_free(NAME *self);

#define __cops_slice_impl(T, NAME)                                             \
        NAME *NAME##_new(uint64_t len)                                         \
        {                                                                      \
                NAME *self = COPS_ALLOC(sizeof(*self) + sizeof(T) * len);      \
                COPS_ASSERT(self && "failed allocation");                      \
                self->len = len;                                               \
                return self;                                                   \
        }                                                                      \
        void NAME##_free(NAME *self)                                           \
        {                                                                      \
                COPS_ASSERT(self && "invalid reference");                      \
                if (!self)                                                     \
                        return;                                                \
                COPS_FREE(self);                                               \
        }

/* variadic macro utility */
#define __cops_get_macro(_1, _2, _3, _4, macro, ...) macro
#define __cops_expand(x) x
#define __cops_err_arg_count(...) char __cops_err_arg_count[-1]
#define __cops_err_var_macro(...) char
