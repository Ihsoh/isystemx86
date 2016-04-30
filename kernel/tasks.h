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
#include "lock.h"
#include "sse.h"

#include "elf/parser.h"

#include "fs/ifs1/fs.h"

#define	MAX_TASK_COUNT				64
#define	MAX_APP_LEN					(16 * 1024 * 1024)
#define	MAX_TASK_OPENED_FILE_COUNT	1024
#define	MAX_TASK_MEMORY_BLOCK_COUNT	(10 * 1024)
#define MAX_TASK_MQUEUE_COUNT		16
#define	TASK_BASE_ADDR				0x00000000

#define MAX_TASK_ELF_SO_COUNT		4

typedef enum
{
	TASK_TYPE_USER		= 1,
	TASK_TYPE_SYSTEM	= 2
} TaskType;

typedef enum
{
	TASK_ATTR_NONE 				= 0x00000000,
} TaskAttr;

typedef enum
{
	TASK_PRIORITY_HIGH		= 1,
	TASK_PRIORITY_NORMAL	= 2,
	TASK_PRIORITY_LOW		= 3
} TaskPriority;

#define TASK_TICK_HIGH		40
#define TASK_TICK_NORMAL	15
#define TASK_TICK_LOW		5

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
	SSEState			sse_state;			//SSE的状态。
	uint32 * 			pagedt_addr;		//页目录表和页表的地址。
	uint32				real_pagedt_addr;	//真正页目录表和页表的地址。
	FileObject **		opened_file_ptrs;	//打开文件指针列表。
	void **				memory_block_ptrs;	//内存块指针列表。
	uint32 *			mqueue_ids;			//任务打开的消息队列的ID集合。
	uint32				used_memory_size;	//任务内存使用量。
	int8 *				working_dir;		//作业目录。
	int32				retvalue;			//任务的返回值。
	BOOL				ready;				//是否准备就绪，任务创建时为 False，
											//直到为 True 时，才会开始运行。
	BOOL				is_system_call;		//是否在进行系统调用。
	BOOL				fs_lock;			//是否获得了文件系统锁。
	FileObject * 		stdin;				//任务的标准输入。如果为NULL则为键盘，否则为文件。
	FileObject *		stdout;				//任务的标准输出。如果为NULL则为屏幕，否则为文件。
	FileObject *		stderr;				//任务的标准错误。如果为NULL则为屏幕，否则为文件。
	uint32				read_count;			//已从标准输入读取了的字符数。
	TaskType 			type;				//任务的类型。
											//如果为TASK_TYPE_USER，则表示该任务为用户任务，运行于Ring3。
											//如果为TASK_TYPE_SYSTEM，则表示该任务为系统任务，运行于Ring0。
	TaskAttr 			attr;				//任务属性。
	TaskPriority 		priority;			//任务优先级。
	int32				tick;				//滴答。到0时切换任务。
	OnTaskExit			on_exit;			//当任务退出时调用。
	uint8 *				elf;				//装载ELF程序的空间。
	ELFContextPtr		elf_so_ctx[MAX_TASK_ELF_SO_COUNT];
	uint32				pad[3];
};


extern
BOOL
TskmgrInit(void);

extern
int32
TskmgrCreateTask(IN int8 * name,
			IN int8 * param,
			IN uint8 * app,
			IN uint32 app_len,
			IN int8 * working_dir);

extern
int32
TskmgrCreateSystemTask(IN int8 * name,
				IN int8 * param,
				IN uint8 * app,
				IN uint32 app_len,
				IN int8 * working_dir);

extern
BOOL
TskmgrKillTask(IN int32 tid);

extern
BOOL
TskmgrKillSystemTask(IN int32 tid);

extern
void
TskmgrKillAllTasks(void);

extern
BOOL
TskmgrGetTaskInfo(	IN int32 tid,
				OUT struct Task * task);

extern
BOOL
TskmgrSetTaskInfo(	IN int32 tid,
				IN struct Task * task);

extern
struct Task *
TskmgrGetTaskInfoPtr(IN int32 tid);

extern
struct Task *
TskmgrGetTaskInfoPtrUnsafely(IN int32 tid);

extern
BOOL
TskmgrSetTaskToReady(IN int32 tid);

extern
int32
TskmgrCreateTaskByFile(IN int8 * filename,
					IN int8 * param,
					IN int8 * working_dir);

extern
int32
TskmgrCreateSystemTaskByFile(IN int8 * filename,
						IN int8 * param,
						IN int8 * working_dir);

extern
BOOL
TskmgrCreateTaskByFileSync(	IN int8 * filename,
							IN int8 * param,
							IN int8 * working_dir,
							OUT int32 * retvalue);

extern
int32
TskmgrGetNextTaskID(void);

extern
void
TaskmgrSetTaskRunningStatus(IN int32 tid);

extern
int32
TaskmgrGetRunningTaskID(void);

extern
void *
TaskmgrConvertLAddrToPAddr(	IN int32 tid, 
						IN void * linear_address);

extern
BOOL
TaskmgrRedirectStdin(	IN int32 tid,
						IN int8 * path);

extern
BOOL
TaskmgrRedirectStdout(	IN int32 tid,
						IN int8 * path);

extern
BOOL
TaskmgrRedirectStderr(	IN int32 tid,
						IN int8 * path);

extern
int32
TaskmgrGetTaskCount(void);

extern
void *
TaskmgrAllocMemory(	IN int32 tid,
					IN uint32 size,
					OUT void ** phyaddr);

extern
void
TaskmgrFreeMemory(	IN int32 tid,
					IN void * ptr);

extern
uint32
TaskmgrLoadElfExecutableFile(	IN int32 tid,
								IN CASCTEXT path);

extern
uint32
TaskmgrLoadElfSharedObjectFile(	IN int32 tid,
								IN CASCTEXT path);

extern
void *
TaskmgrGetElfSharedObjectSymbol(IN int32 tid,
								IN uint32 ctx_idx,
								IN CASCTEXT name);

extern
BOOL
TaskmgrUnloadElfSharedObjectFile(	IN int32 tid,
									IN uint32 ctx_idx);

#define	LOCK_TASK() cli()
#define	UNLOCK_TASK() sti()

DEFINE_LOCK_EXTERN(tasks)

#endif
