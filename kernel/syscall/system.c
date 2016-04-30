/**
	@File:			system.c
	@Author:		Ihsoh
	@Date:			2014-7-29
	@Description:
		系统功能的系统调用处理程序。
*/

#include "system.h"
#include "../types.h"
#include "../sparams.h"
#include "../tasks.h"
#include "../console.h"
#include "../memory.h"
#include "../cmos.h"
#include "../console.h"
#include "../paging.h"
#include "../lock.h"
#include "../mqueue.h"
#include "../timer.h"

#include <ilib/string.h>

/**
	@Function:		_ScSysProcess
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
_ScSysProcess(	IN uint32 func,
				IN uint32 base,
				IN OUT struct SParams * sparams)
{
	switch(func)
	{
		//退出程序
		//
		case SCALL_EXIT:
		{
			int32 tid = INT32_SPARAM(sparams->param0);

			TskmgrKillSystemTask(tid);

			if(tid == ConGetCurrentApplicationTid())
				ConSetCurrentApplicationTid(-1);
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
			struct Task * task = TskmgrGetTaskInfoPtr(tid);
			uint32 len = UINT32_SPARAM(sparams->param0);
			uint32 start;
			uint32 ui;
			for(ui = 0;
				ui < MAX_TASK_MEMORY_BLOCK_COUNT && task->memory_block_ptrs[ui] != NULL;
				ui++);
			if(	ui < MAX_TASK_MEMORY_BLOCK_COUNT
				&& PgFindFreePages((uint32 *)task->real_pagedt_addr, len, &start))
			{
				void * ptr = MemAlloc(len);
				if(ptr != NULL)
				{
					PgMapUserPageTableWithPermission(
						(uint32 *)task->real_pagedt_addr,
						start,
						len,
						(uint32)ptr,
						RW_RWE);
					task->memory_block_ptrs[ui] = ptr;
					task->used_memory_size += MemAlign4KB(len);
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
			struct Task * task = TskmgrGetTaskInfoPtr(tid);
			void * ptr = (void *)(sparams->param0);	//线性地址
			if(ptr == NULL)
			{
				BOOL r = FALSE;
				sparams->param0 = SPARAM(r);
				break;
			}			
			void * physical_address = (void *)TaskmgrConvertLAddrToPAddr(tid, ptr);	//物理地址
			if(physical_address == NULL)
			{
				BOOL r = FALSE;
				sparams->param0 = SPARAM(r);
				break;
			}	
			uint32 ui;
			for(ui = 0;
					ui < MAX_TASK_MEMORY_BLOCK_COUNT && task->memory_block_ptrs[ui] != physical_address;
					ui++);
			if(ui < MAX_TASK_MEMORY_BLOCK_COUNT)
			{
				uint32 length = MemGetBlockSise(physical_address);
				if(length == 0)
				{
					BOOL r = FALSE;
					sparams->param0 = SPARAM(r);
				}
				else
					if(!PgFreePages((uint32 *)task->real_pagedt_addr, (uint32)ptr, length))
					{
						BOOL r = FALSE;
						sparams->param0 = SPARAM(r);
					}
					else
					{
						task->memory_block_ptrs[ui] = NULL;
						task->used_memory_size -= MemAlign4KB(length);
						BOOL r = MemFree(physical_address);
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
			void * buffer = (void *)TaskmgrConvertLAddrToPAddr(
				sparams->tid,
				VOID_PTR_SPARAM(sparams->param0));
			struct Task task;
			TskmgrGetTaskInfo(sparams->tid, &task);
			memcpy(buffer, task.param, 1024);
			break;
		}
		//获取当前时间
		//
		//参数:
		//	Param0=CMOSDateTime结构体地址(相对于调用程序空间的偏移地址)
		case SCALL_GET_DATETIME:
		{
			struct CMOSDateTime * dt = (struct CMOSDateTime *)TaskmgrConvertLAddrToPAddr(
				sparams->tid,
				VOID_PTR_SPARAM(sparams->param0));
			CmosGetDateTime(dt);
			break;
		}
		//设置时钟
		//
		//参数:
		//	Param0=0为禁用时钟, 否则启用时钟
		case SCALL_SET_CLOCK:
		{
			ConSetClock(sparams->param0);
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
			struct Task * task = TskmgrGetTaskInfoPtr(tid);
			uint32 ui;
			for(ui = 0; ui < MAX_TASK_MQUEUE_COUNT && task->mqueue_ids[ui] != 0; ui++);
			if(ui < MAX_TASK_MQUEUE_COUNT)
			{
				int8 * name = (int8 *)TaskmgrConvertLAddrToPAddr(
					sparams->tid,
					VOID_PTR_SPARAM(sparams->param0));
				MQueuePtr mqueue = MqNew(name);
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
			BOOL r = MqFree(mqueue);
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
			MQueueMessagePtr message = (MQueueMessagePtr)TaskmgrConvertLAddrToPAddr(
				sparams->tid,
				VOID_PTR_SPARAM(sparams->param1));
			BOOL r = MqAddMessage(mqueue, MQUEUE_IN, message);
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
			MQueueMessagePtr message = (MQueueMessagePtr)TaskmgrConvertLAddrToPAddr(
				sparams->tid,
				VOID_PTR_SPARAM(sparams->param1));
			MQueueMessagePtr poped_message = MqPopMessage(mqueue, MQUEUE_OUT);
			BOOL r = FALSE;
			if(poped_message == NULL)
				r = FALSE;
			else
			{
				memcpy(message, poped_message, sizeof(MQueueMessage));
				MemFree(poped_message);
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
			int8 * name = (int8 *)TaskmgrConvertLAddrToPAddr(
				sparams->tid,
				VOID_PTR_SPARAM(sparams->param0));
			MQueuePtr mqueue = MqGetPtrByName(name);
			BOOL r = FALSE;
			if(mqueue != NULL)
			{
				MQueueMessagePtr message = (MQueueMessagePtr)TaskmgrConvertLAddrToPAddr(
					sparams->tid,
					VOID_PTR_SPARAM(sparams->param1));
				r = MqAddMessage(mqueue, MQUEUE_OUT, message);
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
			int8 * name = (int8 *)TaskmgrConvertLAddrToPAddr(
				sparams->tid,
				VOID_PTR_SPARAM(sparams->param0));
			MQueuePtr mqueue = MqGetPtrByName(name);
			BOOL r = FALSE;
			if(mqueue != NULL)
			{
				MQueueMessagePtr message = (MQueueMessagePtr)TaskmgrConvertLAddrToPAddr(
					sparams->tid,
					VOID_PTR_SPARAM(sparams->param1));
				MQueueMessagePtr poped_message = MqPopMessageWithTaskID(
					mqueue,
					MQUEUE_IN, sparams->tid);
				if(poped_message == NULL)
					r = FALSE;
				else
				{
					memcpy(message, poped_message, sizeof(MQueueMessage));
					MemFree(poped_message);
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
			if(ConGetCurrentApplicationTid() == sparams->tid)
			{
				ConSetCurrentApplicationTid(-1);
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
			TskmgrGetTaskInfoPtr(tid)->retvalue = retvalue;
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
			int8 * path = (int8 *)TaskmgrConvertLAddrToPAddr(
				sparams->tid,
				VOID_PTR_SPARAM(sparams->param0));
			int8 * param = (int8 *)TaskmgrConvertLAddrToPAddr(
				sparams->tid,
				VOID_PTR_SPARAM(sparams->param1));
			int8 buffer[1024];
			if(strlen(path) < sizeof(buffer))
				UtlCopyString(buffer, sizeof(buffer), path);
			if(strlen(param) != 0)
				if(strlen(buffer) + 1 + strlen(param) < sizeof(buffer))
				{
					UtlConcatString(buffer, sizeof(buffer), " ");
					UtlConcatString(buffer, sizeof(buffer), param);
				}
				else
				{
					BOOL r = FALSE;
					sparams->param0 = SPARAM(r);
					break;
				}
			int8 cpath[1024];
			ConGetCurrentDir(cpath);
			int32 new_task_tid = TskmgrCreateTaskByFile(path, buffer, cpath);
			if(new_task_tid != -1)
			{
				struct Task * new_task = TskmgrGetTaskInfoPtr(new_task_tid);
				new_task->allocable = FALSE;
				TskmgrSetTaskToReady(new_task_tid);
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
			struct Task * task = TskmgrGetTaskInfoPtrUnsafely(tid);
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
		//获取一个内存块的大小。
		//
		//参数:
		//	Param0=内存块地址。
		//
		//返回值:
		//	Param0=如果返回0则失败，否则返回内存块大小。
		case SCALL_MEMORY_BLOCK_SIZE:
		{
			int32 tid = sparams->tid;
			void * ptr = TaskmgrConvertLAddrToPAddr(tid,
								VOID_PTR_SPARAM(sparams->param0));
			uint32 r = 0;
			if(ptr != NULL)
				r = MemGetBlockSise(ptr);
			sparams->param0 = SPARAM(r);
			break;
		}
		//分派定时器事件。
		//
		case SCALL_DISPATCH_TICK:
		{
			TmrDispatchTick();
			break;
		}
		//装载ELF文件。
		//
		case SCALL_LOAD_ELF:
		{
			int32 tid = sparams->tid;
			void * vptr = VOID_PTR_SPARAM(sparams->param0);
			CASCTEXT path = (CASCTEXT)TaskmgrConvertLAddrToPAddr(tid, vptr);
			uint32 entry = TaskmgrLoadElfExecutableFile(tid, path);
			sparams->param0 = SPARAM(entry);
			break;
		}
		//装载ELF SO文件。
		//
		case SCALL_LOAD_ELF_SO:
		{
			int32 tid = sparams->tid;
			void * vptr = VOID_PTR_SPARAM(sparams->param0);
			CASCTEXT path = (CASCTEXT)TaskmgrConvertLAddrToPAddr(tid, vptr);
			uint32 ctx_idx = TaskmgrLoadElfSharedObjectFile(tid, path);
			sparams->param0 = SPARAM(ctx_idx);
			break;
		}
		//获取ELF SO的符号。
		//
		case SCALL_GET_ELF_SO_SYMBOL:
		{
			int32 tid = sparams->tid;
			uint32 ctx_idx = UINT32_SPARAM(sparams->param0);
			void * vptr = VOID_PTR_SPARAM(sparams->param1);
			CASCTEXT name = (CASCTEXT)TaskmgrConvertLAddrToPAddr(tid, vptr);
			void * symaddr = TaskmgrGetElfSharedObjectSymbol(tid, ctx_idx, name);
			sparams->param0 = SPARAM(symaddr);
			break;
		}
		//卸载ELF SO文件。
		//
		case SCALL_UNLOAD_ELF_SO:
		{
			int32 tid = sparams->tid;
			uint32 ctx_idx = UINT32_SPARAM(sparams->param0);
			BOOL r = TaskmgrUnloadElfSharedObjectFile(tid, ctx_idx);
			sparams->param0 = SPARAM(r);
			break;
		}
	}
}
