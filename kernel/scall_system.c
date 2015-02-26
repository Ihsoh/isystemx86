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
#include <string.h>

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
			int tid = sparams->tid;
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
			int tid = sparams->tid;
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
			memcpy(buffer, task.param, sizeof(task.param));
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

		
	}
}
