/**
	@File:			mqueue.h
	@Author:		Ihsoh
	@Date:			2015-02-26
	@Description:
		消息队列。
*/

#ifndef	_MQUEUE_H_
#define	_MQUEUE_H_

#include "types.h"
#include <dslib/value.h>
#include <dslib/linked_list.h>

#define	MAX_MQUEUE_COUNT			256
#define	MAX_MESSAGE_COUNT			16
#define	MAX_MQUEUE_NAME_LEN			255
#define	MAX_MESSAGE_BYTES_PARAM_LEN	1024

#define	MQUEUE_IN 	0
#define	MQUEUE_OUT 	1

typedef struct
{
	uint32		tid;
	uint32		message;
	DSLValueU	param0;
	DSLValueU	param1;
	DSLValueU	param2;
	DSLValueU	param3;
	DSLValueU	param4;
	DSLValueU	param5;
	DSLValueU	param6;
	DSLValueU	param7;
	DSLValueU	param8;
	DSLValueU	param9;
	uint8		bsparam0[MAX_MESSAGE_BYTES_PARAM_LEN];
} MQueueMessage, * MQueueMessagePtr;

typedef struct
{
	int8				name[MAX_MQUEUE_NAME_LEN + 1];	//消息队列名称。
	DSLLinkedListPtr	in;								//输入通道。程序 <--- 服务程序。
	DSLLinkedListPtr	out;							//输出通道。程序 ---> 服务程序。
} MQueue, * MQueuePtr;

extern
BOOL
MqInit(void);

extern
MQueuePtr
MqNew(IN int8 * name);

extern
BOOL
MqFree(IN MQueuePtr mqueue);

extern
BOOL
MqAddMessage(	IN OUT MQueuePtr mqueue,
				IN int32 in_out,
				IN MQueueMessagePtr message);

extern
MQueueMessagePtr
MqPopMessageWithTaskID(IN OUT MQueuePtr mqueue,
							IN int32 in_out,
							IN int32 tid);

extern
MQueueMessagePtr
MqPopMessage(	IN OUT MQueuePtr mqueue,
				IN int32 in_out);

extern
MQueuePtr
MqGetPtrByName(IN int8 * name);

#endif
