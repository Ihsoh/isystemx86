#include "test.h"
#include "types.h"
#include "pathlib.h"
#include "lib.h"

BOOL
pathl_test(void)
{
	PATHLPath path_obj;

	//====================错误的模式====================
	// DA:
	if(pathl_parse("DA:", &path_obj))
		return FALSE;

	// DA:/!
	if(pathl_parse("DA:/!", &path_obj))
		return FALSE;

	// DA://
	if(pathl_parse("DA://", &path_obj))
		return FALSE;

	// DA:/ABC//DEF/
	if(pathl_parse("DA:/ABC//DEF/", &path_obj))
		return FALSE;

	// DA:/ABC/!/DEF/
	if(pathl_parse("DA:/ABC/!/DEF/", &path_obj))
		return FALSE;

	//====================正确的模式====================
	// DA:/
	if(	!(pathl_parse("DA:/", &path_obj)
		&& pathl_type(&path_obj) == PATHLTYPE_DIR
		&& pathl_lib_strcmp(pathl_symbol(&path_obj), "DA") == 0
		&& pathl_dir_depth(&path_obj) == 0))
		return FALSE;

	// DA:/ABC/DEF/
	if(	!(pathl_parse("DA:/ABC/DEF/", &path_obj)
		&& pathl_type(&path_obj) == PATHLTYPE_DIR
		&& pathl_lib_strcmp(pathl_symbol(&path_obj), "DA") == 0
		&& pathl_dir_depth(&path_obj) == 2
		&& pathl_lib_strcmp(pathl_dir(&path_obj, 1), "ABC") == 0
		&& pathl_lib_strcmp(pathl_dir(&path_obj, 2), "DEF") == 0))
		return FALSE;

	// DA:/ABC/DEF/abc.txt
	if(	!(pathl_parse("DA:/ABC/DEF/abc.txt", &path_obj)
		&& pathl_type(&path_obj) == PATHLTYPE_FILE
		&& pathl_lib_strcmp(pathl_symbol(&path_obj), "DA") == 0
		&& pathl_dir_depth(&path_obj) == 2
		&& pathl_lib_strcmp(pathl_dir(&path_obj, 1), "ABC") == 0
		&& pathl_lib_strcmp(pathl_dir(&path_obj, 2), "DEF") == 0
		&& pathl_lib_strcmp(pathl_file(&path_obj), "abc.txt") == 0
		&& pathl_lib_strcmp(pathl_file_without_ext(&path_obj), "abc") == 0
		&& pathl_lib_strcmp(pathl_ext(&path_obj), ".txt") == 0))
		return FALSE;

	// DA:/ABC/DEF/.txt
	if(	!(pathl_parse("DA:/ABC/DEF/.txt", &path_obj)
		&& pathl_type(&path_obj) == PATHLTYPE_FILE
		&& pathl_lib_strcmp(pathl_symbol(&path_obj), "DA") == 0
		&& pathl_dir_depth(&path_obj) == 2
		&& pathl_lib_strcmp(pathl_dir(&path_obj, 1), "ABC") == 0
		&& pathl_lib_strcmp(pathl_dir(&path_obj, 2), "DEF") == 0
		&& pathl_lib_strcmp(pathl_file(&path_obj), ".txt") == 0
		&& pathl_lib_strcmp(pathl_file_without_ext(&path_obj), "") == 0
		&& pathl_lib_strcmp(pathl_ext(&path_obj), ".txt") == 0))
		return FALSE;

	// DA:/ABC/DEF/abc
	if(	!(pathl_parse("DA:/ABC/DEF/abc", &path_obj)
		&& pathl_type(&path_obj) == PATHLTYPE_FILE
		&& pathl_lib_strcmp(pathl_symbol(&path_obj), "DA") == 0
		&& pathl_dir_depth(&path_obj) == 2
		&& pathl_lib_strcmp(pathl_dir(&path_obj, 1), "ABC") == 0
		&& pathl_lib_strcmp(pathl_dir(&path_obj, 2), "DEF") == 0
		&& pathl_lib_strcmp(pathl_file(&path_obj), "abc") == 0
		&& pathl_lib_strcmp(pathl_file_without_ext(&path_obj), "abc") == 0
		&& pathl_lib_strcmp(pathl_ext(&path_obj), "") == 0))
		return FALSE;

	return TRUE;
}
