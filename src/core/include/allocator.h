#ifndef COPS_CORE_ALLOCATOR_H
#define COPS_CORE_ALLOCATOR_H

#include <stddef.h>

typedef struct cops_allocator cops_allocator_t;

struct cops_allocator {
        void *(*alloc)(size_t);
        void (*dealloc)(void *);
};

void *cops_alloc(size_t size);
void cops_dealloc(void *ptr);

extern cops_allocator_t cops_baseAllocator;

#endif /* end of include guard: COPS_CORE_ALLOCATOR_H */
