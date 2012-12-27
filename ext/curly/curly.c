#include <ruby.h>
#include "curly_request.h"
#include "curly_response.h"

void Init_curly(void)
{
  VALUE curly_mod = rb_define_module("Curly");

  Init_curly_response(curly_mod);
  Init_curly_request(curly_mod);
}
