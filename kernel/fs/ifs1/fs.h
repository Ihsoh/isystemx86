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

#define IFS1_STDIN		((FILE *)0x00000000)
#define IFS1_STDOUT		((FILE *)0x00000001)
#define IFS1_STDERR		((FILE *)0x00000002)

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
} FILE;

extern
BOOL
format_disk(IN int8 * symbol);

extern
BOOL
check_disk(IN int8 * symbol);

extern
BOOL
get_slink_link(	IN int8 * path,
				IN uint32 len,
				OUT int8 * link);

extern
uint32
parse_path_ex(	IN int8 * path, 
				OUT int8 * symbol, 
				OUT int32 * type,
				IN BOOL ret_slnk);

extern
uint32
parse_path(	IN int8 * path, 
			OUT int8 * symbol, 
			OUT int32 * type);

extern
int32
df_count(IN int8 * path);

extern
int32
df(	IN int8 * path, 
	OUT struct RawBlock * blocks,
	IN uint32 max);

extern
int32
dir_list(	IN int8 * path,
			OUT DSLLinkedList * list);

extern
BOOL
create_dir(	IN int8 * path,
			IN int8 * name);

extern
BOOL
create_file(IN int8 * path,
			IN int8 * name);

extern
BOOL
create_slink(	IN int8 * path, 
				IN int8 * name,
				IN int8 * link);

extern
BOOL
del_dir(IN int8 * path);

extern
BOOL
del_file(IN int8 * path);

extern
BOOL
del_dirs(IN int8 * path);

extern
BOOL
del_slink(IN int8 * path);

extern
BOOL
prev_dir(	IN int8 * path,
			OUT int8 * new_path);

extern
BOOL
file_dir(	IN int8 * path,
			OUT int8 * new_patn);

extern
BOOL
is_valid_df_name(IN int8 * name);

extern
BOOL
rename_dir(	IN int8 * path,
			IN int8 * new_name);

extern
BOOL
rename_file(IN int8 * path,
			IN int8 * new_name);

extern
BOOL
fix_path(	IN int8 * path,
			IN int8 * current_path,
			OUT int8 * new_path);

extern
BOOL
is_sub_dir(	IN int8 * dir,
			IN int8 * sub_dir);

extern
BOOL
copy_file(	IN int8 * src_path,
			IN int8 * dst_path,
			IN int8 * dst_name);


extern
BOOL
exists_dir(	IN int8 * path,
			IN int8 * name);

extern
BOOL
exists_file(IN int8 * path,
			IN int8 * name);

extern
BOOL
exists_df(IN int8 * path);

extern
FILE *
open_file(	IN int8 * path,
			IN int32 mode);

extern
BOOL
close_file(IN FILE * fptr);

extern
BOOL
write_file(	IN FILE * fptr,
			IN uint8 * buffer,
			IN uint32 len);

extern
uint32
read_file(	IN FILE * fptr,
			OUT uint8 * buffer,
			IN uint32 len);

extern
void
reset_file(IN FILE * fptr);

extern
BOOL
is_eof(IN FILE * fptr);

extern
BOOL
append_file(IN FILE * fptr,
			IN uint8 * buffer,
			IN uint32 len);

extern
BOOL
repair_ifs1(IN int8 * symbol);

#define	flen(fptr)	((fptr)->file_block->length)

DEFINE_LOCK_EXTERN(ifs1)

#endif
