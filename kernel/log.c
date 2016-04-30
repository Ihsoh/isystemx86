/**
	@File:			log.c
	@Author:		Ihsoh
	@Date:			2015-2-18
	@Description:
		日志。
*/

#include "log.h"
#include "types.h"
#include "cmos.h"
#include "memory.h"
#include "system.h"
#include "config.h"

#include "fs/ifs1/fs.h"

#include <ilib/string.h>

static int8 * log_buffer 	= NULL;
static BOOL write_in_rt		= FALSE;

void
LogInit(void)
{
	log_buffer = MemAlloc(LOG_BUFFER_SIZE);
	if(log_buffer == NULL)
		return;
	if(!Ifs1FileExists(SYSTEM_PATH"data/log/", "system.log"))
		Ifs1CreateFile(SYSTEM_PATH"data/log/", "system.log");
	log_buffer[0] = '\0';
	config_system_get_bool("WriteLogToDiskInRealTime", &write_in_rt);
}

void
LogFree(void)
{
	if(log_buffer == NULL)
		return;
	LogWriteToDisk();
	MemFree(log_buffer);
}

void
LogWriteToDisk(void)
{
	if(log_buffer == NULL)
		return;
	FileObject * fptr = Ifs1OpenFile(SYSTEM_LOG_FILE, FILE_MODE_APPEND);
	if(fptr == NULL)
		return;
	Ifs1AppendFile(fptr, log_buffer, strlen(log_buffer));
	Ifs1CloseFile(fptr);
}

void
LogClear(void)
{
	log_buffer[0] = '\0';
}

void
Log(IN const int8 * type,
	IN const int8 * text)
{
	if(log_buffer == NULL)
		return;
	int8 buffer[1024];
	struct CMOSDateTime dt;
	CmosGetDateTime(&dt);
	sprintf_s(	buffer,
				1024,
				"[%s] %d-%d-%d %d:%d:%d %s\n",
				type,
				dt.year, dt.month, dt.day, 
				dt.hour, dt.minute, dt.second,
				text);
	if(strlen(log_buffer) + strlen(buffer) + 1 > LOG_BUFFER_SIZE)
	{
		LogWriteToDisk();
		LogClear();
		UtlCopyString(log_buffer, LOG_BUFFER_SIZE, buffer);
	}
	else
	{
		UtlConcatString(log_buffer, LOG_BUFFER_SIZE, buffer);
		if(write_in_rt)
		{
			LogWriteToDisk();
			LogClear();
		}
	}
}

int8 *
LogGetBuffer(void)
{
	return log_buffer;
}
