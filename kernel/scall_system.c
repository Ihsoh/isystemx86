/**
	@File:			scall_system.c
	@Author:		Ihsoh
	@Date:			2014-7-29
	@Description:
		系统功能的系统调用处理程序。
*/

#include "scall_system.h"
#include "types.h"
#include "sparams.h"
#include "tasks.h"
#include "console.h"
#include "memory.h"
#include "cmos.h"
#include "console.h"
#include "paging.h"
#include "lock.h"
#include "mqueue.h"
#include <ilib/string.h>

#include "screen.h"

/**
	@Function:		system_call_system
	@Access:		Public
	@Description:
		系统功能的系统调用处理程序。
	@Parameters:
		func, uint32, IN
			功能号。
		base, uint32, IN
			任务的基地址。该地址为物理地址。
		sparams, struct SParams *, IN OUT
			系统调用参数。
	@Return:	
*/
void
system_call_system(uint32 func, uint32 base, struct SParams * sparams)
{
	switch(func)
	{
		//退出程序
		//
		case SCALL_EXIT:
		{
			int32 tid = INT32_SPARAM(sparams->param0);

			kill_task(tid);

			if(tid == get_wait_app_tid())
				set_wait_app_tid(-1);
			break;
		}
		//分配内存
		//
		//参数:
		//	Param0=需要的长度
		//返回值:
		//	Param0=分配内存的起始地址. 0则失败.
		case SCALL_ALLOC_M:
		{
			int32 tid = sparams->tid;
			struct Task * task = get_task_info_ptr(tid);
			uint32 len = UINT32_SPARAM(sparams->param0);
			uint32 start;
			uint32 ui;
			for(ui = 0; ui < MAX_TASK_MEMORY_BLOCK_COUNT && task->memory_block_ptrs[ui] != NULL; ui++);
			if(ui < MAX_TASK_MEMORY_BLOCK_COUNT && find_free_pages((uint32 *)task->real_pagedt_addr, len, &start))
			{
				void * ptr = alloc_memory(len);
				if(ptr != NULL)
				{
					map_user_pagedt_with_rw((uint32 *)task->real_pagedt_addr, start, len, (uint32)ptr, RW_RWE);
					task->memory_block_ptrs[ui] = ptr;
					task->used_memory_size += align_4kb(len);
					sparams->param0 = SPARAM(start);
				}
				else
				{
					void * voidptr = NULL;
					sparams->param0 = SPARAM(voidptr);
				}
			}
			else
			{
				void * voidptr = NULL;
				sparams->param0 = SPARAM(voidptr);
			}
			break;
		}
		//释放内存
		//
		//参数:
		//	Param0=内存地址
		//返回值:
		//	Param0=1则成功, 0则失败
		case SCALL_FREE_M:
		{
			int32 tid = sparams->tid;
			struct Task * task = get_task_info_ptr(tid);
			void * ptr = (void *)(sparams->param0);	//线性地址
			if(ptr == NULL)
			{
				BOOL r = FALSE;
				sparams->param0 = SPARAM(r);
				break;
			}			
			void * physical_address = (void *)get_physical_address(tid, ptr);	//物理地址
			if(physical_address == NULL)
			{
				BOOL r = FALSE;
				sparams->param0 = SPARAM(r);
				break;
			}	
			uint32 ui;
			for(ui = 0; ui < MAX_TASK_MEMORY_BLOCK_COUNT && task->memory_block_ptrs[ui] != physical_address; ui++);
			if(ui < MAX_TASK_MEMORY_BLOCK_COUNT)
			{
				uint32 length = get_memory_block_size(physical_address);
				if(length == 0)
				{
					BOOL r = FALSE;
					sparams->param0 = SPARAM(r);
				}
				else
					if(!free_pages((uint32 *)task->real_pagedt_addr, (uint32)ptr, length))
					{
						BOOL r = FALSE;
						sparams->param0 = SPARAM(r);
					}
					else
					{
						task->memory_block_ptrs[ui] = NULL;
						task->used_memory_size -= align_4kb(length);
						BOOL r = free_memory(physical_address);
						sparams->param0 = SPARAM(r);
					}	
			}	
			else
			{
				BOOL r = FALSE;
				sparams->param0 = SPARAM(r);
			}
			break;
		}
		//获取程序的调用参数
		//
		//参数:
		//	Param0=参数缓冲区地址(相对于调用程序空间的偏移地址)
		case SCALL_GET_PARAM:
		{
			void * buffer = (void *)get_physical_address(sparams->tid, VOID_PTR_SPARAM(sparams->param0));
			struct Task task;
			get_task_info(sparams->tid, &task);
			memcpy(buffer, task.param, 1024);
			break;
		}
		//获取当前时间
		//
		//参数:
		//	Param0=CMOSDateTime结构体地址(相对于调用程序空间的偏移地址)
		case SCALL_GET_DATETIME:
		{
			struct CMOSDateTime * dt = (struct CMOSDateTime *)get_physical_address(sparams->tid, VOID_PTR_SPARAM(sparams->param0));
			get_cmos_date_time(dt);
			break;
		}
		//设置时钟
		//
		//参数:
		//	Param0=0为禁用时钟, 否则启用时钟
		case SCALL_SET_CLOCK:
		{
			set_clock(sparams->param0);
			break;
		}
		//服务端，创建消息队列
		//
		//参数:
		//	Param0=消息队列名称(相对于调用程序空间的偏移地址)
		//返回值:
		//	Param0=消息队列指针(相对于内核空间的偏移地址)
		case SCALL_MQUEUE_S_CREATE:
		{
			int32 tid = sparams->tid;
			struct Task * task = get_task_info_ptr(tid);
			uint32 ui;
			for(ui = 0; ui < MAX_TASK_MQUEUE_COUNT && task->mqueue_ids[ui] != 0; ui++);
			if(ui < MAX_TASK_MQUEUE_COUNT)
			{
				int8 * name = (int8 *)get_physical_address(sparams->tid, VOID_PTR_SPARAM(sparams->param0));
				MQueuePtr mqueue = mqueue_new(name);
				task->mqueue_ids[ui] = (uint32)mqueue;
				sparams->param0 = SPARAM(mqueue);
			}
			else
			{
				uint32 r = 0;
				return SPARAM(r);
			}
			break;
		}
		//服务端，删除消息队列
		//
		//参数:
		//	Param0=消息队列指针(相对于内核空间的偏移地址)
		//返回值:
		//	Param0=返回 TRUE 则成功，否则失败。
		case SCALL_MQUEUE_S_DELETE:
		{
			MQueuePtr mqueue = VOID_PTR_SPARAM(sparams->param0);
			BOOL r = mqueue_free(mqueue);
			sparams->param0 = SPARAM(r);
			break;
		}
		//服务端，向消息队列里添加消息
		//
		//参数:
		//	Param0=消息队列指针(相对于内核空间的偏移地址)
		//	Param1=消息的指针(相对于调用程序空间的偏移地址)
		//返回值:
		//	Param0=返回 TRUE 则成功，否则失败。
		case SCALL_MQUEUE_S_PUSH:
		{
			MQueuePtr mqueue = VOID_PTR_SPARAM(sparams->param0);
			MQueueMessagePtr message = (MQueueMessagePtr)get_physical_address(sparams->tid, VOID_PTR_SPARAM(sparams->param1));
			BOOL r = mqueue_add_message(mqueue, MQUEUE_IN, message);
			sparams->param0 = SPARAM(r);
			break;
		}
		//服务端，从消息队列里获取消息
		//
		//参数:
		//	Param0=消息队列指针(相对于内核空间的偏移地址)
		//	Param1=消息的指针(相对于调用程序空间的偏移地址)
		//返回值:
		//	Param0=返回 TRUE 则成功，否则失败。
		case SCALL_MQUEUE_S_POP:
		{
			MQueuePtr mqueue = VOID_PTR_SPARAM(sparams->param0);
			MQueueMessagePtr message = (MQueueMessagePtr)get_physical_address(sparams->tid, VOID_PTR_SPARAM(sparams->param1));
			MQueueMessagePtr poped_message = mqueue_pop_message(mqueue, MQUEUE_OUT);
			BOOL r = FALSE;
			if(poped_message == NULL)
				r = FALSE;
			else
			{
				memcpy(message, poped_message, sizeof(MQueueMessage));
				free_memory(poped_message);
				r = TRUE;
			}
			sparams->param0 = SPARAM(r);
			break;
		}
		//客户端，向消息队列里添加消息
		//
		//参数:
		//	Param0=消息队列名称(相对于调用程序空间的偏移地址)
		//	Param1=消息的指针(相对于调用程序空间的偏移地址)
		//返回值:
		//	Param0=返回 TRUE 则成功，否则失败。
		case SCALL_MQUEUE_C_PUSH:
		{
			int8 * name = (int8 *)get_physical_address(sparams->tid, VOID_PTR_SPARAM(sparams->param0));
			MQueuePtr mqueue = mqueue_get_ptr_by_name(name);
			BOOL r = FALSE;
			if(mqueue != NULL)
			{
				MQueueMessagePtr message = (MQueueMessagePtr)get_physical_address(sparams->tid, VOID_PTR_SPARAM(sparams->param1));
				r = mqueue_add_message(mqueue, MQUEUE_OUT, message);
			}
			else
				r = FALSE;
			sparams->param0 = SPARAM(r);
			break;
		}
		//客户端，从消息队列里获取消息
		//
		//参数:
		//	Param0=消息队列名称(相对于调用程序空间的偏移地址)
		//	Param1=消息的指针(相对于调用程序空间的偏移地址)
		//返回值:
		//	Param0=返回 TRUE 则成功，否则失败。
		case SCALL_MQUEUE_C_POP:
		{
			int8 * name = (int8 *)get_physical_address(sparams->tid, VOID_PTR_SPARAM(sparams->param0));
			MQueuePtr mqueue = mqueue_get_ptr_by_name(name);
			BOOL r = FALSE;
			if(mqueue != NULL)
			{
				MQueueMessagePtr message = (MQueueMessagePtr)get_physical_address(sparams->tid, VOID_PTR_SPARAM(sparams->param1));
				MQueueMessagePtr poped_message = mqueue_pop_message_with_tid(mqueue, MQUEUE_IN, sparams->tid);
				if(poped_message == NULL)
					r = FALSE;
				else
				{
					memcpy(message, poped_message, sizeof(MQueueMessage));
					free_memory(poped_message);
					r = TRUE;
				}
			}
			else
				r = FALSE;
			sparams->param0 = SPARAM(r);
			break;
		}
		//设置程序后台运行。
		//
		//返回值：
		//	返回 TRUE 则成功，否则失败。
		case SCALL_RUN_IN_BG:
		{
			BOOL r = FALSE;
			if(get_wait_app_tid() == sparams->tid)
			{
				set_wait_app_tid(-1);
				r = TRUE;
			}
			sparams->param0 = SPARAM(r);
			break;
		}
		//设置任务的返回值。
		//
		//参数:
		//	Param0=返回值。
		case SCALL_SET_RETVALUE:
		{
			int32 tid = sparams->tid;
			int32 retvalue = INT32_SPARAM(sparams->param0);
			get_task_info_ptr(tid)->retvalue = retvalue;
			break;
		}
		//执行一个程序。
		//
		//参数:
		//	Param0=程序的路径(相对于调用程序空间的偏移地址)。
		//	Param1=调用程序的参数(相对于调用程序空间的偏移地址)。
		//
		//返回值:
		//	Param0=返回新建的任务的TID。
		case SCALL_EXEC:
		{
			int8 * path = (int8 *)get_physical_address(sparams->tid, VOID_PTR_SPARAM(sparams->param0));
			int8 * param = (int8 *)get_physical_address(sparams->tid, VOID_PTR_SPARAM(sparams->param1));
			int8 buffer[1024];
			if(strlen(path) < sizeof(buffer))
				strcpy(buffer, path);
			if(strlen(param) != 0)
				if(strlen(buffer) + 1 + strlen(param) < sizeof(buffer))
				{
					strcat(buffer, " ");
					strcat(buffer, param);
				}
				else
				{
					BOOL r = FALSE;
					sparams->param0 = SPARAM(r);
					break;
				}
			int8 cpath[1024];
			get_current_path(cpath);
			int32 new_task_tid = create_task_by_file(path, buffer, cpath);
			if(new_task_tid != -1)
			{
				struct Task * new_task = get_task_info_ptr(new_task_tid);
				new_task->allocable = FALSE;
				task_ready(new_task_tid);
			}
			sparams->param0 = SPARAM(new_task_tid);
			break;
		}
		//等待一个任务执行结束。
		//
		//参数:
		//	Param0=要等待的任务的ID。
		//
		//返回值:
		//	Param0=返回 TRUE 则任务未结束，否则结束。
		//	Param1=任务的返回值。
		case SCALL_WAIT:
		{
			int32 tid = INT32_SPARAM(sparams->param0);
			struct Task * task = get_task_info_ptr_unsafe(tid);
			BOOL r = TRUE;
			int32 retvalue = -1;
			if(task != NULL && !task->used)
			{
				task->allocable = TRUE;
				r = FALSE;
				retvalue = task->retvalue;
			}
			sparams->param0 = SPARAM(r);
			sparams->param1 = SPARAM(retvalue);
			break;
		}
	}
}
