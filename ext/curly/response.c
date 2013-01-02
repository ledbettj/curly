#include "response.h"
#include <curl/curl.h>


/* :nodoc: */
static VALUE response_init(VALUE self)
{
  rb_iv_set(self, "@body", rb_str_new2(""));
  rb_iv_set(self, "@headers", rb_hash_new());
  rb_iv_set(self, "@status", INT2NUM(0));

  return Qnil;
}

/*
 * call-seq:
 *   success? -> true or false
 *
 * returns +true+ if the HTTP response code returned from the server falls
 * in the +2xx+ range, +false+ otherwise.
 */
static VALUE response_success(VALUE self)
{
  int status = NUM2INT(rb_iv_get(self, "@status"));
  return (status >= 200 && status < 300 ? Qtrue : Qfalse);
}

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

  rb_define_method(response, "initialize", response_init, 0);
  rb_define_method(response, "success?", response_success, 0);
  rb_define_method(response, "timed_out?", response_timed_out, 0);

  /*
   * the body (if any) returned by the server in response to the issued request.
   */
  rb_define_attr(response, "body",    1, 0);
  /* Hash of header name/value pairs returned from the server (if any). */
  rb_define_attr(response, "headers", 1, 0);
  /* the HTTP status code returned by the server. */
  rb_define_attr(response, "status",  1, 0);
  /* The error code returned by curl (if any). */
  rb_define_attr(response, "curl_code",  1, 0);
  /* If +curl_code+ is set, contains a string representation the error. */
  rb_define_attr(response, "curl_error", 1, 0);

  /* provide some aliases to match Typhoeus */
  rb_alias(response, rb_intern("code"), rb_intern("status"));
  rb_alias(response, rb_intern("curl_return_code"), rb_intern("curl_code"));
  rb_alias(response, rb_intern("curl_error_message"), rb_intern("curl_error"));

}
