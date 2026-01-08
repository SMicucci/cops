![COPS logo](cops-logo.svg)

# COPS

**C Object Preprocessing Subsistem**  

This library implement with macro collection object, import one, use anywhere.

the supported collection are:  
- VLA _(variadic lenght array)_
- Vector _(dinamyc array)_
- List _(double linked list)_
- Set _(hash set)_
- Oset _(rb-tree set)_

## Library usage

To use this library in a simple way is recomended to hold all collection in a single header file to use around the project.  

```c
#ifndef COLLETION_H
#define COLLETION_H

#define COPS_ASSERT_ENABLE
#include "cops.h"

init_slice(int, int_slice);
init_vec(int, int_vec, int_slice);
init_set(int, int_set, int_slice);
init_oset(int, int_oset, int_slice);
#endif /* end guard #define COLLETION_H */
```

## Library behavior

This macro shall be used before the `#include`:  
- `COPS_ASSERT_ENABLE`: enable assertion in the library when declared
- `COPS_ALLOC`: override libc `malloc`
- `COPS_REALLOC`: override libc `realloc`
- `COPS_FREE`: override libc `free`

the libc alternative have to be declared all three or could lead to undefued behavior

---

```c
init_collection(T, NAME, SLICE_T);
```

all collection except the slice/VLA have 2 possible initialization setup:
- fast `(T, NAME)`: fast initialization without conversion to other kind of collection to cut short on cerimony
- complete `(T, NAME, SLICE_T)`: complete initialization, require `init_slice(T, NAME2)` to be linked so collection can use slice to import and export data and convert into other collection type

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

all these error trigger an assertion if enabled, if not return this error instead.


