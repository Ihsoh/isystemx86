/**
	@File:			ifs1blocks.h
	@Author:		Ihsoh
	@Date:			2014-6-5
	@Description:
*/

#ifndef	_IFS1BLOCKS_H_
#define	_IFS1BLOCKS_H_

#include "types.h"
#include "cmos.h"

#define	BLOCK_TYPE_FILE		1
#define	BLOCK_TYPE_DATA		2
#define	BLOCK_TYPE_DIR		3

#define	BLOCK_TYPE_SLINK	65536

#define	BLOCK_SECTORS	128
#define	START_BLOCK_ID	10

#define	INVALID_BLOCK_ID	0xffffffff

//结构名:	RawBlock
//说明:		无格式块
struct RawBlock
{
	int32	used;
	int32	type;
	int8	rawdata[65528];
} __attribute__((packed));

//结构名:	FileBlock
//说明:		文件块
struct FileBlock
{
	//288
	int32				used;					//1=使用, 0=未使用
	int32				type;					//1
	uint32				length;					//长度
	int8				filename[256];			//文件名
	struct CMOSDateTime	create;					//创建时间
	struct CMOSDateTime	change;					//修改时间
	int32				lock;					//是否锁定
	
	//736
	uint8 				reserve[736];			//保留

	//64512
	uint32				blockids[16128];		//数据块ID集合. 如果为0xFFFFFFFF, 则未指向任何数据块.
} __attribute__((packed));

#define	DATA_BLOCK_DATA_LEN	65024

//结构名:	DataBlock
//说明:		数据块
struct DataBlock
{
	//12
	int32	used;						//1=使用, 0=未使用
	int32	type;						//2
	uint32	length;						//长度
	
	//500
	uint8	reserve[500];				//保留

	//65024
	uint8	data[DATA_BLOCK_DATA_LEN];	//数据
} __attribute__((packed));

//结构名:	DirBlock
//说明:		目录块
struct DirBlock
{
	//280
	int32				used;						//1=使用, 0=未使用
	int32				type;						//3
	int8				dirname[256];				//目录名
	struct CMOSDateTime	create;						//创建时间
	struct CMOSDateTime	change;						//修改时间
	
	//744
	uint8				reserve[744];				//保留

	//64512
	uint32				blockids[16128];			//文件/目录块ID集合. 如果为0xFFFFFFFF, 则未指向任何文件/目录块.
} __attribute__((packed));

#define	SLINK_BLOCK_LINK_LEN	65024

//结构名:	SLinkBlock
//说明:		软链接块
struct SLinkBlock
{
	int32				used;						//1=使用, 0=未使用
	int32				type;						//65536
	int8				filename[256];				//软链接名

	uint8				reserve[248];				//保留

	int8				link[SLINK_BLOCK_LINK_LEN];	//链接
} __attribute__((packed));

extern
BOOL
get_block(	IN int8 * symbol, 
			IN uint32 id, 
			OUT struct RawBlock * block);

extern
BOOL
set_block(	IN int8 * symbol, 
			IN uint32 id, 
			IN struct RawBlock * block);

extern
BOOL
del_block(	IN int8 * symbol, 
			IN uint32 id);

extern
BOOL
del_all_blocks(IN int8 * symbol);

extern
uint32
add_block(	IN int8 * symbol, 
			IN struct RawBlock * block);

extern
BOOL
fill_dir_block(IN int8 * name,
			OUT struct DirBlock * dir);

extern
BOOL
fill_file_block(IN int8 * name, 
				OUT struct FileBlock * file);

extern
BOOL
fill_slink_block(	IN int8 * name,
					IN int8 * link,
					OUT struct SLinkBlock * slink);

#endif
