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
		允许使用的字符为：'A' ~ 'Z', 'a' ~ 'z', '0' ~ '9', '_'。
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
		if(!(	(chr >= 'A' && chr <= 'Z') 
				|| (chr >= 'a' && chr <= 'z') 
				|| (chr >= '0' && chr <= '9') 
				|| chr == '_'))
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
	if(	mqueues == NULL 
		|| !mqueue_validate_name(name) 
		|| mqueue_get_ptr_by_name(name) != NULL)
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
		dsl_lnklst_free(queue->in);
		free_memory(queue);
		return NULL;
	}
	DSLLinkedListNodePtr node = dsl_lnklst_new_object_node(queue);
	if(node == NULL)
	{
		dsl_lnklst_free(queue->in);
		dsl_lnklst_free(queue->out);
		free_memory(queue);
		return NULL;
	}
	if(!dsl_lnklst_add_node(mqueues, node))
	{
		free_memory(node);
		dsl_lnklst_free(queue->in);
		dsl_lnklst_free(queue->out);
		free_memory(queue);
		return NULL;
	}
	strcpy(queue->name, name);
	return queue;
}

/**
	@Function:		mqueue_free
	@Access:		Public
	@Description:
		删除一个消息队列。
	@Parameters:
		mqueue, MQueuePtr, IN
			指向一个将要删除的消息队列的指针。
	@Return:
		BOOL
			返回 TRUE 则成功，否则失败。	
*/
BOOL
mqueue_free(IN MQueuePtr mqueue)
{
	if(mqueues == NULL || mqueue == NULL)
		return FALSE;
	dsl_lnklst_delete_all_object_node(mqueue->in);
	dsl_lnklst_free(mqueue->in);
	dsl_lnklst_delete_all_object_node(mqueue->out);
	dsl_lnklst_free(mqueue->out);
	DSLLinkedListNodePtr node = mqueues->head;
	while(node != NULL)
	{
		if(node->value.value.object_value == mqueue)
		{
			dsl_lnklst_delete_node(mqueues, node);
			return TRUE;
		}
		node = node->next;
	}
	return FALSE;
}

/**
	@Function:		_mqueue_add_message
	@Access:		Private
	@Description:
		向一个消息队列的通道(In/Out)添加一条消息。
	@Parameters:
		list, DSLLinkedListPtr, IN OUT
			指向消息队列通道的指针。
		message, MQueueMessagePtr, IN
			指向要添加到消息队列的消息的指针。
	@Return:
		BOOL
			返回 TRUE 则成功，否则失败。	
*/
static
BOOL
_mqueue_add_message(IN OUT DSLLinkedListPtr list,
					IN MQueueMessagePtr message)
{
	if(	list == NULL 
		|| message == NULL 
		|| dsl_lnklst_count(list) == MAX_MESSAGE_COUNT)
		return FALSE;
	MQueueMessagePtr new_message = (MQueueMessagePtr)alloc_memory(sizeof(MQueueMessage));
	if(new_message == NULL)
		return FALSE;
	memcpy(new_message, message, sizeof(MQueueMessage));
	DSLLinkedListNodePtr node = dsl_lnklst_new_object_node(new_message);
	if(node == NULL)
		return FALSE;
	if(!dsl_lnklst_add_node(list, node))
	{
		free_memory(node);
		return FALSE;
	}
	return TRUE;
}

/**
	@Function:		mqueue_add_message
	@Access:		Public
	@Description:
		向一个消息队列的通道(In/Out)添加一条消息。
	@Parameters:
		mqueue, MQueuePtr, IN OUT
			指向消息队列的指针。
		in_out, int32, IN
			MQUEUE_IN 和 MQUEUE_OUT。
		message, MQueueMessagePtr, IN
			指向要添加到消息队列的消息的指针。
	@Return:
		BOOL
			返回 TRUE 则成功，否则失败。
*/
BOOL
mqueue_add_message(	IN OUT MQueuePtr mqueue,
					IN int32 in_out,
					IN MQueueMessagePtr message)
{
	if(mqueue == NULL || message == NULL)
		return FALSE;
	DSLLinkedListPtr list;
	switch(in_out)
	{
		case MQUEUE_IN:
			list = mqueue->in;
			break;
		case MQUEUE_OUT:
			list = mqueue->out;
			break;
		default:
			return FALSE;
	}
	return _mqueue_add_message(list, message);
}

