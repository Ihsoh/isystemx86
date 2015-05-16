#ifndef _BINARYHELPER_H_
#define _BINARYHELPER_H_

#include <ilib/ilib.h>

extern void bh_saferead(unsigned char *, int, int, ILFILE *);
extern unsigned char bh_readbyte(ILFILE *);
extern unsigned short bh_readuint16(ILFILE *);
extern int bh_readint32(ILFILE *);
extern long long bh_readint64(ILFILE *);
extern double bh_readdouble(ILFILE *);

#endif
