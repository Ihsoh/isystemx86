//Filename:		system.h
//Author:		Ihsoh
//Date:			2014-8-1
//Descriptor:	System functions

#ifndef	_SYSTEM_H_
#define	_SYSTEM_H_

#include "types.h"
#include <dslib/value.h>

struct CMOSDateTime
{
	ushort year;
	uchar month;
	uchar day;
	uchar day_of_week;
	uchar hour;
	uchar minute;
	uchar second;
} __attribute__((packed));

#define	MAX_MESSAGE_BYTES_PARAM_LEN	1024

typedef struct
{
	uint32		tid;
	uint32		message;
	DSLValueU	param0;
	DSLValueU	param1;
	DSLValueU	param2;
	DSLValueU	param3;
	DSLValueU	param4;
	DSLValueU	param5;
	DSLValueU	param6;
	DSLValueU	param7;
	DSLValueU	param8;
	DSLValueU	param9;
	uint8		bsparam0[MAX_MESSAGE_BYTES_PARAM_LEN];
} MQueueMessage, * MQueueMessagePtr;

#include "../../kernel/syscall/syscall.h"

extern void app_exit(void);
#define	il_app_exit()	(app_exit())
#define	ILExitApplication()	(app_exit())

extern void * allocm(uint len);
#define	il_allocm(len)	(allocm((len)))
#define	ILAllocMemory(len)	(allocm((len)))

extern int freem(void * ptr);
#define	il_freem(ptr)	(freem((ptr)))
#define	ILFreeMemory(ptr)	(freem((ptr)))

extern void get_param(char * buffer);
#define	il_get_param(buffer)	(get_param((buffer)))
#define	ILGetCommand(buffer)	(get_param((buffer)))

extern int get_param_w(char * buffer);
#define	il_get_param_w(buffer)	(get_param_w((buffer)))
#define	ILGetParameter(buffer)	(get_param_w((buffer)))

extern void get_datetime(struct CMOSDateTime * dt);
#define	il_get_datetime(dt)	(get_datetime((dt)))
#define	ILGetDateTime(dt)	(get_datetime((dt)))

extern void set_clock(int enable);
#define	il_set_clock(enable)	(set_clock((enable)))
#define	ILEnableConsoleClock()	(set_clock(1))
#define	ILDisableConsoleClock()	(set_clock(0))

extern uint create_mqueue(char * name);
#define	il_create_mqueue(name)	(create_mqueue((name)))
#define	ILCreateMessageQueue(name)	(create_mqueue((name)))

extern int delete_mqueue(uint addr);
#define	il_delete_mqueue(addr)	(delete_mqueue((addr)))
#define	ILDeleteMessageQueue(addr)	(delete_mqueue((addr)))

extern int push_message_s(uint addr, MQueueMessagePtr message);
#define	il_push_message_s(addr, message)	(push_message_s((addr), (message)))
#define	ILPushMessageServer(addr, message)	(push_message_s((addr), (message)))

extern int pop_message_s(uint addr, MQueueMessagePtr message);
#define	il_pop_message_s(addr, message)		(pop_message_s((addr), (message)))
#define	ILPopMessageServer(addr, message)		(pop_message_s((addr), (message)))

extern int push_message_c(char * name, MQueueMessagePtr message);
#define il_push_message_c(name, message)	(push_message_c((name), (message)))
#define ILPushMessageClient(name, message)	(push_message_c((name), (message)))

extern int pop_message_c(char * name, MQueueMessagePtr message);
#define	il_pop_message_c(name, message)		(pop_message_c((name), (message)))
#define	ILPopMessageClient(name, message)		(pop_message_c((name), (message)))

extern int run_in_bg(void);
#define	il_run_in_bg()	(run_in_bg())
#define	ILRunInBackground()	(run_in_bg())

extern void __set_retvalue(int retvalue);

extern int exec(char * path, char * param);
#define	il_exec(path, param)	(exec((path), (param)))
#define	ILExecute(path, param)	(exec((path), (param)))

extern int wait(int tid, int * retvalue);
#define	il_wait(tid, retvalue)	(wait((tid), (retvalue)))
#define	ILWait(tid, retvalue)	(wait((tid), (retvalue)))

extern int exec_sync(char * path, char * param);
#define	il_exec_sync(path, param)	(exec_sync((path), (param)))
#define	ILExecuteSync(path, param)	(exec_sync((path), (param)))

extern uint32 memory_size(void * ptr);
#define	il_memory_size(ptr)	(memory_size((ptr)))
#define	ILMemorySize(ptr)	(memory_size((ptr)))

extern void console(void);
#define	il_console()	(console())
#define	ILConsole()	(console())

extern void dispatch_tick(void);
#define	il_dispatch_tick()	(dispatch_tick())
#define	ILDispatchTick()	(dispatch_tick())

extern uint32 ILLoadELF(CASCTEXT path);

extern uint32 ILLoadELFSO(CASCTEXT path);

extern void * ILGetELFSOSymbol(uint32 ctx_idx, CASCTEXT name);

extern BOOL ILUnloadELFSO(uint32 ctx_idx);

#endif
