/**
	@File:			tasks.h
	@Author:		Ihsoh
	@Date:			2014-7-22
	@Description:
*/

#ifndef	_TASKS_H_
#define	_TASKS_H_

#include "types.h"
#include "386.h"
#include "ifs1fs.h"
#include "lock.h"

#define	MAX_TASK_COUNT				64
#define	MAX_APP_LEN					(16 * 1024 * 1024)
#define	MAX_TASK_OPENED_FILE_COUNT	1024
#define	MAX_TASK_MEMORY_BLOCK_COUNT	(10 * 1024)
#define MAX_TASK_MQUEUE_COUNT		16
#define	TASK_BASE_ADDR				0x00000000

typedef enum
{
	TASK_TYPE_USER		= 1,
	TASK_TYPE_SYSTEM	= 2
} TaskType;

typedef void (* OnTaskExit)(int32 tid, int32 retvalue);

//结构名:	Task
//功能:		任务
struct Task
{
	int32				used;				//是否使用该任务槽。
	BOOL				allocable;			//该任务槽是否可分配。
	int32				running;			//是否正在运行。
	int32				ran;				//是否运行。
	int8 *				name;				//任务名。
	int8 *				param;				//参数。
	uint32				app_len;			//程序的长度。
	uint8 *				addr;				//任务的起始地址。
	struct TSS 			tss;				//任务的TSS。
	struct I387State	i387_state;			//Intel 80387状态。
	int32				init_i387;			//该任务是否初始化了Intel 80387。
	uint32 * 			pagedt_addr;		//页目录表和页表的地址。
	uint32				real_pagedt_addr;	//真正页目录表和页表的地址。
	FILE **				opened_file_ptrs;	//打开文件指针列表。
	void **				memory_block_ptrs;	//内存块指针列表。
	uint32 *			mqueue_ids;			//任务打开的消息队列的ID集合。
	uint32				used_memory_size;	//任务内存使用量。
	int8 *				working_dir;		//作业目录。
	int32				retvalue;			//任务的返回值。
	BOOL				ready;				//是否准备就绪，任务创建时为 False，
											//直到为 True 时，才会开始运行。
	BOOL				is_system_call;		//是否在进行系统调用。
	BOOL				fs_lock;			//是否获得了文件系统锁。
	FILE * 				stdin;				//任务的标准输入。如果为NULL则为键盘，否则为文件。
	FILE *				stdout;				//任务的标准输出。如果为NULL则为屏幕，否则为文件。
	FILE *				stderr;				//任务的标准错误。如果为NULL则为屏幕，否则为文件。
	uint32				read_count;			//已从标准输入读取了的字符数。
	TaskType 			type;				//任务的类型。
											//如果为TASK_TYPE_USER，则表示该任务为用户任务，运行于Ring3。
											//如果为TASK_TYPE_SYSTEM，则表示该任务为系统任务，运行于Ring0。
	OnTaskExit			on_exit;			//当任务退出时调用。
};


extern
BOOL
init_tasks(void);

extern
int32
create_task(IN int8 * name,
			IN int8 * param,
			IN uint8 * app,
			IN uint32 app_len,
			IN int8 * working_dir);

extern
int32
create_sys_task(IN int8 * name,
				IN int8 * param,
				IN uint8 * app,
				IN uint32 app_len,
				IN int8 * working_dir);

extern
BOOL
kill_task(IN int32 tid);

extern
void
kill_all_tasks(void);

extern
BOOL
get_task_info(	IN int32 tid,
				OUT struct Task * task);

extern
BOOL
set_task_info(	IN int32 tid,
				IN struct Task * task);

extern
struct Task *
get_task_info_ptr(IN int32 tid);

extern
struct Task *
get_task_info_ptr_unsafe(IN int32 tid);

extern
BOOL
task_ready(IN int32 tid);

extern
int32
create_task_by_file(IN int8 * filename,
					IN int8 * param,
					IN int8 * working_dir);

extern
int32
create_sys_task_by_file(IN int8 * filename,
						IN int8 * param,
						IN int8 * working_dir);

extern
BOOL
create_task_by_file_wait(	IN int8 * filename,
							IN int8 * param,
							IN int8 * working_dir,
							OUT int32 * retvalue);

extern
int32
get_next_task_id(void);

extern
void
set_task_ran_state(IN int32 tid);

extern
int32
get_running_tid(void);

extern
void *
get_physical_address(	IN int32 tid, 
						IN void * line_address);

extern
BOOL
tasks_redirect_stdin(	IN int32 tid,
						IN int8 * path);

extern
BOOL
tasks_redirect_stdout(	IN int32 tid,
						IN int8 * path);

extern
BOOL
tasks_redirect_stderr(	IN int32 tid,
						IN int8 * path);

#define	LOCK_TASK() cli()
#define	UNLOCK_TASK() sti()

DEFINE_LOCK_EXTERN(tasks)

#endif
