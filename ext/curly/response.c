#include "response.h"
#include <curl/curl.h>

/*
 * call-seq:
 *   timed_out? -> true or false
 *
 * returns +true+ if the request failed because a timeout was reached,
 * +false+ otherwise.
 */
static VALUE response_timed_out(VALUE self)
{
  int curl_code = NUM2INT(rb_iv_get(self, "@curl_code"));
  return (curl_code == CURLE_OPERATION_TIMEDOUT ? Qtrue : Qfalse);
}

VALUE response_new(void)
{
  return rb_class_new_instance(0, NULL,
           rb_const_get(
             rb_const_get(rb_cObject, rb_intern("Curly")),
             rb_intern("Response")
           )
         );
}

void Init_curly_response(VALUE curly_mod)
{
  VALUE response = rb_define_class_under(curly_mod, "Response", rb_cObject);

  rb_define_method(response, "timed_out?", response_timed_out, 0);

}
