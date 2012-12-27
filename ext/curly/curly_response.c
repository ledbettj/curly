#include "curly_response.h"

static VALUE response_init(VALUE self)
{
  rb_iv_set(self, "@body", rb_str_new2(""));
  rb_iv_set(self, "@headers", rb_hash_new());
  rb_iv_set(self, "@status", INT2NUM(0));

  return Qnil;
}

VALUE response_new(void)
{
  return rb_class_new_instance(0, NULL,
           rb_const_get_at(
             rb_const_get(rb_cObject, rb_intern("Curly")),
             rb_intern("Response")
           )
         );
}

void Init_curly_response(VALUE curly_mod)
{
  VALUE response = rb_define_class_under(curly_mod, "Response", rb_cObject);

  rb_define_method(response, "initialize", response_init, 0);
  rb_define_attr(response, "body",    1, 0);
  rb_define_attr(response, "headers", 1, 0);
  rb_define_attr(response, "status",  1, 0);

  rb_define_attr(response, "curl_code",  1, 0);
  rb_define_attr(response, "curl_error", 1, 0);

}
