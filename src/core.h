#ifndef COPS_CORE_H
#define COPS_CORE_H

/**
 * DON'T IMPORT DIRECTLY
 * INTERNAL USAGE ONLY
 */

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define COPS_OK ((int)0)
#define COPS_INVALID ((int)-1)
#define COPS_ABORT ((int)-2)
#define COPS_MEMERR ((int)-3)

/**
 * basic string hash function
 */
static inline size_t djb2(const char *s)
{
        size_t hash = 5381;
        int c;
        while ((c = *s++))
                hash = ((hash << 5) + hash) + c;
        return hash;
}

static inline size_t longhash(unsigned long l)
{
        size_t x = (size_t)l;
        x = (~x) + (x << 21);
        x = x ^ (x >> 24);
        x = (x + (x << 3)) + (x << 8);
        x = x ^ (x >> 14);
        x = (x + (x << 2)) + (x << 4);
        x = x + (x << 28);
        x = x + (x << 31);
        return x;
}

static inline void *cops_alloc(size_t size) { return calloc(1, size); }

/**
 * rewritable allocation in macros:
 *
 * - alloc:   zeroed new memory slot
 * - realloc: extend or port to new block
 * - free:    release memory
 */
typedef struct cops_allocator {
        void *(*alloc)(size_t);
        void *(*realloc)(void *, size_t);
        void (*free)(void *);
} cops_allocator;

// default allocator used in all the macros
static const cops_allocator cops_default_allocator = {cops_alloc, realloc, free};

#ifdef __init_cops_list
#error "'__init_cops_list' is a reserved word for cops library"
#endif
#ifdef init_cops_list
#error "'init_cops_list' is a reserved word for cops library"
#endif
#ifdef __init_cops_arr
#error "'__init_cops_arr' is a reserved word for cops library"
#endif
#ifdef init_cops_arr
#error "'init_cops_arr' is a reserved word for cops library"
#endif
#ifdef __init_cops_vec
#error "'__init_cops_vec' is a reserved word for cops library"
#endif
#ifdef init_cops_vec
#error "'init_cops_vec' is a reserved word for cops library"
#endif
#ifdef __init_cops_map
#error "'__init_cops_map' is a reserved word for cops library"
#endif
#ifdef init_cops_map
#error "'init_cops_map' is a reserved word for cops library"
#endif
#ifdef __init_cops_set
#error "'__init_cops_set' is a reserved word for cops library"
#endif
#ifdef init_cops_set
#error "'init_cops_set' is a reserved word for cops library"
#endif
#ifdef __init_cops_omap
#error "'__init_cops_omap' is a reserved word for cops library"
#endif
#ifdef init_cops_omap
#error "'init_cops_omap' is a reserved word for cops library"
#endif
#ifdef __init_cops_oset
#error "'__init_cops_oset' is a reserved word for cops library"
#endif
#ifdef init_cops_oset
#error "'init_cops_oset' is a reserved word for cops library"
#endif

#ifdef __cplusplus
}
#endif
#endif /* end of include guard: COPS_CORE_H */
