/**
	@File:			log.h
	@Author:		Ihsoh
	@Date:			2015-2-18
	@Description:
*/

#ifndef	_LOG_H_
#define	_LOG_H_

#include "types.h"

#define	LOG_BUFFER_SIZE		(64 * 1024)

#define	LOG_NORMAL	"Normal"
#define	LOG_WARNING	"Warning"
#define	LOG_ERROR	"Error"

extern
void
LogInit(void);

extern
void
LogFree(void);

extern
void
LogWriteToDisk(void);

extern
void
LogClear(void);

extern
void
Log(IN const int8 * type,
	IN const int8 * text);

extern
int8 *
LogGetBuffer(void);

#endif
