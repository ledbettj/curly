#include "multi.h"
#include "native.h"
#include "request.h"
#include "response.h"
#include <curl/curl.h>
#include <unistd.h>

VALUE multi_run(VALUE self);

void Init_curly_multi(VALUE curly)
{
  VALUE multi = rb_define_class_under(curly, "Multi", rb_cObject);

  rb_define_method(multi, "run", multi_run, 0);
}

static VALUE multi_run_unblocked(void* m)
{
  CURLM* c = (CURLM*)m;
  int pending = 0;
  struct timeval timeout;
  int rc;
  fd_set fr, fw, fx;
  int maxfd;
  long curl_timeout;

  curl_multi_perform(c, &pending);

  do {
    FD_ZERO(&fr);
    FD_ZERO(&fw);
    FD_ZERO(&fx);

    maxfd = -1;
    curl_timeout = -1;

    timeout.tv_sec  = 1;
    timeout.tv_usec = 0;

    curl_multi_timeout(c, &curl_timeout);

    if (curl_timeout >= 0) {
      timeout.tv_sec = curl_timeout / 1000;
      timeout.tv_sec  = timeout.tv_sec > 1 ? 1 : (curl_timeout % 1000) * 1000;
    }

    curl_multi_fdset(c, &fr, &fw, &fx, &maxfd);
    rc = select(maxfd, &fr, &fw, &fx, &timeout);

    curl_multi_perform(c, &pending);

  } while (pending);

  return Qnil;
}

VALUE multi_run(VALUE self)
{
  CURLM* c = curl_multi_init();
  VALUE  requests = rb_iv_get(self, "@requests");
  size_t req_len = RARRAY_LEN(requests);
  VALUE* req_ptr = RARRAY_PTR(requests);
  size_t i;
  native_curly* n = malloc(sizeof(native_curly) * req_len);
  VALUE resp;
  long code;

  for(i = 0; i < req_len; ++i) {
    native_curly_alloc(&n[i]);
    request_prepare(req_ptr[i], &n[i]);
    curl_multi_add_handle(c, n[i].handle);
  }

  rb_thread_blocking_region(multi_run_unblocked, c, NULL, NULL);

  /* cleanup */
  for(i = 0; i < req_len; ++i) {
    resp = response_new();

    rb_iv_set(resp, "@body", rb_str_new(n[i].body.value, n[i].body.length));
    rb_iv_set(resp, "@head", rb_str_new(n[i].head.value, n[i].head.length));
    rb_iv_set(resp, "@curl_code", INT2NUM(n[i].curl_rc));

    if (n[i].curl_rc == CURLE_OK) {
      curl_easy_getinfo(n[i].handle, CURLINFO_RESPONSE_CODE, &code);
      rb_iv_set(resp, "@status", LONG2NUM(code));
    } else {
      rb_iv_set(resp, "@curl_error", rb_str_new2(curl_easy_strerror(n[i].curl_rc)));
    }
    rb_iv_set(req_ptr[i], "@response", resp);

    curl_multi_remove_handle(c, n[i].handle);
    native_curly_free(&n[i]);
  }

  curl_multi_cleanup(c);

  return Qnil;
}
