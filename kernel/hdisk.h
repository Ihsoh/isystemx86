/**
	@File:			hdisk.h
	@Author:		Ihsoh
	@Date:			2014-7-14
	@Description:
*/

#ifndef	_HDISK_H_
#define	_HDISK_H_

#include "types.h"

extern
BOOL
hdisk_locked(void);

extern
uint32
init_hdisk(void);

extern
uint32
sector_count_h(IN int8 * symbol);

extern
uint32
get_hdisk_size(IN int8 * symbol);

extern
BOOL
read_sector_h(	IN int8 * symbol,
				IN uint32 pos,
				OUT uint8 * buffer);

extern
BOOL
write_sector_h(	IN int8 * symbol,
				IN uint32 pos,
				IN uint8 * buffer);

extern
BOOL
read_sectors_h(	IN int8 * symbol,
				IN uint32 pos,
				IN uint8 count,
				OUT uint8 * buffer);

extern
BOOL
write_sectors_h(IN int8 * symbol,
				IN uint32 pos,
				IN uint8 count,
				IN uint8 * buffer);

#endif
