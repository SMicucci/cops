#include <stdio.h>
#include <string.h>

extern int test_arr();
extern int test_vec();
extern int test_map();
extern int test_set();
extern int test_omap();
extern int test_oset();

int main(int argc, char *argv[])
{
        if (argc < 2) {
                printf("Is require at least one of the test to be run:\n"
                       " - 'arr' test cops array\n"
                       " - 'vec' test cops vector\n"
                       " - 'map' test cops hashmap\n"
                       " - 'set' test cops hashset\n"
                       " - 'omap' test cops bstmap\n"
                       " - 'oset' test cops bstset\n");
                return 0;
        }
        for (int i = 1; i < argc; i++) {
                if (!strcmp(argv[i], "arr")) {
                        if (test_arr())
                                return 1;
                }
                if (!strcmp(argv[i], "vec")) {
                        if (test_vec())
                                return 1;
                }
                if (!strcmp(argv[i], "map")) {
                        if (test_map())
                                return 1;
                }
                if (!strcmp(argv[i], "set")) {
                        if (test_set())
                                return 1;
                }
                if (!strcmp(argv[i], "omap")) {
                        if (test_omap())
                                return 1;
                }
                if (!strcmp(argv[i], "oset")) {
                        if (test_oset())
                                return 1;
                }
        }
}
