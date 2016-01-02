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

#define	EXTERN_CONFIG_XXX_GET_STH(xxx)	\
extern	\
BOOL	\
config_##xxx##_get_string(	IN int8 * name,	\
							OUT int8 * v,	\
							IN uint32 max);	\
extern	\
BOOL	\
config_##xxx##_get_bool(IN int8 * name,	\
						OUT BOOL * v);	\
extern	\
BOOL	\
config_##xxx##_get_number(	IN int8 * name,	\
							OUT double * v);	\
extern	\
BOOL	\
config_##xxx##_get_uint(IN int8 * name,	\
						OUT uint32 * v);

EXTERN_CONFIG_XXX_GET_STH(system)
EXTERN_CONFIG_XXX_GET_STH(system_console)
EXTERN_CONFIG_XXX_GET_STH(system_ifs1)

EXTERN_CONFIG_XXX_GET_STH(gui)

#endif
