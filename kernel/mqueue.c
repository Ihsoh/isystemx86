/**
	@File:			mqueue.c
	@Author:		Ihsoh
	@Date:			2015-02-26
	@Description:
		消息队列。
*/

#include "mqueue.h"
#include "types.h"
#include "tasks.h"
#include "string.h"
#include <dslib/linked_list.h>

#define	MAX_MQUEUE_COUNT	256
#define	MAX_MESSAGE_COUNT	16

DSLLinkedListPtr mqueues = NULL;

/**
	@Function:		mqueue_init
	@Access:		Public
	@Description:
		初始化消息队列功能。
	@Parameters:
	@Return:
		BOOL
			返回 TRUE 则表示初始化成功，否则失败。		
*/
BOOL
mqueue_init(void)
{
	mqueues = dsl_lnklst_new();
	if(mqueues == NULL)
		return FALSE;
	return TRUE;
}

/**
	@Function:		mqueue_validate_name
	@Access:		Public
	@Description:
		检查是否是一个合法的消息队列名称。
		允许使用的字符为：'A' ~ 'Z', 'a' ~ 'z', '_'。
	@Parameters:
		name, int8 *, IN
			名称。
	@Return:
		BOOL
			返回 TRUE 则符合规定，否则不符合。		
*/
static
BOOL
mqueue_validate_name(IN int8 * name)
{
	uint32 len = strlen(name);
	if(len > MAX_MQUEUE_NAME_LEN)
		return FALSE;
	uint32 ui;
	for(ui = 0; ui < len; ui++)
	{
		int8 chr = name[ui];
		if(!((chr >= 'A' && chr <= 'Z') || (chr >= 'a' && chr <= 'z') || chr == '_'))
			return FALSE;
	}
	return TRUE;
}

/**
	@Function:		mqueue_new
	@Access:		Public
	@Description:
		新建一个消息队列。
	@Parameters:
		name, int8 *, IN
			消息队列名称。
	@Return:
		MQueuePtr
			如果新建失败，则返回 NULL。否则，返回新建的消息队列的指针。		
*/
MQueuePtr
mqueue_new(IN int8 * name)
{
	if(mqueues == NULL || !mqueue_validate_name(name))
		return NULL;
	MQueuePtr queue = (MQueuePtr)alloc_memory(sizeof(MQueue));
	if(queue == NULL)
		return NULL;
	queue->in = dsl_lnklst_new();
	if(queue->in == NULL)
	{
		free_memory(queue);
		return NULL;
	}
	queue->out = dsl_lnklst_new();
	if(queue->out == NULL)
	{
		free_memory(queue->in);
		free_memory(queue);
		return NULL;
	}
	DSLLinkedListNodePtr node = dsl_lnklst_new_object_node(queue);
	if(node == NULL)
	{
		free_memory(queue->in);
		free_memory(queue->out);
		free_memory(queue);
		return NULL;
	}
	if(!dsl_lnklst_add_node(mqueues, node))
	{
		free_memory(node);
		free_memory(queue->in);
		free_memory(queue->out);
		free_memory(queue);
		return NULL;
	}
	strcpy(queue->name, name);
	return queue;
}


