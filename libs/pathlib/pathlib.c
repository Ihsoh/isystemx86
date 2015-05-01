#include "pathlib.h"
#include "types.h"

PATHLMalloc	pathl_malloc	= NULL;
PATHLCalloc	pathl_calloc	= NULL;
PATHLFree	pathl_free		= NULL;

BOOL
pathl_init(IN PATHLEnvironmentPtr env)
{
	if(	env == NULL
		|| env->pathl_malloc == NULL
		|| env->pathl_calloc == NULL
		|| env->pathl_free == NULL)
		return FALSE;
	pathl_malloc = env->pathl_malloc;
	pathl_calloc = env->pathl_calloc;
	pathl_free = env->pathl_free;
	return TRUE;
}

BOOL
pathl_parse(IN const int8 * path,
			OUT PATHLPathPtr path_obj)
{
	if(path == NULL || path_obj == NULL)
		return FALSE;
	const int8 * s = path;
	if(*s == '\0')
		return FALSE;
	path_obj->symbol[0] = *s;
	s++;
	if(*s == '\0')
		return FALSE;
	path_obj->symbol[1] = *s;
	s++;
	path_obj->symbol[2] = '\0';
	if(*s != ':')
		return FALSE;
	s++;
	if(*s != '/')
		return FALSE;
	s++;
	if(path[pathl_lib_strlen(path) - 1] == '/')
		path_obj->type = PATHLTYPE_DIR;
	else
		path_obj->type = PATHLTYPE_FILE;
	int8 chr;
	path_obj->dir_count = 0;
	uint32 ui = 0;
	int8 name[PATH_MAX_NAME_LEN + 1];
	name[0] = '\0';
	while((chr = *(s++)) != '\0')
		if(chr == '/')
		{
			if(pathl_lib_strlen(name) == 0)
				return FALSE;
			if(path_obj->dir_count == PATHL_MAX_DIR_DEPTH)
				return FALSE;
			pathl_lib_strcpy(	path_obj->dirs[path_obj->dir_count],
								name);
			ui = 0;
			path_obj->dir_count++;
			name[0] = '\0';
		}
		else
			if(ui > PATH_MAX_NAME_LEN)
				return FALSE;
			else
			{
				if(!(	(chr >= 'A' && chr <= 'Z')
						|| (chr >= 'a' && chr <= 'z')
						|| (chr >= '0' && chr <= '9')
						|| chr == '_'
						|| chr == '.'))
					return FALSE;
				name[ui++] = chr;
				name[ui] = '\0';
			}
	if(path_obj->type == PATHLTYPE_FILE)
	{
		pathl_lib_strcpy(path_obj->file, name);
		int32 i;
		for(i = pathl_lib_strlen(name); i >= 0; i--)
			if(name[i] == '.')
				break;
		if(i >= 0)
		{
			pathl_lib_strcpy(path_obj->file_without_ext, name);
			path_obj->file_without_ext[i] = '\0';
			pathl_lib_strcpy(path_obj->ext, name + i);
		}
		else
		{
			pathl_lib_strcpy(path_obj->file_without_ext, name);
			pathl_lib_strcpy(path_obj->ext, "");
		}
	}
	else
	{
		pathl_lib_strcpy(path_obj->file, "");
		pathl_lib_strcpy(path_obj->file_without_ext, "");
		pathl_lib_strcpy(path_obj->ext, "");
	}
	return TRUE;
}

BOOL
pathl_build(IN PATHLPathPtr path_obj,
			OUT int8 * path)
{
	if(path_obj == NULL || path == NULL)
		return FALSE;
	pathl_lib_strcpy(path, path_obj->symbol);
	pathl_lib_strcat(path, ":/");
	uint32 ui;
	for(ui = 0; ui < path_obj->dir_count; ui++)
	{
		pathl_lib_strcat(path, path_obj->dirs[ui]);
		pathl_lib_strcat(path, "/");
	}
	if(path_obj->type == PATHLTYPE_FILE)
		pathl_lib_strcat(path, path_obj->file);
	return TRUE;
}

PATHLType
pathl_type(IN PATHLPathPtr path_obj)
{
	if(path_obj == NULL)
		return PATHLTYPE_ERROR;
	return path_obj->type;
}

const int8 *
pathl_symbol(IN PATHLPathPtr path_obj)
{
	if(path_obj == NULL)
		return NULL;
	return path_obj->symbol;
}

uint32
pathl_dir_depth(IN PATHLPathPtr path_obj)
{
	if(path_obj == NULL)
		return 0xffffffff;
	return path_obj->dir_count;
}

const int8 *
pathl_dir(	IN PATHLPathPtr path_obj,
			IN uint32 index)
{
	if(path_obj == NULL)
		return NULL;
	if(index == 0)
		return "/";
	else
		if(index - 1 < path_obj->dir_count)
			return path_obj->dirs[index - 1];
		else
			return NULL;
}

const int8 *
pathl_file(IN PATHLPathPtr path_obj)
{
	if(path_obj == NULL || path_obj->type != PATHLTYPE_FILE)
		return NULL;
	return path_obj->file;
}

const int8 *
pathl_file_without_ext(IN PATHLPathPtr path_obj)
{
	if(path_obj == NULL || path_obj->type != PATHLTYPE_FILE)
		return NULL;
	return path_obj->file_without_ext;
}

const int8 *
pathl_ext(IN PATHLPathPtr path_obj)
{
	if(path_obj == NULL || path_obj->type != PATHLTYPE_FILE)
		return NULL;
	return path_obj->ext;
}
