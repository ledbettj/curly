#include <ruby.h>
#include <curl/curl.h>
#include "request.h"
#include "response.h"
#include "native.h"

static VALUE request_run(VALUE self);

/* internal helpers */
static VALUE request_alloc(VALUE self);
static VALUE request_perform(VALUE self, CURL* c, VALUE url);
static struct curl_slist* build_headers(CURL* c, VALUE headers);
static int request_add_header(VALUE key, VALUE val, VALUE in);
static VALUE build_query_string(VALUE params);

/* curl callbacks */
static size_t header_callback(void* buffer, size_t size, size_t count, void* self);
static size_t data_callback  (void* buffer, size_t size, size_t count, void* self);

static struct {
  VALUE method;
  VALUE get, post, put, delete;
} syms;

void Init_curly_request(VALUE curly_mod)
{
  VALUE request = rb_define_class_under(curly_mod, "Request", rb_cObject);

  rb_define_method(request, "run", request_run, 0);

  syms.method   = ID2SYM(rb_intern("method"));
  syms.get      = ID2SYM(rb_intern("get"));
  syms.post     = ID2SYM(rb_intern("post"));
  syms.put      = ID2SYM(rb_intern("put"));
  syms.delete   = ID2SYM(rb_intern("delete"));
}

static VALUE request_run(VALUE self)
{
  VALUE url    = rb_iv_get(self, "@url");
  VALUE method = rb_iv_get(self, "@method");
  CURL* c      = curl_easy_init();

  VALUE body, resp;

  method = (method == Qnil ? syms.get : rb_funcall(method, rb_intern("to_sym"), 0));

  if (method == syms.get) {
    curl_easy_setopt(c, CURLOPT_HTTPGET, 1L);
  } else if (method == syms.post) {
    curl_easy_setopt(c, CURLOPT_HTTPPOST, NULL);
  } else if (method == syms.put) {
    curl_easy_setopt(c, CURLOPT_CUSTOMREQUEST, "PUT");
  } else if (method == syms.delete) {
    curl_easy_setopt(c, CURLOPT_CUSTOMREQUEST, "DELETE");
  } else {
    /* TODO: rb_raise_whatever */
  }

  if ((body = rb_iv_get(self, "@body")) != Qnil) {
    /* TODO: handle case where `body` is a hash. */
    curl_easy_setopt(c, CURLOPT_POSTFIELDS, RSTRING_PTR(StringValue(body)));
  }

  resp = request_perform(self, c, url);

  curl_easy_cleanup(c);

  return resp;
}

static VALUE curl_easy_perform_wrapper(void* args)
{
  native_curly* n = (native_curly*)args;

  n->curl_rc = curl_easy_perform(n->handle);

  return Qnil;
}

static VALUE request_perform(VALUE self, CURL* c, VALUE url)
{
  long  code;
  VALUE timeout, params, headers;
  VALUE resp = response_new();
  native_curly n;
  struct curl_slist* header_list = NULL;

  /* invoke header_callback when a header is received and pass `resp` */
  curl_easy_setopt(c, CURLOPT_HEADERFUNCTION, header_callback);
  curl_easy_setopt(c, CURLOPT_WRITEHEADER,    &n);

  /* invoke data_callback when a chunk of data is received and pass `resp` */
  curl_easy_setopt(c, CURLOPT_WRITEFUNCTION, data_callback);
  curl_easy_setopt(c, CURLOPT_WRITEDATA,     &n);

  if ((headers = rb_iv_get(self, "@headers")) != Qnil) {
    header_list = build_headers(c, headers);
  }

  if ((params = rb_iv_get(self, "@params")) != Qnil) {
    url = rb_str_plus(url, rb_str_new2("?"));
    url = rb_str_append(url, build_query_string(params));
  }

  if ((timeout = rb_iv_get(self, "@timeout")) != Qnil) {
    curl_easy_setopt(c, CURLOPT_TIMEOUT_MS, NUM2LONG(timeout));
  }

  curl_easy_setopt(c, CURLOPT_URL, RSTRING_PTR(StringValue(url)));

  native_curly_alloc(&n, c);

  rb_thread_blocking_region(curl_easy_perform_wrapper, &n, NULL, NULL);

  rb_iv_set(resp, "@body", rb_str_new(n.body.value, n.body.length));
  rb_iv_set(resp, "@head", rb_str_new(n.head.value, n.head.length));
  rb_iv_set(resp, "@curl_code", INT2NUM(n.curl_rc));

  if (n.curl_rc == CURLE_OK) {
    curl_easy_getinfo(c, CURLINFO_RESPONSE_CODE, &code);
    rb_iv_set(resp, "@status", INT2NUM(code));
  } else {
    rb_iv_set(resp, "@curl_error", rb_str_new2(curl_easy_strerror(n.curl_rc)));
  }

  curl_slist_free_all(header_list);
  native_curly_free(&n);

  return resp;
}

static int request_add_header(VALUE key, VALUE val, VALUE in)
{
  struct curl_slist** list = (struct curl_slist**)in;

  /* build the 'Header: Value' string */
  VALUE text = rb_str_plus(StringValue(key), rb_str_new2(": "));
  rb_str_append(text, StringValue(val));

  /* update the head of the list so that the next item is appended correctly */
  *list = curl_slist_append(*list, RSTRING_PTR(text));

  return ST_CONTINUE;
}

static struct curl_slist* build_headers(CURL* c, VALUE headers)
{
  struct curl_slist* list = NULL;

  if (TYPE(headers) == T_HASH) {
    rb_hash_foreach(headers, request_add_header, (VALUE)&list);
    curl_easy_setopt(c, CURLOPT_HTTPHEADER, list);
  }

  return list;
}

/* invoked by curl_easy_perform when a header is available */
static size_t header_callback(void* buffer, size_t size, size_t count, void* arg)
{
  native_curly* n = (native_curly*)arg;
  exstr_append(&n->head, buffer, size * count);

  return size * count;
}

/* invoked by curl_easy_perform when data is available */
static size_t data_callback(void* buffer, size_t size, size_t count, void* arg)
{
  native_curly* n = (native_curly*)arg;
  exstr_append(&n->body, buffer, size * count);

  return size * count;
}

/* given a set of key/value pairs, build a url encoded string of the form
 * key=value&key2=value2 using ActiveSupport's to_query if available or
 * our own compatible implementation */
static VALUE build_query_string(VALUE params)
{
  VALUE paramize = rb_const_get(rb_const_get(rb_cObject, rb_intern("Curly")),
                                rb_intern("Parameterize"));
  return rb_funcall(paramize, rb_intern("query_string"), 1, params);
}
