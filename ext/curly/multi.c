#include "multi.h"
#include <curl/curl.h>

VALUE multi_run(VALUE self);

void Init_curly_multi(VALUE curly)
{
  VALUE multi = rb_define_class_under(curly, "Multi", rb_cObject);

  rb_define_method(multi, "run", multi_run, 0);
}

VALUE multi_run(VALUE self)
{
  CURLM* c = curl_multi_init();
  VALUE  requests = rb_iv_get(self, "@requests");

  curl_multi_cleanup(c);

  return Qnil;
}
