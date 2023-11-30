#ifndef URL_CHAR_CATEGORY_H
#define URL_CHAR_CATEGORY_H

enum Category
{
  Scheme     = 0x01,  // alnum + - .
  Unreserved = 0x02,  // alnum - . _ ~
  GenDelim   = 0x04,  // alnum : / ? # [ ] @
  SubDelim   = 0x08,  // alnum ! $ & ' ( ) * + , ; =
  PCharSlash = 0x10,  // alnum unreserved subdelim : @ /
  HexDigit   = 0x20,  // 0-9 a-f A-F
  Query      = 0x40,  // pchar / ?
  Fragment   = 0x40,  // same as Query :-)
  Userinfo   = 0x80,  // alnum unreserved subdelim :
  IPv6Char   = 0x100, // hexdigit :
};

#endif
