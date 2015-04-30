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

#define	FILE_MODE_READ		0x01
#define	FILE_MODE_WRITE		0x02
#define	FILE_MODE_APPEND	0x04
#define	FILE_MODE_ALL		(FILE_MODE_READ | FILE_MODE_WRITE | FILE_MODE_APPEND)

#define	FILE	void
#define	IL_FILE	FILE

extern IL_FILE * fopen(char * path, int mode);
#define	il_fopen(path, mode) (fopen((path), (mode)))

extern int fclose(FILE * fptr);
#define	il_fclose(fptr)	(fclose((fptr)))

extern int fwrite(FILE * fptr, uchar * buffer, uint len);
#define	il_fwrite(fptr, buffer, len) (fwrite((fptr), (buffer), (len)))

extern uint fread(FILE * fptr, uchar * buffer, uint len);
#define	il_fread(fptr, buffer, len)	(fread((fptr), (buffer), (len)))

extern int fappend(FILE * fptr, uchar * buffer, uint len);
#define	il_fappend(fptr, buffer, len)	(fappend((fptr), (buffer), (len)))

extern void freset(FILE * fptr);
#define	il_freset(fptr)	(freset((fptr)))

extern int exists_df(char * path);
#define	il_exists_df(path)	(exists_df((path)))

extern int create_dir(char * path, char * name);
#define	il_create_dir(path, name)	(create_dir((path), (name)))

extern int create_file(char * path, char * name);
#define	il_create_file(path, name)	(create_file((path), (name)))

extern int del_dir(char * path);
#define	il_del_dir(path)	(del_dir((path)))

extern int del_file(char * path);
#define	il_del_file(path)	(del_file((path)))

extern int del_dirs(char * path);
#define	il_del_dirs(path)	(del_dirs((path)))

extern int rename_dir(char * path, char * new_name);
#define	il_rename_dir(path, new_name)	(rename_dir((path), (new_name)))

extern int rename_file(char * path, char * new_name);
#define	il_rename_file(path, new_name)	(rename_file((path), (new_name)))

extern uint flen(FILE * fptr);
#define	il_flen(fptr)	(flen((fptr)))

extern void fcreate_dt(FILE * fptr, struct CMOSDateTime * dt);
#define	il_fcreate_dt(fptr, dt)	(fcreate_dt((fptr), (dt)))

extern void fchange_dt(FILE * fptr, struct CMOSDateTime * dt);
#define	il_fchange_dt(fptr, dt)	(fchange_dt((fptr), (dt)))

extern int df_count(char * path);
#define	il_df_count(path)	(df_count((path))

extern int df(char * path, struct RawBlock * raw_blocks, uint max);
#define	il_df(path, raw_blocks, max)	(df((path), (raw_blocks), (max)))

extern int fix_path(char * path, char * new_path);
#define	il_fix_path(path, new_path) (fix_path((path), (new_path)))

#endif
