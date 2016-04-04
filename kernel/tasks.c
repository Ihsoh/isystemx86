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
#include "paging.h"
#include "die.h"
#include "lock.h"
#include "mqueue.h"
#include "kmpool.h"
#include "kernel.h"
#include "gui.h"
#include "atapi.h"
#include "ahci.h"
#include "ata.h"

#include "fs/ifs1/fs.h"

#include "syscall/fs.h"

#include "elf/parser.h"

#include "utils/sfstr.h"

#include <ilib/string.h>

static struct Task 	tasks[MAX_TASK_COUNT];
static int32 		running_tid 			= -1;
static int32 		task_count 				= 0;

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
		uint32 temp = (uint32)&(tasks[ui].tss);
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

		tasks[ui].opened_file_ptrs = alloc_memory(MAX_TASK_OPENED_FILE_COUNT * sizeof(FileObject *));
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
	@Function:		_create_task
	@Access:		Private
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
		task_type, TaskType, IN
			任务类型。
		task_attr, TaskAttr, IN
			任务属性。
			*===============================*===================================================*
			|属性							|意思												|
			*===============================*===================================================*
			*-------------------------------*---------------------------------------------------*
	@Return:
		int32
			如果失败则返回-1, 否则返回任务的 ID。
*/
static
int32
_create_task(	IN int8 * name,
				IN int8 * param,
				IN uint8 * app,
				IN uint32 app_len,
				IN int8 * working_dir,
				IN TaskType task_type,
				IN TaskAttr task_attr)
{
	if(task_type != TASK_TYPE_USER && task_type != TASK_TYPE_SYSTEM)
		return -1;
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
		struct Desc tss_desc;
		get_desc_from_gdt(400 + tid * 5 + 0, (uint8 *)&tss_desc);
		tss_desc.attr = AT386TSS + DPL3;
		set_desc_to_gdt(400 + tid * 5 + 0, (uint8 *)&tss_desc);

		// 重置与这个任务对应的系统调用。
		if(!reset_syscall(tid))
			return -1;

		task->used = 1;
		task->on_exit = NULL;
		task->retvalue = 0;
		task->running = 0;
		task->is_system_call = FALSE;
		task->fs_lock = FALSE;
		task->stdin = NULL;
		task->stdout = NULL;
		task->stderr = NULL;
		task->read_count = 0;
		task->type = task_type;
		task->priority = TASK_TICK_NORMAL;
		task->tick = TASK_TICK_NORMAL;
		task->elf = NULL;
		for(ui = 0; ui < MAX_TASK_ELF_SO_COUNT; ui++)
			task->elf_so_ctx[ui] = NULL;

		strcpy_safe(task->name, 1024, name);
		strcpy_safe(task->param, 1024, param);
		strcpy_safe(task->working_dir, 1024, working_dir);
		task->app_len = app_len;
		uint32 real_task_len = 3 * 1024 * 1024 + app_len;
		task->addr = (uint8 *)alloc_memory(real_task_len);
		task->used_memory_size = real_task_len;
		if(task->addr == NULL)
			return -1;
		clear_memory((uint8 *)(task->addr), 0, real_task_len);
		memcpy(task->addr + 3 * 1024 * 1024, app, app_len);

		for(ui = 0; ui < MAX_TASK_OPENED_FILE_COUNT; ui++)
			task->opened_file_ptrs[ui] = NULL;

		for(ui = 0; ui < MAX_TASK_MEMORY_BLOCK_COUNT; ui++)
			task->memory_block_ptrs[ui] = NULL;

		for(ui = 0; ui < MAX_TASK_MQUEUE_COUNT; ui++)
			task->mqueue_ids[ui] = 0;

		//task->addr + 0: int32, Task ID
		memcpy(task->addr + 0, (uint8 *)&tid, sizeof(int32));

		//task->addr + 4: uint32, Object address
		//该值域保存了一个32位的无符号整数，该无符号整数为一个地址。
		//该地址指向一个由内核扩展文件(*.sys)提供的对象。
		//如果该值为0，则代表内核扩展文件未初始化完毕。
		*(uint32 *)(task->addr + 4) = 0;
	
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
		if(task_type == TASK_TYPE_USER)
			code_seg_desc.attr 						// 如果任务类型是用户任务，
				= ATCE | DPL3 | D32 | G | 0x0F00;	// 则DPL为3表示运行在Ring3。
		else if(task_type == TASK_TYPE_SYSTEM)
			code_seg_desc.attr 						// 如果任务类型是系统任务，
				= ATCE | DPL0 | D32 | G | 0x0F00;	// 则DPL为0表示运行在Ring0。
		
		data_seg_desc.limitl = 0xFFFF;
		data_seg_desc.basel = (uint16)(base_addr & 0xFFFF);
		data_seg_desc.basem = (uint8)((base_addr >> 16) & 0xFF);
		data_seg_desc.baseh = (uint8)((base_addr >> 24) & 0xFF);
		if(task_type == TASK_TYPE_USER)
			data_seg_desc.attr						// 如果任务类型是用户任务，
				= ATDW | DPL3 | D32 | G | 0x0F00;	// 则DPL为3表示读写数据需要Ring3。
		else if(task_type == TASK_TYPE_SYSTEM)
			data_seg_desc.attr						// 如果任务类型是用户任务，
				= ATDW | DPL0 | D32 | G | 0x0F00;	// 则DPL为0表示读写数据需要Ring0。
		
		data_seg_ring0_desc.limitl = 0xFFFF;
		data_seg_ring0_desc.basel = (uint16)(base_addr & 0xFFFF);
		data_seg_ring0_desc.basem = (uint8)((base_addr >> 16) & 0xFF);
		data_seg_ring0_desc.baseh = (uint8)((base_addr >> 24) & 0xFF);
		data_seg_ring0_desc.attr = ATDW | DPL0 | D32 | G | 0x0F00;

		set_desc_to_gdt(code_seg_desc_index, (uint8 *)&code_seg_desc);
		set_desc_to_gdt(data_seg_desc_index, (uint8 *)&data_seg_desc);
		set_desc_to_gdt(data_seg_ring0_desc_index, (uint8 *)&data_seg_ring0_desc);

		task->tss.back_link = 0;
		task->tss.esp0 = 0x012ffff0;
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
		task->tss.esp = 0x011ffff0;
		task->tss.ebp = 0;
		task->tss.esi = 0;
		task->tss.edi = 0;
		if(task_type == TASK_TYPE_USER)
		{
			task->tss.es = (data_seg_desc_index << 3) | RPL3;
			task->tss.cs = (code_seg_desc_index << 3) | RPL3;
			task->tss.ss = (data_seg_desc_index << 3) | RPL3;
			task->tss.ds = (data_seg_desc_index << 3) | RPL3;
			task->tss.fs = (data_seg_desc_index << 3) | RPL3;
			task->tss.gs = (data_seg_desc_index << 3) | RPL3;
		}
		else if(task_type == TASK_TYPE_SYSTEM)
		{
			task->tss.es = (data_seg_desc_index << 3) | RPL0;
			task->tss.cs = (code_seg_desc_index << 3) | RPL0;
			task->tss.ss = (data_seg_desc_index << 3) | RPL0;
			task->tss.ds = (data_seg_desc_index << 3) | RPL0;
			task->tss.fs = (data_seg_desc_index << 3) | RPL0;
			task->tss.gs = (data_seg_desc_index << 3) | RPL0;
		}

		task->tss.ldt = 0;
		task->tss.trap = 0;
		task->tss.iobase = sizeof(struct TSS);

		task->pagedt_addr = create_empty_user_pagedt(&task->real_pagedt_addr);
		if(task->pagedt_addr == NULL)
		{
			free_memory(task->addr);
			return -1;
		}
		// 把程序空间的0x00000000到0x00ffffff映射到物理空间的0x00000000到0x00ffffff。
		// 权限为读和执行。0x00000000到0x00ffffff为内核空间。
		// 在执行系统调用时会执行包含对内核空间写操作的代码，但是由于这个操作是在0xa0中断
		// 程序中进行，并且执行0xa0中断程序时CPL为Ring0，所以写操作为合法。
		map_user_pagedt_with_rw((uint32 *)task->real_pagedt_addr, 
								0x00000000,
								0x01000000,
								0x00000000, 
								RW_RE);
		
		// 把程序空间的0x01000000到(0x01000000 + task_real_len - 1)映射到
		// 物理空间的(task->addr)~(task->addr + task_real_len - 1)。
		map_user_pagedt_with_rw((uint32 *)task->real_pagedt_addr,
								0x01000000,
								real_task_len,
								task->addr,
								RW_RWE);

		//保护程序空间0x01000000到0x010000000 + 4KB - 1
		map_user_pagedt_with_rw((uint32 *)task->real_pagedt_addr,
								0x01000000,
								KB(4),
								task->addr,
								RW_RE);

		task->tss.cr3 = task->real_pagedt_addr;
		
		task->init_i387 = 0;
		task->ran = 0;
		task->ready = FALSE;
		task_count++;
	}
	return tid;
}

