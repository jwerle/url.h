
#ifndef _URL_H_
#define _URL_H_ 1

/**
 * Dependencies
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>


/**
 * Max length of a url protocol scheme
 */

#define URL_PROTOCOL_MAX_LENGTH 16


/**
 * Max length of a url host part
 */

#define URL_HOST_MAX_LENGTH 128


/**
 * Max length of a url tld part
 */

#define URL_TLD_MAX_LENGTH 16


/**
 * Max length of a url auth part
 */

#define URL_AUTH_MAX_LENGTH 32


/**
 * URI Schemes
 * http://en.wikipedia.org/wiki/URI_scheme
 */

char *URL_SCHEMES[] = {
  // official IANA registered schemes
  "aaa", "aaas", "about", "acap", "acct", "adiumxtra", "afp", "afs", "aim", "apt", "attachment", "aw",
  "beshare", "bitcoin", "bolo", "callto", "cap", "chrome", "crome-extension", "com-evenbrite-attendee",
  "cid", "coap", "coaps","content", "crid", "cvs", "data", "dav", "dict", "lna-playsingle", "dln-playcontainer",
  "dns", "dtn", "dvb", "ed2k", "facetime", "fax", "feed", "file", "finger", "fish","ftp", "geo", "gg","git",
  "gizmoproject", "go", "gopher", "gtalk", "h323", "hcp", "http", "https", "iax", "icap", "icon","im",
  "imap", "info", "ipn", "ipp", "irc", "irc6", "ircs", "iris", "iris.beep", "iris.xpc", "iris.xpcs","iris.lws",
  "itms", "jabber", "jar", "jms", "keyparc", "lastfm", "ldap", "ldaps", "magnet", "mailserver","mailto",
  "maps", "market", "message", "mid", "mms", "modem", "ms-help", "mssettings-power", "msnim", "msrp",
  "msrps", "mtqp", "mumble", "mupdate", "mvn", "news", "nfs", "ni", "nih", "nntp", "notes","oid",
  "paquelocktoken", "pack", "palm", "paparazzi", "pkcs11", "platform", "pop", "pres", "prospero", "proxy",
  "psyc","query", "reload", "res", "resource", "rmi", "rsync", "rtmp","rtsp",  "secondlife", "service","session",
  "sftp", "sgn", "shttp", "sieve", "sip", "sips", "skype", "smb", "sms", "snews", "snmp", "soap.beep","soap.beeps",
  "soldat", "spotify", "ssh", "steam", "svn", "tag", "teamspeak", "tel", "telnet", "tftp", "things","thismessage",
  "tn3270", "tip", "tv", "udp", "unreal", "urn", "ut2004", "vemmi","ventrilo", "videotex", "view-source", "wais","webcal",
  "ws", "wss", "wtai", "wyciwyg", "xcon", "xcon-userid", "xfire","xmlrpc.beep",  "xmlrpc.beeps", "xmpp", "xri","ymsgr",

  // unofficial schemes
  "javascript", "jdbc", "doi"
};


/**
 * `url_data` struct that defines parts
 * of a parsed URL such as host and protocol
 */

typedef struct url_data {
  char *href;
  char *protocol;
  char *host;
  char *auth;
  char *hostname;
  char *pathname;
  char *search;
  char *path;
  char *hash;
  char *query;
  int port;
} url_data_t;


// prototype

/**
 * Parses a url into parts and returns
 * a `url_data_t *` pointer
 */

url_data_t *
url_parse (char *url);

char *
url_get_protocol (char *url);

char *
url_get_auth (char *url);

char *
url_format (url_data_t *data);

char *
url_resolve (char *from, char *to);

void
url_free (url_data_t *data);

bool
url_is_protocol (char *str);

bool
url_is_host (char *str);

bool
url_has_auth (char *str);

void
url_inspect (char *url);

void
url_data_inspect (url_data_t *data);



// implementation

url_data_t *
url_parse (char *url) {
  url_data_t *data = malloc(sizeof(url_data_t));
  data->href = url;

  // required
  if (!(data->protocol = url_get_protocol(url))) return NULL;

  // optional
  data->auth = url_get_auth(url);


  printf("protocol = %s\n", data->protocol);
  printf("auth     = %s\n", data->auth);

  return data;
}

char *
url_get_protocol (char *url) {
  char *protocol = malloc(URL_PROTOCOL_MAX_LENGTH * sizeof(char));
  sscanf(url, "%[^://]", protocol);
  if (url_is_protocol(protocol)) return protocol;
  else return NULL;
}


bool
url_is_protocol (char *str) {
  int count = sizeof(URL_SCHEMES) / sizeof(URL_SCHEMES[0]);

  for (int i = 0; i < count; ++i) {
    if (0 == strcmp(URL_SCHEMES[i], str)) {
      return true;
    }
  }

  return false;
}


bool
url_is_host (char *str) {
  return false;
}


char *
url_get_auth (char *url) {
  char *tmp = malloc(sizeof(char));
  char *tmp_url = strdup(url);
  char *fmt_url = strdup(url);
  char *protocol = url_get_protocol(tmp_url);
  char *auth = malloc(sizeof(char));
  int n;
  bool has = false;

  strcpy(tmp, "");
  strcpy(fmt_url, "");

  // move pointer exactly the amount
  // of characters in the `prototcol` char
  // plus 3 characters that represent the `://`
  // part of the url
  for (int i = 0, l = (int) strlen(protocol) + 3; i < l; ++i) {
    n = *fmt_url++;
  }

  sscanf(fmt_url, "%[^@]", tmp);

  if (0 != strcmp(tmp, fmt_url)) {
    has = true;
    auth = strdup(tmp);
  }


  free(tmp);
  free(tmp_url);
  //free(fmt_url);

  return has? auth : NULL;
}


void
url_inspect (char *url) {
  url_data_inspect(url_parse(url));
}


void
url_data_inspect (url_data_t *data) {
  printf("#url =>\n");
  printf("    .href: \"%s\"\n",     data->href);
  printf("    .protocol: \"%s\"\n", data->protocol);
  printf("    .host: \"%s\"\n",     data->host);
  printf("    .auth: \"%s\"\n",     data->auth);
  printf("    .hostname: \"%s\"\n", data->hostname);
  printf("    .pathname: \"%s\"\n", data->pathname);
  printf("    .search: \"%s\"\n",   data->search);
  printf("    .path: \"%s\"\n",     data->path);
  printf("    .hash: \"%s\"\n",     data->hash);
  printf("    .query): \"%s\"\n",   data->query);
  printf("    .port: \"%d\"\n",     data->port);
}


#endif
