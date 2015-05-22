#ifndef	_CTYPE_H_
#define	_CTYPE_H_

#include "types.h"

/* These are all the characteristics of characters.  All the
   interdependencies (such as that an alphabetic is an uppercase or a
   lowercase) are here.  If there get to be more than
   (sizeof (unsigned short int) * CHAR_BIT) distinct characteristics,
   many things must be changed that use `unsigned short int's.  */
enum
{
  _ISupper = 1 << 0,			/* UPPERCASE.  */
  _ISlower = 1 << 1,			/* lowercase.  */
  _IScntrl = 1 << 2,			/* Control character.  */
  _ISdigit = 1 << 3,			/* Numeric.  */
  _ISspace = 1 << 4,			/* Whitespace.  */
  _IShex = 1 << 5,			/* A - F, a - f.  */
  _ISpunct = 1 << 6,			/* Punctuation.  */
  _NOgraph = 1 << 7,			/* Printing but nongraphical.  */
  _ISblank = 1 << 8,			/* Blank (usually SPC and TAB).  */
  _ISalpha = _ISupper | _ISlower, 	/* Alphabetic.  */
  _ISalnum = _ISalpha | _ISdigit,	/* Alphanumeric.  */
  _ISxdigit = _ISdigit | _IShex,	/* Hexadecimal numeric.  */
  _ISgraph = _ISalnum | _ISpunct,	/* Graphical.  */
  _ISprint = _ISgraph | _NOgraph	/* Printing.  */
};

extern int isalpha(int ch);
extern int isalnum(int c);
extern int isascii(int ch);
extern int iscntrl(int ch);
extern int isdigit(int ch);
extern int isgraph(int ch);
extern int islower(int ch);
extern int isprint(int ch);
extern int ispunct(int ch);
extern int isspace(int ch);
extern int isupper(int ch);
extern int isxdigit(int ch);
extern int tolower(int ch);
extern int tolower(int ch);
extern int toupper(int ch);

#endif
