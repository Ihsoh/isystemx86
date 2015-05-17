//Filename:		fs.h
//Author:		Ihsoh
//Date:			2014-8-1
//Descriptor:	File System Functions

#ifndef	_FS_H_
#define	_FS_H_

#include "types.h"
#include "system.h"

//结构名:	RawBlock
//说明:		无格式块
struct RawBlock
{
	int used;
	int type;
	uchar rawdata[65528];
} __attribute__((packed));

//结构名:	FileBlock
//说明:		文件块
struct FileBlock
{
	//284
	int used;						//1=使用, 0=未使用
	int type;						//1
	uint length;					//长度
	char filename[256];				//文件名
	struct CMOSDateTime create;		//创建时间
	struct CMOSDateTime change;		//修改时间
	int lock;						//是否锁定
	
	//736
	uchar reserve[736];				//保留

	//64512
	uint blockids[16128];			//数据块ID集合. 如果为0xFFFFFFFF, 则未指向任何数据块.
} __attribute__((packed));

#define	DATA_BLOCK_DATA_LEN	65024

//结构名:	DataBlock
//说明:		数据块
struct DataBlock
{
	//12
	int used;						//1=使用, 0=未使用
	int type;						//2
	uint length;					//长度
	
	//500
	uchar reserve[500];				//保留

	//65024
	uchar data[65024];				//数据
} __attribute__((packed));

//结构名:	DirBlock
//说明:		目录块
struct DirBlock
{
	//280
	int used;						//1=使用, 0=未使用
	int type;						//3
	char dirname[256];				//目录名
	struct CMOSDateTime create;		//创建时间
	struct CMOSDateTime change;		//修改时间
	
	//744
	uchar reserve[744];				//保留

	//64512
	uint blockids[16128];			//文件/目录块ID集合. 如果为0xFFFFFFFF, 则未指向任何文件/目录块.
} __attribute__((packed));

#define	BLOCK_TYPE_FILE		1
#define	BLOCK_TYPE_DATA		2
#define	BLOCK_TYPE_DIR		3

#define	SCALL_FS			2

#define	SCALL_FOPEN			0
#define	SCALL_FCLOSE		1
#define	SCALL_FWRITE		2
#define	SCALL_FREAD			3
#define	SCALL_FAPPEND		4
#define	SCALL_FRESET		5
#define	SCALL_EXISTS_DF		6
#define	SCALL_CREATE_DIR	7
#define	SCALL_CREATE_FILE	8
#define	SCALL_DEL_DIR		9
#define	SCALL_DEL_FILE		10
#define	SCALL_DEL_DIRS		11
#define	SCALL_RENAME_DIR	12
#define	SCALL_RENAME_FILE	13
#define	SCALL_FLEN			14
#define	SCALL_FCREATE_DT	15
#define	SCALL_FCHANGE_DT	16
#define	SCALL_DF_COUNT		17
#define	SCALL_DF			18
#define	SCALL_FIX_PATH		19
#define	SCALL_LOCK_FS		20
#define	SCALL_UNLOCK_FS		21
#define	SCALL_FS_LOCK_STATE	22
#define	SCALL_FEOF			23

#define	FILE_MODE_READ		0x01
#define	FILE_MODE_WRITE		0x02
#define	FILE_MODE_APPEND	0x04
#define	FILE_MODE_ALL		(FILE_MODE_READ | FILE_MODE_WRITE | FILE_MODE_APPEND)

#define	ILFILE	void

extern ILFILE * ILOpenFile(char * path, int mode);
#define	il_fopen(path, mode) (ILOpenFile((path), (mode)))

extern int ILCloseFile(ILFILE * fptr);
#define	il_fclose(fptr)	(ILCloseFile((fptr)))

extern int ILWriteFile(ILFILE * fptr, uchar * buffer, uint len);
#define	il_fwrite(fptr, buffer, len) (ILWriteFile((fptr), (buffer), (len)))

extern uint ILReadFile(ILFILE * fptr, uchar * buffer, uint len);
#define	il_fread(fptr, buffer, len)	(ILReadFile((fptr), (buffer), (len)))

extern int ILAppendFile(ILFILE * fptr, uchar * buffer, uint len);
#define	il_fappend(fptr, buffer, len)	(ILAppendFile((fptr), (buffer), (len)))

extern void ILResetFile(ILFILE * fptr);
#define	il_freset(fptr)	(ILResetFile((fptr)))

extern int ILPathExists(char * path);
#define	il_exists_df(path)	(ILPathExists((path)))

extern int ILCreateDirectory(char * path, char * name);
#define	il_create_dir(path, name)	(ILCreateDirectory((path), (name)))

extern int ILCreateFile(char * path, char * name);
#define	il_create_file(path, name)	(ILCreateFile((path), (name)))

extern int ILDeleteDirectory(char * path);
#define	il_del_dir(path)	(ILDeleteDirectory((path)))

extern int ILDeleteFile(char * path);
#define	il_del_file(path)	(ILDeleteFile((path)))

extern int ILDeleteDirectories(char * path);
#define	il_del_dirs(path)	(ILDeleteDirectories((path)))

extern int ILRenameDirectory(char * path, char * new_name);
#define	il_rename_dir(path, new_name)	(ILRenameDirectory((path), (new_name)))

extern int ILRenameFile(char * path, char * new_name);
#define	il_rename_file(path, new_name)	(ILRenameFile((path), (new_name)))

extern uint ILGetFileLength(ILFILE * fptr);
#define	il_flen(fptr)	(ILGetFileLength((fptr)))

extern void ILGetFileCreatedTime(ILFILE * fptr, struct CMOSDateTime * dt);
#define	il_fcreate_dt(fptr, dt)	(ILGetFileCreatedTime((fptr), (dt)))

extern void ILGetFileChangedTime(ILFILE * fptr, struct CMOSDateTime * dt);
#define	il_fchange_dt(fptr, dt)	(ILGetFileChangedTime((fptr), (dt)))

extern int ILGetDirectoryItemCount(char * path);
#define	il_df_count(path)	(ILGetDirectoryItemCount((path))

extern int ILGetDirectoryItems(char * path, struct RawBlock * raw_blocks, uint max);
#define	il_df(path, raw_blocks, max)	(ILGetDirectoryItems((path), (raw_blocks), (max)))

extern int ILFixPath(char * path, char * new_path);
#define	il_fix_path(path, new_path) (ILFixPath((path), (new_path)))

extern int ILIsEndOfFile(ILFILE * fptr);
#define	il_feof(fptr)	(ILIsEndOfFile((fptr)))

#endif
