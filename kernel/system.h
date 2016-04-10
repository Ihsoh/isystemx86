//Filename:		system.h
//Author:		Ihsoh
//Date:			2014-7-25
//Descriptor:	System

#ifndef	_SYSTEM_H_
#define	_SYSTEM_H_

#define	MASTER_VER	"0"
#define	SLAVE_VER	"0"

#define	SYSTEM_SIGNATURE_STRING	"ISys20"

// 明确的系统磁盘盘符。
// 如果SATA的主盘为系统盘，则系统磁盘盘符为SA，否则为DA。
#define	EXPLICIT_SYSTEM_DISK	\
	((DISK_HARD_DISK_TYPE == DISK_TYPE_SATA) ? "SA" : (DISK_HARD_DISK_TYPE == DISK_TYPE_ATA ? "DA" : "DA"))

#define	SYSTEM_DISK	"VS"						// 系统盘符。
#define	SYSTEM_PATH	SYSTEM_DISK":/isystem/"		// 系统目录。
#define	SYSTEM_BINS_PATH	SYSTEM_PATH"bins/"	// 系统非GUI程序目录。

#define	SYSTEM_POINTER_WIDTH		16
#define	SYSTEM_POINTER_HEIGHT		16

#define	SYSTEM_INIT_BAT				SYSTEM_PATH"init.bat"
#define	SYSTEM_HELP_FILE			SYSTEM_PATH"help.txt"
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

#define	SYSTEM_LOG_FILE				SYSTEM_PATH"log/system.log"
#define	SYSTEM_SYSTEM_CONFIG_FILE	SYSTEM_PATH"config/system.json"
#define	SYSTEM_GUI_CONFIG_FILE		SYSTEM_PATH"config/gui.json"

#endif
