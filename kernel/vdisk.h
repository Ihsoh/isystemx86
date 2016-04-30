/**
	@File:			vdisk.h
	@Author:		Ihsoh
	@Date:			2014-6-5
	@Description:
*/

#ifndef	_VDISK_H_
#define	_VDISK_H_

#include "types.h"

#define VDISK_SECTORS			5000
#define VDISK_SECTOR_SIZE		512
#define VDISK_BUFFER_SIZE		(VDISK_SECTORS * VDISK_SECTOR_SIZE)

extern
void
VdskInit(IN int8 * symbol);

extern
void
VdskDestroy(IN int8 * symbol);

extern
uint32
VdskGetSectorCount(void);

extern
uint32
VdskGetSize(IN int8 * symbol);

extern
BOOL
VdskReadSector(	IN int8 * symbol,
				IN uint32 pos,
				OUT uint8 * buffer);

extern
BOOL
VdskWriteSector(IN int8 * symbol,
				IN uint32 pos, 
				IN uint8 * buffer);

extern
BOOL
VdskReadSectors(IN int8 * symbol,
				IN uint32 pos,
				IN uint8 count,
				OUT uint8 * buffer);

extern
BOOL
VdskWriteSectors(	IN int8 * symbol,
					IN uint32 pos,
					IN uint8 count,
					IN uint8 * buffer);

#endif
