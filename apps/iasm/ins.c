#include <ilib.h>
#include "ins.h"

#define	INS_MODE_16BITS		0
#define	INS_MODE_32BITS		1

static int32 mode = INS_MODE_16BITS;

void
ins_bits16(void)
{
	mode = INS_MODE_16BITS;
}

void
ins_bits32(void)
{
	mode = INS_MODE_32BITS;
}

#define	INS_PREFIX_XX(xx, XX)	\
	uint32	\
	ins_prefix_##xx(OUT uint8 * buffer,	\
					IN uint32 size)	\
	{	\
		if(size == 0)	\
			return 0;	\
		*buffer = PREFIX_##XX;	\
		return 1;	\
	}

INS_PREFIX_XX(cs, CS)
INS_PREFIX_XX(ss, SS)
INS_PREFIX_XX(ds, DS)
INS_PREFIX_XX(es, ES)
INS_PREFIX_XX(fs, FS)
INS_PREFIX_XX(gs, GS)
INS_PREFIX_XX(lock, LOCK)
INS_PREFIX_XX(rep, REP)
INS_PREFIX_XX(repe, REPE)
INS_PREFIX_XX(repz, REPZ)
INS_PREFIX_XX(repne, REPNE)
INS_PREFIX_XX(repnz, REPNZ)
