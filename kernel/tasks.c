/**
	@File:			tasks.c
	@Author:		Ihsoh
	@Date:			2014-7-22
	@Description:
		提供多任务功能。
*/

#include "tasks.h"
#include "types.h"
#include "386.h"
#include "memory.h"
#include "screen.h"
#include "ifs1fs.h"
#include "paging.h"
#include "die.h"
#include "lock.h"
#include "mqueue.h"
#include "scall_fs.h"
#include <string.h>

static struct Task tasks[MAX_TASK_COUNT];
static int32 running_tid = -1;

DEFINE_LOCK_IMPL(tasks)

/**
	@Function:		init_tasks
	@Access:		Public
	@Description:
		初始化多任务。
	@Parameters:
	@Return:
		BOOL
			返回TRUE则成功，否则失败。
*/
BOOL
init_tasks(void)
{
	struct die_info info;
	uint32 ui;
	for(ui = 0; ui < MAX_TASK_COUNT; ui++)
	{
		tasks[ui].used = 0;
		tasks[ui].allocable = TRUE;
		struct Desc tss_desc;
		struct Gate task_gate;
		uint32 temp = (uint32)&tasks[ui].tss;
		tss_desc.limitl = sizeof(struct TSS) - 1;
		tss_desc.basel = (uint16)(temp & 0xFFFF);
		tss_desc.basem = (uint8)((temp >> 16) & 0xFF);
		tss_desc.baseh = (uint8)((temp >> 24) & 0xFF);
		tss_desc.attr = AT386TSS + DPL3;
		set_desc_to_gdt(400 + ui * 5 + 0, (uint8 *)&tss_desc);
	
		task_gate.offsetl = 0;
		task_gate.offseth = 0;
		task_gate.dcount = 0;
		task_gate.selector = ((400 + ui * 5 + 0) << 3) | RPL3;
		task_gate.attr = ATTASKGATE | DPL3;
		set_desc_to_gdt(400 + ui * 5 + 1, (uint8 *)&task_gate);

		tasks[ui].opened_file_ptrs = alloc_memory(MAX_TASK_OPENED_FILE_COUNT * sizeof(FILE *));
		tasks[ui].memory_block_ptrs = alloc_memory(MAX_TASK_MEMORY_BLOCK_COUNT * sizeof(void *));
		tasks[ui].mqueue_ids = alloc_memory(MAX_TASK_MQUEUE_COUNT * sizeof(uint32));
		tasks[ui].working_dir = alloc_memory(1024);
		tasks[ui].name = alloc_memory(1024);
		tasks[ui].param = alloc_memory(1024);
		if(	tasks[ui].opened_file_ptrs == NULL
			|| tasks[ui].memory_block_ptrs == NULL
			|| tasks[ui].mqueue_ids == NULL
			|| tasks[ui].working_dir == NULL
			|| tasks[ui].name == NULL
			|| tasks[ui].param == NULL)
		{
			fill_info(info, DC_INIT_TSKMGR, DI_INIT_TSKMGR);
			die(&info);
		}		
	}
	return TRUE;
}

