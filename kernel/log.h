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
init_log(void);

extern
void
free_log(void);

extern
void
write_log_to_disk(void);

extern
void
clear_log(void);

extern
void
log(IN const int8 * type,
	IN const int8 * text);

extern
int8 *
get_log(void);

#endif
