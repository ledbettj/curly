#include <curl/curl.h>
#include "native.h"

#define INITIAL_BODY_CAPACITY 4096
#define INITIAL_HEAD_CAPACITY 512

static size_t header_callback(void* buffer, size_t size, size_t count, void* n_);
static size_t data_callback  (void* buffer, size_t size, size_t count, void* n_);

int native_curly_alloc(native_curly* n)
{
  n->curl_rc = 0;
  n->handle  = curl_easy_init();

  if (exstr_alloc(&n->body, INITIAL_BODY_CAPACITY) < 0) {
    return -1;
  }

  if (exstr_alloc(&n->head, INITIAL_HEAD_CAPACITY) < 0) {
    exstr_free(&n->body);
    return -1;
  }

  n->req_headers = NULL;

  return 0;
}

void native_curly_free(native_curly* n)
{
  curl_easy_cleanup(n->handle);
  curl_slist_free_all(n->req_headers);

  exstr_free(&n->body);
  exstr_free(&n->head);
}

void native_curly_add_header(native_curly* n, const char* hdr)
{
  n->req_headers = curl_slist_append(n->req_headers, hdr);
}

void native_curly_prepare(native_curly* n, const char* url, long timeout,
                          const char* body, int follow_location)
{
  /* invoke header_callback when a header is received and pass `resp` */
  curl_easy_setopt(n->handle, CURLOPT_HEADERFUNCTION, header_callback);
  curl_easy_setopt(n->handle, CURLOPT_WRITEHEADER,    n);

  /* invoke data_callback when a chunk of data is received and pass `resp` */
  curl_easy_setopt(n->handle, CURLOPT_WRITEFUNCTION, data_callback);
  curl_easy_setopt(n->handle, CURLOPT_WRITEDATA,     n);

  curl_easy_setopt(n->handle, CURLOPT_URL, url);
  curl_easy_setopt(n->handle, CURLOPT_HTTPHEADER, n->req_headers);

  curl_easy_setopt(n->handle, CURLOPT_FOLLOWLOCATION, follow_location);

  if (timeout > 0) {
    curl_easy_setopt(n->handle, CURLOPT_TIMEOUT_MS, timeout);
  }

  if (body) {
    curl_easy_setopt(n->handle, CURLOPT_POSTFIELDS, body);
  }

}

void native_curly_run_simple(native_curly* n)
{
  n->curl_rc = curl_easy_perform(n->handle);
}

/* invoked by curl_easy_perform when a header is available */
static size_t header_callback(void* buffer, size_t size, size_t count, void* n_)
{
  native_curly* n = (native_curly*)n_;
  exstr_append(&n->head, buffer, size * count);

  return size * count;
}

/* invoked by curl_easy_perform when data is available */
static size_t data_callback(void* buffer, size_t size, size_t count, void* n_)
{
  native_curly* n = (native_curly*)n_;
  exstr_append(&n->body, buffer, size * count);

  return size * count;
}
