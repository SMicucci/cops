#include <cops/core/allocator.h>
#include <stddef.h>
#include <stdlib.h>

static void *_alloc(size_t size) { return calloc(1, size); }
static void _dealloc(void *ptr) { free(ptr); }

cops_allocator_t cops_baseAllocator = {_alloc, _dealloc};

void *cops_alloc(size_t size) { return cops_baseAllocator.alloc(size); }
void cops_dealloc(void *ptr) { cops_baseAllocator.dealloc(ptr); }
