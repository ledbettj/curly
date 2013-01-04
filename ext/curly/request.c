#include <ruby.h>
#include <curl/curl.h>
#include "request.h"
#include "response.h"
#include "native.h"

static VALUE request_run(VALUE self);

/* internal helpers */
static VALUE request_alloc(VALUE self);
static VALUE request_perform(VALUE self, CURL* c, VALUE url);
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

static int request_add_header(VALUE key, VALUE val, VALUE ptr)
{
  native_curly* n = (native_curly*)ptr;
  VALUE text = rb_str_plus(StringValue(key), rb_str_new2(": "));
  rb_str_append(text, StringValue(val));

  native_curly_add_header(n, RSTRING_PTR(text));

  return ST_CONTINUE;
}

static VALUE unlock_vm_wrapper(void* arg)
{
  native_curly* n = (native_curly*)arg;
  native_curly_run_simple(n);

  return Qnil;
}

static VALUE request_run(VALUE self)
{
  VALUE url     = rb_funcall(self, rb_intern("effective_url"), 0);
  VALUE method  = rb_iv_get(self, "@method");
  VALUE body    = rb_iv_get(self, "@body");
  VALUE timeout = rb_iv_get(self, "@timeout");
  VALUE headers = rb_iv_get(self, "@headers");
  VALUE resp    = response_new();
  long  code;
  native_curly n;

  native_curly_alloc(&n);

  method = (method == Qnil ? syms.get : rb_funcall(method, rb_intern("to_sym"), 0));

  if (method == syms.get) {
    curl_easy_setopt(n.handle, CURLOPT_HTTPGET, 1L);
  } else if (method == syms.post) {
    curl_easy_setopt(n.handle, CURLOPT_HTTPPOST, NULL);
  } else if (method == syms.put) {
    curl_easy_setopt(n.handle, CURLOPT_CUSTOMREQUEST, "PUT");
  } else if (method == syms.delete) {
    curl_easy_setopt(n.handle, CURLOPT_CUSTOMREQUEST, "DELETE");
  } else {
    /* TODO: rb_raise_whatever */
  }

  if (headers != Qnil) {
    rb_hash_foreach(headers, request_add_header, (VALUE)&n);
  }

  native_curly_prepare(&n, RSTRING_PTR(url),
                       timeout != Qnil ? NUM2LONG(timeout) : -1,
                       body    != Qnil ? RSTRING_PTR(StringValue(body)) : NULL
                       );

  rb_thread_blocking_region(unlock_vm_wrapper, &n, NULL, NULL);

  rb_iv_set(resp, "@body", rb_str_new(n.body.value, n.body.length));
  rb_iv_set(resp, "@head", rb_str_new(n.head.value, n.head.length));
  rb_iv_set(resp, "@curl_code", INT2NUM(n.curl_rc));

  if (n.curl_rc == CURLE_OK) {
    curl_easy_getinfo(n.handle, CURLINFO_RESPONSE_CODE, &code);
    rb_iv_set(resp, "@status", LONG2NUM(code));
  } else {
    rb_iv_set(resp, "@curl_error", rb_str_new2(curl_easy_strerror(n.curl_rc)));
  }

  native_curly_free(&n);

  return resp;
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
