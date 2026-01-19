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

## Library usage

To use this library in a mantainable way is recomended to hold all collection in a single header file to use around the project.  

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

In the main file you should use `COPS_IMPLEMENT` to enable implementation.  
```c
#define COPS_IMPLEMENT
#include "collection.h"

int main(int argc, char *argv[]) {
//...
```

## Library behavior

This macro shall be used before the `#include`:  
- `COPS_ASSERT_ENABLE` enable assertion in the library when declared
- `COPS_IMPLEMENT` enable implementation of functions (standard stb behavior)
- `COPS_ALLOC` override libc `malloc`
- `COPS_REALLOC` override libc `realloc`
- `COPS_FREE` override libc `free`

the libc alternative have to be declared all three or could lead to undefued behavior

---

```c
init_collection(T, NAME, SLICE_T);
```

all collection except the Slice have 2 possible initialization setup:
- fast `(T, NAME)`: deny conversion to other kind of collection to cut short on cerimony
- complete `(T, NAME, SLICE_T)`: require `init_slice(T, SLICE_T)` to allow conversion between collection

## Common Concept

All collection share those concept and are always true

```c
void (*free)(T)
```
Function pointer to hold the free implementation of the element `T`.  
If functino pointer is not declared the item passed are considered borrowed without any warranty against read after free.  
free, if present, is used at collection free or when value is removed `T *res` argument `NULL`

```c
T (*dup)(T)
```
Function pointer to hold the clone implementation of the element `T`.  
If function pointer is not declared the exported slice will hold the ownership of exported data.  

```c
SLICE_T *collection_export(NAME *self);
```
export function return an array unordered of the data with lenght boundary in it.  
slice is the common join between collection types.  

```c
int collection_import(NAME *self, SLICE_T *slice);
```
import function return error as result, consent a bulk insert of a slice.  

```c
int  collection_func(NAME *self, ...);
```
All collection function return status as error.  
- `COPS_OK` => 0, nothing wrong happened
- `COPS_INVALID` => -1, invalid state happened (invalid pointer most common case)
- `COPS_MEMERR` => -2, memory allocation given error  

all these error when `COPS_ASSERT_ENABLE` is defined, trigger an assertion with little description.

```c

```

## Slice
Generic VLA useful for compact data storage and convertion between container.  
Each instance have `len` value as boundaries, doesn't know if all value are valid.  
Collection implements following API:  
- `collection *collection_new(uint64_t len)` for correct allocation
- `void collection_free(collection *self)` for correct release

## Vec
Generic dynamic array implementation, unordered for fast internal operation.  
Sorting is not implemented but can be performed in this manner:  
```c
#include <stdlib.h>
qsort(self->data, sizeof(*self->data), self->len, my_cmp_func);
```
Each instance have `data` as external storage, `cap` as storage size, `len` as valid element, `free` and `dup` as optional parameter for handling ownership (__for easy of use an arena could be a favourite choice__).  
Collection implements following API:  
- `collection *collection_new()`
- `void collection_free(collection *self)`
- `int collection_reset(collection *self)`
- `int collection_push(collection *self, T val)`
- `int collection_pop(collection *self, T *res)`
- `int collection_set(collection *self, uint64_t pos, T val, T *res)`
- `int collection_get(collection *self, uint64_t pos, T *res)`
- `int collection_insert(collection *self, uint64_t pos, T val, T *res)`
- `int collection_remove(collection *self, uint64_t pos, T *res)`
- `int collection_import(collection *self, slice *slice)`
- `slice *collection_export(collection *self)`

## List


## Set
## Oset
