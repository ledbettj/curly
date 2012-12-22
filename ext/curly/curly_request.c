#include <ruby.h>
#include <curl/curl.h>
#include "curly_request.h"
#include "curly_response.h"

static VALUE request_get (int argc, VALUE* argv, VALUE self);
static VALUE request_post(int argc, VALUE* argv, VALUE self);

static VALUE request_alloc(VALUE self);
static VALUE request_perform(VALUE self, CURL* c, VALUE url, VALUE opts);

static size_t header_callback(void* buffer, size_t size, size_t count, void* self);
static size_t data_callback  (void* buffer, size_t size, size_t count, void* self);

void Init_curly_request(void)
{
  VALUE curly   = rb_const_get(rb_cObject, rb_intern("Curly"));
  VALUE request = rb_define_class_under(curly, "Request", rb_cObject);

  rb_define_singleton_method(request, "get",  request_get, -1);
  rb_define_singleton_method(request, "post", request_get, -1);

}

static VALUE request_perform(VALUE self, CURL* c, VALUE url, VALUE opts)
{
  int   rc;
  long  code;
  VALUE resp = response_new();

  curl_easy_setopt(c, CURLOPT_HEADERFUNCTION, header_callback);
  curl_easy_setopt(c, CURLOPT_WRITEFUNCTION, data_callback);

  curl_easy_setopt(c, CURLOPT_URL, RSTRING_PTR(StringValue(url)));

  /* pass the response instance to our header/data callback functions */
  curl_easy_setopt(c, CURLOPT_WRITEHEADER, resp);
  curl_easy_setopt(c, CURLOPT_WRITEDATA,   resp);

  rc = curl_easy_perform(c);
  rb_iv_set(resp, "@curl_code", INT2NUM(rc));

  if (rc == CURLE_OK) {
    curl_easy_getinfo(c, CURLINFO_RESPONSE_CODE, &code);
    rb_iv_set(resp, "@status", INT2NUM(code));
  } else {
    rb_iv_set(resp, "@curl_error", rb_str_new2(curl_easy_strerror(rc)));
  }

  return resp;
}

/* externally visable functions */
static VALUE request_get(int argc, VALUE* argv, VALUE self)
{
  CURL* c = curl_easy_init();
  VALUE url, opts = Qnil, rc;

  rb_scan_args(argc, argv, "11", &url, &opts);

  curl_easy_setopt(c, CURLOPT_HTTPGET, 1L);

  rc = request_perform(self, c, url, opts);

  curl_easy_cleanup(c);

  return rc;
}

static VALUE request_post(int argc, VALUE* argv, VALUE self)
{
  CURL* c = curl_easy_init();
  VALUE url, opts = Qnil, rc;

  rb_scan_args(argc, argv, "11", &url, &opts);

  curl_easy_setopt(c, CURLOPT_HTTPPOST, NULL);

  rc = request_perform(self, c, url, opts);

  curl_easy_cleanup(c);

  return rc;
}



/* internal curl callback functions */
static size_t header_callback(void* buffer, size_t size, size_t count, void* self)
{
  VALUE hash = rb_iv_get((VALUE)self, "@headers");
  VALUE str  = rb_str_new2(buffer);
  VALUE arr;

  rb_funcall(str, rb_intern("chomp!"), 0);
  arr = rb_funcall(str, rb_intern("split"), 2, rb_str_new2(": "), INT2NUM(2));

  if (rb_funcall(arr, rb_intern("length"), 0) == INT2NUM(2)) {
    rb_hash_aset(hash, rb_ary_entry(arr, 0), rb_ary_entry(arr, 1));
  }

  return size * count;
}

static size_t data_callback(void* buffer, size_t size, size_t count, void* self)
{
  VALUE body = rb_iv_get((VALUE)self, "@body");
  rb_funcall(body, rb_intern("<<"), 1, rb_str_new2(buffer));

  return size * count;
}
