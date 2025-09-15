![COPS logo](cops-logo.svg)

# COPS

**C Object Precompile Subsistem**  

This is a simple implementation for every kind of base data structure as public object with macro, as light as possible.

There are two kind of macro, one with default naming convention, one with custom naming (user responsability to avoid collision), this macro are all checked to avoid name collision (pretty rare in my opinion).

The macro with given name are `init_cops_` + _object type_.  
The macro with rename are `__init_cops_` + _object type_.  

## Allocator

```c
typedef struct cops_allocator {
        void *(*alloc)(size_t);
        void *(*realloc)(void *, size_t);
        void (*free)(void *);
} cops_allocator;

static inline void *cops_alloc(size_t size) { return calloc(size, 1); }
const cops_allocator cops_default_allocator = {cops_alloc, realloc, free};
```

_If I want to use my arena?_ **no problemo**

This structure consent to rewrite `alloc`, `realloc` and `free` in any way prefeared (_technically realloc is not really used for now_)

The only request, instead of linking a custom library upfront before `-libc`, is to mutate the item `cops_default_allocator` pointers to keep easy insertion of custom arena inside data structure objects.

## Array

```c
init_cops_arr(T)
```

Static Array with refcount and lenght, a small payload for a safer generic array.

## Vector

```c
init_cops_vec(T)
```

Dynamic Array with refcount, lenght and capacity (log capacity).   
Classic vector, start with a default size to avoid primary reallocation, not prone to be used outside the lenght, data is public and open to iterate throw it (like in array).

## Map

```c
init_cops_map(K, V)
```

Hashmap, require an hash function and a compare function at creation, is given a string hash function (djb2).  
in the hasmap is used triangular/quadratic probing in a power of 2 size, robin hood logic to balance jump, in this way we avoid chunking and mitigate probing with robin hood logic.

this kind of implementation is used in rust and other language standard library implementation

## Set

```c
init_cops_set(T)
```

Hashset, porting map logic to set implementation.  
Set with cmp and hash function can be used with a struct like is a map but with a more simple "mind logic" in my opinion

## Omap

```c
init_cops_omap(K, V)
```

Red-Black tree map, a pristine implementation of red black tree BST balancing logic.

This implementation have `add`, `has`, `mod`, `del` to insert, peek, set and remove a node (key-value).  
Unrequired API exposed are node rotation and node struct but required for BST implementation.

## Oset

```c
init_cops_oset(T)
```

as for unordered set, this too have same implementation of omap
