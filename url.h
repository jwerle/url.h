#ifndef URL_H
#define URL_H 1

/**
 * Dependencies
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

/**
 * url.h version
 */

#define URL_VERSION 0.3.0


/**
 * Max length of a url protocol scheme
 */

#define URL_PROTOCOL_MAX_LENGTH 32


/**
 * Max length of a url host part
 */

#define URL_HOSTNAME_MAX_LENGTH 128


/**
 * Max length of a url tld part
 */

#define URL_TLD_MAX_LENGTH 16


/**
 * Max length of a url auth part
 */

#define URL_AUTH_MAX_LENGTH 32


struct url_key_value;

/**
 * `url_data` struct that defines parts
 * of a parsed URL such as host and protocol
 */
typedef struct url_data {
  char* whole_url; // holds the whole URL, but with '\0' to separae the parts of the URL
  const char* protocol; // URL scheme
  const char* userinfo; // can be NULL
  const char* host;
  const char* port;  // can be NULL
  const char* path;
  const struct url_key_value* query; // can be NULL
  const char* fragment;  // can be NULL
} url_data_t;


// prototype

/**
 * Parses a url into parts and returns
 * a `url_data_t *` pointer
 */
url_data_t *
url_parse (const char* url);

// Parses url, returns the "scheme" (a.k.a. "protocol") of the URL.
// Caller must free() the returned string.
char *
url_get_scheme (const char* url);

// Parses url, returns the protocol (a.k.a. URL "scheme") of the URL.
// Caller must free() the returned string.
// DEPRECATED! Use url_get_scheme() instead.
inline
char *
url_get_protocol (const char* url) { return url_get_scheme(url); }

// Parses url, returns "username:password" of the URL if present or NULL.
// Caller must free() the returned string if not NULL.
char *
url_get_userinfo (const char* url);

// Parses url, returns the hostname of the URL.
// Caller must free() the returned string.
char *
url_get_hostname (const char* url);

// Parses url, returns the path of the URL.
// Caller must free() the returned string.
char *
url_get_path (const char* url);

// returns the value for the URL query key, if present.
// returns NULL if URL query does not contain this key.
// value belongs to url, caller MUST NOT free() the string!
const char *
url_get_query_value (const url_data_t* url, const char* key);

// Parses url, returns the fragment (after the "#") of the URL if present or NULL.
// Caller must free() the returned string if not NULL.
char *
url_get_fragment (const char* url);

// Parses url, returns the fragment (after the "#") of the URL if present or NULL.
// Caller must free() the returned string if not NULL.
// DEPRECATED! Use url_get_fragment() instead.
inline
char *
url_get_hash (const char* url) { return url_get_fragment(url); }

// Parses url, returns the port of the URL if present or NULL.
// Caller must free() the returned string if not NULL.
char *
url_get_port (const char* url);

// Frees the data and its members appropiately.
void
url_free (url_data_t* data);

// Returns 'true' if the str is the name of a well-known URL scheme.
bool
url_is_protocol (const char* str);

// Returns 'true' if the string is "ssh" or "git"
bool
url_is_ssh (const char* str);

// Parses the URL and prints its members to stdout.
void
url_inspect (const char* url);

// Prints the members to stdout.
void
url_data_inspect (const url_data_t* data);

#endif
