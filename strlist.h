#ifndef STRLIST_H
#define STRLIST_H

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <assert.h>

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

typedef char* strlist;
typedef const char* cstrlist;

// --- Char variants
size_t strlist_len_char(cstrlist list, char sep) {
    assert(list);

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

size_t strlist_element_position_char(cstrlist list, size_t n, char sep) {
    assert(list);

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
    assert(list);

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
    assert(list);

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

// --- String variants
size_t strlist_len_str(strlist list, const char * sep) {
    assert(list);
    assert(sep);

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
    assert(list);
    assert(sep);

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
    assert(list);
    assert(sep);

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
    assert(list);
    assert(sep);

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

// --- String array
typedef const char * const * sep_t;
/* Possible examples:
 *   const sep_t UNIX_PATH_SEP = (const char * const []){ "/", NULL, };
 *   const sep_t DOS_PATH_SEP  = (const char * const []){ "\\", NULL, };
 *   const sep_t UNIX_SEP      = (const char * const []){ ":", NULL, };
 *   const sep_t EXT_SEP       = (const char * const []){ ".", NULL, };
 *   const sep_t CPP_SEP       = (const char * const []){ "::", ".", "->", NULL, };
 */

int strlist_strstrlcmp_(const char * s, sep_t sep, const char * * match) {
    for (auto w = sep; *w != NULL; w++) {
        if (!strncmp(s, *w, strlen(*w))) {
            *match = *w;
            return 0;
        }
    }

    return 1;
}

char * strlist_strstrl_(const char * s, sep_t sep, const char * * match) {
    char * r = NULL;
    for (auto w = sep; *w != NULL; w++) {
        char * i = strstr(s, *w);
        if (i
        && (r == NULL || r > i)) {
            *match = *w;
            r = i;
        }
    }

    return r;
}

size_t strlist_len_strl(strlist list, sep_t sep) {
    assert(list);
    assert(sep);

    const char * s = list;

    if (s[0] == '\0') { return 0; }

    do {
        const char * first_sep;
        if (!strlist_strstrlcmp_(s, sep, &first_sep)) {
            s += strlen(first_sep);
        }
    } while (0);

    size_t r = 1;
    const char * separator;
    while ((s = strlist_strstrl_(s, sep, &separator))) {
        s += strlen(separator);
        ++r;
    }
    return r;
}

size_t strlist_element_position_strl(strlist list, size_t n, sep_t sep) {
    assert(list);
    assert(sep);

    const char * s = list;

    if (n == 0) { return 0; }

    size_t i = 0;

    const char * start = s;
    while (true) {
        const char * separator;
        start = strlist_strstrl_(start, sep, &separator);
        if (!start) {
            return SIZE_MAX;
        }
        start += strlen(separator);
        ++i;
        if (i == n) {
            break;
        }
    }

    return start - s;
}

char * strlist_element_strl(strlist list, size_t n, sep_t sep) {
    assert(list);
    assert(sep);

    // Find start
    const size_t start_pos = strlist_element_position_strl(list, n, sep);
    if (start_pos == SIZE_MAX) { goto out_of_range; }
    const char * start = list + start_pos;

    // Find end
    [[ maybe_unused ]] const char * dummy;
    const char * end = strlist_strstrl_(start, sep, &dummy);
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

strlist strlist_elements_strl(strlist list, size_t from, size_t n, sep_t sep) {
    assert(list);
    assert(sep);

    const char * leading_separator;
    const bool has_leading_separator = !strlist_strstrlcmp_(list, sep, &leading_separator);

    // Find start
    char * start;
    if (from == 0) {
        start = list;
    } else {
        const int correction = (has_leading_separator ? strlen(leading_separator) : 0);
        const size_t start_pos = strlist_element_position_strl(
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
            ? start + strlen(leading_separator)
            : start
        );
        const size_t end_element_start_pos = strlist_element_position_strl(
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
        [[ maybe_unused ]] const char * dummy;
        end = strlist_strstrl_(end, sep, &dummy);
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

// --- Generics
/* For convencience, we offer these in 3 overloads
 *  sharing a generic interface.
 *
 * Char     - fastest
 * String   - slightly slower
 * String[] - slowest; null terminated array of strings
 */

#define strlist_len(list, sep) \
    _Generic(sep                         \
        , char        : strlist_len_char \
        , int         : strlist_len_char \
        , char*       : strlist_len_str  \
        , const char* : strlist_len_str  \
        , sep_t       : strlist_len_strl \
    )(list, sep)

/* This function in an abstract sense performs list indexing.
 *  The result overwrites the `list` argument and is returned.
 *  (We know that this may never result in an overflow.)
 */
#define strlist_element(list, n, sep) \
    _Generic(sep                             \
        , char        : strlist_element_char \
        , int         : strlist_element_char \
        , char*       : strlist_element_str  \
        , const char* : strlist_element_str  \
        , sep_t       : strlist_element_strl \
    )(list, n, sep)

/* This function returns a range.
 */
#define strlist_elements(list, from, n, sep) \
    _Generic(sep                              \
        , char        : strlist_elements_char \
        , int         : strlist_elements_char \
        , char*       : strlist_elements_str  \
        , const char* : strlist_elements_str  \
        , sep_t       : strlist_elements_strl \
    )(list, from, n, sep)

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

/* Iteration
 *
 * Faster than a naiv implementation.
 *
 * While individual operations are reasonably fast,
 *  you are not supposed to grind strlist-s like real *lists*,
 *  instead, you are advised to convert them to a proper data structure
 *  with a foreach.
 *
 * Foreach does not destroy the source strlist to avoid confusion. // XXX
 */
#define strlist_iterator(list_, sep_) struct { \
    char mutable_copy[strlen(list_)+1]; \
    typeof(sep_) sep;                   \
    size_t i;                           \
    size_t n;                           \
    size_t offset;                      \
    size_t next_offset;                 \
}

#define strlist_iterator_init(iter, list_, sep_) ( \
    memcpy(iter.mutable_copy, list_, sizeof(iter.mutable_copy)), \
    iter.sep = sep_,                                             \
    iter.i = 0,                                                  \
    iter.n = strlist_len(list_, sep_)                            \
)

#define strlist_iterator_next(iter) ( \
    iter.i == iter.n                                                                \
        ? NULL                                                                      \
        :                                                                           \
            (                                                                       \
                strlist_element(iter.mutable_copy + iter.next_offset, 0, iter.sep), \
                iter.offset = iter.next_offset,                                     \
                iter.next_offset += strlen(iter.mutable_copy + iter.offset) + 1,    \
                ++iter.i,                                                           \
                iter.mutable_copy + iter.offset                                     \
            )                                                                       \
)                                                                                   \

#define foreach_strlist(list, sep, i_) \
    for (                                                                 \
      strlist_iterator(list, sep) it = {};                                \
      (it.i != 0 || strlist_iterator_init(it, list, sep)) && it.i < it.n; \
    )                                                                     \
        for (                                                             \
          char * i_ = strlist_iterator_next(it);                          \
          i_ != NULL;                                                     \
          i_ = strlist_iterator_next(it)                                  \
        )

#endif
