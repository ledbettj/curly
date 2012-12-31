#include "multi.h"
#include <curl/curl.h>

VALUE multi_run(VALUE self);
VALUE multi_add(VALUE self, VALUE req);
VALUE multi_initialize(VALUE self);

void Init_curly_multi(VALUE curly)
{
  VALUE multi = rb_define_class_under(curly, "Multi", rb_cObject);

  rb_define_method(multi, "initialize",  multi_initialize, 0);
  rb_define_method(multi, "run", multi_run, 0);
  rb_define_method(multi, "add", multi_add, 1);

}

VALUE multi_initialize(VALUE self)
{
  rb_iv_set(self, "@requests", rb_ary_new());
}

VALUE multi_run(VALUE self)
{
  CURLM* c = curl_multi_init();
  VALUE  req, r_reqs = rb_iv_get(self, "@requests");
  VALUE* reqs   = RARRAY_PTR(r_reqs);
  size_t len    = RARRAY_LEN(r_reqs);
  size_t i;

  curl_multi_cleanup(c);

  return Qnil;
}


VALUE multi_add(VALUE self, VALUE req)
{
  rb_ary_push(rb_iv_get(self, "@requests"), req);
}
