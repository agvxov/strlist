// @BAKE gcc -o $*.out $@ -std=c23 -Wall -Wpedantic -ggdb
#include "strlist.h"
#include <stdio.h>

signed main(void) {
    //do {
    //    char my_str[] = "1:2:3";

    //    printf("%ld\n", strlist_element_position_char(my_str, 0, ':'));
    //    printf("%ld\n", strlist_element_position_char(my_str, 1, ':'));
    //    printf("%ld\n", strlist_element_position_char(my_str, 2, ':'));
    //} while (0);

    //puts("\n\n-------------------\n\n");

    //do {
    //    char my_str[] = "1:2:3";

    //    printf("%ld\n", strlist_element_position_str(my_str, 0, ":"));
    //    printf("%ld\n", strlist_element_position_str(my_str, 1, ":"));
    //    printf("%ld\n", strlist_element_position_str(my_str, 2, ":"));
    //} while (0);

    //puts("\n\n-------------------\n\n");

    do {
        char my_path[] = "/home/anon/Swap/strlist/strlist.h";

        printf("len: %ld\n", strlist_len(my_path, '/'));

        puts(strlist_root(strdup(my_path), '/'));
        puts(strlist_base(strdup(my_path), '/'));
        puts(strlist_head(strdup(my_path), '/'));
        puts(strlist_tail(strdup(my_path), '/'));
    } while (0);

    puts("\n\n-------------------\n\n");

    do {
        // NOTE: \n\r replaced with \\n\\r so i can print this without an anurism
        char my_dos_txt[] = "l1\\n\\rl2\\n\\rl3\\n\\rl4\\n\\rl5";

        printf("len: %ld\n\n", strlist_len(my_dos_txt, "\\n\\r"));

        puts(strlist_root(strdup(my_dos_txt), "\\n\\r"));
        puts(strlist_base(strdup(my_dos_txt), "\\n\\r"));
        puts(strlist_head(strdup(my_dos_txt), "\\n\\r"));
        puts(strlist_tail(strdup(my_dos_txt), "\\n\\r"));
    } while (0);

    return 0;
}
