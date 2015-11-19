//Filename:		system.c
//Author:		Ihsoh
//Date:			2014-8-1
//Descriptor:	System functions

#include "system.h"
#include "sparams.h"
#include "types.h"
#include "string.h"

static int get_tid(void)
{
	int tid = *(int *)(0x01000000 + 0);
	return tid;
}

void app_exit(void)
{
	struct SParams sparams;
	int tid = get_tid();
	sparams.param0 = SPARAM(tid);	
	system_call(SCALL_SYSTEM, SCALL_EXIT, &sparams);
	while(1);
}

void * allocm(uint len)
{
	struct SParams sparams;
	sparams.param0 = SPARAM(len);
	system_call(SCALL_SYSTEM, SCALL_ALLOC_M, &sparams);
	return (void *)UINT32_PTR_SPARAM(sparams.param0);
}

int freem(void * ptr)
{
	struct SParams sparams;
	sparams.param0 = SPARAM(ptr);
	system_call(SCALL_SYSTEM, SCALL_FREE_M, &sparams);
	return INT32_SPARAM(sparams.param0);
}

void get_param(char * buffer)
{
	struct SParams sparams;
	sparams.param0 = SPARAM(buffer);
	system_call(SCALL_SYSTEM, SCALL_GET_PARAM, &sparams);
}

static int parse_cmd(char * cmd, char * word, uint n)
{
	static uint len = 0;
	static char buffer[1024];
	static uint pos = 0;
	if(cmd != NULL)
	{
		strcpy(buffer, cmd);
		len = strlen(buffer);
		pos = 0;
	}
	else
	{
		uint ui = 0;
		int state = 0;
		for(; pos < len && ui < n; pos++)
		{
			char chr = buffer[pos];
			//普通
			if(state == 0 && chr != ' ' && chr != '\t' && chr != '"' && chr != '\'')
			{
				word[ui++] = chr;
				state = 1;
			}
			else if(state == 1 && chr != ' ' && chr != '\t' && chr != '"')
				word[ui++] = chr;
			else if(state == 1 && (chr == ' ' || chr == '\t' || chr == '"'))
				break;
			//双引号字符串
			else if(state == 0 && chr == '"')
				state = 2;
			else if(state == 2 && chr == '\\' && pos + 1 < len && (buffer[pos + 1] == '"' || buffer[pos + 1] == '\\'))
				word[ui++] = buffer[++pos];
			else if(state == 2 && chr == '\\' && pos + 1 < len && buffer[pos + 1] == 'r')
			{
				word[ui++] = '\r';
				pos++;			
			}	
			else if(state == 2 && chr == '\\' && pos + 1 < len && buffer[pos + 1] == 'n')
			{
				word[ui++] = '\n';
				pos++;			
			}
			else if(state == 2 && chr == '\\' && pos + 1 < len && buffer[pos + 1] == 't')
			{
				word[ui++] = '\t';
				pos++;			
			}		
			else if(state == 2 && chr == '\\' && pos + 1 < len && buffer[pos + 1] != '"' && buffer[pos + 1] != '\\')
				word[ui++] = '\\';
			else if(state == 2 && chr != '\\' && chr != '"')
				word[ui++] = chr;
			else if(state == 2 && chr == '"')
			{
				pos++;
				break;
			}
			//单引号字符串
			else if(state == 0 && chr == '\'')
				state = 3;
			else if(state == 3 && chr == '\\' && pos + 1 < len && (buffer[pos + 1] == '\'' || buffer[pos + 1] == '\\'))
				word[ui++] = buffer[++pos];
			else if(state == 3 && chr == '\\' && pos + 1 < len && buffer[pos + 1] == 'r')
			{
				word[ui++] = '\r';
				pos++;			
			}	
			else if(state == 3 && chr == '\\' && pos + 1 < len && buffer[pos + 1] == 'n')
			{
				word[ui++] = '\n';
				pos++;			
			}
			else if(state == 3 && chr == '\\' && pos + 1 < len && buffer[pos + 1] == 't')
			{
				word[ui++] = '\t';
				pos++;			
			}
			else if(state == 3 && chr == '\\' && pos + 1 < len && buffer[pos + 1] != '\'' && buffer[pos + 1] != '\\')
				word[ui++] = '\\';
			else if(state == 3 && chr != '\\' && chr != '\'')
				word[ui++] = chr;
			else if(state == 3 && chr == '\'')
			{
				pos++;
				break;
			}
		}
		word[ui] = '\0';
	}
	return len - pos;
}

int get_param_w(char * buffer)
{
	static char param_buffer[1024];
	static int run_cmd = 0;
	int r = 0;
	if(buffer == NULL)
	{
		get_param(param_buffer);
		r = parse_cmd(param_buffer, NULL, 0);
	}
	else
	{
		r = parse_cmd(NULL, buffer, 1023);
		if(strcmp(buffer, "run") == 0 && !run_cmd)
		{
			r = parse_cmd(NULL, buffer, 1023);
			run_cmd = 1;
		}
	}
	return r;
}

void get_datetime(struct CMOSDateTime * dt)
{
	struct SParams sparams;
	sparams.param0 = SPARAM(dt);
	system_call(SCALL_SYSTEM, SCALL_GET_DATETIME, &sparams);
}

void set_clock(int enable)
{
	struct SParams sparams;
	sparams.param0 = SPARAM(enable);
	system_call(SCALL_SYSTEM, SCALL_SET_CLOCK, &sparams);
}

uint create_mqueue(char * name)
{
	struct SParams sparams;
	sparams.param0 = SPARAM(name);
	system_call(SCALL_SYSTEM, SCALL_MQUEUE_S_CREATE, &sparams);
	return UINT32_SPARAM(sparams.param0);
}

