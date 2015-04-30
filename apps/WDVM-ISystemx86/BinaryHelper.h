#ifndef _BINARYHELPER_H_
#define _BINARYHELPER_H_

#include <ilib/ilib.h>

extern void bh_saferead(unsigned char *, int, int, FILE *);
extern unsigned char bh_readbyte(FILE *);
extern unsigned short bh_readuint16(FILE *);
extern int bh_readint32(FILE *);
extern long long bh_readint64(FILE *);
extern double bh_readdouble(FILE *);

#endif
