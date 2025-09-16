#include "../src/vec.h"
#include "test.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

struct test {
        long id;
        char *name;
        unsigned char age;
};

#define FORMATTER(name, T) __init_cops_vec(name, T)
FORMATTER(vint, int)
FORMATTER(vtest, struct test)
FORMATTER(str_vec, char *)
#undef FORMATTER

void print_vtest(vtest *v);
void print_vint(vint *v);
void print_str_vec(str_vec *v);

static inline void format_number(unsigned int n, char res[28]);
static inline int pow_(int base, int exp);

static inline void str_free(char *mem);

int test_vec()
{
        char *names[] = {
            "apple",  "book",   "cat",    "dog",    "elephant", "flower",   "grape",    "house",
            "island", "jungle", "kite",   "lemon",  "mountain", "notebook", "ocean",    "piano",
            "queen",  "river",  "sun",    "tree",   "cloud",    "stone",    "forest",   "valley",
            "desert", "hill",   "star",   "planet", "comet",    "galaxy",   "universe", "light",
            "shadow", "flame",  "storm",  "rain",   "snow",     "wind",     "sand",     "lake",
            "bridge", "tower",  "castle", "garden", "field",    "path",     "road",     "harbor",
            "canyon", "meadow", "cliff",  "spring", "cave",     "harvest",  "dawn",     "ember",
            "peak"};
        printf("~~~~~~~~~~   test cops_vec   ~~~~~~~~~~\n");
        printf("\n - create new test vector\n");
        vtest *v2 = vtest_new();
        print_vtest(v2);
        printf("\n - increase refcount\n");
        vtest *v2_arr[4];
        for (int i = 0; i < 4; i++) {
                if (i != 0)
                        v2_arr[i] = vtest_dup(v2_arr[i - 1]);
                else
                        v2_arr[i] = vtest_dup(v2);
        }
        print_vtest(v2);
        printf("\n - insert some element\n");
        for (int i = 0; i < 14; i++) {
                if (vtest_push(v2, (struct test){(30 + (i * i + i) / 2), names[i], 15 + i / 2})) {
                        printf("failed insertion!\n");
                        return 1;
                }
        }
        print_vtest(v2);
        printf("\n - get and set some element\n");
        for (unsigned int i = 0; i < v2->nelem; i++) {
                if (!(i % 3)) {
                        struct test t;
                        vtest_get(v2, i, &t);
                        t.name = "~~~~~ mod ~~~~~";
                        t.age += 30;
                        vtest_set(v2, i, t);
                }
        }
        print_vtest(v2);
        printf("\n - delete all element\n");
        for (unsigned int i = 0; i < (v2->nelem / 2); i++) {
                vtest_pop(v2, NULL);
        }
        print_vtest(v2);
        printf("\n - decrease refcount\n");
        for (int i = 0; i < 4; i++) {
                v2_arr[i] = vtest_free(v2_arr[i]);
        }
        print_vtest(v2);
        printf("\n - free test vector\n");
        v2 = vtest_free(v2);
        print_vtest(v2);

        printf("\n - new string vector\n");
        str_vec *v3 = str_vec_new();
        v3->free = str_free;
        print_str_vec(v3);
        printf("\n - insert some string\n");
        for (int i = 0; i < 14; i++) {
                int t = rand() % 50;
                const char *val = names[t];
                size_t len = strlen(val);
                char *trg = cops_default_allocator.alloc(len + 1);
                strcpy(trg, val);
                str_vec_push(v3, trg);
        }
        print_str_vec(v3);
        printf("\n - free string vector\n");
        v3 = str_vec_free(v3);
        print_str_vec(v3);

        printf("\n~~~~~~~~~~  " Yb B Pf "performance test" D " ~~~~~~~~~~\n");
        for (int k = 2; k < 10; k++) {
                printf("\n");
                vint *v = vint_new();
                const int N = 1 << (18 + (k));
                srand((unsigned int)time(NULL));
                clock_t s, e;
                s = clock();
                for (int i = 0; i < N; i++) {
                        int val = rand() % 1000;
                        vint_push(v, val);
                        if (rand() % 2)
                                vint_pop(v, NULL);
                }
                e = clock();
                double elapse = (double)(e - s) * 1000.0 / CLOCKS_PER_SEC;
                double avg = (double)(e - s) * 1000000.0 / CLOCKS_PER_SEC / (double)N;
                char strval[28];
                format_number(N, strval);
                printf(" > (" Gf "%12s insert" D ") %.3f ms\n", strval, elapse);
                printf(" > (        " Cf "avg" D "        ) %.6f \u03bcs\n", avg);
                v = vint_free(v);
        }
        printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
        return 0;
}

