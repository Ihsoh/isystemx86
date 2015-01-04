/**
	@File:			ifs1vdisk.h
	@Author:		Ihsoh
	@Date:			2014-6-5
	@Description:
*/

#ifndef	_IFS1VDISK_H_
#define	_IFS1VDISK_H_

#include "types.h"

#define VDISK_SECTORS			5000
#define VDISK_SECTOR_SIZE		512
#define VDISK_BUFFER_SIZE		(VDISK_SECTORS * VDISK_SECTOR_SIZE)

extern
void init_vdisk(IN int8 * symbol);

extern
void destroy_vdisk(IN int8 * symbol);

extern
uint32 sector_count_v(void);

extern
uint32 get_vdisk_size(IN int8 * symbol);

extern
BOOL read_sector_v(	IN int8 * symbol,
					IN uint32 pos,
					OUT uint8 * buffer);

extern
BOOL write_sector_v(IN int8 * symbol,
					IN uint32 pos, 
					IN uint8 * buffer);

extern
BOOL read_sectors_v(IN int8 * symbol,
					IN uint32 pos,
					IN uint8 count,
					OUT uint8 * buffer);

extern
BOOL write_sectors_v(	IN int8 * symbol,
						IN uint32 pos,
						IN uint8 count,
						IN uint8 * buffer);

#endif
