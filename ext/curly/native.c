#include <curl/curl.h>
#include "native.h"

#define INITIAL_BODY_CAPACITY 4096
#define INITIAL_HEAD_CAPACITY 512

int native_curly_alloc(native_curly* n, CURL* handle)
{
  n->curl_rc = 0;
  n->handle  = handle;

  if (exstr_alloc(&n->body, INITIAL_BODY_CAPACITY) < 0) {
    return -1;
  }

  if (exstr_alloc(&n->head, INITIAL_HEAD_CAPACITY) < 0) {
    exstr_free(&n->body);
    return -1;
  }

  return 0;
}

void native_curly_free(native_curly* n)
{
  exstr_free(&n->body);
  exstr_free(&n->head);
}