/**
	@Function:		create_task
	@Access:		Public
	@Description:
		创建一个任务。
	@Parameters:
		name, int8 *, IN
			任务名。
		param, int8 *, IN
			运行这个任务所提供的参数。
		app, uint8, IN
			程序。
		app_len, uint32, IN
			程序的长度。
		working_dir, int8 *, IN
			工作目录。
	@Return:
		int32
			如果失败则返回-1, 否则返回任务的 ID。
*/
int32
create_task(IN int8 * name,
			IN int8 * param,
			IN uint8 * app,
			IN uint32 app_len,
			IN int8 * working_dir)
{
	if(app_len > MAX_APP_LEN)
		return -1;
	int32 tid = -1;
	struct Task * task;
	uint32 ui;	
	for(ui = 0; ui < MAX_TASK_COUNT; ui++)
	{
		task = tasks + ui;
		if(!task->used && task->allocable)
		{
			tid = ui;
			break;
		}
	}
	if(tid != -1)
	{
		task->on_exit = NULL;
		task->retvalue = 0;
		task->running = 0;
		tasks->is_system_call = FALSE;
		tasks->fs_lock = FALSE;
		strcpy(task->name, name);
		strcpy(task->param, param);
		strcpy(task->working_dir, working_dir);
		task->app_len = app_len;
		uint32 real_task_len = 3 * 1024 * 1024 + MAX_APP_LEN;
		task->addr = (uint8 *)alloc_memory(real_task_len);
		task->used_memory_size = real_task_len;
		if(task->addr == NULL)
			return -1;
		clear_memory((uint8 *)(task->addr), real_task_len, 0);
		memcpy(task->addr + 3 * 1024 * 1024, app, app_len);

		for(ui = 0; ui < MAX_TASK_OPENED_FILE_COUNT; ui++)
			task->opened_file_ptrs[ui] = NULL;

		for(ui = 0; ui < MAX_TASK_MEMORY_BLOCK_COUNT; ui++)
			task->memory_block_ptrs[ui] = NULL;

		for(ui = 0; ui < MAX_TASK_MQUEUE_COUNT; ui++)
			task->mqueue_ids[ui] = 0;

		//task->addr + 0: int32, Task ID
		//task->addr + 4KB + 0: uint32, System Call Function Number and Sub Function Number
		//task->addr + 4KB + 4: uint32, SParams Structure address
		memcpy(task->addr + 0, (uint8 *)&tid, sizeof(int32));
		*(uint32 *)(task->addr + KB(4) + 0) = 0;
		*(uint32 *)(task->addr + KB(4) + 4) = 0;
		*(uint32 *)(task->addr + KB(4) + 8) = 0;
	
		uint32 code_seg_desc_index = 400 + tid * 5 + 2;
		uint32 data_seg_desc_index = 400 + tid * 5 + 3;
		uint32 data_seg_ring0_desc_index = 400 + tid * 5 + 4;
		
		uint32 base_addr = TASK_BASE_ADDR;
		struct Desc code_seg_desc;
		struct Desc data_seg_desc;
		struct Desc data_seg_ring0_desc;
		
		code_seg_desc.limitl = 0xFFFF;
		code_seg_desc.basel = (uint16)(base_addr & 0xFFFF);
		code_seg_desc.basem = (uint8)((base_addr >> 16) & 0xFF);
		code_seg_desc.baseh = (uint8)((base_addr >> 24) & 0xFF);
		code_seg_desc.attr = ATCE | DPL3 | D32 | G | 0x0F00;
		
		data_seg_desc.limitl = 0xFFFF;
		data_seg_desc.basel = (uint16)(base_addr & 0xFFFF);
		data_seg_desc.basem = (uint8)((base_addr >> 16) & 0xFF);
		data_seg_desc.baseh = (uint8)((base_addr >> 24) & 0xFF);
		data_seg_desc.attr = ATDW | DPL3 | D32 | G | 0x0F00;
		
		data_seg_ring0_desc.limitl = 0xFFFF;
		data_seg_ring0_desc.basel = (uint16)(base_addr & 0xFFFF);
		data_seg_ring0_desc.basem = (uint8)((base_addr >> 16) & 0xFF);
		data_seg_ring0_desc.baseh = (uint8)((base_addr >> 24) & 0xFF);
		data_seg_ring0_desc.attr = ATDW | DPL0 | D32 | G | 0x0F00;

		set_desc_to_gdt(code_seg_desc_index, (uint8 *)&code_seg_desc);
		set_desc_to_gdt(data_seg_desc_index, (uint8 *)&data_seg_desc);
		set_desc_to_gdt(data_seg_ring0_desc_index, (uint8 *)&data_seg_ring0_desc);

		task->tss.back_link = 0;
		task->tss.esp0 = 0x012ffffe;
		task->tss.ss0 = data_seg_ring0_desc_index << 3 | RPL0;
		task->tss.esp1 = 0;
		task->tss.ss1 = 0;
		task->tss.esp2 = 0;
		task->tss.ss2 = 0;
		task->tss.cr3 = 0;
		task->tss.eip = 0x01300000;
		task->tss.flags = 0x200;
		task->tss.eax = 0;
		task->tss.ecx = 0;
		task->tss.edx = 0;
		task->tss.ebx = 0;
		task->tss.esp = 0x011ffffe;
		task->tss.ebp = 0;
		task->tss.esi = 0;
		task->tss.edi = 0;
		task->tss.es = data_seg_desc_index << 3 | RPL3;
		task->tss.cs = code_seg_desc_index << 3 | RPL3;
		task->tss.ss = data_seg_desc_index << 3 | RPL3;
		task->tss.ds = data_seg_desc_index << 3 | RPL3;
		task->tss.fs = data_seg_desc_index << 3 | RPL3;
		task->tss.gs = data_seg_desc_index << 3 | RPL3;
		task->tss.ldt = 0;
		task->tss.trap = 0;
		task->tss.iobase = sizeof(struct TSS);

		task->pagedt_addr = create_empty_user_pagedt(&task->real_pagedt_addr);
		if(task->pagedt_addr == NULL)
		{
			free_memory(task->addr);
			return -1;
		}
		//把程序空间的0x00000000到0x00ffffff映射到物理空间的0x00000000到0x00ffffff
		map_user_pagedt_with_rw((uint32 *)task->real_pagedt_addr, 0x00000000, 0x01000000, 0x00000000, RW_RWE);
		
		//把程序空间的0x01000000到(0x01000000 + task_real_len - 1)映射到物理空间的(task->addr)~(task->addr + task_real_len - 1)
		map_user_pagedt_with_rw((uint32 *)task->real_pagedt_addr, 0x01000000, real_task_len, task->addr, RW_RWE);

		//保护程序空间0x01000000到0x010000000 + 4KB - 1
		map_user_pagedt_with_rw((uint32 *)task->real_pagedt_addr, 0x01000000, KB(4), task->addr, RW_RE);

		task->tss.cr3 = task->real_pagedt_addr;
		
		task->init_i387 = 0;
		task->used = 1;
		task->running = 0;
		task->ran = 0;
		task->ready = FALSE;
	}
	return tid;
}

