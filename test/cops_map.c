#include "../src/arr.h"
#include "../src/map.h"
#include "test.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define X(name, K, V) __init_cops_map(name, K, V)
X(map, string *, double)
X(fmap, long, long)
#undef X

static inline int _cmp(string *s1, string *s2) { return strcmp(s1->data, s2->data); }
static inline size_t _hash(string *s) { return djb2(s->data); }
static inline void _free(string *s) { string_free(s); }

static inline int _fcmp(long l1, long l2) { return (l1 > l2) - (l1 < l2); }
static inline size_t _fhash(long l) { return longhash(l); }

void print_map(map *m);

int test_map(int type)
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

        srand((unsigned int)time(NULL));
        printf("~~~~~~~~~~   test cops_map   ~~~~~~~~~~\n");
        if (type != 1) {
                printf("\n - create new map\n");
                map *m = map_new(_hash, _cmp);
                m->free_key = _free;
                print_map(m);
                printf("\n - insert some key\n");
                const int N = 27;
                int r = rand() % 64;
                for (int i = 0; i < N * 2; i++) {
                        char *t = names[i];
                        size_t l = strlen(t);
                        string *s = string_new(l + 1);
                        strcpy(s->data, t);
                        if (map_add(m, s, (double)(rand() % 100))) {
                                printf("('%s' exist yet!)\n", s->data);
                                s = string_free(s);
                                i--;
                        }
                        r += 3;
                        r %= 64;
                }
                print_map(m);
                printf("\n - remove some key\n");
                printf("   > deleted: {");
                r = rand() % 64;
                for (int i = 0; i < N; i++) {
                        char *t = names[r];
                        size_t l = strlen(t);
                        string *s = string_new(l + 1);
                        strcpy(s->data, t);
                        if (map_has(m, s) == 1) {
                                double d = 0;
                                map_del(m, s, &d);
                                printf("\"" Gf "%s" D "\"", t);
                                if (i != (N / 2) - 1)
                                        printf(", ");
                        } else {
                                i--;
                        }
                        r++;
                        r %= 64;
                        s = string_free(s);
                }
                printf("}\n\n");
                print_map(m);
                printf("\n - change some value\n");
                r = rand() % 64;
                printf("   > modified: {");
                for (int i = 0; i < N; i++) {
                        char *t = names[r];
                        size_t l = strlen(t);
                        string *s = string_new(l + 1);
                        strcpy(s->data, t);
                        if (map_has(m, s) == 1) {
                                double d;
                                printf("\"" Gf "%s" D "\":", t);
                                map_get(m, s, &d);
                                printf(Bf "%.1f" D "", d);
                                map_set(m, s, -2.0);
                                if (i != (N / 2) - 1)
                                        printf(", ");
                        } else {
                                i--;
                        }
                        r++;
                        r %= 64;
                        s = string_free(s);
                }
                printf("}\n\n");
                print_map(m);
                printf("\n - free map\n");
                m = map_free(m);
                print_map(m);
                printf("\n - new maps\n");
                map *m1 = map_new(_hash, _cmp);
                map *m2 = map_new(_hash, _cmp);
                m1->free_key = _free;
                m2->free_key = _free;
                m1->dup_key = string_dup;
                m2->dup_key = string_dup;
                for (int i = 0; i < 8; i++) {
                        char *t = names[i];
                        size_t l = strlen(t);
                        string *s = string_new(l + 1);
                        strcpy(s->data, t);
                        map_add(m1, s, (double)i);
                }
                for (int i = 6; i < 13; i++) {
                        char *t = names[i];
                        size_t l = strlen(t);
                        string *s = string_new(l + 1);
                        strcpy(s->data, t);
                        map_add(m2, s, (double)-i);
                }
                print_map(m1);
                print_map(m2);
                printf("\n - import maps\n");
                map_import(m1, m2);
                print_map(m1);
                m2 = map_free(m2);
                m1 = map_free(m1);
        }

        printf("\n~~~~~~~~~~  " Yf B Pb "performance test" D " ~~~~~~~~~~\n");
        for (int k = 0; k < 3; k++) {
                printf("\n");
                fmap *f = fmap_new(_fhash, _fcmp);
                const int N = 1 << (20 + (k << 1));
                const int M = 1000000;
                char strval[28];
                format_number(N, strval);
                printf(" > (" Gf "%12s insert" D ")", strval);
                clock_t s, e;
                s = clock();
                for (long i = 0; i < N; i++) {
                        fmap_add(f, i, i);
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
                        fmap_set(f, pos, i);
                }
                e = clock();
                elapse = (double)(e - s) * 1000.0 / CLOCKS_PER_SEC;
                avg = (double)(e - s) * 1000000.0 / CLOCKS_PER_SEC / (double)M;
                printf(" %.3f ms\n", elapse);
                printf(" > (      " Rf "avg set" D "      ) %.6f \u03bcs\n", avg);
                f = fmap_free(f);
        }
        printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
        return 0;
}

void print_map(map *m)
{
        if (!m) {
                printf(Cf "map" Yf "<" Bf "string" D "," Bf "double" Yf ">" D " [" Rf " 0" D "/" Rf
                          " 0" D "] (" B "refcount: " Rf "0" D ")\n");
                return;
        }
        printf(Cf "map" Yf "<" Bf "string" D "," Bf "double" Yf ">" D " [" Gf "%2d" D "/" Gf "%2d" D
                  "] (" B "refcount: " Pf "%d" D ")\n",
               m->nelem, m->cap, m->rc);
        for (size_t i = 0; i < m->cap; i++) {
                map_node d = m->data[i];
                if (d.flag >= 0x40) {
                        printf(" [" Rf "%2ld" D "]: " Yf "0x%02x" D " " Yf "(nil)" D " " Rf "0.0" D
                               "\n",
                               i, d.flag);
                } else {
                        printf(" [" Bf "%2ld" D "]: " Cf "0x%02x" D " \"" Gf "%s" D "\"(rc:%d) " Bf
                               "%.1f" D "\n",
                               i, d.flag, d.key->data, d.key->rc, d.val);
                }
        }
}
