#include <ruby.h>
#include <curl/curl.h>
#include "request.h"
#include "response.h"
#include "native.h"

/* Request member functions */
static VALUE request_get   (int argc, VALUE* argv, VALUE self);
static VALUE request_post  (int argc, VALUE* argv, VALUE self);
static VALUE request_put   (int argc, VALUE* argv, VALUE self);
static VALUE request_delete(int argc, VALUE* argv, VALUE self);

/* internal helpers */
static VALUE request_alloc(VALUE self);
static VALUE request_perform(VALUE self, CURL* c, VALUE url, VALUE opts);
static struct curl_slist* request_build_headers(VALUE self, CURL* c, VALUE opts);
static int request_add_header(VALUE key, VALUE val, VALUE in);
static VALUE build_query_string(VALUE params);

/* curl callbacks */
static size_t header_callback(void* buffer, size_t size, size_t count, void* self);
static size_t data_callback  (void* buffer, size_t size, size_t count, void* self);

static struct {
  VALUE to_query;
  VALUE params;
  VALUE body;
  VALUE headers;
  VALUE timeout;
} syms;

void Init_curly_request(VALUE curly_mod)
{
  VALUE request = rb_define_class_under(curly_mod, "Request", rb_cObject);

  rb_define_singleton_method(request, "get",    request_get,    -1);
  rb_define_singleton_method(request, "post",   request_post,   -1);
  rb_define_singleton_method(request, "put",    request_put,    -1);
  rb_define_singleton_method(request, "delete", request_delete, -1);

  syms.to_query = ID2SYM(rb_intern("to_query"));
  syms.params   = ID2SYM(rb_intern("params"));
  syms.body     = ID2SYM(rb_intern("body"));
  syms.headers  = ID2SYM(rb_intern("headers"));
  syms.timeout  = ID2SYM(rb_intern("timeout"));
}

static VALUE curl_easy_perform_wrapper(void* args)
{
  native_curly* n = (native_curly*)args;

  n->curl_rc = curl_easy_perform(n->handle);

  return Qnil;
}

static VALUE request_perform(VALUE self, CURL* c, VALUE url, VALUE opts)
{
  long  code;
  VALUE timeout, params, headers;
  VALUE resp = response_new();
  native_curly n;
  struct curl_slist* header_list = NULL;

  /* invoke header_callback when a header is received and pass `resp` */
  curl_easy_setopt(c, CURLOPT_HEADERFUNCTION, header_callback);
  curl_easy_setopt(c, CURLOPT_WRITEHEADER,    &n);

  /* invoke data_callback when a chunk of data is received and pass `resp` */
  curl_easy_setopt(c, CURLOPT_WRITEFUNCTION, data_callback);
  curl_easy_setopt(c, CURLOPT_WRITEDATA,     &n);

  /* handle headers, query string, timeout, etc. */
  if (TYPE(opts) == T_HASH) {

    if ((headers = rb_hash_aref(opts, syms.headers)) != Qnil) {
      header_list = request_build_headers(self, c, headers);
    }

    if ((params = rb_hash_aref(opts, syms.params)) != Qnil) {
      url = rb_str_plus(url, rb_str_new2("?"));
      url = rb_str_append(url, build_query_string(params));
    }

    if ((timeout = rb_hash_aref(opts, syms.timeout)) != Qnil) {
      curl_easy_setopt(c, CURLOPT_TIMEOUT_MS, NUM2LONG(timeout));
    }
  }

  curl_easy_setopt(c, CURLOPT_URL, RSTRING_PTR(StringValue(url)));

  native_curly_alloc(&n, c);

  rb_thread_blocking_region(curl_easy_perform_wrapper, &n, NULL, NULL);

  rb_iv_set(resp, "@body", rb_str_new(n.body.value, n.body.length));
  rb_iv_set(resp, "@curl_code", INT2NUM(n.curl_rc));

  if (n.curl_rc == CURLE_OK) {
    curl_easy_getinfo(c, CURLINFO_RESPONSE_CODE, &code);
    rb_iv_set(resp, "@status", INT2NUM(code));
  } else {
    rb_iv_set(resp, "@curl_error", rb_str_new2(curl_easy_strerror(n.curl_rc)));
  }

  curl_slist_free_all(header_list);
  native_curly_free(&n);

  return resp;
}

/*
 * call-seq:
 *   Request.get(url, options = {}) -> Response
 *
 * Issues a HTTP GET to the specified +url+ using the provided
 * +options+.
 *
 */
static VALUE request_get(int argc, VALUE* argv, VALUE self)
{
  CURL* c = curl_easy_init();
  VALUE url, rc;
  VALUE opts = Qnil;

  rb_scan_args(argc, argv, "11", &url, &opts);

  curl_easy_setopt(c, CURLOPT_HTTPGET, 1L);

  rc = request_perform(self, c, url, opts);

  curl_easy_cleanup(c);

  return rc;
}

