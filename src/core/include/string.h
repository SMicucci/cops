#ifndef COPS_CORE_STRING_H
#define COPS_CORE_STRING_H

#include <cops/core/interface.h>
#include <stddef.h>
typedef struct cops_string cops_string_t;

// String instance
extern const cops_interface_t *const CopsString;

struct cops_string {
        cops_interface_t super;
        char *str;
        size_t cap;
        size_t len;
};

/*
 * get memory and initialize
 * */
void string_init(cops_string_t *self, const char *value);

/*
 * allocate memory (cops_allocator) and return a clone
 * */
cops_string_t *string_clone(const cops_string_t *self);

/*
 * remove spaces in the tail and head
 * */
void string_trim(cops_string_t *self);

/*
 * return substring,
 * overflow or 0 at end return end,
 * overflow in the start return NULL
 * */
cops_string_t *string_substring(cops_string_t *self, size_t start, size_t end);

/*
 * concatenate other into first
 * */
void string_join(cops_string_t *self, const cops_string_t *other);

/*
 * free internal memory gracefully
 * */
void string_clean(cops_string_t *self);

/*
 * return (unsafe) standard string
 * */
char *string_cstring(cops_string_t *self);

/*
 * compare two string (-1 second greater, 0 equal, 1 first greater)
 * wrap strcmp safetly
 * */
int string_compare(const cops_string_t *self, const cops_string_t *other);

/*
 * hash string using djb2
 * */
unsigned long string_hash(const cops_string_t *self);

#endif /* end of include guard: COPS_CORE_STRING_H */
