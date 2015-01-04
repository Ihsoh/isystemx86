//Filename:		system.c
//Author:		Ihsoh
//Date:			2014-8-1
//Descriptor:	System functions

#include <system.h>
#include <sparams.h>
#include <types.h>
#include <string.h>

void app_exit(void)
{
	struct SParams sparams;
	int tid = *(int *)(0x01000000 + 1 * 1024 * 1024);
	sparams.param0 = SPARAM(tid);	
	system_call(SCALL_SYSTEM, SCALL_EXIT, &sparams);
	while(1);
}

void * allocm(uint len)
{
	struct SParams sparams;
	sparams.param0 = SPARAM(len);
	system_call(SCALL_SYSTEM, SCALL_ALLOC_M, &sparams);
	return (void *)(sparams.param0);
}

int freem(void * ptr)
{
	struct SParams sparams;
	sparams.param0 = SPARAM(ptr);
	system_call(SCALL_SYSTEM, SCALL_FREE_M, &sparams);
	return (int)(sparams.param0);
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
