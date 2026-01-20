![COPS logo](cops-logo.svg)

# COPS

**C Organized Plain Structures**  

This library implement macro of collections, import once, use anywhere.

Integrated and interchangable collection are:  
- Slice _(variadic lenght array)_
- Vec _(dinamyc array)_
- List _(double linked list)_
- Hset _(robin hood hash set)_
- Tset _(red-black tree set)_

## Quick Start

To mantain usability, clone "cops.h", create you own centered header (like this example "collection.h")

```c
#ifndef COLLETION_H
#define COLLETION_H

// use only in dev, will enable assert instead of handling error
#define COPS_ASSERT_ENABLE
#include "cops.h"

init_slice(int, int_slice);
init_vec(int, int_vec, int_slice);
init_set(int, int_set, int_slice);
init_oset(int, int_oset, int_slice);
#endif /* end guard #define COLLETION_H */
```

In the main file you should use `COPS_IMPLEMENTATION` to enable implementation (classic stb-pattern).  
```c
#define COPS_IMPLEMENTATION
#include "collection.h"

int main(int argc, char *argv[]) {
//...
```

## Configuration Macro

This macro shall be used **before** the `#include`:  

| Macro | Description |
| --- | --- |
| `COPS_OK` | function returned correctly |
| `COPS_INVALID` | function made something wrong |
| `COPS_MEMERR` | function failed allocation |
| `COPS_ASSERT_ENABLE` | on error run `assert` |
| `COPS_IMPLEMENTATION` | enable function implementation |
| `COPS_ALLOC` | override libc `malloc` |
| `COPS_REALLOC` | override libc `realloc` |
| `COPS_FREE` | override libc `free` |


**Note**: If you declare `COPS_ALLOC` you should define all three, if not you would encounted undefined behavior

## Core Concept
Coherent implementations exist in the library to simplify usage and navigability.

### 1. Ownership patterns (`dup` and `free`)
Collection can manage memory. Each collection (except slice) contain two optional function pointer.
- `T (*dup)(T)`: if set, collection return clone.  
- `void (*free)(T)`: if set, collection take ownership of input.  

### 2. API Design Pattern
COPS relay on consistent API pattern.  
`int collection_func(NAME *self, [args..], [T *res]);`
- **Error as result**: Posix behavior to communicate status with `int` return value
- **Output parameter**: Secondary output could be optionally obtained by `T *res` who is ignored when `NULL`

### 3. Interoperability of Collections
Slice work as intermidiate representation between collections (vec, list, hset, tset).  
- `collection_export`: Create a slice containing all elements (return a borrowed value, if `dup` is declared return copies)  
- `collection_import`: Bulk-inserts from slice  

## Roadmap
- **`pool`**: memory pool (list of slab allocator), non interoperable  
- **`tset`**: implement interoperability  
- **`pqueue`**: priority queue (heap) implementation, interoperable  
- **`mgraph`**: matric graph (matrix bitmap + node vec) implementation, non interoperable  
- **`lgraph`**: light graph (node and edge vec) implementation, non interoperable  

# Collection Reference

## Slice

```c
struct slice {
    uint64_t len;
    T data[];
};
```


| API | Input | Return | Description |
| :-: | :-: | :-: | --- |
| `new` | `uint64_t len` | `*self` | create new slice |
| `free` | `NAME *self` | `status` | destroy given slice |


## Vec

```c
struct vec {
    uint64_t len;
    uint64_t cap;
    T *data;
    void (*free)(T);
    T (*dup)(T);
};
```

> Sorting is not implemented but can be performed in this manner:  
>
> ```c
> #include <stdlib.h>
> qsort(self->data, sizeof(*self->data), self->len, my_cmp_func);
> ```