/*
 * call-seq:
 *   Request.post(url, options = {}) -> Response
 *
 * Issues a HTTP POST to the specified +url+ using the provided
 * +options+.
 *
 */
static VALUE request_post(int argc, VALUE* argv, VALUE self)
{
  CURL* c = curl_easy_init();
  VALUE opts = Qnil, body = Qnil;
  VALUE url, rc;

  rb_scan_args(argc, argv, "11", &url, &opts);

  curl_easy_setopt(c, CURLOPT_HTTPPOST, NULL);

  if (opts != Qnil &&
      (body = rb_hash_aref(opts, syms.body)) != Qnil) {
    /* TODO: handle case where `body` is a hash. */
    curl_easy_setopt(c, CURLOPT_POSTFIELDS, RSTRING_PTR(StringValue(body)));
  }

  rc = request_perform(self, c, url, opts);

  curl_easy_cleanup(c);

  return rc;
}

/*
 * call-seq:
 *   Request.put(url, options = {}) -> Response
 *
 * Issues a HTTP PUT to the specified +url+ using the provided
 * +options+.
 *
 */
static VALUE request_put(int argc, VALUE* argv, VALUE self)
{
  CURL* c = curl_easy_init();
  VALUE opts = Qnil, body = Qnil;
  VALUE url, rc;

  rb_scan_args(argc, argv, "11", &url, &opts);


  if (opts != Qnil &&
      (body = rb_hash_aref(opts, syms.body)) != Qnil) {
    /* TODO: handle case where `body` is a hash. */
    curl_easy_setopt(c, CURLOPT_POSTFIELDS, RSTRING_PTR(StringValue(body)));
  }

  curl_easy_setopt(c, CURLOPT_CUSTOMREQUEST, "PUT");

  rc = request_perform(self, c, url, opts);

  curl_easy_cleanup(c);

  return rc;
}

/*
 * call-seq:
 *   Request.delete(url, options = {}) -> Curly::Response
 *
 * Issues a HTTP DELETE to the specified +url+ using the provided
 * +options+.
 *
 */
static VALUE request_delete(int argc, VALUE* argv, VALUE self)
{
  CURL* c = curl_easy_init();
  VALUE url, rc;
  VALUE opts = Qnil;

  rb_scan_args(argc, argv, "11", &url, &opts);

  curl_easy_setopt(c, CURLOPT_CUSTOMREQUEST, "DELETE");

  rc = request_perform(self, c, url, opts);

  curl_easy_cleanup(c);

  return rc;
}


static int request_add_header(VALUE key, VALUE val, VALUE in)
{
  struct curl_slist** list = (struct curl_slist**)in;

  /* build the 'Header: Value' string */
  VALUE text = rb_str_plus(StringValue(key), rb_str_new2(": "));
  rb_str_append(text, StringValue(val));

  /* update the head of the list so that the next item is appended correctly */
  *list = curl_slist_append(*list, RSTRING_PTR(text));

  return ST_CONTINUE;
}

static struct curl_slist* request_build_headers(VALUE self, CURL* c, VALUE headers)
{
  struct curl_slist* list = NULL;

  if (TYPE(headers) == T_HASH) {
    rb_hash_foreach(headers, request_add_header, (VALUE)&list);
    curl_easy_setopt(c, CURLOPT_HTTPHEADER, list);
  }

  return list;
}

/* invoked by curl_easy_perform when a header is available */
static size_t header_callback(void* buffer, size_t size, size_t count, void* arg)
{
  native_curly* n = (native_curly*)arg;
  exstr_append(&n->head, buffer, size * count);

  return size * count;
}

/* invoked by curl_easy_perform when data is available */
static size_t data_callback(void* buffer, size_t size, size_t count, void* arg)
{
  native_curly* n = (native_curly*)arg;
  exstr_append(&n->body, buffer, size * count);

  return size * count;
}

/* given a set of key/value pairs, build a url encoded string of the form
 * key=value&key2=value2 using ActiveSupport's to_query if available or
 * our own compatible implementation */
static VALUE build_query_string(VALUE params)
{
  VALUE paramize;
  VALUE has_to_query = rb_funcall(params, rb_intern("respond_to?"), 1,
                                  syms.to_query);

  if (has_to_query == Qtrue) {
    /* use active support */
    return rb_funcall(params, rb_intern("to_query"), 0);
  } else {
    paramize = rb_const_get(rb_const_get(rb_cObject, rb_intern("Curly")),
                            rb_intern("Parameterize"));
    /* use our own version */
    return rb_funcall(paramize, rb_intern("query_string"), 1, params);
  }
}
