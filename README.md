url.h
----

Parse URLs much like Node's [url](http://nodejs.org/api/url.html) module.

## usage

```c
#include <url.h>
```

## example

```c
#include <url.h>

int
main (void) {
  url_data_t *parsed = url_parse("http://user:pass@host.com:8080/p/a/t/h?query=string#hash");

  url_free(parsed);
}
```

The `url_data_t *` holds the parsed values and can be written to `stdout` with `url_inspect(parsed);`

```c
url_inspect(parsed);
```

***output***

```
#url =>
    .href: "http://user:pass@subdomain.host.com:8080/p/a/t/h?query=string#hash"
    .protocol: "http"
    .host: "subdomain.host.com"
    .auth: "user:pass"
    .hostname: "subdomain.host.com:8080"
    .pathname: "/p/a/t/h"
    .search: "?query=string"
    .path: "/p/a/t/h?query=string#hash"
    .hash: "#hash"
    .query: "query=string"
    .port: "8080"
```

## api

### url_parse(url);

Parses a url into parts and returns a `url_data_t *` pointer`. 

```c
url_data_t *
url_parse (char *url);
```

***example***

```c
url_data_t *parsed = url_parse("http://google.com");
```

### url_get_protocol(url);

Returns the protocol part of a url.

```c
char *
url_get_protocol (char *url);
```

***example***

```c
char *protocol = url_get_protocol("https://github.com"); // https
```

### url_get_auth(url);

Returns the auth part of a url.

```c
char *
url_get_auth (char *url);
```

***example***

```c
char *auth = url_get_auth("sftp://user:password@domain.com:5000/path/to/directory"); // user:password
```

### url_get_hostname(url);

Returns the hostname part of a url.

```c
char *
url_get_hostname (char *url);
```

***example***

```c
char *hostname = url_get_hostname("http://john@website.host.com:3000/index.html"); // website.host.com:3000
```

### url_get_host(url);

Returns the host part of a url.

```c
char *
url_get_host (char *url);
```

***example***

```c
char *host = url_get_host("https://cache.us.site.com:4444/pixel.gif"); // cache.us.site.com
```

### url_get_pathname(url);

Returns the pathname part of a url.

```c
char *
url_get_pathname (char *url);
```

***example***

```c
char *pathname =  url_get_pathname("http://github.com/jwerle/url.h"); // /jwerle/url.h
```

### url_get_path(url);

Returns the path part of a url.

```c
char *
url_get_path (char *url);
```

***example***

```c
char *path = url_get_path("https://socialnetwork.com/login?user=jwerle"); // /login?user=jwerle
```

### url_get_search(url);

Returns the search part of a url.

```c
char *
url_get_search (char *url);
```

***example***

```c
char *search = url_get_search("https://www.google.com/search?q=hack"); // ?q=hack
```

### url_get_query(url);

```c
char *
url_get_query (char *url);
```

***example***

```c
char *query = url_get_query("http://site.com/home?ref=stream&id=12345"); // ref=stream&id=12345
```

### url_get_hash(url);

Returns the hash part of a url.

```c
char *
url_get_hash (char *url);
```

***example***

```c
char *hash = url_get_hash("http://single-page-app.com/#page=home&id=12345"); // #page=home&id=12345
```

### url_get_port(url);

Returns the port part of a url.

```c
char *
url_get_port (char *url);
```

***example***

```c
char *port = url_get_port("https://site.com:9000"); // 9000
```

### url_free(data);

Frees a `url_data_t` pointer.

```c
void
url_free (url_data_t *data);
```

***example***

```c
url_free(parsed);
```

### url_is_protocol(str);

Checks if given input is a valid url protocol.

```c
bool
url_is_protocol (char *str);
```

***example***

```c
assert(url_is_protocol("https"));
assert(! url_is_protocol("foo"));
```

### url_inspect(url);

Parses and prints all parts of a url.

```c
void
url_inspect (char *url);
```

***example***

```c
url_inspect("https://google.com/search?q=github");
```

***output***

```
#url =>
    .href: "https://google.com/search?q=github"
    .protocol: "https"
    .host: "google.com"
    .auth: ""
    .hostname: "google.com"
    .pathname: "/search"
    .search: "?q=github"
    .path: "/search?q=github"
    .hash: ""
    .query: "q=github"
    .port: ""
```

### url_data_inspect(data);

Prints all parts of a parsed url from a `url_data_t` pointer.

```c
void
url_data_inspect (url_data_t *data);
```

***example***

```c
url_data_inspect(parsed);
```

***output***

```
#url =>
    .href: "http://user:pass@subdomain.host.com:8080/p/a/t/h?query=string#hash"
    .protocol: "http"
    .host: "subdomain.host.com"
    .auth: "user:pass"
    .hostname: "subdomain.host.com:8080"
    .pathname: "/p/a/t/h"
    .search: "?query=string"
    .path: "/p/a/t/h?query=string#hash"
    .hash: "#hash"
    .query: "query=string"
    .port: "8080"
```

### url_is_ssh(protocol);

Checks if a given protocol is an ssh protocol like `ssh` or `git`

```
bool
url_is_ssh (char *str);
```

***example***

```
bool is_ssh = url_is_ssh(url_get_protocol(url));
```

## licence

MIT
