/**
	@File:			fs.h
	@Author:		Ihsoh
	@Date:			2014-7-18
	@Description:
*/

#ifndef	_FS_IFS1_FS_H_
#define	_FS_IFS1_FS_H_

#include "blocks.h"
#include "../../types.h"
#include "../../lock.h"

#include <dslib/linked_list.h>

#define	MAX_FILENAME_LEN		255
#define	MAX_FILENAME_BUFFER_LEN	(MAX_FILENAME_LEN + 1)
#define	MAX_DIRNAME_LEN			(MAX_FILENAME_LEN)
#define	MAX_DIRNAME_BUFFER_LEN	(MAX_DIRNAME_LEN + 1)
#define	MAX_PATH_LEN			1023
#define	MAX_PATH_BUFFER_LEN		(MAX_PATH_LEN + 1)
#define	MAX_FILE_LEN			(DATA_BLOCK_DATA_LEN * 16128)

#define	FILE_MODE_READ		0x01
#define	FILE_MODE_WRITE		0x02
#define	FILE_MODE_APPEND	0x04
#define	FILE_MODE_ALL		(FILE_MODE_READ | FILE_MODE_WRITE | FILE_MODE_APPEND)

#define	MAX_OPENED_FILE_COUNT	(10 * 1024)

#define IFS1_STDIN		((FileObject *)0x00000000)
#define IFS1_STDOUT		((FileObject *)0x00000001)
#define IFS1_STDERR		((FileObject *)0x00000002)

typedef struct File
{
	int8				symbol[3];									// 被打开的文件所在的磁盘的符号。
	uint32				file_block_id;								// 文件的ID。
	struct FileBlock *	file_block;									// 文件的块。
	int32				mode;										// 打开的模式。
	uint32				next_block_index;							// 下一个要读取的字节所在的块的索引。
	uint32				next_block_pos;								// 下一个要读取的字节在块中的位置。
	BOOL				read_buffer_was_enabled;					// 读取缓冲区是否被启用。
	BOOL				read_buffer_is_valid;						// 读取缓冲区的数据是否有效。
	uint32				read_buffer_block_index;					// 读取缓冲区储存的数据对应的数据块的块ID。
	struct DataBlock 	read_buffer_data_block;						// 读取缓冲区的数据块。
	BOOL				append_buffer_was_enabled;
	uint32				append_buffer_byte_count;
	uint8				append_buffer_bytes[DATA_BLOCK_DATA_LEN];
} FileObject, * FileObjectPtr;

extern
BOOL
Ifs1Init(void);

extern
BOOL
Ifs1Format(IN int8 * symbol);

extern
BOOL
Ifs1Check(IN int8 * symbol);

extern
BOOL
Ifs1GetSLinkTarget(	IN int8 * path,
				IN uint32 len,
				OUT int8 * link);

extern
uint32
Ifs1ParsePathEx(	IN int8 * path, 
				OUT int8 * symbol, 
				OUT int32 * type,
				IN BOOL ret_slnk);

extern
uint32
Ifs1ParsePath(	IN int8 * path, 
			OUT int8 * symbol, 
			OUT int32 * type);

extern
int32
Ifs1GetItemCount(IN int8 * path);

extern
int32
Ifs1GetItems(	IN int8 * path, 
	OUT struct RawBlock * blocks,
	IN uint32 max);

extern
int32
Ifs1GetItemList(	IN int8 * path,
			OUT DSLLinkedList * list);

extern
BOOL
Ifs1CreateDir(	IN int8 * path,
			IN int8 * name);

extern
BOOL
Ifs1CreateFile(IN int8 * path,
			IN int8 * name);

extern
BOOL
Ifs1CreateSLink(	IN int8 * path, 
				IN int8 * name,
				IN int8 * link);

extern
BOOL
Ifs1DeleteDir(IN int8 * path);

extern
BOOL
Ifs1DeleteFile(IN int8 * path);

extern
BOOL
Ifs1DeleteDirRecursively(IN int8 * path);

extern
BOOL
Ifs1DeleteSLink(IN int8 * path);

extern
BOOL
Ifs1GetParentDir(	IN int8 * path,
			OUT int8 * new_path);

extern
BOOL
Ifs1GetFileLocation(	IN int8 * path,
			OUT int8 * new_patn);

extern
BOOL
Ifs1IsValidName(IN int8 * name);

extern
BOOL
Ifs1RenameDir(	IN int8 * path,
			IN int8 * new_name);

extern
BOOL
Ifs1RenameFile(IN int8 * path,
			IN int8 * new_name);

extern
BOOL
Ifs1GetAbsolutePath(	IN int8 * path,
			IN int8 * current_path,
			OUT int8 * new_path);

extern
BOOL
Ifs1IsChildPath(	IN int8 * dir,
			IN int8 * sub_dir);

extern
BOOL
Ifs1CopyFile(	IN int8 * src_path,
			IN int8 * dst_path,
			IN int8 * dst_name);


extern
BOOL
Ifs1DirExists(	IN int8 * path,
			IN int8 * name);

extern
BOOL
Ifs1FileExists(IN int8 * path,
			IN int8 * name);

extern
BOOL
Ifs1Exists(IN int8 * path);

extern
FileObject *
Ifs1OpenFile(	IN int8 * path,
			IN int32 mode);

extern
BOOL
Ifs1CloseFile(IN FileObject * fptr);

extern
BOOL
Ifs1WriteFile(	IN FileObject * fptr,
			IN uint8 * buffer,
			IN uint32 len);

extern
uint32
Ifs1ReadFile(	IN FileObject * fptr,
			OUT uint8 * buffer,
			IN uint32 len);

extern
void
Ifs1ResetFile(IN FileObject * fptr);

extern
BOOL
Ifs1IsEOF(IN FileObject * fptr);

extern
BOOL
Ifs1AppendFile(IN FileObject * fptr,
			IN uint8 * buffer,
			IN uint32 len);

extern
BOOL
Ifs1Repair(IN int8 * symbol);

#define	flen(fptr)	((fptr)->file_block->length)

DEFINE_LOCK_EXTERN(ifs1)

#endif
