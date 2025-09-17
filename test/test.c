#include <stdio.h>
#include <string.h>

extern int test_arr(int type);
extern int test_vec(int type);
extern int test_map(int type);
extern int test_set(int type);
extern int test_omap(int type);
extern int test_oset(int type);

int main(int argc, char *argv[])
{
        if (argc < 2) {
                printf("Is require at least one of the test to be run:\n"
                       " - 'speed' check only speed\n"
                       " - 'arr' test cops array\n"
                       " - 'vec' test cops vector\n"
                       " - 'map' test cops hashmap\n"
                       " - 'set' test cops hashset\n"
                       " - 'omap' test cops bstmap\n"
                       " - 'oset' test cops bstset\n");
                return 0;
        }
        int x = 0;
        for (int i = 1; i < argc; i++) {
                if (!strcmp(argv[i], "speed")) {
                        x = 1;
                } else if (!strcmp(argv[i], "arr")) {
                        if (test_arr(x))
                                return 1;
                } else if (!strcmp(argv[i], "vec")) {
                        if (test_vec(x))
                                return 1;
                } else if (!strcmp(argv[i], "map")) {
                        if (test_map(x))
                                return 1;
                } else if (!strcmp(argv[i], "set")) {
                        if (test_set(x))
                                return 1;
                } else if (!strcmp(argv[i], "omap")) {
                        if (test_omap(x))
                                return 1;
                } else if (!strcmp(argv[i], "oset")) {
                        if (test_oset(x))
                                return 1;
                }
        }
}
