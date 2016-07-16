#ifndef	STABLE_H_
#define	STABLE_H_

#include <ilib/ilib.h>

#define	MAX_SYMBOL_NAME_LENGTH	49
#define	MAX_SYMBOL_COUNT		1024

extern void InitSTable(void);
extern void DestroySTable(void);
extern void AddLabelToSTable(char * Name, uint Offset);
extern uint GetLabelOffsetFromSTable(char * Name, int * Found);
extern int HasLabel(char * Name);

#endif
