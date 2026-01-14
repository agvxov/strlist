// @BAKE gcc -o $*.out $@ -std=c23 -Wall -Wpedantic
#include "strlist.h"
#include <stdio.h>

signed main(void) {
    const char my_path[] = "/home/anon/Swap/strlist/strlist.h";

    puts(strlist_root(strdup(my_path), '/'));
    puts(strlist_base(strdup(my_path), '/'));
    puts(strlist_head(strdup(my_path), '/'));
    puts(strlist_tail(strdup(my_path), '/'));

    return 0;
}
