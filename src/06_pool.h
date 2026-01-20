#include "01_core.h"

#if defined(COPS_IMPLEMENTATION)
#define __cops_init_pool(T, NAME)                                              \
        __cops_init_pool_decl(T, NAME) __cops_init_pool_impl(T, NAME)
#else
#define __cops_init_pool(T, NAME) __cops_init_pool_decl(T, NAME)
#endif /* if defined(COPS_IMPLEMENTATION) */

#define __cops_init_pool_decl(T, NAME)
#define __cops_init_pool_impl(T, NAME)
