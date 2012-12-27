#include <ruby.h>
#include <curl/curl.h>
#include "request.h"
#include "response.h"

static VALUE request_get (int argc, VALUE* argv, VALUE self);
static VALUE request_post(int argc, VALUE* argv, VALUE self);

static VALUE request_alloc(VALUE self);
static VALUE request_perform(VALUE self, CURL* c, VALUE url, VALUE opts);

static struct curl_slist* request_setheaders(VALUE self, CURL* c, VALUE opts);
static int request_add_header(VALUE key, VALUE val, VALUE in);
static VALUE build_query_string(VALUE params);

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

  rb_define_singleton_method(request, "get",  request_get,  -1);
  rb_define_singleton_method(request, "post", request_post, -1);

  syms.to_query = ID2SYM(rb_intern("to_query"));
  syms.params   = ID2SYM(rb_intern("params"));
  syms.body     = ID2SYM(rb_intern("body"));
  syms.headers  = ID2SYM(rb_intern("headers"));
  syms.timeout  = ID2SYM(rb_intern("timeout"));
}

static VALUE request_perform(VALUE self, CURL* c, VALUE url, VALUE opts)
{
  int   rc;
  long  code;
  VALUE timeout, params;
  VALUE resp = response_new();

  struct curl_slist* hdrs = NULL;

  /* invoke header_callback when a header is received and pass `resp` */
  curl_easy_setopt(c, CURLOPT_HEADERFUNCTION, header_callback);
  curl_easy_setopt(c, CURLOPT_WRITEHEADER,    resp);

  /* invoke data_callback when a chunk of data is received and pass `resp` */
  curl_easy_setopt(c, CURLOPT_WRITEFUNCTION, data_callback);
  curl_easy_setopt(c, CURLOPT_WRITEDATA,     resp);

  /* handle headers, query string, timeout, etc. */
  if (TYPE(opts) == T_HASH) {
    hdrs = request_setheaders(self, c, opts);

    if ((params = rb_hash_aref(opts, syms.params)) != Qnil) {
      url = rb_str_plus(url, rb_str_new2("?"));
      url = rb_str_append(url, build_query_string(params));
    }

    if ((timeout = rb_hash_aref(opts, syms.timeout)) != Qnil) {
      curl_easy_setopt(c, CURLOPT_TIMEOUT_MS, NUM2LONG(timeout));
    }
  }

  curl_easy_setopt(c, CURLOPT_URL, RSTRING_PTR(StringValue(url)));

  rc = curl_easy_perform(c);
  rb_iv_set(resp, "@curl_code", INT2NUM(rc));

  if (rc == CURLE_OK) {
    curl_easy_getinfo(c, CURLINFO_RESPONSE_CODE, &code);
    rb_iv_set(resp, "@status", INT2NUM(code));
  } else {
    rb_iv_set(resp, "@curl_error", rb_str_new2(curl_easy_strerror(rc)));
  }

  curl_slist_free_all(hdrs);

  return resp;
}

/* externally visable functions */
static VALUE request_get(int argc, VALUE* argv, VALUE self)
{
  CURL* c = curl_easy_init();
  VALUE url, opts = Qnil, rc;

  rb_scan_args(argc, argv, "11", &url, &opts);

  curl_easy_setopt(c, CURLOPT_HTTPGET, 1L);

  rc = request_perform(self, c, url, opts);

  curl_easy_cleanup(c);

  return rc;
}

static VALUE request_post(int argc, VALUE* argv, VALUE self)
{
  CURL* c = curl_easy_init();
  VALUE url, opts = Qnil, rc, body = Qnil;

  rb_scan_args(argc, argv, "11", &url, &opts);

  curl_easy_setopt(c, CURLOPT_HTTPPOST, NULL);

  if (opts != Qnil &&
      (body = rb_hash_aref(opts, syms.body)) != Qnil) {
    curl_easy_setopt(c, CURLOPT_POSTFIELDS, RSTRING_PTR(StringValue(body)));
  }

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

static struct curl_slist* request_setheaders(VALUE self, CURL* c, VALUE opts)
{
  VALUE headers = rb_hash_aref(opts, syms.headers);
  struct curl_slist* list = NULL;

  if (TYPE(headers) == T_HASH) {
    rb_hash_foreach(headers, request_add_header, (VALUE)&list);
    curl_easy_setopt(c, CURLOPT_HTTPHEADER, list);
  }

  return list;
}

/* invoked by curl_easy_perform when a header is available */
static size_t header_callback(void* buffer, size_t size, size_t count, void* self)
{
  VALUE hash = rb_iv_get((VALUE)self, "@headers");
  VALUE str  = rb_str_new(buffer, size * count);
  VALUE arr;

  /* make sure we have a string in the form 'Header: Value' then parse it. */
  rb_funcall(str, rb_intern("chomp!"), 0);
  arr = rb_funcall(str, rb_intern("split"), 2, rb_str_new2(": "), INT2NUM(2));

  if (rb_funcall(arr, rb_intern("length"), 0) == INT2NUM(2)) {
    rb_hash_aset(hash, rb_ary_entry(arr, 0), rb_ary_entry(arr, 1));
  }

  return size * count;
}

/* invoked by curl_easy_perform when data is available */
static size_t data_callback(void* buffer, size_t size, size_t count, void* self)
{
  VALUE body = rb_iv_get((VALUE)self, "@body");
  rb_str_cat(body, buffer, size * count);

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
