#include <ruby.h>
#include <curl/curl.h>
#include <stdio.h>

static VALUE new_response(void)
{
  return rb_class_new_instance(0, NULL,
           rb_const_get_at(
             rb_const_get(rb_cObject, rb_intern("Curly")),
             rb_intern("Response")
           )
         );
}

static size_t header_callback(void* buffer, size_t size, size_t count, VALUE self)
{
  rb_funcall(self, rb_intern("add_header"), 1, rb_str_new2(buffer));
  return size * count;
}

static size_t data_callback(void* buffer, size_t size, size_t count, VALUE self)
{
  rb_funcall(self, rb_intern("add_body"), 1, rb_str_new2(buffer));
  return size * count;
}


static VALUE curly_alloc(VALUE self)
{
  CURL* c = curl_easy_init();

  Data_Wrap_Struct(self, NULL, curl_easy_cleanup, c);

}


static VALUE client_get(VALUE self, VALUE url)
{
  CURL* c;
  long code;
  VALUE response = new_response();
  int rc;

  Data_Get_Struct(self, CURL, c);

  curl_easy_setopt(c, CURLOPT_WRITEHEADER, response);
  curl_easy_setopt(c, CURLOPT_HEADERFUNCTION, header_callback);

  curl_easy_setopt(c, CURLOPT_WRITEDATA, response);
  curl_easy_setopt(c, CURLOPT_WRITEFUNCTION, data_callback);

  curl_easy_setopt(c, CURLOPT_URL, RSTRING_PTR(StringValue(url)));
  rc = curl_easy_perform(c);

  rb_iv_set(response, "@curl_code", INT2NUM(rc));

  if (!rc) {
    curl_easy_getinfo(c, CURLINFO_RESPONSE_CODE, &code);
    rb_iv_set(response, "@code", INT2NUM(code));
  }

  return response;
}

void Init_curly(void)
{
  VALUE mod   = rb_define_module("Curly");
  VALUE klass = rb_define_class_under(mod, "Request", rb_cObject);

  rb_define_alloc_func(klass, curly_alloc);
  rb_define_method(klass, "get", client_get, 1);
}
