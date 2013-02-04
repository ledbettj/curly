#ifndef _CURLY_NATIVE_H_
#define _CURLY_NATIVE_H_

#include <curl/curl.h>
#include "exstr.h"

typedef struct native_curly_ {
  CURL* handle;
  int   curl_rc;
  exstr body;
  exstr head;

  struct curl_slist* req_headers;
} native_curly;

int  native_curly_alloc(native_curly* n);
void native_curly_free(native_curly* n);
void native_curly_run_simple(native_curly* n);
void native_curly_add_header(native_curly* n, const char* hdr);
void native_curly_prepare(native_curly* n, const char* url, long timeout,
                          const char* body, int follow_location,
                          int verify_peer);


#endif
