/**
	@File:			msg.h
	@Author:		Ihsoh
	@Date:			2015-07-16
	@Description:
		System dump messages。
*/

#ifndef	_DUMP_MSG_H_
#define	_DUMP_MSG_H_

#include "../types.h"

typedef enum
{
	DMPMSGT_UNKNOW		= 0x00000000,		//未知原因引发System dump。
	DMPMSGT_MEMORY		= 0x00000001,		//由于内存的原因引发System dump。
	DMPMSGT_TASKMGR		= 0x00000002		//由于任务管理器引发System dump。

} DumpMessageType;

typedef	struct
{

} DumpMessage, * DumpMessagePtr;

#endif
