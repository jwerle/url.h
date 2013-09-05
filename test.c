
#include <url.h>
#include <assert.h>

int
main (void) {
  char *url = "http://user:pass@host.com:8080/p/a/t/h?query=string#hash";
  url_data_t *parsed = url_parse(url);

  assert(parsed);
  assert(parsed->href);
  assert(parsed->protocol);
  assert(parsed->auth);

  url_data_inspect(parsed);
  //assert(parsed->href);
  return 0;
}
