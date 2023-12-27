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


struct url_key_value
{
  const char* key;
  const char* value;
};


static
int unhex(const char* s)
{
  if(*s>='0' && *s<='9')
    return *s - '0';
  
  if(*s>='A' && *s<='F')
    return *s - 'A' + 10;
  
  if(*s>='a' && *s<='f')
    return *s - 'a' + 10;
  
  return -1;
}


// decode %xx encoding inplace.
// return NULL in case of error
static
char* decode_percent(char* s)
{
  char* in  = s;
  char* out = s;
  while(*in)
  {
    if(*in=='%')
    {
      const int high = unhex(++in); if(high<0 || *in=='\0') return NULL;
      const int low  = unhex(++in); if(low <0 || *in=='\0') return NULL;
      *out = (char)(high*16u + low);
      ++out;
      ++in;
    }else{
      *out++ = *in++; // usual string copy idiom. :-)
    }
  }
  *out = '\0';
  return s;
}


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


static
struct url_key_value* parse_query_string(char* begin, char* end)
{
  unsigned elements = 1; // each query has at least 1 element,
  for(const char* p = begin; p!=end; ++p)
  {
    if(*p=='&' || *p==';')
      ++elements;
  }
  
  struct url_key_value* kv = calloc(elements+1, sizeof(struct url_key_value)); // add one {NULL,NULL} element as array terminator.
  if(!kv)
    return NULL;
  
  char* p = begin;
  for(unsigned element=0; (element<=elements) && (p<end); ++element)
  {
    char* key = p;
    char* kv_end = scan_part(p, Query, '&', ';');
    if(!kv_end)
      GOTO_ERROR;
    
    *kv_end = '\0';
    
    char* key_end = scan_part(p, Query, '=', '\0');
    
    const bool has_value = (*key_end == '=');
    *key_end = '\0';
    
    kv[element].key = decode_percent(key);
    if(has_value) // real key-value pair
    {
      char* value = key_end+1;
      kv[element].value = decode_percent(value);
    }else{
      kv[element].value = key_end;
    }
    
    p = kv_end+1;
    
  }
  
  return kv;
  
error:
  free(kv);
  return NULL;
}


url_data_t*
url_parse (const char* url) {
  url_data_t *data = (url_data_t *) calloc(1, sizeof(url_data_t));
  if (!data) return NULL;

  char* p = strdup(url);
  if(!p)
    GOTO_ERROR;
  
  data->whole_url = p;
  const char* const p_end = p + strlen(p);

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
  
  char* const second_slash = p;
  
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
        data->port = hostname_end+1;
        p = port_end;
      }else{
        GOTO_ERROR;
      }
    }else{ // no port number
      p = hostname_end;
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
        data->port = hostname_end+1;
        p = port_end;
      }else{
        GOTO_ERROR;
      }
    }else{ // no port number
      p = hostname_end;
    }
  }
  
  // HACK: move userinfo, host and port by one char to add a \0 before the first '/' in the path:
  memmove(second_slash, second_slash+1, p-second_slash);
  if(data->userinfo)
    --(data->userinfo);
  
  --(data->host);
  
  if(data->port)
    --(data->port);
  
  p[-1] = '\0';
  // END HACK
  
  if(is_ssh && *p == ':')
  {
    ++p; // omit the first ':' from ssh URL's path
  }
  
  // FIXME: accepts _any_ sequence of "PChar"s and slashes, which is not RFC compliant
  char* path_end = scan_part( p, PCharSlash, '?', '#' );
  if(!path_end)
    GOTO_ERROR;
  
  const bool has_query = (*path_end == '?');
  const bool has_fragment = (*path_end == '#');
  *path_end = '\0';
  
  data->path = decode_percent(p);
  p = path_end + 1;
  if(has_query)
  {
    char* query_end = scan_part( p, Query, '#', '\0' );
    if(query_end)
    {
      const bool has_fragment = (*query_end == '#');
      *query_end = '\0';
      
      data->query = parse_query_string(p, query_end);
      if(has_fragment) // fragment followes query: ...?query#fragment
      {
        char* fragment_end = scan_part( query_end+1, Fragment, '\0', '\0' );
        if(fragment_end)
        {
          data->fragment = decode_percent(query_end+1);
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
          data->fragment = decode_percent(p);
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


#define GET_MEMBER(member) do{ \
    url_data_t* data = url_parse(url);  \
    char* out = data && data->member ? strdup(data->member) : NULL; \
    url_free(data);                   \
    return out;                       \
 }while(0)


char *
url_get_userinfo (const char* url) {
  GET_MEMBER(userinfo);
}

char *
url_get_hostname (const char* url) {
  GET_MEMBER(host);
}

char *
url_get_host (const char* url) {
  GET_MEMBER(host);
}

char *
url_get_pathname (const char* url) {
  GET_MEMBER(path);
}

char *
url_get_path (const char* url) {
  GET_MEMBER(path);
}


const char *
url_get_query_value (const url_data_t* url, const char* key)
{
  if(url->query == NULL)
    return NULL;
    
  for( const struct url_key_value* kv = url->query; kv->key; ++kv)
  {
     if(strcmp(kv->key, key) == 0)
       return kv->value;
  }
  return NULL;
}


char *
url_get_fragment (const char* url) {
  GET_MEMBER(fragment);
}

char *
url_get_port (const char* url) {
  GET_MEMBER(port);
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
  if(data->query)
  {
     for(unsigned nr=0; data->query[nr].key; ++nr)
     {
        printf("    .query[%u]: \"%s\" -> ", nr, data->query[nr].key);
        if(data->query[nr].value)
          printf("\"%s\"\n", data->query[nr].value);
        else
          printf("(NULL)\n");
     }
  }
  PRINT_MEMBER(fragment);
}

void
url_free (url_data_t *data) {
  if (!data) return;
  free(data->whole_url);
  free((void*)data->query);
  free(data);
}


// C99 is brain-dead, so we need these:
extern inline
char* url_get_protocol (const char* url);

extern inline
char* url_get_hash (const char* url);
