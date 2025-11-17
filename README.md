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
        void (*free)(void *);
} cops_allocator;

static inline void *cops_alloc(size_t size) { return calloc(size, 1); }
const cops_allocator cops_default_allocator = {cops_alloc, free};
```

_If I want to use my arena?_ **no problemo**

This structure consent to rewrite `alloc` and `free` in any way prefeared  

The only request, instead of linking a custom library upfront before `-libc`, is to mutate the item `cops_default_allocator` pointers to keep easy insertion of custom arena inside data structure objects.

## Array

```c
init_cops_arr(T)
```

Static Array with refcount and lenght, a small payload for a safer generic array.

**Structure**:  
- `len` data lenght
- `rc` reference counter (smart pointer)
- `data` variable array to hold statically data

**API**:  
- new = initialize new object
- dup = duplicate (adjust rc)
- free = free memory (adjust rc)

Those **API** are in common with all the other classes to handle multithreading in a simple way for a fundamental structure like oset or vec.

## Vector

```c
init_cops_vec(T)
```

Dynamic Array with refcount, lenght and capacity.   
Classic vector, start with a default size to avoid primary reallocation, not prone to be used outside the lenght, data is public and open to iterate throw it (like in array).

**Structure**: 
- `rc` reference count (smart pointer)
- `nelem` number of element saved
- `cap` current max capacity
- `data` array containing values
- `free` function pointer to free an element (nullable)
- `dup` function pointer to duplicate an element (nullable)

**API**:  
- push = insert in the top a value
- pop = remove from the top
- set = overright value in a (valid) position
- get = retrieve value from a (valid) position
- import = import all value from another (same type) vector without side effect

If you want to sort it just use `qsort(vec->data, vec->nelem, sizeof(vec->data[0]), my_cmp_func)`.  
No need to reinvent the wheel (for ordered set there are other useful object)

# List

```c
init_cops_list(T)
```

Double linked list

**Structure**:  
- `<name>_node` linked list node structure
    - `next` next node
    - `prev` previous node
    - `val` value
- `nelem` number of nodes
- `rc` reference counter (smart pointer)
- `head`  start of the list
- `tail` end of the list
- `free` function pointer to free an element (nullable)

**API**:  
- push_front = add new head
- push_back = add new tail
- pop_front = remoev head
- pop_back = remove tail
- add_next = add next to a node
- add_prev = add prev to a node
- del = delete node

minimal utility to handle queue without circular array and without priority

## Map

```c
init_cops_map(K, V)
```

Hashmap with some implemented hash function.  
vector logic behind data structure, robin hood brobe mechanic with a quadratic probing.  
`(i * i + 1) / 2`

**Structure**: 
- `<name>_node` key-value map node
    - `flag` robin hodd flag, 0x80 free, 0x40 tombstone, other are jump occurred
    - `key` hashmap key (unique)
    - `val` hasmap value
- `rc` reference count (smart pointer)
- `nelem` number of element saved
- `cap` current max capacity
- `data` array containing values
- `free_key` function pointer to free a node key (nullable)
- `free_val` function pointer to free a node value (nullable)
- `dup_key` function pointer update rc of node key (nullable)
- `dup_val` function pointer update rc of node value (nullable)

**API**:  
- add = add key-value pair if key doesn't exist
- set = update value on map node if key exist
- has = check if key exist, 1 if true, 0 if false
- del = remove map node if key exist
- get = retrieve value from a (valid) position
- import = import all value from another (same type) map with side effect


## Set

```c
init_cops_set(T)
```


Hashset with some implemented hash function.  
vector logic behind data structure, robin hood brobe mechanic with a quadratic probing.  
`(i * i + 1) / 2`

**Structure**: 
- `<name>_node` set node with robin hood flag
    - `flag` robin hodd flag, 0x80 free, 0x40 tombstone, other are jump occurred
    - `val` hasset value
- `rc` reference count (smart pointer)
- `nelem` number of element saved
- `cap` current max capacity
- `data` array containing values
- `free` function pointer to free a node value (nullable)
- `dup` function pointer update rc of node value (nullable)

**API**:  
- add = add key-value pair if key doesn't exist
- set = update value on map node if key exist
- has = check if key exist, 1 if true, 0 if false
- del = remove map node if key exist
- get = retrieve value from a (valid) position
- import = import all value from another (same type) map with side effect

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