/**
	@Function:		kill_task
	@Access:		Public
	@Description:
		杀死一个任务。
	@Parameters:
		tid, int32, IN
			任务的 ID。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。
*/
BOOL
kill_task(IN int32 tid)
{
	if(tid < 0 || tid >= MAX_TASK_COUNT || !tasks[tid].used)
		return FALSE;
	free_memory(tasks[tid].addr);
	
	uint32 ui;
	for(ui = 0; ui < MAX_TASK_OPENED_FILE_COUNT; ui++)
		if(tasks[tid].opened_file_ptrs[ui] != NULL)
		{
			fclose(tasks[tid].opened_file_ptrs[ui]);
			tasks[tid].opened_file_ptrs[ui] = NULL;
		}
		
	for(ui = 0; ui < MAX_TASK_MEMORY_BLOCK_COUNT; ui++)
		if(tasks[tid].memory_block_ptrs[ui] != NULL)
		{
			free_memory(tasks[tid].memory_block_ptrs[ui]);
			tasks[tid].memory_block_ptrs[ui] = NULL;
		}

	for(ui = 0; ui < MAX_TASK_MQUEUE_COUNT; ui++)
	{
		uint32 id = tasks[tid].mqueue_ids[ui];
		if(id != 0)
		{
			mqueue_free((MQueuePtr)id);
			tasks[tid].mqueue_ids[ui] = 0;
		}
	}

	free_memory(tasks[tid].pagedt_addr);

	if(tasks[tid].on_exit != NULL)
		tasks[tid].on_exit(tid, tasks[tid].retvalue);

	if(tasks[tid].fs_lock)
		system_call_fs_unlock_fs();

	if(tid == running_tid)
		running_tid = -1;
	tasks[tid].used = 0;
	return TRUE;
}

/**
	@Function:		kill_all_tasks
	@Access:		Public
	@Description:
		杀死所有任务。
	@Parameters:
	@Return:	
*/
void
kill_all_tasks(void)
{
	uint32 ui;
	for(ui = 0; ui < MAX_TASK_COUNT; ui++)
		if(tasks[ui].used)
			kill_task(ui);
}