int delete_mqueue(uint addr)
{
	struct SParams sparams;
	sparams.param0 = SPARAM(addr);
	system_call(SCALL_SYSTEM, SCALL_MQUEUE_S_DELETE, &sparams);
	return INT32_SPARAM(sparams.param0);
}

int push_message_s(uint addr, MQueueMessagePtr message)
{
	struct SParams sparams;
	sparams.param0 = SPARAM(addr);
	sparams.param1 = SPARAM(message);
	system_call(SCALL_SYSTEM, SCALL_MQUEUE_S_PUSH, &sparams);
	return INT32_SPARAM(sparams.param0);
}

int pop_message_s(uint addr, MQueueMessagePtr message)
{
	struct SParams sparams;
	sparams.param0 = SPARAM(addr);
	sparams.param1 = SPARAM(message);
	system_call(SCALL_SYSTEM, SCALL_MQUEUE_S_POP, &sparams);
	return INT32_SPARAM(sparams.param0);
}

int push_message_c(char * name, MQueueMessagePtr message)
{
	struct SParams sparams;
	message->tid = get_tid();
	sparams.param0 = SPARAM(name);
	sparams.param1 = SPARAM(message);
	system_call(SCALL_SYSTEM, SCALL_MQUEUE_C_PUSH, &sparams);
	return INT32_SPARAM(sparams.param0);
}

int pop_message_c(char * name, MQueueMessagePtr message)
{
	struct SParams sparams;
	sparams.param0 = SPARAM(name);
	sparams.param1 = SPARAM(message);
	system_call(SCALL_SYSTEM, SCALL_MQUEUE_C_POP, &sparams);
	return INT32_SPARAM(sparams.param0);
}

int run_in_bg(void)
{
	struct SParams sparams;
	system_call(SCALL_SYSTEM, SCALL_RUN_IN_BG, &sparams);
	return INT32_SPARAM(sparams.param0);
}

void __set_retvalue(int retvalue)
{
	struct SParams sparams;
	sparams.param0 = SPARAM(retvalue);
	system_call(SCALL_SYSTEM, SCALL_SET_RETVALUE, &sparams);
}

int exec(char * path, char * param)
{
	struct SParams sparams;
	sparams.param0 = SPARAM(path);
	sparams.param1 = SPARAM(param);
	system_call(SCALL_SYSTEM, SCALL_EXEC, &sparams);
	return INT32_SPARAM(sparams.param0);
}

int wait(int tid, int * retvalue)
{
	struct SParams sparams;
	sparams.param0 = SPARAM(tid);
	system_call(SCALL_SYSTEM, SCALL_WAIT, &sparams);
	*retvalue = INT32_SPARAM(sparams.param1);
	return INT32_SPARAM(sparams.param0);
}

int exec_sync(char * path, char * param)
{
	int tid = exec(path, param);
	if(tid == -1)
		return -1;
	int retvalue = -1;
	while(wait(tid, &retvalue))
		asm volatile ("pause");
	return retvalue;
}

uint32 memory_size(void * ptr)
{
	struct SParams sparams;
	sparams.param0 = SPARAM(ptr);
	system_call(SCALL_SYSTEM, SCALL_MEMORY_BLOCK_SIZE, &sparams);
	return UINT32_SPARAM(sparams.param0);
}

void dispatch_tick(void)
{
	struct SParams sparams;
	system_call(SCALL_SYSTEM, SCALL_DISPATCH_TICK, &sparams);
}

uint32 ILLoadELF(CASCTEXT path)
{
	struct SParams sparams;
	sparams.param0 = SPARAM(path);
	system_call(SCALL_SYSTEM, SCALL_LOAD_ELF, &sparams);
	return UINT32_SPARAM(sparams.param0);
}

uint32 ILLoadELFSO(CASCTEXT path)
{
	struct SParams sparams;
	sparams.param0 = SPARAM(path);
	system_call(SCALL_SYSTEM, SCALL_LOAD_ELF_SO, &sparams);
	uint32 ctx_idx = UINT32_SPARAM(sparams.param0);
	if(ctx_idx != 0xffffffff)
	{
		BOOL (* __elfso32_init)(void);
		__elfso32_init = ILGetELFSOSymbol(ctx_idx, "__elfso32_init");
		if(__elfso32_init != NULL)
			if(!__elfso32_init())
			{
				ILUnloadELFSO(ctx_idx);
				ctx_idx = 0xffffffff;
			}
	}
	return ctx_idx;
}

void * ILGetELFSOSymbol(uint32 ctx_idx, CASCTEXT name)
{
	struct SParams sparams;
	sparams.param0 = SPARAM(ctx_idx);
	sparams.param1 = SPARAM(name);
	system_call(SCALL_SYSTEM, SCALL_GET_ELF_SO_SYMBOL, &sparams);
	return VOID_PTR_SPARAM(sparams.param0);
}

BOOL ILUnloadELFSO(uint32 ctx_idx)
{
	if(ctx_idx != 0xffffffff)
	{
		void (* __elfso32_uninit)(void);
		__elfso32_uninit = ILGetELFSOSymbol(ctx_idx, "__elfso32_uninit");
		if(__elfso32_uninit != NULL)
			__elfso32_uninit();
	}
	struct SParams sparams;
	sparams.param0 = SPARAM(ctx_idx);
	system_call(SCALL_SYSTEM, SCALL_UNLOAD_ELF_SO, &sparams);
	return BOOL_SPARAM(sparams.param0);
}