/**
	@Function:		create_task
	@Access:		Public
	@Description:
		创建一个用户任务。
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
	lock();
	int32 tid = _create_task(name, 
							param, 
							app, 
							app_len, 
							working_dir, 
							TASK_TYPE_USER,
							TASK_ATTR_NONE);
	unlock();
	return tid;
}

/**
	@Function:		create_sys_task
	@Access:		Public
	@Description:
		创建一个系统任务。
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
create_sys_task(IN int8 * name,
				IN int8 * param,
				IN uint8 * app,
				IN uint32 app_len,
				IN int8 * working_dir)
{
	lock();
	int32 tid = _create_task(name, 
							param, 
							app, 
							app_len, 
							working_dir, 
							TASK_TYPE_SYSTEM,
							TASK_ATTR_NONE);
	unlock();
	return tid;
}

/**
	@Function:		_kill_task
	@Access:		Public
	@Description:
		可以杀死一个用户任务或系统任务。
	@Parameters:
		tid, int32, IN
			任务的 ID。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。
*/
static
BOOL
_kill_task(IN int32 tid)
{
	if(tid < 0 || tid >= MAX_TASK_COUNT || !tasks[tid].used)
		return FALSE;

	// 确认将要被杀死的任务是否正在进行ATA磁盘的读取操作，
	// 如果是的话则等待其完成操作。
	int32 ll = LOCK_LEVEL;
	int32 retry = 0x0fffffff;
	if(ll)
		UNLOCK_TASK();
	while(	
			(
				// ATA。
				(ata_locked() && ata_lock_tid() == tid)
				// ATAPI。
				|| (atapi_locked() && atapi_lock_tid() == tid)
				// AHCI。
				|| (ahci_locked() && ahci_lock_tid() == tid)
			)
			&& --retry != 0);
	if(ll)
		LOCK_TASK();
	// 再次确认。
	if(	// ATA。
		(ata_locked() && ata_lock_tid() == tid)
		// ATAPI。
		|| (atapi_locked() && atapi_lock_tid() == tid)
		// AHCI。
		|| (ahci_locked() && ahci_lock_tid() == tid))
		return FALSE;

	struct Task * task = tasks + tid;

	if(task->addr != NULL)
		free_memory(task->addr);
	
	uint32 ui;
	for(ui = 0; ui < MAX_TASK_OPENED_FILE_COUNT; ui++)
		if(task->opened_file_ptrs[ui] != NULL)
		{
			close_file(task->opened_file_ptrs[ui]);
			task->opened_file_ptrs[ui] = NULL;
		}
		
	for(ui = 0; ui < MAX_TASK_MEMORY_BLOCK_COUNT; ui++)
		if(task->memory_block_ptrs[ui] != NULL)
		{
			free_memory(task->memory_block_ptrs[ui]);
			task->memory_block_ptrs[ui] = NULL;
		}

	for(ui = 0; ui < MAX_TASK_MQUEUE_COUNT; ui++)
	{
		uint32 id = task->mqueue_ids[ui];
		if(id != 0)
		{
			mqueue_free((MQueuePtr)id);
			task->mqueue_ids[ui] = 0;
		}
	}

	if(task->pagedt_addr != NULL)
		free_memory(task->pagedt_addr);

	if(task->on_exit != NULL)
		task->on_exit(tid, task->retvalue);

	if(task->fs_lock)
		system_call_fs_unlock_fs();

	if(task->stdin != NULL)
		close_file(task->stdin);
	if(task->stdout != NULL)
		close_file(task->stdout);
	if(task->stderr != NULL)
		close_file(task->stderr);

	if(task->elf != NULL)
		free_memory(task->elf);

	// 释放ELF SO槽中的所有ELF上下文。
	for(ui = 0; ui < MAX_TASK_ELF_SO_COUNT; ui++)
		if(task->elf_so_ctx[ui] != NULL)
		{
			elf_free(task->elf_so_ctx[ui]);
			DELETE(task->elf_so_ctx[ui]);
			task->elf_so_ctx[ui] = NULL;
		}

	// 释放与这个任务对应的系统调用。
	free_syscall(tid);

	// 尝试释放ATA驱动的锁。
	ata_attempt_to_unlock(tid);

	// 尝试释放ATAPI驱动的锁。
	atapi_attempt_to_unlock(tid);

	gui_close_windows(tid);

	if(tid == running_tid)
		running_tid = -1;

	task->used = 0;

	task_count--;

	return TRUE;
}

