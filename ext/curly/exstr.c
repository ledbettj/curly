#include "exstr.h"
#include <stdlib.h>
#include <string.h>

static int exstr_grow(exstr* s, size_t at_least);

int exstr_alloc(exstr* s, size_t capacity)
{
  s->capacity = capacity;
  s->length   = 0;
  s->value    = malloc(capacity);

  return s->value ? 0 : -1;
}

void exstr_free(exstr* s)
{
  free(s->value);
}

int exstr_append(exstr* s, char* buffer, size_t length)
{
  if (exstr_grow(s, s->length + length) < 0) {
    return -1;
  }

  memcpy(&s->value[s->length], buffer, length);
  s->length += length;

  return 0;
}

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
