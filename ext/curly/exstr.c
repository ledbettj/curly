#include "exstr.h"
#include <stdlib.h>
#include <string.h>

static int exstr_grow(exstr* s, size_t at_least);

/*
 * initialize a new expandable string with an initial capacity of `capacity`.
 *
 * returns 0 on success, -1 on failure.
 */
int exstr_alloc(exstr* s, size_t capacity)
{
  s->capacity = capacity;
  s->length   = 0;
  s->value    = malloc(capacity);

  return s->value ? 0 : -1;
}

/*
 * release any memory allocated to the expandable string `s`.
 */
void exstr_free(exstr* s)
{
  free(s->value);
  s->value    = NULL;
  s->capacity = 0;
  s->length   = 0;
}

/*
 * append `length` characters from `buffer` onto the expandable string
 * `s`.  Note that you should not include the null terminator (if any)
 * in the length count, or you will end up with \0 bytes inside your string.
 * if you need the string stored in `s->value` to be null terminated,
 * invoke exstr_append(s, "", 1) at the end of your string building operation.
 * Be aware that in this case unlike strlen(), s->length will include the null
 * terminator.
 *
 * returns 0 if the operation was successful, -1 otherwise.
 */
int exstr_append(exstr* s, char* buffer, size_t length)
{
  if (exstr_grow(s, s->length + length) < 0) {
    return -1;
  }

  memcpy(&s->value[s->length], buffer, length);
  s->length += length;

  return 0;
}

/*
 * internal helper function -- make sure that the expandable string `s`
 * has a capacity of `at_least` bytes, reallocating if necessary.
 * if reallocation fails, the current contents of `s` will still be available.
 *
 * returns 0 on success, -1 otherwise.
 */
static int exstr_grow(exstr* s, size_t at_least)
{
  size_t new_capacity;
  char*  new_value;

  if (s->capacity <= at_least) {
    new_capacity = 2 * at_least;
    new_value    = realloc(s->value, new_capacity);

    if (!new_value) {
      /* realloc failed, did not grow */
      return -1;
    }

    s->value    = new_value;
    s->capacity = new_capacity;

  }

  return 0;
}
