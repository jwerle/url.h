
#include <url.h>
#include <assert.h>
#include <string.h>

#define STRING_ASSERT(expect, str) \
	do{  \
		char* s = str; \
		assert( s ); \
		assert( strcmp(expect, s) == 0); \
		free(s); \
	}while(0)


int
main (void) {
  //url_inspect("https://google.com/search?q=github");

  char *gh_url = "git://git@github.com:jwerle/url.h.git";
  char *url = "http://user:pass@subdomain.host.com:8080/p/a/t/h?query=string#hash";

  url_data_t *parsed = url_parse(url);
  url_data_t *gh_parsed = url_parse(gh_url);

  assert(parsed);
  assert(gh_parsed);

  url_data_inspect(parsed);
  url_data_inspect(gh_parsed);

  assert(parsed->href);
  assert(parsed->auth);
  assert(parsed->protocol);
  assert(parsed->port);
  assert(parsed->hostname);
  assert(parsed->host);
  assert(parsed->pathname);
  assert(parsed->path);
  assert(parsed->hash);
  assert(parsed->search);
  assert(parsed->query);

  assert(gh_parsed->href);
  assert(gh_parsed->protocol);
  assert(gh_parsed->host);
  assert(gh_parsed->auth);
  assert(gh_parsed->hostname);
  assert(gh_parsed->pathname);
  assert(gh_parsed->path);

  assert(url_is_protocol("http"));
  assert(url_is_protocol("https"));
  assert(url_is_protocol("git"));
  assert(url_is_protocol("ssh"));
  assert(url_is_protocol("sftp"));
  assert(url_is_protocol("ftp"));
  assert(url_is_protocol("javascript"));

  STRING_ASSERT("http",                       url_get_protocol(url));
  STRING_ASSERT("user:pass",                  url_get_auth    (url));
  STRING_ASSERT("subdomain.host.com:8080",    url_get_hostname(url));
  STRING_ASSERT("subdomain.host.com",         url_get_host    (url));
  STRING_ASSERT("/p/a/t/h",                   url_get_pathname(url));
  STRING_ASSERT("/p/a/t/h?query=string#hash", url_get_path    (url));
  STRING_ASSERT("?query=string",              url_get_search  (url));
  STRING_ASSERT("query=string",               url_get_query   (url));
  STRING_ASSERT("#hash",                      url_get_hash    (url));
  STRING_ASSERT("8080",                       url_get_port    (url));

  STRING_ASSERT("git",               url_get_protocol(gh_url));
  STRING_ASSERT("github.com",        url_get_host    (gh_url));
  STRING_ASSERT("github.com",        url_get_hostname(gh_url));
  STRING_ASSERT("git",               url_get_auth    (gh_url));
  STRING_ASSERT("jwerle/url.h.git",  url_get_pathname(gh_url));
  STRING_ASSERT("jwerle/url.h.git",  url_get_path    (gh_url));

  url_free(parsed);
  url_free(gh_parsed);

  return 0;
}
