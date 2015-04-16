/**
	@File:			config.h
	@Author:		Ihsoh
	@Date:			2015-4-16
	@Description:
		系统配置。
*/

#ifndef	_CONFIG_H_
#define	_CONFIG_H_

#include "types.h"

extern
BOOL
config_init(void);

extern
BOOL
config_system_get_string(	IN int8 * name,
							OUT int8 * v,
							IN uint32 max);

extern
BOOL
config_system_get_bool(	IN int8 * name,
						OUT BOOL * v);

extern
BOOL
config_system_get_number(	IN int8 * name,
							OUT double * v);

extern
BOOL
config_system_console_get_string(	IN int8 * name,
									OUT int8 * v,
									IN uint32 max);

extern
BOOL
config_system_console_get_bool(	IN int8 * name,
								OUT BOOL * v);

extern
BOOL
config_system_console_get_number(	IN int8 * name,
									OUT double * v);

#endif
