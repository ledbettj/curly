#include "multi.h"

void Init_curly_multi(VALUE curly)
{
  VALUE multi = rb_define_class_under(curly, "Multi", rb_cObject);
}