| API | Input | Return | Description |
| :-: | --- | :-: | --- |
| `new` | `uint64_t len` | `*self` | create new slice |
| `free` | `NAME *self` | `status` | destroy given slice |
| `reset` | `NAME *self` | `status` | reset slice length |
| `push` | `NAME *self, T val` | `status` | push back val |
| `pop` | `NAME *self, T *res` | `status` | pop back res |
| `get` | `NAME *self, uint64_t pos, T val` | `status` | get val from pos |
| `set` | `NAME *self, uint64_t pos, T val, T *res` | `status` | update pos entry with val, could get res |
| `insert` | `NAME *self, uint64_t pos, T val` | `status` | insert new val in pos |
| `remove` | `NAME *self, uint64_t pos, T val, T *res` | `status` | delete val in pos, could get res |
| `import` | `NAME *self, slice *slice` | `status` | bulk inserts and free slice |
| `export` | `NAME *self` | `*slice` | create slice as representation |


## List

```c
struct ll_node {
    ll_node *next;
    ll_node *prev;
    T val;
};
struct list {
    uint64_t len;
    ll_node *head;
    ll_node *tail;
    void (*free)(T);
    T (*dup)(T);
};
```


| API | Input | Return | Description |
| :-: | :-- | :-: | :-- |
| `new` | `uint64_t len` | `*self` | create new slice |
| `free` | `NAME *self` | `status` | destroy given slice |
| `push_front` | `NAME *self, T val` | `status` | push head val |
| `push_back` | `NAME *self, T val` | `status` | push tail val |
| `pop_front` | `NAME *self, T *res` | `status` | pop head res |
| `pop_back` | `NAME *self, T *res` | `status` | pop tail res |
| `insert_before` | `NAME *self, ll_node *node, T val` | `status` | insert val before node |
| `insert_after` | `NAME *self, ll_node *node, T val` | `status` | insert val after node |
| `remove` | `NAME *self, ll_node *node, T val, list_node *res` | `status` | delete val in pos, could get res |
| `import` | `NAME *self, slice *slice` | `status` | bulk inserts and free slice |
| `export` | `NAME *self` | `*slice` | create slice as representation |


## Hset

```c
struct rh_node {
    uint8_t free: 1;
    uint8_t tomb: 1;
    uint8_t jumps: 6;
    T val;
};
struct list {
    uint64_t len;
    uint64_t cap;
    rh_node *data;
    uint64_t (*hash)(T);
    int (*cmp)(T, T);
    void (*free)(T);
    T (*dup)(T);
};
```


| API | Input | Return | Description |
| :-: | --- | :-: | --- |
| `new` | `uint64_t len` | `*self` | create new slice |
| `free` | `NAME *self` | `status` | destroy given slice |
| `set` | `NAME *self, T val` | `status` | update val if exist |
| `get` | `NAME *self, uint64_t pos, T val, T *res` | `status` | search val if exist and get res |
| `has` | `NAME *self, uint64_t pos, T val` | `bool` | search if val exist |
| `push` | `NAME *self, T val` | `status` | push back val |
| `pop` | `NAME *self, T *res` | `status` | pop back res |
| `add` | `NAME *self, T val` | `status` | insert new val if not exist |
| `del` | `NAME *self, T val` | `status` | delete val if exist |
| `import` | `NAME *self, slice *slice` | `status` | bulk inserts and free slice |
| `export` | `NAME *self` | `*slice` | create slice as representation |


## Oset

```c
struct rb_node {
    rb_node *parent;
    rb_node *left;
    rb_node *right;
    uint8_t isred;
    T val;
};

struct list {
    uint64_t len;
    rb_node *root;
    int (*cmp)(T, T);
    void (*free)(T);
    T (*dup)(T);
};
```


| API | Input | Return | Description |
| :-: | --- | :-: | --- |
| `new` | `uint64_t len` | `*self` | create new slice |
| `free` | `NAME *self` | `status` | destroy given slice |
| `has` | `NAME *self, T val` | `bool` | search if val exist |
| `add` | `NAME *self, T val` | `status` | insert new val if not exist |
| `del` | `NAME *self, T *val` | `status` | delete val if exist |
| `import` | `NAME *self, slice *slice` | `status` | bulk inserts and free slice |
| `export` | `NAME *self` | `*slice` | create slice as representation |

