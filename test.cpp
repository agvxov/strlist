// @BAKE g++ -o $*.out $@
#include <stdio.h>
#define STRLIST_IMPLEMENTATION
#include "strlist.h"

signed main(void) {
    // Example data
    const char * list = "parrot,elephant,cat";

    // Iterate
    foreach_strlist(list, ',', [](char * a){
        puts(a);
    });
}
