#ifndef	_PATHLIB_H_
#define	_PATHLIB_H_

#include "types.h"

typedef void * (* PATHLMalloc)(IN uint32 num_bytes);
typedef void * (* PATHLCalloc)(IN uint32 n, IN uint32 size);
typedef void (* PATHLFree)(IN void * ptr);

typedef struct _PATHLEnvironment
{
	PATHLMalloc	pathl_malloc;
	PATHLCalloc	pathl_calloc;
	PATHLFree	pathl_free;
} PATHLEnvironment, * PATHLEnvironmentPtr;

extern PATHLMalloc	pathl_malloc;
extern PATHLCalloc	pathl_calloc;
extern PATHLFree	pathl_free;

typedef enum _PATHLType
{
	PATHLTYPE_ERROR = -1,
	PATHLTYPE_FILE	= 1,
	PATHLTYPE_DIR	= 2
} PATHLType, * PATHLTypePtr;

#define	PATHL_MAX_DIR_DEPTH		32
#define	PATH_MAX_NAME_LEN		255
#define	PATHL_MAX_EXT_LEN		255

typedef struct _PATHLPath
{
	PATHLType 	type;
	int8		symbol[3];
	int8 		dirs[PATHL_MAX_DIR_DEPTH][PATH_MAX_NAME_LEN + 1];
	uint32		dir_count;
	int8		file[PATH_MAX_NAME_LEN + 1];
	int8		file_without_ext[PATH_MAX_NAME_LEN + 1];
	int8		ext[PATHL_MAX_EXT_LEN + 1];
} PATHLPath, * PATHLPathPtr;

extern
BOOL
pathl_init(IN PATHLEnvironmentPtr env);

extern
BOOL
pathl_parse(IN const int8 * path,
			OUT PATHLPathPtr path_obj);

extern
BOOL
pathl_build(IN PATHLPathPtr path_obj,
			OUT int8 * path);

extern
PATHLType
pathl_type(IN PATHLPathPtr path_obj);

extern
const int8 *
pathl_symbol(IN PATHLPathPtr path_obj);

extern
uint32
pathl_dir_depth(IN PATHLPathPtr path_obj);

extern
const int8 *
pathl_dir(	IN PATHLPathPtr path_obj,
			IN uint32 index);

extern
const int8 *
pathl_file(IN PATHLPathPtr path_obj);

extern
const int8 *
pathl_file_without_ext(IN PATHLPathPtr path_obj);

extern
const int8 *
pathl_ext(IN PATHLPathPtr path_obj);

#endif
