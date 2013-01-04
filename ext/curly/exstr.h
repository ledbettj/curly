#ifndef _CURLY_EXSTR_H_
#define _CURLY_EXSTR_H_
#include <stdlib.h>

typedef struct exstr_ {
  size_t capacity;
  size_t length;
  char*  value;
} exstr;

int  exstr_alloc(exstr* s, size_t capacity);
void exstr_free(exstr* s);
int  exstr_append(exstr* s, char* buffer, size_t length);
#endif
