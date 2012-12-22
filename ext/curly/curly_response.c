#include "curly_response.h"

VALUE response_new(void)
{
  return rb_class_new_instance(0, NULL,
           rb_const_get_at(
             rb_const_get(rb_cObject, rb_intern("Curly")),
             rb_intern("Response")
           )
         );
}

void Init_curly_response(void)
{

}
