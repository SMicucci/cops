#include "../src/arr.h"
#include "../src/set.h"
#include "test.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct user {
        long id;
        string *name;
} user_t;

static inline size_t _user_hash(user_t u) { return longhash((unsigned long)u.id); }
static inline int _user_cmp(user_t u1, user_t u2) { return (u1.id > u2.id) - (u1.id < u2.id); }
static inline void _user_free(user_t u) { string_free(u.name); }
static inline user_t _user_dup(user_t u) { return (user_t){u.id, string_dup(u.name)}; }

#define X(name, T) __init_cops_set(name, T)
X(set, user_t)
X(fast_set, long)
#undef X

static inline int _cmp(string *s1, string *s2) { return strcmp(s1->data, s2->data); }
static inline size_t _hash(string *s) { return djb2(s->data); }

static inline int _fcmp(long l1, long l2) { return (l1 > l2) - (l1 < l2); }
static inline size_t _fhash(long l) { return longhash(l); }

static inline void _free_str(string *s) { string_free(s); }

void print_set(set *s);

int test_set(int type)
{
        char *names[] = {"apple",    "book",     "cat",     "dog",       "elephant", "flower",
                         "grape",    "house",    "island",  "jungle",    "kite",     "lemon",
                         "mountain", "notebook", "ocean",   "piano",     "queen",    "river",
                         "sun",      "tree",     "cloud",   "stone",     "forest",   "valley",
                         "desert",   "hill",     "star",    "planet",    "comet",    "galaxy",
                         "universe", "light",    "shadow",  "flame",     "storm",    "rain",
                         "snow",     "wind",     "sand",    "lake",      "bridge",   "tower",
                         "castle",   "garden",   "field",   "path",      "road",     "harbor",
                         "canyon",   "meadow",   "cliff",   "spring",    "cave",     "harvest",
                         "dawn",     "ember",    "peak",    "riverbank", "mirror",   "crown",
                         "torch",    "temple",   "lantern", "village"};

        printf("~~~~~~~~~~   test cops_set   ~~~~~~~~~~\n");
        if (type != 1) {
                printf("\n - create new set\n");
                set *m = set_new(_user_hash, _user_cmp);
                m->free = _user_free;
                m->dup = _user_dup;
                print_set(m);
                printf("\n - insert some key\n");
                const int N = 6;
                for (int i = 0; i < N * 2; i++) {
                        user_t s;
                        char *t = names[i];
                        size_t l = strlen(t);
                        s.name = string_new(l + 1);
                        s.id = i;
                        strcpy(s.name->data, t);
                        if (set_add(m, s)) {
                                printf("('%s' exist yet!)\n", s.name->data);
                                s.name = string_free(s.name);
                        }
                }
                print_set(m);
                printf("\n - remove some key\n");
                printf("   > deleted: {");
                for (int i = 0; i < N; i++) {
                        user_t s;
                        char *t = names[i];
                        size_t l = strlen(t);
                        s.name = string_new(l + 1);
                        s.id = i;
                        strcpy(s.name->data, t);
                        if (set_has(m, s) == 1) {
                                set_del(m, s);
                                printf("\"" Gf "%s" D "\"", t);
                                if (i != (N / 2) - 1)
                                        printf(", ");
                        }
                        s.name = string_free(s.name);
                }
                printf("}\n\n");
                print_set(m);
                printf("\n - change some value\n");
                printf("   > modified: [");
                for (int i = N; i < N * 2; i++) {
                        user_t s;
                        char *t = "~~~ update ~~~";
                        size_t l = strlen(t);
                        s.name = string_new(l + 1);
                        s.id = i;
                        strcpy(s.name->data, t);
                        if (set_has(m, s) == 1) {
                                user_t u;
                                printf("\"" Gf "%s" D "\":", t);
                                set_get(m, s, &u);
                                printf(Bf "{id:" Bf B "%02ld" D " name:'" B Gf "%s" D "'}", u.id,
                                       u.name->data);
                                set_set(m, s);
                                if (i != (N / 2) - 1)
                                        printf(", ");
                        }
                }
                printf("]\n\n");
                print_set(m);
                printf("\n - free set\n");
                m = set_free(m);
                print_set(m);
                printf("\n - new sets\n");
                set *m1 = set_new(_user_hash, _user_cmp);
                set *m2 = set_new(_user_hash, _user_cmp);
                m1->free = _user_free;
                m1->dup = _user_dup;
                m2->free = _user_free;
                m2->dup = _user_dup;
                for (int i = 0; i < 8; i++) {
                        user_t s;
                        char *t = names[i];
                        size_t l = strlen(t);
                        s.name = string_new(l + 1);
                        s.id = i;
                        strcpy(s.name->data, t);
                        if (set_add(m1, s)) {
                                printf("   (%02ld, '%s' exist yet!)\n", s.id, s.name->data);
                                s.name = string_free(s.name);
                        }
                }
                for (int i = 6; i < 13; i++) {
                        user_t s;
                        char *t = names[i];
                        size_t l = strlen(t);
                        s.name = string_new(l + 1);
                        s.id = i;
                        strcpy(s.name->data, t);
                        if (set_add(m2, s)) {
                                printf("   (%02ld, '%s' exist yet!)\n", s.id, s.name->data);
                                s.name = string_free(s.name);
                        }
                }
                print_set(m1);
                print_set(m2);
                printf("\n - import set\n");
                set_import(m1, m2);
                print_set(m1);
                m1 = set_free(m1);
                m2 = set_free(m2);
        }

        printf("\n~~~~~~~~~~  " Yf B Pb "performance test" D " ~~~~~~~~~~\n");
        for (int k = 0; k < 3; k++) {
                printf("\n");
                fast_set *f = fast_set_new(_fhash, _fcmp);
                const int N = 1 << (20 + (k << 1));
                const int M = 100000000;
                char strval[28];
                format_number(N, strval);
                printf(" > (" Gf "%12s insert" D ")", strval);
                clock_t s, e;
                s = clock();
                for (long i = 0; i < N; i++) {
                        fast_set_add(f, i);
                }
                e = clock();
                double elapse = (double)(e - s) * 1000.0 / CLOCKS_PER_SEC;
                double avg = (double)(e - s) * 1000000.0 / CLOCKS_PER_SEC / (double)N;
                printf(" %.3f ms\n", elapse);
                printf(" > (        " Cf "avg" D "        ) %.6f \u03bcs\n", avg);
                format_number(M, strval);
                printf(" > (" Cf "%12s update" D ")", strval);
                s = clock();
                for (int i = 0; i < M; i++) {
                        size_t pos = rand() % N;
                        fast_set_set(f, pos);
                }
                e = clock();
                elapse = (double)(e - s) * 1000.0 / CLOCKS_PER_SEC;
                avg = (double)(e - s) * 1000000.0 / CLOCKS_PER_SEC / (double)M;
                printf(" %.3f ms\n", elapse);
                printf(" > (      " Rf "avg set" D "      ) %.6f \u03bcs\n", avg);
                f = fast_set_free(f);
        }
        printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
        return 0;
}

void print_set(set *s)
{
        if (!s) {
                printf(Cf "set" Yf "<" Bf "double" Yf ">" D " [" Rf " 0" D "/" Rf " 0" D "] (" B
                          "refcount: " Rf "0" D ")\n");
                return;
        }
        printf(Cf "set" Yf "<" Bf "double" Yf ">" D " [" Gf "%2d" D "/" Gf "%2d" D "] (" B
                  "refcount: " Pf "%d" D ")\n",
               s->nelem, s->cap, s->rc);
        for (size_t i = 0; i < s->cap; i++) {
                set_node d = s->data[i];
                if (d.flag >= 0x40) {
                        printf(" [" Rf "%2ld" D "]: " Yf "0x%02x" D " " Yf "(nil)" D " " Rf "0.0" D
                               "\n",
                               i, d.flag);
                } else {
                        printf(" [" Bf "%2ld" D "]: " Cf "0x%02x" D " {id:" Bf B "%02ld" D
                               " name:'" Gf B "%s" D "'(rc:" I Yf "%d" D ")}\n",
                               i, d.flag, d.val.id, d.val.name->data, d.val.name->rc);
                }
        }
}