/**
	@Function:		_mqueue_pop_message_with_tid
	@Access:		Private
	@Description:
		从一个消息队列的通道(In/Out)获取一条消息。
		并且获取指定任务ID的消息。
	@Parameters:
		list, DSLLinkedListPtr, IN OUT
			指向消息队列通道的指针。
		tid, int32, IN
			任务ID。
	@Return:
		MQueueMessagePtr
			指向获取的消息队列的消息的指针。
			使用完毕需要自行释放。
*/
static
MQueueMessagePtr
_mqueue_pop_message_with_tid(	IN OUT DSLLinkedListPtr list,
								IN int32 tid)
{
	if(list == NULL || dsl_lnklst_count(list) == 0)
		return NULL;
	DSLLinkedListNodePtr node = list->head;
	MQueueMessagePtr message = NULL;
	while(node != NULL)
	{
		MQueueMessagePtr msg = node->value.value.object_value;
		if(msg->tid == tid)
		{
			message = msg;
			dsl_lnklst_delete_node(list, node);
		}
		node = node->next;
	}
	return message;
}

/**
	@Function:		mqueue_pop_message_with_tid
	@Access:		Public
	@Description:
		从一个消息队列的通道(In/Out)获取一条消息。
		并且获取指定任务ID的消息。
	@Parameters:
		mqueue, MQueuePtr, IN OUT
			指向消息队列的指针。
		in_out, int32, IN
			MQUEUE_IN 和 MQUEUE_OUT。
		tid, int32, IN
			任务ID。
	@Return:
		MQueueMessagePtr
			指向获取的消息队列的消息的指针。
			使用完毕需要自行释放。
*/
MQueueMessagePtr
mqueue_pop_message_with_tid(IN OUT MQueuePtr mqueue,
							IN int32 in_out,
							IN int32 tid)
{
	if(mqueue == NULL)
		return NULL;
	DSLLinkedListPtr list;
	switch(in_out)
	{
		case MQUEUE_IN:
			list = mqueue->in;
			break;
		case MQUEUE_OUT:
			list = mqueue->out;
			break;
		default:
			return NULL;
	}
	return _mqueue_pop_message_with_tid(list, tid);
}

/**
	@Function:		_mqueue_pop_message
	@Access:		Private
	@Description:
		从一个消息队列的通道(In/Out)获取一条消息。
	@Parameters:
		list, DSLLinkedListPtr, IN OUT
			指向消息队列通道的指针。
	@Return:
		MQueueMessagePtr
			指向获取的消息队列的消息的指针。
			使用完毕需要自行释放。
*/
static
MQueueMessagePtr
_mqueue_pop_message(IN OUT DSLLinkedListPtr list)
{
	if(list == NULL || dsl_lnklst_count(list) == 0)
		return NULL;
	DSLLinkedListNodePtr node = dsl_lnklst_get(list, 0);
	if(node == NULL)
		return NULL;
	MQueueMessagePtr message = node->value.value.object_value;
	dsl_lnklst_delete_node(list, node);
	return message;
}

/**
	@Function:		mqueue_pop_message
	@Access:		Public
	@Description:
		从一个消息队列的通道(In/Out)获取一条消息。
	@Parameters:
		mqueue, MQueuePtr, IN OUT
			指向消息队列的指针。
		in_out, int32, IN
			MQUEUE_IN 和 MQUEUE_OUT。
	@Return:
		MQueueMessagePtr
			指向获取的消息队列的消息的指针。
			使用完毕需要自行释放。
*/
MQueueMessagePtr
mqueue_pop_message(	IN OUT MQueuePtr mqueue,
					IN int32 in_out)
{
	if(mqueue == NULL)
		return NULL;
	DSLLinkedListPtr list;
	switch(in_out)
	{
		case MQUEUE_IN:
			list = mqueue->in;
			break;
		case MQUEUE_OUT:
			list = mqueue->out;
			break;
		default:
			return NULL;
	}
	return _mqueue_pop_message(list);
}

/**
	@Function:		mqueue_get_ptr_by_name
	@Access:		Public
	@Description:
		通过一个名称获取指向消息队列的指针。
	@Parameters:
		name, int8 *, IN
			名称。
	@Return:
		MQueuePtr
			指向消息队列的指针
*/
MQueuePtr
mqueue_get_ptr_by_name(IN int8 * name)
{
	if(mqueues == NULL || name == NULL)
		return NULL;
	DSLLinkedListNodePtr node = mqueues->head;
	while(node != NULL)
	{
		MQueuePtr mqueue = node->value.value.object_value;
		if(strcmp(mqueue->name, name) == 0)
			return mqueue;
		node = node->next;
	}
	return NULL;
}
