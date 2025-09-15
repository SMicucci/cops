#ifndef COPS_STRING_H
#define COPS_STRING_H
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef struct cops_string {
        uint32_t len;
        uint32_t rc;
        char str[];
} cops_string;

/**
 * string structure initializer with fixed len
 * max len is UINT16_MAX
 */
static inline cops_string *cops_string_new_len(size_t len, char *str)
{
        if (len > UINT16_MAX)
                return NULL;
        cops_string *res =
            (cops_string *)malloc(sizeof(*res) + (size_t)len + 1);
        if (!res)
                return res;
        res->len = (uint32_t)len;
        res->rc = 1;
        memcpy(res->str, str, len);
        res->str[res->len] = 0;
        return res;
}

/**
 * string structure initializer
 * max len is UINT16_MAX
 */
static inline cops_string *cops_string_new(char *str)
{
        size_t len = strnlen(str, UINT16_MAX);
        cops_string *res =
            (cops_string *)malloc(sizeof(*res) + (size_t)len + 1);
        if (!res)
                return res;
        res->len = (uint32_t)len;
        res->rc = 1;
        memcpy(res->str, str, len);
        res->str[res->len] = 0;
        return res;
}

/**
 * create a new reference to the string
 */
static inline cops_string *cops_string_dup(cops_string *self)
{
        if (self == NULL)
                return self;
        self->rc++;
        return self;
}

/*
 * remove a reference and return the updated pointer
 *
 * @warning is mandatory to override the freed reference with the return value
 */
static inline cops_string *cops_string_free(cops_string *self)
{
        if (self == NULL)
                return self;
        if (self->rc) {
                self->rc--;
        }
        free(self);
        return NULL;
}

#endif /* ifndef COPS_STRING_H */
