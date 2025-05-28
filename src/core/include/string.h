#ifndef COPS_CORE_STRING_H
#define COPS_CORE_STRING_H

#include <cops/core/interface.h>
#include <stddef.h>
typedef struct cops_string cops_string_t;

extern const cops_interface_t *const CopsString;

struct cops_string {
        cops_interface_t super;
        char *str;
        size_t cap;
        size_t len;
};

void string_init(cops_string_t *self, const char *value);
cops_string_t *string_clone(const cops_string_t *self);
void string_trim(cops_string_t *self);
cops_string_t *string_substring(cops_string_t *self, size_t start, size_t end);
void string_join(cops_string_t *self, const cops_string_t *other);
void string_clean(cops_string_t *self);
char *string_cstring(cops_string_t *self);
int string_compare(const cops_string_t *self, const cops_string_t *other);
unsigned long string_hash(const cops_string_t *self);

#endif /* end of include guard: COPS_CORE_STRING_H */