/**
	@Function:		get_task_info
	@Access:		Public
	@Description:
		获取任务信息。
	@Parameters:
		tid, int32, IN
			任务的 ID。
		task, struct Task *, IN
			指向任务信息结构体的指针。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。
*/
BOOL
get_task_info(	IN int32 tid,
				OUT struct Task * task)
{
	if(tid < 0 || tid >= MAX_TASK_COUNT || !tasks[tid].used)
		return FALSE;
	memcpy(task, tasks + tid, sizeof(struct Task));
	return TRUE;
}

/**
	@Function:		set_task_info
	@Access:		Public
	@Description:
		设置任务信息。
	@Parameters:
		tid, int32, IN
			任务的 ID。
		task, struct Task *, OUT
			指向任务信息结构体的指针。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。		
*/
BOOL
set_task_info(	IN int32 tid,
				IN struct Task * task)
{
	if(tid < 0 || tid >= MAX_TASK_COUNT || !tasks[tid].used)
		return FALSE;
	memcpy(tasks + tid, task, sizeof(struct Task));
	return TRUE;
}

/**
	@Function:		get_task_info_ptr
	@Access:		Public
	@Description:
		获取任务信息结构体的指针。
	@Parameters:
		tid, int32, IN
			任务的 ID。
	@Return:
		struct Task *
			任务信息结构体的指针。	
*/
struct Task *
get_task_info_ptr(IN int32 tid)
{
	if(tid < 0 || tid >= MAX_TASK_COUNT || !tasks[tid].used)
		return NULL;
	return tasks + tid;
}

/**
	@Function:		get_task_info_ptr_unsafe
	@Access:		Public
	@Description:
		获取任务信息结构体的指针。非安全版本。
	@Parameters:
		tid, int32, IN
			任务的 ID。
	@Return:
		struct Task *
			任务信息结构体的指针。	
*/
struct Task *
get_task_info_ptr_unsafe(IN int32 tid)
{
	if(tid < 0 || tid >= MAX_TASK_COUNT)
		return NULL;
	return tasks + tid;
}

/**
	@Function:		task_ready
	@Access:		Public
	@Description:
		使任务就绪。
	@Parameters:
		tid, int32, IN
			任务的 ID。
	@Return:
		BOOL
			返回 TRUE 则成功，否则失败。
*/
BOOL
task_ready(IN int32 tid)
{
	struct Task * task = get_task_info_ptr(tid);
	if(task == NULL)
		return FALSE;
	task->ready = TRUE;
	return TRUE;
}

/**
	@Function:		create_task_by_file
	@Access:		Public
	@Description:
		通过程序文件创建一个任务。
	@Parameters:
		filename, int8 *, IN
			程序文件路径。
		param, int8 *, IN
			运行这个任务所提供的参数。
		working_dir, int8 *, IN
			工作目录。
	@Return:
		int32
			如果失败则返回-1, 否则返回任务的 ID。
*/
int32
create_task_by_file(IN int8 * filename,
					IN int8 * param,
					IN int8 * working_dir)
{
	if(	filename == NULL 
		|| param == NULL 
		|| working_dir == NULL)
		return -1;
	uint8 * app;
	FILE * fptr = fopen(filename, FILE_MODE_READ);
	if(fptr == NULL)
		return -1;
	app = (uint8 *)alloc_memory(flen(fptr));
	if(app == NULL)
	{
		fclose(fptr);
		return -1;
	}
	if(flen(fptr) <= 256 || !fread(fptr, app, flen(fptr)))
	{
		free_memory(app);
		fclose(fptr);
		return -1;
	}

	char file_symbol[6] = {0, 0, 0, 0, 0, 0};
	memcpy(file_symbol, app, 5);
	if(strcmp(file_symbol, "MTA32") != 0)
	{
		free_memory(app);
		fclose(fptr);
		return -1;
	}

	int32 tid = create_task(filename,
							param, app + 256,
							flen(fptr) - 256,
							working_dir);
	free_memory(app);
	fclose(fptr);
	return tid;
}

