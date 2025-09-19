#include "../src/arr.h"
#include "test.h"
#include <stdio.h>
#include <string.h>

#define FORMATTER(name, T) __init_cops_arr(name, T)
FORMATTER(intarr, int)
#undef FORMATTER

void print_intarr(intarr *a);
void print_string(string *a);

#define N (10)

int test_arr(int type)
{
        printf("~~~~~~~~~~   test cops_arr   ~~~~~~~~~~\n");
        if (type != 1) {
                printf("\n - create new array:\n");
                intarr *i_arr = intarr_new(10);
                if (!i_arr) {
                        printf(" > " Rf "failed allocation" D "!\n");
                        return 1;
                }
                print_intarr(i_arr);
                printf("\n - duplicate array " I "(%d times)" D "\n", N);
                intarr *arr_int[N];
                for (int i = 0; i < N; i++) {
                        arr_int[i] = intarr_dup(i_arr);
                        // printf(" > refcount:" Cf "%d" D "\n", arr[i]->rc);
                }
                if (!i_arr || i_arr->rc != 11)
                        return 1;
                print_intarr(i_arr);
                printf("\n - dereference arrays " I "(%d times)" D "\n", N);
                for (int i = 0; i < N; i++) {
                        arr_int[i] = intarr_free(arr_int[i]);
                        // printf(" > refcount:" Cf "%d" D "\n", i_arr->rc);
                }
                if (!i_arr || i_arr->rc != 1)
                        return 1;
                print_intarr(i_arr);
                printf("\n - dereference last array\n");
                i_arr = intarr_free(i_arr);
                if (i_arr)
                        return 1;
                print_intarr(i_arr);
                printf("\n - create empty array\n");
                i_arr = intarr_new(0);
                print_intarr(i_arr);
                printf("\n - dereference empty array\n");
                i_arr = intarr_free(i_arr);
                print_intarr(i_arr);

                printf("\n - create new char array:\n");
                string *c_arr = string_new(20);
                strncpy(c_arr->data, "a bad cafe fr fr", c_arr->len - 1);
                c_arr->data[c_arr->len - 1] = '\0';
                if (!c_arr) {
                        printf(" > " Rf "failed allocation" D "!\n");
                        return 1;
                }
                print_string(c_arr);
                printf("\n - duplicate array " I "(%d times)" D "\n", N);
                string *arr_char[N];
                for (int i = 0; i < N; i++) {
                        arr_char[i] = string_dup(c_arr);
                        // printf(" > refcount:" Cf "%d" D "\n", arr[i]->rc);
                }
                if (!c_arr || c_arr->rc != 11)
                        return 1;
                print_string(c_arr);
                printf("\n - dereference arrays " I "(%d times)" D "\n", N);
                for (int i = 0; i < N; i++) {
                        arr_char[i] = string_free(arr_char[i]);
                        // printf(" > refcount:" Cf "%d" D "\n", c_arr->rc);
                }
                if (!c_arr || c_arr->rc != 1)
                        return 1;
                print_string(c_arr);
                printf("\n - dereference last array\n");
                c_arr = string_free(c_arr);
                if (c_arr)
                        return 1;
                print_string(c_arr);
                printf("\n - create empty array\n");
                c_arr = string_new(0);
                print_string(c_arr);
                printf("\n - dereference empty array\n");
                c_arr = string_free(c_arr);
                print_string(c_arr);
        }

        printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
        return 0;
}

void print_intarr(intarr *a)
{
        if (!a) {
                printf(" > " B "(" Gf "%p" D B " ref:" Yf "  0" D B ")" D "\n", (void *)a);
                return;
        }
        printf(" > " B "(" Bf "%p" D B " ref:" Rf "%3d" D B ")" D "[" Yf "%3d" D "]:\n\t{",
               (void *)a, a->rc, a->len);
        for (size_t i = 0; i < a->len; i++) {
                printf(Gf I "%d" D, a->data[i]);
                if (i != a->len - 1)
                        printf(",");
        }
        printf("}\n");
}

void print_string(string *a)
{
        if (!a) {
                printf(" > " B "(" Gf "%p" D B " ref:" Yf "  0" D B ")" D "\n", (void *)a);
                return;
        }
        printf(" > " B "(" Bf "%p" D B " ref:" Rf "%3d" D B ")" D "[" Yf "%3d" D "]:\n\t{",
               (void *)a, a->rc, a->len);
        for (size_t i = 0; i < a->len; i++) {
                if (a->data[i] >= ' ' && a->data[i] <= '~')
                        printf("'" Cf I "%c" D "'", a->data[i]);
                else
                        printf(Gf I "0x%02x" D, a->data[i]);
                if (i != a->len - 1)
                        printf(",");
        }
        printf("}\n");
}
