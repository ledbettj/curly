#include <ruby.h>
#include <curl/curl.h>
#include <stdio.h>

/* internal curl callback functions */
static size_t header_callback(void* buffer, size_t size, size_t count, void* self)
{
  rb_funcall((VALUE)self, rb_intern("add_header"), 1, rb_str_new2(buffer));
  return size * count;
}

static size_t data_callback(void* buffer, size_t size, size_t count, void* self)
{
  rb_funcall((VALUE)self, rb_intern("add_body"), 1, rb_str_new2(buffer));
  return size * count;
}

/* internal helper functions */
static VALUE response_new(void)
{
  return rb_class_new_instance(0, NULL,
           rb_const_get_at(
             rb_const_get(rb_cObject, rb_intern("Curly")),
             rb_intern("Response")
           )
         );
}

static VALUE request_perform(VALUE self, VALUE url, VALUE opts)
{
  CURL* c;
  int   rc;
  long  code;
  VALUE resp = response_new();

  Data_Get_Struct(self, CURL, c);

  curl_easy_setopt(c, CURLOPT_URL, RSTRING_PTR(StringValue(url)));

  /* pass the response instance to our header/data callback functions */
  curl_easy_setopt(c, CURLOPT_WRITEHEADER, resp);
  curl_easy_setopt(c, CURLOPT_WRITEDATA,   resp);

  rc = curl_easy_perform(c);
  rb_iv_set(resp, "@curl_code", INT2NUM(rc));

  if (!rc) {
    curl_easy_getinfo(c, CURLINFO_RESPONSE_CODE, &code);
    rb_iv_set(resp, "@code", INT2NUM(code));
  }

  return resp;
}

VALUE request_alloc(VALUE self)
{
  CURL* c = curl_easy_init();
  curl_easy_setopt(c, CURLOPT_HEADERFUNCTION, header_callback);
  curl_easy_setopt(c, CURLOPT_WRITEFUNCTION, data_callback);

  return Data_Wrap_Struct(self, NULL, curl_easy_init, c);
}

/* externally visable functions */
static VALUE request_get(int argc, VALUE* argv, VALUE self)
{
  CURL* c;
  VALUE url, opts = Qnil;

  rb_scan_args(argc, argv, "11", &url, &opts);

  Data_Get_Struct(self, CURL, c);
  curl_easy_setopt(c, CURLOPT_HTTPGET, 1L);

  return request_perform(self, url, opts);
}

static VALUE request_post(int argc, VALUE* argv, VALUE self)
{
  CURL* c;
  VALUE url, opts = Qnil;

  rb_scan_args(argc, argv, "11", &url, &opts);

  Data_Get_Struct(self, CURL, c);
  curl_easy_setopt(c, CURLOPT_HTTPPOST, NULL);

  return request_perform(self, url, opts);
}

void Init_curly(void)
{
  VALUE mod   = rb_define_module("Curly");
  VALUE klass = rb_define_class_under(mod, "Request", rb_cObject);

  rb_define_alloc_func(klass, request_alloc);
  rb_define_method(klass, "get",  request_get,  -1);
  rb_define_method(klass, "post", request_post, -1);
}
