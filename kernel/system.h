//Filename:		system.h
//Author:		Ihsoh
//Date:			2014-7-25
//Descriptor:	System

#ifndef	_SYSTEM_H_
#define	_SYSTEM_H_

#define	MASTER_VER	"0"
#define	SLAVE_VER	"0"

#define	SYSTEM_DISK	"DA"
#define	SYSTEM_PATH	SYSTEM_DISK":/isystem/"
#define	SYSTEM_BINS_PATH	SYSTEM_PATH"bins/"

#define	SYSTEM_POINTER_WIDTH		16
#define	SYSTEM_POINTER_HEIGHT		16

#define	SYSTEM_INIT_BAT				SYSTEM_PATH"init.bat"
#define	SYSTEM_HELP_FILE			SYSTEM_PATH"help.txt"
#define	SYSTEM_DEFAULT_FONT_FILE	SYSTEM_PATH"fonts/default.ef"
#define	SYSTEM_POINTER				SYSTEM_PATH"resources/images/pointer.img0"
#define	SYSTEM_BG_640_480			SYSTEM_PATH"resources/images/bg_640_480.img0"
#define	SYSTEM_BG_800_600			SYSTEM_PATH"resources/images/bg_800_600.img0"
#define	SYSTEM_BG_1024_768			SYSTEM_PATH"resources/images/bg_1024_768.img0"
#define	SYSTEM_BG_1280_1024			SYSTEM_PATH"resources/images/bg_1280_1024.img0"

#define	KERNELLDR_TEXT				SYSTEM_PATH"kernelldrs/kernelldrtm.bin"
#define	KERNELLDR_VESA_640_480		SYSTEM_PATH"kernelldrs/kernelldrgm_640_480.bin"
#define	KERNELLDR_VESA_800_600		SYSTEM_PATH"kernelldrs/kernelldrgm_800_600.bin"
#define	KERNELLDR_VESA_1024_768		SYSTEM_PATH"kernelldrs/kernelldrgm_1024_768.bin"
#define	KERNELLDR_VESA_1280_1024	SYSTEM_PATH"kernelldrs/kernelldrgm_1280_1024.bin"

#define	SYSTEM_FLAGS_PATH			SYSTEM_PATH"flags/"
#define	SYSTEM_FLAGS_CHECK_FS		SYSTEM_FLAGS_PATH"cfs.flg"

#define	SYSTEM_LOG_FILE				SYSTEM_PATH"data/log/system.log"
#define	SYSTEM_CONFIG_FILE			SYSTEM_PATH"data/config/system.json"

#endif
