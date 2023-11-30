#include <string.h>
#include "url.h"

#include "url_char_category.h"
#include "url_char_category_table.h" // generated file!


#define GOTO_ERROR  do{ fprintf(stderr, "ERROR %s Line %u! p=«%s»\n", __FILE__, __LINE__ , p); goto error; }while(0)


/**
 * URI Schemes
 * http://en.wikipedia.org/wiki/URI_scheme
 */

static const char *URL_SCHEMES[] = {
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


#ifndef HAVE_STRDUP
// non C99 standard functions
#if __POSIX_C_SOURCE__ < 200809L
char *
strdup (const char *str) {
  const int n = strlen(str) + 1;
  char *dup = (char *) malloc(n);
  if (dup) strcpy(dup, str);
  return dup;
}
#endif
#endif


static
char*
scan_part(char* start, enum Category category, char delimiter1, char delimiter2) {
  char* p = start;
  for(;;)
  {
    if( *p=='\0' || *p==delimiter1 || *p==delimiter2)
       return p; // success! :-)
    
    if(char_cat[ (unsigned char) *p ] & category) {
      ++p;
    }else{
      return NULL; // illegal character in URL string -> Abort!
    }
  }
}


static
char*
scan_decimal_number(char* start)
{
  char* p = start;
  while(*p >='0' && *p<='9')
  {
    ++p;
  }
  
  return (p!=start) ? p : NULL;
}


/*
static char *
get_part (const char* url, const char *format, int l) {
  bool has = false;
  char *tmp = strdup(url);
  char *tmp_url = strdup(url);
  char *fmt_url = strdup(url);
  char *ret = NULL;

  if (!tmp || !tmp_url || !fmt_url) {
    free(tmp);
    free(tmp_url);
    free(fmt_url);
    return NULL;
  }

  strcpy(tmp, "");
  strcpy(fmt_url, "");

  // move pointer exactly the amount
  // of characters in the `prototcol` char
  // plus 3 characters that represent the `://`
  // part of the url
  char* fmt_url_new = strff(fmt_url, l);
  free(fmt_url);
  fmt_url = fmt_url_new;

  sscanf(fmt_url, format, tmp);

  if (0 != strcmp(tmp, tmp_url)) {
    has = true;
    ret = strdup(tmp);
  }

  free(tmp);
  free(tmp_url);
  free(fmt_url);

  return has? ret : NULL;
}
*/


url_data_t*
url_parse (const char* url) {
  url_data_t *data = (url_data_t *) calloc(1, sizeof(url_data_t));
  if (!data) return NULL;

  char* p = strdup(url);
  if(!p)
    GOTO_ERROR;
  
  data->whole_url = p;
  const char* p_end = p + strlen(p);

  char* protocol_end = scan_part(p, Scheme, ':', '\0');
  if (!protocol_end || *protocol_end=='\0')
    GOTO_ERROR;

  *protocol_end = '\0';
  data->protocol = p;
  const bool is_ssh = url_is_ssh(data->protocol);

  p = protocol_end + 1;
  if(p>=p_end || *p != '/')
    GOTO_ERROR;
    
  ++p; // consume first slash
  if(p>=p_end || *p != '/')
    GOTO_ERROR;
  
  ++p; // consume second slash
  if(p>=p_end)
    GOTO_ERROR;

  char* userinfo_end = scan_part(p, Userinfo, '@', '\0');
  if(userinfo_end && *userinfo_end == '@') { // userinfo found
    *userinfo_end = '\0';
    data->userinfo = p;
    p = userinfo_end + 1;
  }

  if(p>=p_end)
    GOTO_ERROR;

  char* hostname_end = NULL;
  
  if(*p == '[') // IPv6 literal address
  {
    ++p;
    hostname_end = scan_part( p, IPv6Char, ']', '\0' );
    if(!hostname_end)
      GOTO_ERROR;
    
    *hostname_end = '\0'; // eliminate ']'
    data->host = p;
    ++hostname_end;
    if(hostname_end < p_end && !is_ssh && *hostname_end==':') // port number follows the host
    {
      char* port_end = scan_decimal_number( hostname_end+1 );
      if(port_end)
      {
        *port_end = '\0';
        data->port = hostname_end+1;
        p = port_end + 1;
      }else{
        GOTO_ERROR;
      }
    }else{ // no port number
      p = hostname_end + 1;
    }
  }else{ // alphanumeric hostname or IPv4 address
    hostname_end = scan_part( p, Unreserved | SubDelim, ':', '/' );
    if (!hostname_end)
      GOTO_ERROR;
    
    data->host = p;
    if(!is_ssh && *hostname_end==':') // we have a port number
    {
      *hostname_end = '\0';
      char* port_end = scan_decimal_number( hostname_end+1 );
      if(port_end)
      {
        *port_end = '\0';
        data->port = hostname_end+1;
        p = port_end + 1;
      }else{
        GOTO_ERROR;
      }
    }else{ // no port number
      *hostname_end = '\0';
      p = hostname_end + 1;
    }
  }
  
  // FIXME: accepts _any_ sequence of "PChar"s and slashes, which is not RFC compliant
  char* path_end = scan_part( p, PCharSlash, '?', '#' );
  if(!path_end)
    GOTO_ERROR;
  
  data->path = p;
  const bool has_query = (*path_end == '?');
  const bool has_fragment = (*path_end == '#');
  *path_end = '\0';
  
  p = path_end + 1;
  if(has_query)
  {
    char* query_end = scan_part( p, Query, '#', '\0' );
    if(query_end)
    {
      data->query = p;
      if(*query_end == '#') // fragment followes query: ...?query#fragment
      {
        *query_end = '\0';
        char* fragment_end = scan_part( query_end+1, Fragment, '\0', '\0' );
        if(fragment_end)
        {
          data->fragment = query_end+1;
        }else{
          GOTO_ERROR;
        }
      }
    }else{
      GOTO_ERROR; // no valid query string
    }
  }else if(has_fragment) // ...#fragment
  {
        char* fragment_end = scan_part( p, Fragment, '\0', '\0' );
        if(fragment_end)
        {
          data->fragment = p;
        }else{
          GOTO_ERROR;
        }
  }
  
//finished:
  return data;

error:
  url_free(data);
  return NULL;
}


bool
url_is_protocol (const char* str) {
  const unsigned count = sizeof(URL_SCHEMES) / sizeof(URL_SCHEMES[0]);

  for (unsigned i = 0; i < count; ++i) {
    if (0 == strcmp(URL_SCHEMES[i], str)) {
      return true;
    }
  }

  return false;
}

bool
url_is_ssh (const char* str) {
  if (0 == strcmp(str, "ssh") || 0 == strcmp(str, "git")) {
    return true;
  }
  return false;
}

char *
url_get_scheme (const char* url) {
  char *protocol = (char *) malloc(URL_PROTOCOL_MAX_LENGTH);
  if (!protocol) return NULL;

  sscanf(url, "%[^://]", protocol);
  if (url_is_protocol(protocol)) return protocol;

  free(protocol);
  return NULL;
}


char *
url_get_userinfo (const char* url) {
	return NULL; // TODO
}

char *
url_get_hostname (const char* url) {
/*
  size_t l = 3;
  char *protocol = url_get_protocol(url);
  char *tmp_protocol = strdup(protocol);
  char *auth = url_get_auth(url);

  if (!protocol) return NULL;
  if (auth) {
    l += strlen(auth) + 1; // add one @ symbol
    free(auth);
  }

  l += strlen(protocol);

  free(protocol);

  char * hostname = url_is_ssh(tmp_protocol)
    ? get_part(url, "%[^:]", l)
    : get_part(url, "%[^/]", l);
  free(tmp_protocol);
  return hostname;
  */ return NULL;
}

char *
url_get_host (const char* url) {
  char *host = NULL;
/*
  char *hostname = url_get_hostname(url);

  if (!hostname) return NULL;

  sscanf(hostname, "%m[^:]", &host);

  free(hostname);
*/
  return host;
}

char *
url_get_pathname (const char* url) {
/*
  char *path = url_get_path(url);
  char *pathname = NULL;

  if (!path) return NULL;

  sscanf(path, "%m[^?]", &pathname);

  free(path);
*/
  return NULL;
}

char *
url_get_path (const char* url) {
/*
  size_t l = 3;
  char *protocol = url_get_protocol(url);
  char *auth     = url_get_auth(url);
  char *hostname = url_get_hostname(url);


  if (!protocol || !hostname)
  {
    free(protocol);
    free(auth);
    free(hostname);
    return NULL;
  }

  const bool is_ssh = url_is_ssh(protocol);

  l += strlen(protocol) + strlen(hostname);

  if (auth) l+= strlen(auth) +1; // @ symbol

  char* tmp_path = (is_ssh)
    ? get_part(url, ":%s", l)
    : get_part(url, "/%s", l);

  const char *fmt = (is_ssh)? "%s" : "/%s";
  char *path = (char *) malloc(strlen(tmp_path)+2);
  sprintf(path, fmt, tmp_path);

  free(auth);
  free(protocol);
  free(hostname);
  free(tmp_path);
*/
  return NULL;
}

char *
url_get_search (const char* url) {
/*
  char *path = url_get_path(url);
  char *pathname = url_get_pathname(url);

  if (!path) return NULL;

  char *search = NULL;
  sscanf(path + strlen(pathname), "%m[^#]", &search);

  free(path);
  free(pathname);
*/
  return NULL;
}

char *
url_get_query (const char* url) {
/*
  char *search = url_get_search(url);
  char *query = NULL;
  if (!search) return NULL;

  sscanf(search, "?%ms", &query);
  free(search);
*/
  return NULL;
}

char *
url_get_fragment (const char* url) {
/*
  char *path = url_get_path(url);
  if (!path) return NULL;

  char *pathname = url_get_pathname(url);
  if (!pathname) {
    free(path);
    return NULL;
  }
  
  char *search = url_get_search(url);

  const size_t pathname_len = strlen(pathname);
  const size_t search_len   = strlen(search);
  char *tmp_path = strff(path, pathname_len + search_len);

  char* hash = NULL;
  sscanf(tmp_path, "%ms", &hash);
//  tmp_path = strrwd(tmp_path, pathname_len + search_len);
  free(tmp_path);
  free(pathname);
  free(path);
  free(search);
*/
  return NULL;
}

char *
url_get_port (const char* url) {
/*
  char *port = NULL;
  char *hostname = url_get_hostname(url);
  char *host = url_get_host(url);
  if (!hostname) return NULL;

  sscanf(hostname + strlen(host) + 1, "%ms", &port);
  free(hostname);
  free(host);
*/
  return NULL;
}

void
url_inspect (const char* url) {
  url_data_inspect(url_parse(url));
}


#define PRINT_MEMBER(member)  do{ \
	if(data->member) \
		printf("    ." #member ": \"%s\"\n", data->member);  \
	else   \
		printf("    ." #member ": (NULL)\n");  \
	}while(0)

void
url_data_inspect (const url_data_t* data) {
  printf("#url =>\n");
  PRINT_MEMBER(protocol);
  PRINT_MEMBER(host);
  PRINT_MEMBER(userinfo);
  PRINT_MEMBER(host);
  PRINT_MEMBER(port);
  PRINT_MEMBER(path);
  PRINT_MEMBER(query);
  PRINT_MEMBER(fragment);
}

void
url_free (url_data_t *data) {
  if (!data) return;
  free(data->whole_url);
  free(data);
}


// C99 is brain-dead, so we need these:
extern inline
char* url_get_protocol (const char* url);

extern inline
char* url_get_hash (const char* url);
