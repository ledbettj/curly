#include <ruby.h>
#include "curly_request.h"
#include "curly_response.h"

void Init_curly(void)
{
  VALUE mod = rb_define_module("Curly");

  Init_curly_response();
  Init_curly_request();
}