/**
	@Function:		kill_task
	@Access:		Public
	@Description:
		可以杀死一个用户任务，但不能杀死系统任务。
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
	if(	tid < 0
		|| tid >= MAX_TASK_COUNT
		|| !tasks[tid].used)
		return FALSE;
	struct Task * task = tasks + tid;
	if(task->type != TASK_TYPE_USER)
		return FALSE;
	lock();
	BOOL r = _kill_task(tid);
	unlock();
	return r;
}

/**
	@Function:		kill_sys_task
	@Access:		Public
	@Description:
		可以杀死任何类型的任务。
	@Parameters:
		tid, int32, IN
			任务的 ID。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。
*/
BOOL
kill_sys_task(IN int32 tid)
{
	if(	tid < 0
		|| tid >= MAX_TASK_COUNT
		|| !tasks[tid].used)
		return FALSE;
	lock();
	BOOL r = _kill_task(tid);
	unlock();
	return r;
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
			kill_sys_task(ui);
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
	if(	tid < 0
		|| tid >= MAX_TASK_COUNT
		|| !tasks[tid].used)
		return FALSE;
	lock();
	memcpy(task, tasks + tid, sizeof(struct Task));
	unlock();
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
	if(	tid < 0
		|| tid >= MAX_TASK_COUNT
		|| !tasks[tid].used)
		return FALSE;
	lock();
	memcpy(tasks + tid, task, sizeof(struct Task));
	unlock();
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
	if(	tid < 0
		|| tid >= MAX_TASK_COUNT
		|| !tasks[tid].used)
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
	if(	tid < 0
		|| tid >= MAX_TASK_COUNT)
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
	@Function:		_create_task_by_file
	@Access:		Private
	@Description:
		通过程序文件创建一个任务。
	@Parameters:
		filename, int8 *, IN
			程序文件路径。
		param, int8 *, IN
			运行这个任务所提供的参数。
		working_dir, int8 *, IN
			工作目录。
		task_type, TaskType, IN
			任务类型。
		task_attr, TaskAttr, IN
			任务属性。
	@Return:
		int32
			如果失败则返回-1, 否则返回任务的 ID。
*/
static
int32
_create_task_by_file(	IN int8 * filename,
						IN int8 * param,
						IN int8 * working_dir,
						IN TaskType task_type,
						IN TaskAttr task_attr)
{
	if(	filename == NULL 
		|| param == NULL 
		|| working_dir == NULL)
		return -1;
	uint8 * app;
	FileObject * fptr = open_file(filename, FILE_MODE_READ);
	if(fptr == NULL)
		return -1;
	app = (uint8 *)alloc_memory(flen(fptr));
	if(app == NULL)
	{
		close_file(fptr);
		return -1;
	}
	if(flen(fptr) <= 256 || !read_file(fptr, app, flen(fptr)))
	{
		free_memory(app);
		close_file(fptr);
		return -1;
	}

	char file_symbol[6] = {0, 0, 0, 0, 0, 0};
	memcpy(file_symbol, app, 5);
	if(task_type == TASK_TYPE_USER)
	{
		if(strcmp(file_symbol, "MTA32") != 0)
		{
			free_memory(app);
			close_file(fptr);
			return -1;
		}
	}
	else if(task_type == TASK_TYPE_SYSTEM)
	{
		if(strcmp(file_symbol, "SYS32") != 0)
		{
			free_memory(app);
			close_file(fptr);
			return -1;
		}
	}

	int32 tid = _create_task(	filename,
								param,
								app + 256,
								flen(fptr) - 256,
								working_dir,
								task_type,
								task_attr);
	free_memory(app);
	close_file(fptr);
	return tid;
}

/**
	@Function:		create_task_by_file
	@Access:		Private
	@Description:
		通过程序文件创建一个用户任务。
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
create_task_by_file(	IN int8 * filename,
						IN int8 * param,
						IN int8 * working_dir)
{
	lock();
	int32 tid = _create_task_by_file(	filename,
										param,
										working_dir,
										TASK_TYPE_USER,
										TASK_ATTR_NONE);
	unlock();
	return tid;
}

/**
	@Function:		create_sys_task_by_file
	@Access:		Private
	@Description:
		通过程序文件创建一个系统任务。
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
create_sys_task_by_file(IN int8 * filename,
						IN int8 * param,
						IN int8 * working_dir)
{
	lock();
	int32 tid = _create_task_by_file(	filename,
										param,
										working_dir,
										TASK_TYPE_SYSTEM,
										TASK_ATTR_NONE);
	unlock();
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
	lock();
	int32 tid = create_task_by_file(filename, param, working_dir);
	if(tid == -1)
	{
		unlock();
		return FALSE;
	}
	tasks[tid].allocable = FALSE;
	task_ready(tid);
	unlock();
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
	if(running_tid != -1)
	{
		struct Task * running_task = tasks + running_tid;
		if(--running_task->tick > 0)
			return running_tid;
		else
			switch(running_task->priority)
			{
				case TASK_PRIORITY_HIGH:
					running_task->tick = TASK_TICK_HIGH;
					break;
				case TASK_PRIORITY_NORMAL:
					running_task->tick = TASK_TICK_NORMAL;
					break;
				case TASK_PRIORITY_LOW:
					running_task->tick = TASK_TICK_LOW;
					break;
			}
	}
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

/**
	@Function:		_tasks_redirect_io
	@Access:		Private
	@Description:
		重定向任务的IO。
	@Parameters:
		v, FileObject **, IN OUT
			指向struct Task结构体中的stdin或stdout或stderr的指针。
		path, int8 *, IN
			重定向到文件的文件路径。
		mode, int32, IN
			打开文件的模式。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。
*/
static
BOOL
_tasks_redirect_io(	IN OUT FileObject ** v,
					IN int8 * path,
					IN int32 mode)
{
	if(v == NULL || path == NULL)
		return FALSE;
	FileObject * new = open_file(path, mode);
	if(new == NULL)
		return FALSE;
	if(*v != NULL)
		close_file(*v);
	*v = new;
	return TRUE;
}

/**
	@Function:		tasks_redirect_stdin
	@Access:		Public
	@Description:
		重定向任务的stdin。
	@Parameters:
		tid, int32, IN
			任务的TID。
		path, int8 *, IN
			重定向到文件的文件路径。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。
*/
BOOL
tasks_redirect_stdin(	IN int32 tid,
						IN int8 * path)
{
	struct Task * task = get_task_info_ptr(tid);
	if(task == NULL)
		return FALSE;
	return _tasks_redirect_io(	&(task->stdin), 
								path,
								FILE_MODE_READ);
}

/**
	@Function:		tasks_redirect_stdout
	@Access:		Public
	@Description:
		重定向任务的stdout。
	@Parameters:
		tid, int32, IN
			任务的TID。
		path, int8 *, IN
			重定向到文件的文件路径。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。
*/
BOOL
tasks_redirect_stdout(	IN int32 tid,
						IN int8 * path)
{
	struct Task * task = get_task_info_ptr(tid);
	if(task == NULL)
		return FALSE;
	return _tasks_redirect_io(	&(task->stdout), 
								path,
								FILE_MODE_APPEND);
}

/**
	@Function:		tasks_redirect_stderr
	@Access:		Public
	@Description:
		重定向任务的stderr。
	@Parameters:
		tid, int32, IN
			任务的TID。
		path, int8 *, IN
			重定向到文件的文件路径。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。
*/
BOOL
tasks_redirect_stderr(	IN int32 tid,
						IN int8 * path)
{
	struct Task * task = get_task_info_ptr(tid);
	if(task == NULL)
		return FALSE;
	return _tasks_redirect_io(	&(task->stderr), 
								path,
								FILE_MODE_APPEND);
}

/**
	@Function:		tasks_get_count
	@Access:		Public
	@Description:
		获取当前任务数量。
	@Parameters:
	@Return:
		int32
			当前任务数量。
*/
int32
tasks_get_count(void)
{
	return task_count;
}

/**
	@Function:		tasks_alloc_memory
	@Access:		Public
	@Description:
		分配一段指定任务可以访问的内存。
	@Parameters:
		tid, int32, IN
			任务ID。
		size, uint32, IN
			请求的内存的大小。
		phyaddr, void **, OUT
			指向用于储存分配的内存的物理地址的缓冲区的指针。
	@Return:
		void *
			任务可以访问的内存的指针。
*/
void *
tasks_alloc_memory(	IN int32 tid,
					IN uint32 size,
					OUT void ** phyaddr)
{
	struct Task * task = get_task_info_ptr(tid);
	if(task == NULL || phyaddr == NULL)
		return NULL;
	uint32 start;
	uint32 ui;
	for(ui = 0;
		ui < MAX_TASK_MEMORY_BLOCK_COUNT && task->memory_block_ptrs[ui] != NULL;
		ui++);
	if(	ui < MAX_TASK_MEMORY_BLOCK_COUNT
		&& find_free_pages((uint32 *)task->real_pagedt_addr, size, &start))
	{
		void * ptr = alloc_memory(size);
		if(ptr != NULL)
		{
			*phyaddr = ptr;
			map_user_pagedt_with_rw((uint32 *)task->real_pagedt_addr,
									start,
									size,
									(uint32)ptr,
									RW_RWE);
			task->memory_block_ptrs[ui] = ptr;
			task->used_memory_size += align_4kb(size);
			return (void *)start;
		}
		else
			return NULL;
	}
	else
		return NULL;
}

/**
	@Function:		tasks_free_memory
	@Access:		Public
	@Description:
		释放任务申请的内存。
	@Parameters:
		tid, int32, IN
			任务ID。
		ptr, void *, IN
			用户空间的线性地址。
	@Return:
*/
void
tasks_free_memory(	IN int32 tid,
					IN void * ptr)
{
	struct Task * task = get_task_info_ptr(tid);
	if(task == NULL)
		return;
	void * phyptr = get_physical_address(tid, ptr);
	if(phyptr != NULL)
	{
		free_pages(	(uint32 *)task->real_pagedt_addr,
					(uint32)ptr,
					get_memory_block_size(phyptr));
		uint32 ui;
		for(ui = 0; ui < MAX_TASK_MEMORY_BLOCK_COUNT; ui++)
			if(task->memory_block_ptrs[ui] == phyptr)
				task->memory_block_ptrs[ui] = NULL;
		DELETE(phyptr);
	}
}

/**
	@Function:		tasks_load_elf
	@Access:		Public
	@Description:
		加载ELF进任务的内存空间。
	@Parameters:
		tid, int32, IN
			任务ID。
		path, CASCTEXT, IN
			ELF文件路径。
	@Return:
		uint32
			ELF入口函数地址（相对于任务的内存空间）。
*/
uint32
tasks_load_elf(	IN int32 tid,
				IN CASCTEXT path)
{
	BOOL ctx_inited = FALSE;
	if(path == NULL)
		goto err;
	struct Task * task = get_task_info_ptr(tid);
	if(task == NULL)
		goto err;
	if(task->elf != NULL)
		return 0;	// 如果已经载入了ELF程序，则不执行载入逻辑。
	ELFContext ctx;
	if(!elf_parse(path, &ctx))
		goto err;
	ctx_inited = TRUE;
	uint32 vbase = 0xffffffff;
	uint32 max_vaddr = 0x00000000;	// 储存PHDR表中最大的虚拟地址。
	uint32 max_msize = 0;			// 拥有最大的虚拟地址的PHDR的内存大小。
	Elf32_Phdr * phdr = NULL;
	elf_reset_phdr(&ctx);
	while((phdr = elf_next_phdr(&ctx)) != NULL)
	{
		if(	phdr->p_type == PT_LOAD
			&& phdr->p_vaddr < vbase)
			vbase = phdr->p_vaddr;
		if(	phdr->p_type == PT_LOAD
			&& phdr->p_vaddr > max_vaddr)
		{
			max_vaddr = phdr->p_vaddr;
			max_msize = phdr->p_memsz;
		}
	}
	uint32 msize = max_vaddr - vbase + max_msize;
	task->elf = alloc_memory(msize);
	memset(task->elf, 0, msize);
	if(task->elf == NULL)
		goto err;
	if(!map_user_pagedt_with_rw((uint32 *)task->real_pagedt_addr,
								vbase,
								msize,
								(uint32)task->elf,
								RW_RWE))
		goto err;
	elf_reset_phdr(&ctx);
	while((phdr = elf_next_phdr(&ctx)) != NULL)
		if(phdr->p_type == PT_LOAD)
			memcpy(	task->elf + (phdr->p_vaddr - vbase),
					ctx.file_content + phdr->p_offset,
					phdr->p_filesz);
	elf_free(&ctx);
	return ctx.header->e_entry;
err:
	if(task->elf != NULL)
	{
		free_memory(task->elf);
		task->elf = NULL;
	}
	if(ctx_inited)
		elf_free(&ctx);
	return 0;
}

/**
	@Function:		tasks_load_elf_so
	@Access:		Public
	@Description:
		把ELF Shared Object文件加载进任务内存空间。
	@Parameters:
		tid, int32, IN
			任务ID。
		path, CASCTEXT, IN
			ELF Shared Object文件路径。
	@Return:
		uint32
			ELF Shared Object上下文索引。
			通过该索引可以获取ELF Shared Object的符号在任务内存空间的地址。
			当加载失败时返回0xffffffff。
*/
uint32
tasks_load_elf_so(	IN int32 tid,
					IN CASCTEXT path)
{
	uint8 * elf_knl = NULL;
	uint8 * elf_usr = NULL;
	uint32 ctx_idx = 0xffffffff;
	ELFContextPtr ctx = NULL;
	if(path == NULL)
		goto err;

	// 获取任务信息。
	struct Task * task = get_task_info_ptr(tid);
	if(task == NULL)
		goto err;

	// 获取任务可用的ELF SO槽。
	for(ctx_idx = 0; ctx_idx < MAX_TASK_ELF_SO_COUNT; ctx_idx++)
		if(task->elf_so_ctx[ctx_idx] == NULL)
			break;
	if(ctx_idx >= MAX_TASK_ELF_SO_COUNT)
		goto err;
	
	// 新建ELF上下文，并且添加到任务的ELF SO槽中。
	ctx = NEW(ELFContext);
	if(ctx == NULL)
		goto err;
	task->elf_so_ctx[ctx_idx] = ctx;
	if(!elf_parse(path, ctx))
		goto err;
	
	// 把Program Header Table内的LOAD类型Program Header加载进内存。
	uint32 vbase = 0xffffffff;
	uint32 max_vaddr = 0x00000000;	// 储存PHDR表中最大的虚拟地址。
	uint32 max_msize = 0;			// 拥有最大的虚拟地址的PHDR的内存大小。
	Elf32_Phdr * phdr = NULL;
	elf_reset_phdr(ctx);
	while((phdr = elf_next_phdr(ctx)) != NULL)
	{
		if(	phdr->p_type == PT_LOAD
			&& phdr->p_vaddr < vbase)
			vbase = phdr->p_vaddr;
		if(	phdr->p_type == PT_LOAD
			&& phdr->p_vaddr > max_vaddr)
		{
			max_vaddr = phdr->p_vaddr;
			max_msize = phdr->p_memsz;
		}
	}
	uint32 msize = max_vaddr - vbase + max_msize;
	elf_usr = tasks_alloc_memory(tid, msize, &elf_knl);
	if(elf_knl == NULL)
		goto err;
	memset(elf_knl, 0, msize);
	elf_reset_phdr(ctx);
	while((phdr = elf_next_phdr(ctx)) != NULL)
		if(phdr->p_type == PT_LOAD)
			memcpy(	elf_knl + (phdr->p_vaddr - vbase),
					ctx->file_content + phdr->p_offset,
					phdr->p_filesz);
	ctx->elf_knl = elf_knl;
	ctx->elf_usr = elf_usr;

	// 修正动态重定向符号的地址（.rel.dyn）。
	if(ctx->rel_dyn != NULL)
	{
		elf_reset_rel_dyn(ctx);
		Elf32_Rel * rel = NULL;
		while((rel = elf_next_rel_dyn(ctx)) != NULL)
		{
			uint32 sym_idx = ELF32_R_SYM(rel->r_info);
			Elf32_Sym * sym = (Elf32_Sym *)(ctx->file_content + ctx->dynsym->sh_offset) + sym_idx;
			uint32 * off = (uint32 *)(ctx->elf_knl + rel->r_offset);
			*off = (uint32)ctx->elf_usr + sym->st_value;
		}
	}

	// 修正动态重定向符号的地址（.rel.plt）。
	if(ctx->rel_plt != NULL)
	{
		elf_reset_rel_plt(ctx);
		Elf32_Rel * rel = NULL;
		while((rel = elf_next_rel_plt(ctx)) != NULL)
		{
			uint32 sym_idx = ELF32_R_SYM(rel->r_info);
			Elf32_Sym * sym = (Elf32_Sym *)(ctx->file_content + ctx->dynsym->sh_offset) + sym_idx;
			uint32 * off = (uint32 *)(ctx->elf_knl + rel->r_offset);
			*off = (uint32)ctx->elf_usr + sym->st_value;
		}
	}

	return ctx_idx;
err:
	if(elf_usr != NULL)
		tasks_free_memory(tid, elf_usr);
	if(ctx != NULL)
	{
		if(ctx_idx < MAX_TASK_ELF_SO_COUNT)
			task->elf_so_ctx[ctx_idx] = NULL;
		elf_free(ctx);
		DELETE(ctx);
	}
	return 0xffffffff;
}

/**
	@Function:		tasks_get_elf_so_symbol
	@Access:		Public
	@Description:
		获取任务加载的ELF Shared Object的符号的地址。
		该地址相对于用户内存空间。
	@Parameters:
		tid, int32, IN
			任务ID。
		ctx_idx, uint32, IN
			ELF Shared Object上下文索引。
		name, CASCTEXT, IN
			符号名。
	@Return:
		void *
			ELF Shared Object的符号的地址。
			该地址相对于用户内存空间。
*/
void *
tasks_get_elf_so_symbol(IN int32 tid,
						IN uint32 ctx_idx,
						IN CASCTEXT name)
{
	ELFContextPtr ctx = NULL;
	if(name == NULL || ctx_idx >= MAX_TASK_ELF_SO_COUNT)
		goto err;

	// 获取任务信息和ELF上下文。
	struct Task * task = get_task_info_ptr(tid);
	if(task == NULL || task->elf_so_ctx[ctx_idx] == NULL)
		goto err;
	ctx = task->elf_so_ctx[ctx_idx];

	uint32 symaddr = 0;
	elf_reset_dynsym(ctx);
	Elf32_Sym * sym = NULL;
	while((sym = elf_next_dynsym_sym(ctx)) != NULL)
		if(strcmp_safe(elf_parse_dynsym_sym_name(ctx, sym), name) == SFSTR_R_EQUAL)
			symaddr = sym->st_value;
	elf_reset_dynsym(ctx);
	if(symaddr == 0)
		goto err;
	return (void *)(ctx->elf_usr + symaddr);
err:
	return NULL;
}

/**
	@Function:		tasks_unload_elf_so
	@Access:		Public
	@Description:
		释放任务加载的ELF Shared Object上下文。
	@Parameters:
		tid, int32, IN
			任务ID。
		ctx_idx, uint32, IN
			ELF Shared Object上下文索引。
		name, CASCTEXT, IN
			符号名。
	@Return:
		void *
			ELF Shared Object的符号的地址。
			该地址相对于用户内存空间。
*/
BOOL
tasks_unload_elf_so(IN int32 tid,
					IN uint32 ctx_idx)
{
	ELFContextPtr ctx = NULL;
	if(ctx_idx >= MAX_TASK_ELF_SO_COUNT)
		goto err;

	// 获取任务信息和ELF SO上下文。
	struct Task * task = get_task_info_ptr(tid);
	if(task == NULL || task->elf_so_ctx[ctx_idx] == NULL)
		goto err;
	ctx = task->elf_so_ctx[ctx_idx];

	// 释放ELF SO上下文。
	task->elf_so_ctx[ctx_idx] = NULL;
	tasks_free_memory(tid, ctx->elf_usr);
	ctx->elf_usr = NULL;
	ctx->elf_knl = NULL;
	elf_free(ctx);
	DELETE(ctx);
	
	return TRUE;
err:
	return FALSE;
}
