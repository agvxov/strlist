# String lists

A string list is a list which is stored as a string,
each element delimited by some select token.

It is a common pattern, as it is a the most intuitive way to serialize a list.

Examples would include:
* file paths `a/b`
* file extensions `a.b`
* unix style option lists `a:b`
* symbol hierarchies `a->b`

This library is to aid working with them.
It could be viewed as `strtok` reimagined.

## Rationale
> I've called libgen's basename one too many times

## Interface
```c
// Example data
const char * list = "parrot,elephant,cat";

// Get the length
auto l = strlist_len(list, ',');

// Get element
auto cat = strlist_element(strdup(list), ',', l-1);

// Get range
auto two_three = strlist_element(strdup(list), ',', /*from*/ 1, /*n*/ 2);

/* Range shorthands
 * Visual explanation:
 *       this/is/my/example/path
 *  Root <---------------->
 *  Base                    <-->
 *  Head <-->
 *  Tail      <---------------->
 */
two_three = strlist_tail(strdup(list), ',');

// Iterate
foreach_strlist(list, ',', a) {
    puts(a);
    // ideal place to fill a *real* list for extensive use
}

// Variants
const char * list2 = "parrot, elephant, cat"; // works w/ sep = ", ")
const char * list3 = "parrot, elephant,cat";  // works w/ sep = (const char * const []){",", ", ", NULL})
```
