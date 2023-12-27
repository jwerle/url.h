
#include "url.h"
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
  char *url = "http://user:pass@subdomain.host.com:8080/p/%C3%A5/t/h?qu%C3%ABry=strin%C4%9F&foo=bar=yuk&key%23%26%3D=%25&lol#h%C3%a6sh";

  url_data_t *parsed = url_parse(url);
  url_data_t *gh_parsed = url_parse(gh_url);

  assert(parsed);
  assert(gh_parsed);

  url_data_inspect(parsed);
  url_data_inspect(gh_parsed);

  assert(parsed->whole_url);
  assert(parsed->protocol);
  assert(parsed->userinfo);
  assert(parsed->host);
  assert(parsed->port);
  assert(parsed->path);
  assert(parsed->query);
  assert(parsed->fragment);

  assert(gh_parsed->whole_url);
  assert(gh_parsed->protocol);
  assert(gh_parsed->userinfo);
  assert(gh_parsed->host);
  assert(gh_parsed->path);

  assert(url_is_protocol("http"));
  assert(url_is_protocol("https"));
  assert(url_is_protocol("git"));
  assert(url_is_protocol("ssh"));
  assert(url_is_protocol("sftp"));
  assert(url_is_protocol("ftp"));
  assert(url_is_protocol("javascript"));

  STRING_ASSERT("http",                       url_get_protocol(url));
  STRING_ASSERT("user:pass",                  url_get_userinfo(url));
  STRING_ASSERT("subdomain.host.com",         url_get_hostname(url));
  STRING_ASSERT("/p/\xc3\xa5/t/h",            url_get_path    (url));
  assert( strcmp("strin\xc4\x9f",             url_get_query_value   (parsed, "qu\xc3\xabry"))==0 );
  assert( strcmp("bar=yuk",                   url_get_query_value   (parsed, "foo"))==0 );
  assert( strcmp("%",                         url_get_query_value   (parsed, "key#&="))==0 );
  assert( strcmp("",                          url_get_query_value   (parsed, "lol"))==0 );
  STRING_ASSERT("hæsh",                       url_get_fragment(url));
  STRING_ASSERT("8080",                       url_get_port    (url));

  STRING_ASSERT("git",               url_get_protocol(gh_url));
  STRING_ASSERT("github.com",        url_get_hostname(gh_url));
  STRING_ASSERT("git",               url_get_userinfo(gh_url));
  STRING_ASSERT("jwerle/url.h.git",  url_get_path    (gh_url));

  url_free(parsed);
  url_free(gh_parsed);

  return 0;
}
