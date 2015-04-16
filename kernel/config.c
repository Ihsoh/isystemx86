/**
	@File:			config.c
	@Author:		Ihsoh
	@Date:			2015-4-16
	@Description:
		系统配置。
*/

#include "config.h"
#include "types.h"
#include "die.h"
#include "system.h"
#include "ifs1fs.h"
#include "log.h"

#include <jsonlib/jsonlib.h>

static JSONLObjectPtr system_json 				= NULL;
static JSONLObjectPtr system_console_json 		= NULL;

BOOL
config_init(void)
{
	struct die_info info;
	int8 buffer[KB(64)];
	FILE * fptr = fopen(SYSTEM_CONFIG_FILE, FILE_MODE_READ);
	if(fptr != NULL)
	{
		uint32 len = fread(fptr, buffer, sizeof(buffer));
		buffer[len] = '\0';
		fclose(fptr);
		JSONLRawPtr system_json_raw = jsonl_parse_json(buffer);
		if(	system_json_raw != NULL
			&& JSONL_TYPE(system_json_raw) == JSONL_TYPE_OBJECT)
		{
			system_json = JSONL_OBJECT(system_json_raw);
			JSONLRawPtr value_raw = NULL;
			if(	!JSONL_OBJECT_GET(system_json, "Console", &value_raw)
				|| JSONL_TYPE(value_raw) != JSONL_TYPE_OBJECT)
			{
				fill_info(info, DC_INIT_CONFIG, DI_INIT_CONFIG);
				die(&info);
			}
			system_console_json = JSONL_OBJECT(value_raw);
		}
		else
		{
			fill_info(info, DC_INIT_CONFIG, DI_INIT_CONFIG);
			die(&info);
		}
	}
	else
	{
		fill_info(info, DC_INIT_CONFIG, DI_INIT_CONFIG);
		die(&info);
	}
	return TRUE;
}

static
BOOL
config_get_string(	IN JSONLObjectPtr obj,
					IN int8 * name,
					OUT int8 * v,
					IN uint32 max)
{
	if(	name == NULL
		|| obj == NULL
		|| v == NULL)
		return FALSE;
	JSONLRawPtr value_raw = NULL;
	if(	!JSONL_OBJECT_GET(obj, name, &value_raw)
		|| JSONL_TYPE(value_raw) != JSONL_TYPE_VALUE
		|| jsonl_value_type(value_raw) != JSONL_VALUE_TYPE_STRING)
		return FALSE;
	return jsonl_string_value(value_raw, v, max);
}

static
BOOL
config_get_bool(IN JSONLObjectPtr obj,
				IN int8 * name,
				OUT BOOL * v)
{
	if(	name == NULL
		|| obj == NULL
		|| v == NULL)
		return FALSE;
	JSONLRawPtr value_raw = NULL;
	if(	!JSONL_OBJECT_GET(obj, name, &value_raw)
		|| jsonl_value_type(value_raw) != JSONL_VALUE_TYPE_BOOL)
		return FALSE;
	return jsonl_bool_value(value_raw, v);
}

static
BOOL
config_get_number(	IN JSONLObjectPtr obj,
					IN int8 * name,
					OUT double * v)
{
	if(	name == NULL
		|| obj == NULL
		|| v == NULL)
		return FALSE;
	JSONLRawPtr value_raw = NULL;
	if(	!JSONL_OBJECT_GET(obj, name, &value_raw)
		|| jsonl_value_type(value_raw) != JSONL_VALUE_TYPE_NUMBER)
		return FALSE;
	return jsonl_number_value(value_raw, v);
}

BOOL
config_system_get_string(	IN int8 * name,
							OUT int8 * v,
							IN uint32 max)
{
	BOOL r = config_get_string(system_json, name, v, max);
	if(!r)
	{
		int8 buffer[1024];
		strcpy(buffer, "System configuration object doesn't contain key '");
		strcat(buffer, name);
		strcat(buffer, "'(STRING) or other some unknow errors.");
		log(LOG_ERROR, buffer);
	}
	return r;
}

BOOL
config_system_get_bool(	IN int8 * name,
						OUT BOOL * v)
{
	BOOL r = config_get_bool(system_json, name, v);
	if(!r)
	{
		int8 buffer[1024];
		strcpy(buffer, "System configuration object doesn't contain key '");
		strcat(buffer, name);
		strcat(buffer, "'(BOOL) or other some unknow errors.");
		log(LOG_ERROR, buffer);
	}
	return r;
}

BOOL
config_system_get_number(	IN int8 * name,
							OUT double * v)
{
	BOOL r = config_get_number(system_json, name, v);
	if(!r)
	{
		int8 buffer[1024];
		strcpy(buffer, "System configuration object doesn't contain key '");
		strcat(buffer, name);
		strcat(buffer, "'(NUMBER) or other some unknow errors.");
		log(LOG_ERROR, buffer);
	}
	return r;
}

BOOL
config_system_console_get_string(	IN int8 * name,
									OUT int8 * v,
									IN uint32 max)
{
	BOOL r = config_get_string(system_console_json, name, v, max);
	if(!r)
	{
		int8 buffer[1024];
		strcpy(buffer, "System-Console configuration object doesn't contain key '");
		strcat(buffer, name);
		strcat(buffer, "'(STRING) or other some unknow errors.");
		log(LOG_ERROR, buffer);
	}
	return r;
}

BOOL
config_system_console_get_bool(	IN int8 * name,
								OUT BOOL * v)
{
	BOOL r = config_get_bool(system_console_json, name, v);
	if(!r)
	{
		int8 buffer[1024];
		strcpy(buffer, "System-Console configuration object doesn't contain key '");
		strcat(buffer, name);
		strcat(buffer, "'(BOOL) or other some unknow errors.");
		log(LOG_ERROR, buffer);
	}
	return r;
}

BOOL
config_system_console_get_number(	IN int8 * name,
									OUT double * v)
{
	BOOL r = config_get_number(system_console_json, name, v);
	if(!r)
	{
		int8 buffer[1024];
		strcpy(buffer, "System-Console configuration object doesn't contain key '");
		strcat(buffer, name);
		strcat(buffer, "'(NUMBER) or other some unknow errors.");
		log(LOG_ERROR, buffer);
	}
	return r;
}
