#ifndef STRLIST_H
#define STRLIST_H

#include <stdint.h>
#include <stddef.h>
#include <string.h>

thread_local size_t _strlist_len_tmp;

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
//char * strlist_element(char * list, size_t n, sep_t sep);
/* This function returns a range.
 */
//char * strlist_elements(char * list, size_t from, size_t to, sep_t sep);

/* For convencience, we offer these in 3 overloads
 *  sharing a generic interface.
 *
 * Char     - fastest
 * String   - slightly slower
 * String[] - slowest; null terminated array of strings
 */

/* The following are shorthands for element()/elements(),
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

// XXX i might be able to generic element and elements functions

typedef char* strlist;

// Char variants
// XXX should be using strchr
size_t strlist_len_char(strlist list, char sep) {
    const char * s = list;

    if (s[0] == '\0') { return 0; }

    if (s[0] == sep) { ++s; }

    size_t r = 1;
    while ((s = strchr(s, sep))) {
        ++s;
        ++r;
    }
    return r;
}

size_t strlist_element_position_char(strlist list, size_t n, char sep) {
    const char * s = list;

    if (n == 0) { return 0; }

    size_t i = 0;

    const char * start = s;
    while (true) {
        start = strchr(start, sep);
        if (!start) {
            return SIZE_MAX;
        }
        ++start;
        ++i;
        if (i == n) {
            break;
        }
    }

    return start - s;
}

char * strlist_element_char(strlist list, size_t n, char sep) {
    // Find start
    const size_t start_pos = strlist_element_position_char(list, n, sep);
    if (start_pos == SIZE_MAX) { goto out_of_range; }
    const char * start = list + start_pos;

    // Find end
    const char * end = strchr(start, sep);
    if (!end) {
        end = start + strlen(start);
    }

    // Finalize
    memmove(list, start, end - start);
    list[end - start] = '\0';
    return list;

  out_of_range:
    list[0] = '\0';
    return list;
}

strlist strlist_elements_char(strlist list, size_t from, size_t n, char sep) {
    // Find start
    char * start;
    if (from == 0) {
        start = list;
    } else {
        const bool correction = (list[0] == sep);
        const size_t start_pos = strlist_element_position_char(
            list + correction,
            from,
            sep
        );
        if (start_pos == SIZE_MAX) { goto out_of_range; }
        start = list + start_pos + correction;
    }

    // Find end
    char * end;
    do {
        char * search_end_from = (from == 0 && list[0] == sep ? start + 1 : start);
        const size_t end_element_start_pos = strlist_element_position_char(
            search_end_from,
            n ? n-1 : n,
            sep
        );
        if (end_element_start_pos == SIZE_MAX) {
            const size_t end_pos = strlen(start);
            memmove(list, start, end_pos);
            list[end_pos] = '\0';
            return list;
        }
        end = search_end_from + end_element_start_pos;
        while (*end != sep
        &&     *end != '\0') {
            ++end;
        }
    } while (0);

    // Finalize
    memmove(list, start, end - start);
    list[end - start] = '\0';
    return list;

  out_of_range:
    list[0] = '\0';
    return list;
}

// String variants
size_t strlist_len_str(strlist list, const char * sep) {
    const char * s = list;

    if (s[0] == '\0') { return 0; }

    if (!strncmp(s, sep, strlen(sep))) { s += strlen(sep); }

    size_t r = 1;
    while ((s = strstr(s, sep))) {
        s += strlen(sep);
        ++r;
    }
    return r;
}

size_t strlist_element_position_str(strlist list, size_t n, const char * sep) {
    const char * s = list;

    if (n == 0) { return 0; }

    size_t i = 0;

    const char * start = s;
    while (true) {
        start = strstr(start, sep);
        if (!start) {
            return SIZE_MAX;
        }
        start += strlen(sep);
        ++i;
        if (i == n) {
            break;
        }
    }

    return start - s;
}

char * strlist_element_str(strlist list, size_t n, const char * sep) {
    // Find start
    const size_t start_pos = strlist_element_position_str(list, n, sep);
    if (start_pos == SIZE_MAX) { goto out_of_range; }
    const char * start = list + start_pos;

    // Find end
    const char * end = strstr(start, sep);
    if (!end) {
        end = start + strlen(start);
    }

    // Finalize
    memmove(list, start, end - start);
    list[end - start] = '\0';
    return list;

  out_of_range:
    list[0] = '\0';
    return list;
}

strlist strlist_elements_str(strlist list, size_t from, size_t n, const char * sep) {
    const bool has_leading_separator = !strncmp(list, sep, strlen(sep));

    // Find start
    char * start;
    if (from == 0) {
        start = list;
    } else {
        const int correction = (has_leading_separator ? strlen(sep) : 0);
        const size_t start_pos = strlist_element_position_str(
            list + correction,
            from,
            sep
        );
        if (start_pos == SIZE_MAX) { goto out_of_range; }
        start = list + start_pos + correction;
    }

    // Find end
    char * end;
    do {
        char * search_end_from = (from == 0 && has_leading_separator
            ? start + strlen(sep)
            : start
        );
        const size_t end_element_start_pos = strlist_element_position_str(
            search_end_from,
            n ? n-1 : n,
            sep
        );
        if (end_element_start_pos == SIZE_MAX) {
            const size_t end_pos = strlen(start);
            memmove(list, start, end_pos);
            list[end_pos] = '\0';
            return list;
        }
        end = search_end_from + end_element_start_pos;
        end = strstr(end, sep);
        if (!end) {
            end = search_end_from + end_element_start_pos
                + strlen(search_end_from + end_element_start_pos);
        }
    } while (0);

    // Finalize
    memmove(list, start, end - start);
    list[end - start] = '\0';
    return list;

  out_of_range:
    list[0] = '\0';
    return list;
}

// Generics
#define strlist_len(list, sep) \
    _Generic(sep                          \
        , char        : strlist_len_char  \
        , int         : strlist_len_char  \
        , char*       : strlist_len_str   \
        , const char* : strlist_len_str   \
    )(list, sep)

#define strlist_elements(list, from, n, sep) \
    _Generic(sep                               \
        , char        : strlist_elements_char  \
        , int         : strlist_elements_char  \
        , char*       : strlist_elements_str   \
        , const char* : strlist_elements_str   \
    )(list, from, n, sep)

#define strlist_root(list, sep) (\
    _strlist_len_tmp = strlist_len(list, sep), \
    strlist_elements(list, 0, _strlist_len_tmp ? _strlist_len_tmp-1 : 0, sep) \
)

#define strlist_base(list, sep) (\
    _strlist_len_tmp = strlist_len(list, sep), \
    strlist_elements(list, _strlist_len_tmp ? _strlist_len_tmp-1 : 0, 1, sep) \
)

#define strlist_head(list, sep) \
    strlist_elements(list, 0, 1, sep)

#define strlist_tail(list, sep) (\
    _strlist_len_tmp = strlist_len(list, sep), \
    strlist_elements(list, 1, _strlist_len_tmp ? _strlist_len_tmp-1 : 0, sep) \
)

// TODO: add assertions

#endif