/**
	@Function:		create_task_by_file_wait
	@Access:		Public
	@Description:
		通过程序文件创建一个任务。并且等待该任务结束，
		可以获取创建的任务的返回值。
	@Parameters:
		filename, int8 *, IN
			程序文件路径。
		param, int8 *, IN
			运行这个任务所提供的参数。
		working_dir, int8 *, IN
			工作目录。
		retvalue, int32 *, OUT
			用于保存被调用的程序的返回值。
	@Return:
		int32
			如果失败则返回 FALSE，否则返回 TRUE。
*/
BOOL
create_task_by_file_wait(	IN int8 * filename,
							IN int8 * param,
							IN int8 * working_dir,
							OUT int32 * retvalue)
{
	if(	filename == NULL
		|| param == NULL
		|| working_dir == NULL
		|| retvalue == NULL)
		return FALSE;
	int32 tid = create_task_by_file(filename, param, working_dir);
	if(tid == -1)
		return FALSE;
	tasks[tid].allocable = FALSE;
	task_ready(tid);
	while(tasks[tid].used)
		asm volatile ("pause");
	*retvalue = tasks[tid].retvalue;
	tasks[tid].allocable = TRUE;
	return TRUE;
}

/**
	@Function:		get_next_task_id
	@Access:		Public
	@Description:
		获取下一个将要执行的任务的 ID。
	@Parameters:
	@Return:
		int32
			下一个将要执行的任务的 ID。	
*/
int32
get_next_task_id(void)
{
	int32 i;
	if(running_tid == -1)
	{
		for(i = 0; i < MAX_TASK_COUNT; i++)
			if(tasks[i].used && tasks[i].ready)
			{
				running_tid = i;
				break;
			}
	}
	else
	{
		int32 new_tid = -1;
		if(tasks[running_tid].ran)
		{
			for(i = running_tid + 1; i < MAX_TASK_COUNT; i++)
				if(tasks[i].used && tasks[i].ready)
				{
					tasks[i].ran = 0;
					tasks[i].running = 1;
					tasks[running_tid].running = 0;
					new_tid = i;
					break;
				}
			if(new_tid == -1)
				for(i = 0; i < running_tid; i++)
					if(tasks[i].used && tasks[i].ready)
					{
						tasks[i].ran = 0;
						tasks[i].running = 1;
						tasks[running_tid].running = 0;
						new_tid = i;
						break;
					}
			running_tid = new_tid;
		}
	}
	return running_tid;
}

/**
	@Function:		set_task_ran_state
	@Access:		Public
	@Description:
		设置指定任务的状态为已运行。
	@Parameters:
		tid, int32, IN
			任务的 ID。
	@Return:	
*/
void
set_task_ran_state(IN int32 tid)
{
	tasks[running_tid].ran = 1;
}

/**
	@Function:		get_running_tid
	@Access:		Public
	@Description:
		获取正在运行的任务的 ID。
	@Parameters:
	@Return:
		int32
			正在运行的任务的 ID。
*/
int32
get_running_tid(void)
{
	return running_tid;
}

/**
	@Function:		get_physical_address
	@Access:		Public
	@Description:
		把线性地址转换为物理地址。
	@Parameters:
		tid, int32, IN
			任务的 ID。
		line_address, void *, IN
			线性地址。
	@Return:
		void *
			物理地址。
*/
void *
get_physical_address(	IN int32 tid, 
						IN void * line_address)
{
	if(tid < 0 || tid >= MAX_TASK_COUNT || tasks[tid].used == 0)
		return NULL;
	struct Task * task = tasks + tid;
	if(!task->used)
		return NULL;
	uint32 * dp_ptr = (uint32 *)task->real_pagedt_addr;
	uint32 paddress = 0, laddress = (uint32)line_address;
	uint32 laddress_dir_index = (laddress >> 22) & 0x3ff;
	uint32 laddress_page_index = (laddress >> 12) & 0x3ff;
	uint32 laddress_low_12bits = laddress & 0xfff;
	paddress = 	(dp_ptr[TABLE_ITEM_COUNT + laddress_dir_index * TABLE_ITEM_COUNT + laddress_page_index] 
				& 0xfffff000) 
				| laddress_low_12bits;
	return (void *)paddress;
}
