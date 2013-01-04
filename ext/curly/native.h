#ifndef _CURLY_NATIVE_H_
#define _CURLY_NATIVE_H_

#include <curl/curl.h>
#include "exstr.h"

typedef struct native_curly_ {
  CURL* handle;
  int   curl_rc;
  exstr body;
  exstr head;
} native_curly;

int  native_curly_alloc(native_curly* n, CURL* handle);
void native_curly_free(native_curly* n);


#endif
