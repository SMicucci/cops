#include "../src/arr.h"
#include "../src/omap.h"
#include "test.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define X(name, K, V) __init_cops_omap(name, K, V)
X(map, string *, double)
X(f_omap, long, long)
#undef X

static inline int _cmp(string *s1, string *s2) { return strcmp(s1->data, s2->data); }
static inline void _free(string *s) { string_free(s); }

static inline int _fcmp(long l1, long l2) { return (l1 > l2) - (l1 < l2); }

void print_omap(map *m);

int test_omap(int type)
{
        // char *names[] = {"apple",    "book",     "cat",     "dog",       "elephant", "flower",
        //                  "grape",    "house",    "island",  "jungle",    "kite",     "lemon",
        //                  "mountain", "notebook", "ocean",   "piano",     "queen",    "river",
        //                  "sun",      "tree",     "cloud",   "stone",     "forest",   "valley",
        //                  "desert",   "hill",     "star",    "planet",    "comet",    "galaxy",
        //                  "universe", "light",    "shadow",  "flame",     "storm",    "rain",
        //                  "snow",     "wind",     "sand",    "lake",      "bridge",   "tower",
        //                  "castle",   "garden",   "field",   "path",      "road",     "harbor",
        //                  "canyon",   "meadow",   "cliff",   "spring",    "cave",     "harvest",
        //                  "dawn",     "ember",    "peak",    "riverbank", "mirror",   "crown",
        //                  "torch",    "temple",   "lantern", "village"};

        srand((unsigned int)time(NULL));
        printf("~~~~~~~~~~   test cops_omap  ~~~~~~~~~~\n");
        if (type != 1) {
        }
        printf("\n~~~~~~~~~~  " Yf B Pb "performance test" D " ~~~~~~~~~~\n");
        for (int k = 0; k < 3; k++) {
                printf("\n");
                f_omap *f = f_omap_new(_fcmp);
                const int N = 1 << (20 + (k << 1));
                // const int M = 100000000;
                const int M = 1000000;
                char strval[28];
                format_number(N, strval);
                printf(" > (" Gf "%12s insert" D ")", strval);
                clock_t s, e;
                s = clock();
                for (long i = 0; i < N; i++) {
                        f_omap_add(f, i, i);
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
                        // f_omap_has(f, pos, NULL);
                        f_omap_mod(f, pos, i);
                }
                e = clock();
                elapse = (double)(e - s) * 1000.0 / CLOCKS_PER_SEC;
                avg = (double)(e - s) * 1000000.0 / CLOCKS_PER_SEC / (double)M;
                printf(" %.3f ms\n", elapse);
                printf(" > (      " Rf "avg set" D "      ) %.6f \u03bcs\n", avg);
                f = f_omap_free(f);
        }
        printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
        return 0;
}
