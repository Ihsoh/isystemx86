//Filename:		ifs1blocks.h
//Author:		Ihsoh
//Date:			2014-6-5
//Descriptor:	IFS1 Blocks

#ifndef	_IFS1BLOCKS_H_
#define	_IFS1BLOCKS_H_

#include "types.h"
#include "cmos.h"

#define	BLOCK_TYPE_FILE		1
#define	BLOCK_TYPE_DATA		2
#define	BLOCK_TYPE_DIR		3

#define	BLOCK_SECTORS	128
#define	START_BLOCK_ID	10

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

#define	DATA_BLOCK_DATA_LEN	65492
//结构名:	DataBlock
//说明:		数据块
struct DataBlock
{
	//12
	int used;						//1=使用, 0=未使用
	int type;						//2
	uint length;					//长度
	
	//32
	uchar reserve[32];				//保留

	//65492
	uchar data[DATA_BLOCK_DATA_LEN];//数据
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

extern int get_block(char * symbol, uint id, struct RawBlock * block);
extern int set_block(char * symbol, uint id, struct RawBlock * block);
extern int del_block(char * symbol, uint id);
extern int del_all_blocks(char * symbol);
extern uint add_block(char * symbol, struct RawBlock * block);
extern int fill_dir_block(char * name, struct DirBlock * dir);
extern int fill_file_block(char * name, struct FileBlock * file);

#endif
