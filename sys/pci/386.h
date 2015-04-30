#ifndef	_386_H_
#define	_386_H_

#include <ilib/ilib.h>

extern
uint8
inb(IN uint16 port);

extern
uint16
inw(IN uint16 port);

extern
void 
outl(	IN uint16 port,
		IN uint32 data);

extern
uint32
inl(IN uint16 port);

#endif
