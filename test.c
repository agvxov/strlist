// @BAKE gcc -o $*.out $@ -std=c23 -Wall -Wpedantic -ggdb
#include "strlist.h"
#include <stdio.h>

signed main(void) {
    char my_path[] = "/home/anon/Swap/strlist/strlist.h";

    printf("len: %ld\n", strlist_len_char(my_path, '/'));

    puts(strlist_root(strdup(my_path), '/'));
    puts(strlist_base(strdup(my_path), '/'));
    puts(strlist_head(strdup(my_path), '/'));
    puts(strlist_tail(strdup(my_path), '/'));

    return 0;
}
