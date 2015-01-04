//Filename:		tasks.c
//Author:		Ihsoh
//Date:			2014-7-22
//Descriptor:	Task schedule

#include "tasks.h"
#include "types.h"
#include "386.h"
#include "memory.h"
#include "screen.h"
#include "ifs1fs.h"
#include "paging.h"
#include "die.h"
#include "lock.h"
#include <string.h>

static struct Task tasks[MAX_TASK_COUNT];
static int32 running_tid = -1;

DEFINE_LOCK_IMPL(tasks)

BOOL
init_tasks(void)
{
	struct die_info info;
	uint32 ui;
	for(ui = 0; ui < MAX_TASK_COUNT; ui++)
	{
		tasks[ui].used = 0;
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
		if(tasks[ui].opened_file_ptrs == NULL)
		{
			fill_info(info, DC_INIT_TSKMGR, DI_INIT_TSKMGR);
			die(&info);
		}

		tasks[ui].memory_block_ptrs = alloc_memory(MAX_TASK_MEMORY_BLOCK_COUNT * sizeof(void *));
		if(tasks[ui].memory_block_ptrs == NULL)
		{
			fill_info(info, DC_INIT_TSKMGR, DI_INIT_TSKMGR);
			die(&info);
		}
	}
	return TRUE;
}

int32
create_task(int8 * name, int8 * param, uint8 * app, uint32 app_len)
{
	if(app_len > MAX_APP_LEN)
		return -1;
	int32 tid = -1;
	struct Task * task;
	uint32 ui;	
	for(ui = 0; ui < MAX_TASK_COUNT; ui++)
	{
		task = tasks + ui;
		if(!task->used)
		{
			tid = ui;
			break;
		}
	}
	if(tid != -1)
	{
		task->running = 0;
		strcpy(task->name, name);
		strcpy(task->param, param);
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

		//task->addr + 1 * 1024 * 1024 + 0: int32, Task ID
		//task->addr + 1 * 1024 * 1024 + 4: uint32, System Call Function Number and Sub Function Number
		//task->addr + 1 * 1024 * 1024 + 8: uint32, SParams Structure address
		//task->addr + 1 * 1024 * 1024 + 12: uint32, System Call
		memcpy(task->addr + 1 * 1024 * 1024, (uint8 *)&tid, sizeof(int32));
		*(uint32 *)(task->addr + 1 * 1024 * 1024 + 4) = 0;
		*(uint32 *)(task->addr + 1 * 1024 * 1024 + 8) = 0;
		*(uint32 *)(task->addr + 1 * 1024 * 1024 + 12) = 0;
	
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
		task->tss.esp = 0x010ffffe;
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

		task->tss.cr3 = task->real_pagedt_addr;
		
		task->init_i387 = 0;
		task->used = 1;
		task->running = 0;
		task->ran = 0;
	}
	return tid;
}

BOOL
kill_task(int32 tid)
{
	if(tid < 0 || tid >= MAX_TASK_COUNT || !tasks[tid].used)
		return FALSE;
	free_memory(tasks[tid].addr);
	
	//关闭任务未关闭的文件
	uint32 ui;
	for(ui = 0; ui < MAX_TASK_OPENED_FILE_COUNT; ui++)
		if(tasks[tid].opened_file_ptrs[ui] != NULL)
		{
			fclose(tasks[tid].opened_file_ptrs[ui]);
			tasks[tid].opened_file_ptrs[ui] = NULL;
		}

	//释放任务未释放的内存
	for(ui = 0; ui < MAX_TASK_MEMORY_BLOCK_COUNT; ui++)
		if(tasks[tid].memory_block_ptrs[ui] != NULL)
		{
			free_memory(tasks[tid].memory_block_ptrs[ui]);
			tasks[tid].memory_block_ptrs[ui] = NULL;
		}

	free_memory(tasks[tid].pagedt_addr);

	tasks[tid].used = 0;
	return TRUE;
}

void
kill_all_tasks(void)
{
	uint32 ui;
	for(ui = 0; ui < MAX_TASK_COUNT; ui++)
		if(tasks[ui].used)
			kill_task(ui);
}

BOOL
get_task_info(int32 tid, struct Task * task)
{
	if(tid < 0 || tid >= MAX_TASK_COUNT || !tasks[tid].used)
		return FALSE;
	memcpy(task, tasks + tid, sizeof(struct Task));
	return TRUE;
}

BOOL
set_task_info(int32 tid, struct Task * task)
{
	if(tid < 0 || tid >= MAX_TASK_COUNT || !tasks[tid].used)
		return FALSE;
	memcpy(tasks + tid, task, sizeof(struct Task));
	return TRUE;
}

struct Task *
get_task_info_ptr(int32 tid)
{
	if(tid < 0 || tid >= MAX_TASK_COUNT || !tasks[tid].used)
		return NULL;
	return tasks + tid;
}

int32
create_task_by_file(int8 * filename, int8 * param)
{
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

	//检查程序是否为MTA32程序
	char file_symbol[6] = {0, 0, 0, 0, 0, 0};
	memcpy(file_symbol, app, 5);
	if(strcmp(file_symbol, "MTA32") != 0)
	{
		free_memory(app);
		fclose(fptr);
		return -1;
	}

	int32 tid = create_task(filename, param, app + 256, flen(fptr) - 256);
	free_memory(app);
	fclose(fptr);
	return tid;
}

int32
get_next_task_id(void)
{
	int32 i;
	if(running_tid == -1)
	{
		for(i = 0; i < MAX_TASK_COUNT; i++)
			if(tasks[i].used)
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
				if(tasks[i].used)
				{
					tasks[i].ran = 0;
					tasks[i].running = 1;
					tasks[running_tid].running = 0;
					new_tid = i;
					break;
				}
			if(new_tid == -1)
				for(i = 0; i < running_tid; i++)
					if(tasks[i].used)
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

void
set_task_ran_state(int32 tid)
{
	tasks[running_tid].ran = 1;
}

int32
get_running_tid(void)
{
	return running_tid;
}

void *
get_physical_address(int32 tid, void * line_address)
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
