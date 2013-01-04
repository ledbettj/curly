#ifndef _CURLY_REQUEST_H_
#define _CURLY_REQUEST_H_
#include <ruby.h>
#include "native.h"

void Init_curly_request(VALUE curly_mod);
void request_prepare(VALUE self, native_curly* n);

#endif
