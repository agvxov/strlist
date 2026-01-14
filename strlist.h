#ifndef STRLIST_H
#define STRLIST_H

#include <stdint.h>
#include <stddef.h>
#include <string.h>

/* The string based list is a common pattern,
 *  as it is the most intuitive way to serialize a list.
 *
 * In many languages you would handle it by instantiating
 *  an actual list by means of splitting,
 *  but in C we can often do better
 *  (and we dont have general lists to begin with).
 *
 * Examples would include:
 *  + file paths (a/b)
 *  + file extensions (a.b)
 *  + unix style option lists (a:b)
 *  + symbol hierarchies (a->b)
 */

//size_t strlist_len(char * list, sep_t sep);

/* This function in an abstract sense performs list indexing.
 *  The result overwrites the `list` argument and is returned.
 *  (We know that this may never result in an overflow.)
 */
//char * strlist_component(char * list, size_t n, sep_t sep);
/* This function returns a range.
 */
//char * strlist_components(char * list, size_t from, size_t to, sep_t sep);
/* For convencience, we offer these in 3 overloads
 *  sharing a generic interface.
 *
 * Char     - fastest
 * String   - slightly slower
 * String[] - slowest; null terminated array of strings
 */

/* The following are shorthands for component()/components(),
 *  with specific numbers which may or may not be length specific
 *
 * Visual explanation:
 *       this/is/my/example/path
 *  Root <---------------->
 *  Base                    <-->
 *  Head <-->
 *  Tail      <---------------->
 */
//char * strlist_root(char * list, sep_t sep);
//char * strlist_base(char * list, sep_t sep);
//char * strlist_head(char * list, sep_t sep);
//char * strlist_tail(char * list, sep_t sep);

/* Notes:
 *  + we very consciously made the decision to not take a destination operand;
 *     you would have to allocate it just the same,
 *     copying the source string is not a real performance concern,
 *     but we want our interface to be as clean as possible
 *  + copy the result to the start of the string so that assuming the allocated length
 *     of the return value is not a footgun
 *  + a strlist is considered to have 0 elements if and only if when the string is of length 0
 */

/* Example:
 *  Getting the absolute basename of a file.
 *      char name[] = "this/is/my.file.example";
 *      name = strlist_head(strlist_base(name, UNIX_PATH_SEP), EXT_SEP);
 */

//typedef const char * const sep_t;
//sep_t UNIX_PATH_SEP = { "/", NULL, };
//sep_t DOS_PATH_SEP  = { "\\", NULL, };
//sep_t UNIX_SEP      = { ":", NULL, };
//sep_t CPP_SEP       = { "::", ".", "->", NULL, };
//sep_t EXT_SEP       = { ".", NULL, };

size_t strlist_len_char(const char * const list, char sep) {
    if (list[0] == '\0') { return 0; }

    size_t r = 1;
    for (const char * s = list; *s != '\0'; s++) {
        if (*s == sep) {
            ++r;
        }
    }
    return r;
}

size_t strlist_component_position_char(char * list, size_t n, char sep) {
    if (n == 0) { return 0; }

    size_t i = 0;

    const char * start = list;
    while (true) {
        if (*start == '\0') {
            return SIZE_MAX;
        }
        if (*start == sep) {
            ++i;
        }
        ++start;
        if (i == n) {
            break;
        }
    }

    return start - list;
}

char * strlist_component_char(char * list, size_t n, char sep) {
    const size_t start_pos = strlist_component_position_char(list, n, sep);
    if (start_pos == SIZE_MAX) { goto out_of_range; }

    const char * start = list + start_pos;
    const char * end = start;
    while (*end != sep
    &&     *end != '\0') {
        ++end;
    }

    memmove(list, start, end - start);
    list[end - start] = '\0';
    return list;

  out_of_range:
    list[0] = '\0';
    return list;
}

char * strlist_components(char * list, size_t from, size_t to, char sep) {
    size_t start_pos = strlist_component_position_char(list, from, sep);
    if (start_pos == SIZE_MAX) { goto out_of_range; }
    char * const start = list + start_pos;

    const size_t end_component_start_pos = strlist_component_position_char(start, to - from, sep);
    if (end_component_start_pos == SIZE_MAX) {
        const size_t end_pos = strlen(start);
        memmove(list, start, end_pos);
        list[end_pos] = '\0';
        return list;
    }
    char * end = list + end_component_start_pos;
    while (*end != sep
    &&     *end != '\0') {
        ++end;
    }

    memmove(list, start, end - start);
    list[end - start] = '\0';
    return list;

  out_of_range:
    list[0] = '\0';
    return list;
}

char * strlist_root(char * list, char sep) {
    size_t len = strlist_len_char(list, sep);
    return strlist_components(list, 0, len-2, sep);
}

char * strlist_base(char * list, char sep) {
    size_t len = strlist_len_char(list, sep);
    return strlist_components(list, len-1, len, sep);
}

char * strlist_head(char * list, char sep) {
    return strlist_components(list, 0, 1, sep);
}

char * strlist_tail(char * list, char sep) {
    size_t len = strlist_len_char(list, sep);
    return strlist_components(list, 1, len, sep);
}

#endif
