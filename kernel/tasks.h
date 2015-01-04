//Filename:		tasks.h
//Author:		Ihsoh
//Date:			2014-7-22
//Descriptor:	Task schedule

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
#define	TASK_BASE_ADDR				0x00000000

//结构名:	Task
//功能:		任务
struct Task
{
	int32				used;				//是否使用该任务槽
	int32				running;			//是否正在运行
	int32				ran;				//是否运行
	int8				name[1024];			//任务名
	int8				param[1024];		//参数
	uint32				app_len;			//程序的长度
	uint8 *				addr;				//任务的起始地址
	struct TSS 			tss;				//任务的TSS
	struct I387State	i387_state;			//Intel 80387状态
	int32				init_i387;			//该任务是否初始化了Intel 80387
	uint32 * 			pagedt_addr;		//页目录表和页表的地址
	uint32				real_pagedt_addr;	//真正页目录表和页表的地址
	FILE **				opened_file_ptrs;	//打开文件指针列表
	void **				memory_block_ptrs;	//内存块指针列表
	uint32				used_memory_size;	//任务内存使用量
};

extern BOOL init_tasks(void);
extern int32 create_task(int8 * name, int8 * param, uint8 * app, uint32 app_len);
extern BOOL kill_task(int32 tid);
extern void kill_all_tasks(void);
extern BOOL get_task_info(int32 tid, struct Task * task);
extern BOOL set_task_info(int32 tid, struct Task * task);
extern struct Task * get_task_info_ptr(int32 tid);
extern int32 create_task_by_file(int8 * filename, int8 * param);
extern int32 get_next_task_id(void);
extern void set_task_ran_state(int32 tid);
extern int32 get_running_tid(void);
extern void * get_physical_address(int32 tid, void * line_address);

#define	LOCK_TASK() cli()
#define	UNLOCK_TASK() sti()

DEFINE_LOCK_EXTERN(tasks)

#endif