void print_vtest(vtest *v)
{
        if (!v) {
                printf(Bf "vector" Yf "<" Cf "struct test" Yf ">" D "[" B "0" D "] (ref: " Pf "0" D
                          ", " Gf "%p" D " )\n",
                       (void *)v);
                return;
        }
        printf(Bf "vector" Yf "<" Cf "struct test" Yf ">" D "[" Rf "%2d" D "] (ref:" Pf "%2d" D
                  ", " Bf "%p" D " )\n",
               v->nelem, v->rc, (void *)v);
        for (size_t i = 0; i < (size_t)(v->cap); i++) {
                struct test d = v->data[i];
                if (i < (size_t)v->nelem)
                        printf(" [" Gf "%02d" D "] {" Cf "id" D ": " Bf B "%ld" D ", " Cf "name" D
                               ": " B Gf "\"%s\"" D ", " Cf "age" D ": " Bf B "%d" D "}",
                               (int)i, d.id, d.name, d.age);
                else
                        printf(" [" Pf "%02d" D "] {" Cf "id" D ": " Rf "0" D ", " Cf "name" D
                               ": " Rf "(nil)" D ", " Cf "age" D ": " Rf "0" D "}",
                               (int)i);
                if (i != (size_t)(v->cap - 1))
                        printf(",");
                printf("\n");
        }
}

void print_str_vec(str_vec *v)
{
        if (!v) {
                printf(Bf "vector" Yf "<" Cf "char*" Yf ">" D "[" B "0" D "] (ref: " Pf "0" D
                          ", " Gf "%p" D " )\n",
                       (void *)v);
                return;
        }
        printf(Bf "vector" Yf "<" Cf "char*" Yf ">" D "[" Rf "%2d" D "] (ref:" Pf "%2d" D ", " Bf
                  "%p" D " )\n",
               v->nelem, v->rc, (void *)v);
        for (size_t i = 0; i < (size_t)(v->cap); i++) {
                char *d = v->data[i];
                if (i < (size_t)v->nelem)
                        printf(" [" Bf "%02d" D "]: '" Gf B "%s" D "'", (int)i, d);
                else
                        printf(" [" Pf "%02d" D "]: " Cf B "%p" D, (int)i, (void *)d);
                if (i != (size_t)(v->cap - 1))
                        printf(",");
                printf("\n");
        }
}

#ifndef C
#define C (7)
#endif

void print_vint(vint *v)
{
        if (!v) {
                printf(Bf "vector" Yf "<" Cf "int" Yf ">" D "[" B "0" D "] (ref: " Pf "0" D ", " Gf
                          "%p" D " )\n",
                       (void *)v);
                return;
        }
        printf(Bf "vector" Yf "<" Cf "struct test" Yf ">" D "[" Rf "%2d" D "] (ref:" Pf "%2d" D
                  ", " Bf "%p" D " )\n",
               v->nelem, v->rc, (void *)v);
        for (size_t i = 0; i < (size_t)(v->cap); i++) {
                int d = v->data[i];
                if (i < (size_t)v->nelem)
                        printf(" [" Gf "%04d" D "]: " B Bf "%3d" D, (int)i, d);
                else
                        printf(" [" Pf "%04d" D "]: " Cf "   0" D, (int)i);
                if (i != (size_t)(v->cap) - 1)
                        printf(",");
                if ((i % C) == (C - 1) || i == (size_t)(v->cap) - 1)
                        printf("\n");
        }
}

#ifdef C
#undef C
#endif

static inline int pow_(int base, int exp)
{
        int res = 1;
        for (int i = 0; i < exp; i++) {
                res *= base;
        }
        return res;
}

static inline void format_number(unsigned int n, char res[28])
{
        int dept = 0, tmp = n;
        while (tmp > 9) {
                tmp = tmp / 10;
                dept++;
        }
        char *c = res;
        tmp = n;
        for (int i = dept; i >= 0; i--) {
                int p = pow_(10, i);
                int val = tmp / p;
                *c++ = (char)(val + 48);
                tmp -= val * p;
                if (!(i % 3) && i)
                        *c++ = '\'';
        }
        *c = '\0';
}

static inline void str_free(char *mem)
{
        if (mem)
                cops_default_allocator.free((void *)mem);
}
