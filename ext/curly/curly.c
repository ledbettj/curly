#include <ruby.h>
#include "request.h"
#include "response.h"
#include "multi.h"

void Init_curly(void)
{
  VALUE curly_mod = rb_define_module("Curly");

  Init_curly_response(curly_mod);
  Init_curly_request(curly_mod);
  Init_curly_multi(curly_mod);
}
