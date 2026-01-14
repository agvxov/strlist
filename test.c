// @BAKE gcc -o $*.out $@ -std=c23 -Wall -Wpedantic -ggdb -lcriterion && ./test.out --verbose=0
#include <criterion/criterion.h>
#include "strlist.h"

// NOTE: \n\r replaced with \\n\\r so we can print without an anurism

/* =======================
 * =======================
 * ===  _    ___ _  _  ===
 * === | |  | __| \| | ===
 * === | |__| _|| .` | ===
 * === |____|___|_|\_| ===
 * =======================
 * =======================
 */
#define suite_strlist_len suite_strlist_len
Test(suite_strlist_len, char) {
    char my_path[] = "././.";

    cr_assert_eq(3, strlist_len_char(my_path, '/'));
}

Test(suite_strlist_len, str) {
    char my_dos_txt[] = "l1\\n\\rl2\\n\\rl3";

    cr_assert_eq(3, strlist_len_str(my_dos_txt, "\\n\\r"));
}

Test(suite_strlist_len, strarray) {
    char my_fields[] = "a::b.c->d.e";

    cr_assert_eq(5, strlist_len_strl(my_fields, (const char * const []){"::", ".", "->", NULL }));
}

Test(suite_strlist_len, char_lead) {
    const char my_path[] = "/home/anon/Swap/strlist/strlist.h";

    cr_assert_eq(5, strlist_len_char(my_path, '/'));
}

Test(suite_strlist_len, str_lead) {
    const char my_path[] = "/home/anon/Swap/strlist/strlist.h";

    cr_assert_eq(5, strlist_len_str(my_path, "/"));
}
#undef suite_strlist_len

/* =======================
 * =======================
 * ===  ___  ___  ___  ===
 * === | _ \/ _ \/ __| ===
 * === |  _/ (_) \__ \ ===
 * === |_|  \___/|___/ ===
 * =======================
 * =======================
 */
#define suite_strlist_pos suite_strlist_pos
Test(suite_strlist_pos, char) {
    const char my_str[] = "1:2:3";

    cr_assert_eq(0, strlist_element_position_char(my_str, 0, ':'));
    cr_assert_eq(2, strlist_element_position_char(my_str, 1, ':'));
    cr_assert_eq(4, strlist_element_position_char(my_str, 2, ':'));
}

Test(suite_strlist_pos, str) {
    const char my_str[] = "1:2:3";

    cr_assert_eq(0, strlist_element_position_str(my_str, 0, ":"));
    cr_assert_eq(2, strlist_element_position_str(my_str, 1, ":"));
    cr_assert_eq(4, strlist_element_position_str(my_str, 2, ":"));
}
#undef suite_strlist_pos

/* ==================================
 * ==================================
 * ===  ___ _  _  ___  ___ _____  ===
 * === / __| || |/ _ \| _ \_   _| ===
 * === \__ \ __ | (_) |   / | |   ===
 * === |___/_||_|\___/|_|_\ |_|   ===
 * ==================================
 * ==================================
 */
#define suite_strlist_shorthand suite_strlist_shorthand
Test(suite_strlist_short_hands, char) {
    const char my_path[] = "/home/anon/Swap/strlist/strlist.h";

    cr_assert_str_eq(strlist_root(strdup(my_path), '/'), "/home/anon/Swap/strlist");
    cr_assert_str_eq(strlist_base(strdup(my_path), '/'), "strlist.h");
    cr_assert_str_eq(strlist_head(strdup(my_path), '/'), "/home");
    cr_assert_str_eq(strlist_tail(strdup(my_path), '/'), "anon/Swap/strlist/strlist.h");
}

Test(suite_strlist_short_hands, str) {
    char my_dos_txt[] = "l1\\n\\rl2\\n\\rl3\\n\\rl4\\n\\rl5";

    cr_assert_str_eq(strlist_root(strdup(my_dos_txt), "\\n\\r"), "l1\\n\\rl2\\n\\rl3\\n\\rl4");
    cr_assert_str_eq(strlist_base(strdup(my_dos_txt), "\\n\\r"), "l5");
    cr_assert_str_eq(strlist_head(strdup(my_dos_txt), "\\n\\r"), "l1");
    cr_assert_str_eq(strlist_tail(strdup(my_dos_txt), "\\n\\r"), "l2\\n\\rl3\\n\\rl4\\n\\rl5");
}

Test(suite_strlist_short_hands, strarray) {
    char my_fields[] = "a::b.c->d.e";

    sep_t sps = (const char * const []){"::", ".", "->", NULL};

    cr_assert_str_eq(strlist_root(strdup(my_fields), sps), "a::b.c->d");
    cr_assert_str_eq(strlist_base(strdup(my_fields), sps), "e");
    cr_assert_str_eq(strlist_head(strdup(my_fields), sps), "a");
    cr_assert_str_eq(strlist_tail(strdup(my_fields), sps), "b.c->d.e");
}
#undef suite_strlist_shorthand

/* =============================
 * =============================
 * ===  _    ___   ___  ___  ===
 * === | |  / _ \ / _ \| _ \ ===
 * === | |_| (_) | (_) |  _/ ===
 * === |____\___/ \___/|_|   ===
 * =============================
 * =============================
 */
#define suite_strlist_loop suite_strlist_loop
Test(suite_strlist_loop, example) {
    const char my_path[] = ".:/bin/:/usr/bin:/opt/bin:/usr/sbin";

    const char * elements[] = {
        ".",
        "/bin/",
        "/usr/bin",
        "/opt/bin",
        "/usr/sbin",
    };
    int i = 0;

    foreach_strlist (my_path, ':', path) {
        cr_assert_str_eq(elements[i], path);
        ++i;
    }

    cr_assert_eq(i, 5);
}
#undef suite_strlist_loop
