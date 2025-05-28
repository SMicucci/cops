#include <cops/core/allocator.h>
#include <cops/core/interface.h>
#include <cops/core/string.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define COPS_STRING_BASE_CAP 8

static size_t __strnlen(const char *string, size_t maxlen)
{
        size_t res = 0;
        while (res < maxlen && *(string++))
                res++;
        return res;
}

static void _init(void *self_);
static void _clean(cops_interface_t *self_);
static int _compare(const cops_interface_t *self,
                    const cops_interface_t *other);
static unsigned long _hash(const cops_interface_t *self);
static void *_clone(const cops_interface_t *self);
static void string_cap(cops_string_t *self, size_t min_len);

static cops_interface_t __string = {_init, _clean, _compare,
                                    _hash, _clone, sizeof(cops_string_t)};

const cops_interface_t *const CopsString = &__string;

/* public definition */

void string_init(cops_string_t *self, const char *value)
{
        size_t new_len = __strnlen(value, 0xffff);
        if (new_len == 0xffff)
                return;
        self->super = __string;
        self->cap = COPS_STRING_BASE_CAP;
        self->str = cops_alloc(self->cap);
        if (!self->str)
                return;
        string_cap(self, new_len);
        strncpy(self->str, value, new_len);
        self->len = new_len;
}

cops_string_t *string_clone(const cops_string_t *self)
{
        cops_string_t *copy = cops_alloc(sizeof(*copy));
        if (!copy)
                return NULL;
        copy->cap = self->cap;
        copy->len = self->len;
        copy->str = cops_alloc(copy->cap);
        for (size_t i = 0; i < copy->cap; i++) {
                copy->str[i] = self->str[i];
        }
        return copy;
}

void string_trim(cops_string_t *self)
{
        size_t start = 0, end = 0;
        for (size_t i = 0; i < self->len; i++) {
                char trg = self->str[i];
                if (trg <= 0x20) {
                        if (!end)
                                start = i + 1;
                } else {
                        end = i;
                }
        }
        for (size_t i = 0; i < self->cap; i++) {
                if (i <= end - start)
                        self->str[i] = self->str[start + i];
                else
                        self->str[i] = 0;
        }
        self->len = end - start + 1;
}
/*
 "  test  ":8
    ^  ^
    2  5
*/

cops_string_t *string_substring(cops_string_t *self, size_t start, size_t end)
{
        if (start > self->len)
                return NULL;
        if (!end || end > self->len)
                end = self->len;
        if (!start && end == self->len)
                return string_clone(self);
        cops_string_t *sub = cops_alloc(sizeof(*sub));
        if (!sub)
                return NULL;
        size_t len = end - start;
        char *str = cops_alloc(len + 1);
        if (!str)
                return NULL;
        memcpy(str, &(self->str[start]), len);
        str[len] = 0;
        string_init(sub, str);
        cops_dealloc(str);
        return sub;
}

void string_join(cops_string_t *self, const cops_string_t *other)
{
        size_t new_len = self->len + other->len;
        string_cap(self, new_len);
        void *app_str = (int8_t *)self->str + self->len;
        if (!(&(self->str[self->len]) == app_str))
                printf("DEBUG: (%p) = (%p)\n", &(self->str[self->len]),
                       app_str);
        memcpy(app_str, other->str, other->len);
        self->len = new_len;
        (void)app_str;
}
// 0x9310
// 0xa40c

void string_clean(cops_string_t *self)
{
        cops_dealloc(self->str);
        self->cap = 0;
        self->len = 0;
}

char *string_cstring(cops_string_t *self)
{
        char *res = cops_alloc(self->len);
        strncpy(res, self->str, self->len);
        res[self->len] = 0;
        return res;
}

int string_compare(const cops_string_t *self, const cops_string_t *other)
{
        size_t safe_len = self->len > other->len ? other->len : self->len;
        return strncmp(self->str, other->str, safe_len);
}

unsigned long string_hash(const cops_string_t *self)
{
        size_t hash = 5381;
        for (size_t i = 0; i < self->len; i++) {
                hash = ((hash << 5) + hash) + self->str[i];
        }
        return hash;
}

/* static definition */

static void _init(void *self_)
{
        string_init(self_, "");
        return;
}

static void _clean(cops_interface_t *self)
{
        string_clean((cops_string_t *)self);
        return;
}

static int _compare(const cops_interface_t *self, const cops_interface_t *other)
{
        return string_compare((cops_string_t *)self, (cops_string_t *)other);
}

static unsigned long _hash(const cops_interface_t *self)
{
        return string_hash((cops_string_t *)self);
}

static void *_clone(const cops_interface_t *self)
{
        return (void *)string_clone((cops_string_t *)self);
}

static void string_cap(cops_string_t *self, size_t min_len)
{
        if (min_len <= self->cap)
                return;
        while (min_len > self->cap) {
                self->cap *= 2;
        }
        char *str = cops_alloc(self->cap);
        if (!str)
                return;
        memcpy(str, self->str, self->len);
        cops_dealloc(self->str);
        self->str = str;
}
