
#include <url.h>
#include <assert.h>

int
main (void) {
  char *url = "http://joseph@host.com:3000";
  url_data_t *parsed = url_parse(url);

  assert(parsed);

  url_data_inspect(parsed);

  assert(parsed->href);
  assert(parsed->auth);
  assert(parsed->protocol);
  assert(parsed->hostname);
  assert(parsed->host);
  assert(parsed->pathname);
  assert(parsed->path);
  assert(parsed->hash);
  assert(parsed->search);
  assert(parsed->query);

  printf("%s\n", url_format(parsed));

  url_free(parsed);
  return 0;
}
