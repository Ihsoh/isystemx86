//Filename:		ifs1fs.c
//Author:		Ihsoh
//Date:			2014-7-18
//Descriptor:	File System

#include "ifs1fs.h"
#include "ifs1blocks.h"
#include "cmos.h"
#include <string.h>

int format_disk(char * symbol)
{
	if(!del_all_blocks(symbol))
		return 0;
	struct DirBlock dir;
	fill_dir_block("/", &dir);
	return add_block(symbol, (struct RawBlock *)&dir) != 0xFFFFFFFF;
}

int check_disk(char * symbol)
{
	struct DirBlock dir;
	if(!get_block(symbol, START_BLOCK_ID, (struct RawBlock *)&dir))
		return 0;
	return dir.type == BLOCK_TYPE_DIR && strcmp(dir.dirname, "/") == 0;
}

static int _exists_dir(char * symbol, uint id, char * name)
{
	struct DirBlock dir;
	if(	id == 0xFFFFFFFF
		|| !get_block(symbol, id, (struct RawBlock *)&dir) 
		|| dir.used == 0 
		|| dir.type != BLOCK_TYPE_DIR)
		return 0;
	uint ui;
	for(ui = 0; ui < sizeof(dir.blockids) / sizeof(uint); ui++)
	{
		uint subdir_id = dir.blockids[ui];
		struct DirBlock subdir;
		if(	subdir_id != 0xFFFFFFFF 
			&& get_block(symbol, subdir_id, (struct RawBlock *)&subdir) 
			&& subdir.type == BLOCK_TYPE_DIR
			&& strcmp(subdir.dirname, name) == 0)
			return 1;
	}
	return 0;
}

static int _exists_file(char * symbol, uint id, char * name)
{
	struct DirBlock dir;
	if(	id == 0xFFFFFFFF
		|| !get_block(symbol, id, (struct RawBlock *)&dir) 
		|| dir.used == 0 
		|| dir.type != BLOCK_TYPE_DIR)
		return 0;
	uint ui;
	for(ui = 0; ui < sizeof(dir.blockids) / sizeof(uint); ui++)
	{
		uint subfile_id = dir.blockids[ui];
		struct FileBlock subfile;
		if(	subfile_id != 0xFFFFFFFF 
			&& get_block(symbol,subfile_id, (struct RawBlock *)&subfile) 
			&& subfile.type == BLOCK_TYPE_FILE
			&& strcmp(subfile.filename, name) == 0)
			return 1;
	}
	return 0;
}

static int _create_dir(char * symbol, uint id, char * name)
{
	struct DirBlock dir;
	if(	id == 0xFFFFFFFF
		|| strlen(name) > MAX_DIRNAME_LEN
		|| !get_block(symbol, id, (struct RawBlock *)&dir) 
		|| dir.used == 0 
		|| dir.type != BLOCK_TYPE_DIR
		|| _exists_dir(symbol, id, name))
		return 0;
	struct DirBlock new_dir;
	fill_dir_block(name, &new_dir);
	uint new_dir_id = add_block(symbol, (struct RawBlock *)&new_dir);
	if(new_dir_id == 0xFFFFFFFF)
		return 0;
	uint ui;
	for(ui = 0; ui < sizeof(dir.blockids) / sizeof(uint); ui++)
		if(dir.blockids[ui] == 0xFFFFFFFF)
		{
			dir.blockids[ui] = new_dir_id;
			set_block(symbol, id, (struct RawBlock *)&dir);
			return 1;
		}
	return 0;
}

static int _create_file(char * symbol, uint id, char * name)
{
	struct DirBlock dir;
	if(	id == 0xFFFFFFFF
		|| strlen(name) > MAX_FILENAME_LEN
		|| !get_block(symbol, id, (struct RawBlock *)&dir) 
		|| dir.used == 0 
		|| dir.type != BLOCK_TYPE_DIR
		|| _exists_file(symbol, id, name))
		return 0;
	struct FileBlock new_file;
	fill_file_block(name, &new_file);
	uint new_file_id = add_block(symbol, (struct RawBlock *)&new_file);
	if(new_file_id == 0xFFFFFFFF)
		return 0;
	uint ui;
	for(ui = 0; ui < sizeof(dir.blockids) / sizeof(uint); ui++)
		if(dir.blockids[ui] == 0xFFFFFFFF)
		{
			dir.blockids[ui] = new_file_id;
			set_block(symbol, id, (struct RawBlock *)&dir);
			return 1;
		}
	return 0;
}

static int _del_dir(char * symbol, uint id)
{
	struct DirBlock dir;
	if(	id == 0xFFFFFFFF
		|| !get_block(symbol, id, (struct RawBlock *)&dir) 
		|| dir.used == 0 
		|| dir.type != BLOCK_TYPE_DIR)
		return 0;
	uint ui;	
	for(ui = 0; ui < sizeof(dir.blockids) / sizeof(uint); ui++)
		if(dir.blockids[ui] != 0xFFFFFFFF)
			return 0;
	dir.used = 0;
	return set_block(symbol, id, (struct RawBlock *)&dir);
}

static int _del_file(char * symbol, uint id)
{
	struct FileBlock file;
	if(	id == 0xFFFFFFFF
		|| !get_block(symbol, id, (struct RawBlock *)&file)
		|| file.used == 0
		|| file.type != BLOCK_TYPE_FILE
		|| file.lock)
		return 0;
	file.used = 0;
	return set_block(symbol, id, (struct RawBlock *)&file);
}

int _df_count(char * symbol, uint id)
{
	int count = 0; 
	struct DirBlock dir;
	if(	id == 0xFFFFFFFF
		|| !get_block(symbol, id, (struct RawBlock *)&dir) 
		|| dir.used == 0 
		|| dir.type != BLOCK_TYPE_DIR)
		return -1;
	uint ui;
	for(ui = 0; ui < sizeof(dir.blockids) / sizeof(uint); ui++)
	{
		uint sub_block_id = dir.blockids[ui];
		struct RawBlock sub_block;
		if(	sub_block_id != 0xFFFFFFFF
			&& get_block(symbol, sub_block_id, &sub_block)
			&& sub_block.used)
			count++;
	}
	return count;
}

static int _df(char * symbol, uint id, struct RawBlock * blocks)
{
	int count = 0; 
	struct DirBlock dir;
	if(	id == 0xFFFFFFFF
		|| !get_block(symbol, id, (struct RawBlock *)&dir) 
		|| dir.used == 0 
		|| dir.type != BLOCK_TYPE_DIR)
		return -1;
	uint ui;
	for(ui = 0; ui < sizeof(dir.blockids) / sizeof(uint); ui++)
	{
		uint sub_blockid = dir.blockids[ui];
		struct RawBlock raw_block;
		if(	sub_blockid != 0xFFFFFFFF 
			&& get_block(symbol, sub_blockid, &raw_block)
			&& raw_block.used)
		{
			memcpy(blocks + count, &raw_block, sizeof(struct RawBlock));
			count++;
		}
		else
			dir.blockids[ui] = 0xFFFFFFFF;
	}
	if(set_block(symbol, id, (struct RawBlock *)&dir))
		return count;
	else
		return -1;
}

static int _rename_dir(char * symbol, uint id, char * new_name)
{
	struct DirBlock dir;
	if(	id == 0xFFFFFFFF
		|| strlen(new_name) > MAX_DIRNAME_LEN
		|| !get_block(symbol, id, (struct RawBlock *)&dir) 
		|| dir.used == 0 
		|| dir.type != BLOCK_TYPE_DIR)
		return 0;
	strcpy(dir.dirname, new_name);
	return set_block(symbol, id, (struct RawBlock *)&dir);
}

static int _rename_file(char * symbol, uint id, char * new_name)
{
	struct FileBlock file;
	if(	id == 0xFFFFFFFF
		|| strlen(new_name) > MAX_FILENAME_LEN
		|| !get_block(symbol, id, (struct RawBlock *)&file)
		|| file.used == 0
		|| file.type != BLOCK_TYPE_FILE)
		return 0;
	strcpy(file.filename, new_name);
	return set_block(symbol, id, (struct RawBlock *)&file);
}

uint parse_path(char * path, char * symbol, int * type)
{
	char * s = path;	
	char chr;
	int state = 0;
	char disk[3] = {'\0', '\0', '\0'};
	char name[MAX_FILENAME_BUFFER_LEN];
	char * n = name;
	int processed = 0;
	*type = 0;
	uint blockid = 0xFFFFFFFF;
	name[0] = '\0';
	while((chr = *(s++)) != '\0')
		if(state == 0)
		{
			disk[0] = chr;
			state = 1;
		}
		else if(state == 1)
		{
			disk[1] = chr;
			state = 2;
		}
		else if(state == 2)
		{
			if(chr != ':')
				break;
			state = 3;		
		}
		else if(state == 3)
		{
			if(chr != '/')
				break;
			blockid = START_BLOCK_ID;
			*type = BLOCK_TYPE_DIR;
			processed = 1;
			state = 4;
		}
		else if(state == 4 && chr != '/')
		{
			if(strlen(name) > MAX_FILENAME_LEN)
				return 0xFFFFFFFF;
			*(n++) = chr;
			*n = '\0';
			processed = 0;
		}
		else if(state == 4 && chr == '/')
		{
			if(strcmp(name, "") == 0)
				return 0xFFFFFFFF;
			struct DirBlock dir;
			if(	!get_block(disk, blockid, (struct RawBlock *)&dir) 
				|| dir.used == 0 
				|| dir.type != BLOCK_TYPE_DIR)
				return 0xFFFFFFFF;
			uint ui;
			for(ui = 0; ui < sizeof(dir.blockids) / sizeof(uint); ui++)
			{
				uint subdir_id = dir.blockids[ui];
				struct DirBlock subdir;
				if(	subdir_id != 0xFFFFFFFF 
					&& get_block(disk, subdir_id, (struct RawBlock *)&subdir) 
					&& subdir.type == BLOCK_TYPE_DIR
					&& strcmp(subdir.dirname, name) == 0)
				{
					blockid = subdir_id;
					*type = BLOCK_TYPE_DIR;
					name[0] = '\0';
					n = name;
					processed = 1;
				}
			}
		}
	if(!processed)
	{
		*type = BLOCK_TYPE_FILE;
		struct DirBlock dir;
		if(	!get_block(disk, blockid, (struct RawBlock *)&dir) 
			|| dir.used == 0 
			|| dir.type != BLOCK_TYPE_DIR)
			return 0xFFFFFFFF;
		uint ui;
		for(ui = 0; ui < sizeof(dir.blockids) / sizeof(uint); ui++)
		{
			uint file_id = dir.blockids[ui];
			struct FileBlock file;
			if(	file_id != 0xFFFFFFFF 
				&& get_block(disk, file_id, (struct RawBlock *)&file)
				&& file.type == BLOCK_TYPE_FILE 
				&& strcmp(file.filename, name) == 0)
			{
				strcpy(symbol, disk);
				return file_id;
			}
		}
		return 0xFFFFFFFF;
	}
	strcpy(symbol, disk);
	return blockid;
}

int df_count(char * path)
{
	int type;
	char symbol[3];
	uint id = parse_path(path, symbol, &type);
	if(type != BLOCK_TYPE_DIR)
		return -1;
	return _df_count(symbol, id);
}

int df(char * path, struct RawBlock * blocks)
{
	int type;
	char symbol[3];
	uint id = parse_path(path, symbol, &type);
	if(type != BLOCK_TYPE_DIR)
		return -1;
	return _df(symbol, id, blocks);
}

int create_dir(char * path, char * name)
{
	int type;
	char symbol[3];
	uint id = parse_path(path, symbol, &type);
	return _create_dir(symbol, id, name);
}

int create_file(char * path, char * name)
{
	int type;
	char symbol[3];
	uint id = parse_path(path, symbol, &type);
	return _create_file(symbol, id, name);
}

int del_dir(char * path)
{
	int type;
	char symbol[3];
	uint id = parse_path(path, symbol, &type);
	if(!_del_dir(symbol, id))
		return 0;
	char temp[MAX_PATH_BUFFER_LEN];
	prev_dir(path, temp);
	uint pdir_id = parse_path(temp, symbol, &type);
	struct DirBlock pdir;
	if(!get_block(symbol, pdir_id, (struct RawBlock *)&pdir))
		return 0;
	uint ui;	
	for(ui = 0; ui < sizeof(pdir.blockids) / sizeof(uint); ui++)
		if(pdir.blockids[ui] == id)
		{
			pdir.blockids[ui] = 0xFFFFFFFF;
			if(!set_block(symbol, pdir_id, (struct RawBlock *)&pdir))
				return 0;
			break;
		}
	return 1;
}

int del_file(char * path)
{
	int type;
	char symbol[3];
	uint id = parse_path(path, symbol, &type);
	if(!_del_file(symbol, id))
		return 0;
	char temp[MAX_PATH_BUFFER_LEN];
	file_dir(path, temp);
	uint pdir_id = parse_path(temp, symbol, &type);
	struct DirBlock pdir;
	if(!get_block(symbol, pdir_id, (struct RawBlock *)&pdir))
		return 0;
	uint ui;	
	for(ui = 0; ui < sizeof(pdir.blockids) / sizeof(uint); ui++)
		if(pdir.blockids[ui] == id)
		{
			pdir.blockids[ui] = 0xFFFFFFFF;
			if(!set_block(symbol, pdir_id, (struct RawBlock *)&pdir))
				return 0;
			break;
		}
	return 1;
}

int del_dirs(char * path)
{
	uint count = df_count(path);
	if(count != 0)
	{
		struct RawBlock * subs = (struct RawBlock *)alloc_memory(count * sizeof(struct RawBlock));
		if(subs == NULL)
			return 0;
		df(path, subs);
		uint ui;		
		for(ui = 0; ui < count; ui++)
		{
			struct RawBlock * sub = subs + ui;
			if(sub->type == BLOCK_TYPE_FILE)
			{
				struct FileBlock * file = (struct FileBlock *)sub;
				char temp[MAX_PATH_BUFFER_LEN];
				strcpy(temp, path);
				strcat(temp, file->filename);
				if(!del_file(temp))
					return 0;
			}
			else if(sub->type == BLOCK_TYPE_DIR)
			{
				struct DirBlock * dir = (struct DirBlock *)sub;
				char temp[MAX_PATH_BUFFER_LEN];
				strcpy(temp, path);
				strcat(temp, dir->dirname);
				strcat(temp, "/");
				if(!del_dirs(temp))
					return 0;
			}
			else
				return 0;
		}
		free_memory(subs);
	}	
	return del_dir(path);
}

int is_valid_df_name(char * name)
{
	uint len = strlen(name);
	if(len > MAX_FILENAME_LEN || len == 0)
		return 0;
	uint ui;
	for(ui = 0; ui < len; ui++)
	{
		char chr = name[ui];
		if(!(chr >= 'a' && chr <= 'z') && !(chr >= 'A' && chr <= 'Z') && !(chr >= '0' && chr <= '9') && chr != '.' && chr != '_')
			return 0;
	}
	return 1;
}

int prev_dir(char * path, char * new_path)
{
	strcpy(new_path, path);
	uint len = strlen(new_path);
	if(len > 4)
	{
		new_path[len - 1] = '\0';
		int i;
		for(i = len - 2; i >= 0 && new_path[i] != '/'; i--);
		new_path[i + 1] = '\0';
		return 1;
	}
	else
		return 0;
}

int file_dir(char * path, char * new_path)
{
	strcpy(new_path, path);
	uint len = strlen(new_path);
	if(len > 4)
	{
		int i;
		for(i = len - 2; i >= 0 && new_path[i] != '/'; i--);
		new_path[i + 1] = '\0';
		return 1;
	}
	else
		return 0;
}

int is_valid_path(char * path)
{
	char * s = path;
	int state = 0;
	char chr;
	char name[MAX_FILENAME_BUFFER_LEN];
	char * n = name;
	name[0] = '\0';
	while((chr = *(s++)) != '\0')
		if(state == 0)
			if(!(chr >= 'A' && chr <= 'Z'))
				return 0;
			else
				state = 1;
		else if(state == 1)
			if(!(chr >= 'A' && chr <= 'Z'))
				return 0;
			else
				state = 2;
		else if(state == 2)
			if(chr != ':')
				return 0;
			else
				state = 3;
		else if(state == 3)
			if(chr != '/')
				return 0;
			else
				state = 4;
		else if(state == 4 && chr != '/')
		{
			if(strlen(name) > MAX_FILENAME_LEN)
				return 0;
			*(n++) = chr;
			*n = '\0';
		}
		else if(state == 4 && chr == '/')
		{
			if(!is_valid_df_name(name))
				return 0;
			n = name;
			name[0] = '\0';
		}
	return 1;//state == 3 || (state == 4 && chr == '/');				
}

int rename_dir(char * path, char * new_name)
{
	int type;
	char symbol[3];
	uint id = parse_path(path, symbol, &type);
	return _rename_dir(symbol, id, new_name);
}

int rename_file(char * path, char * new_name)
{
	int type;
	char symbol[3];
	uint id = parse_path(path, symbol, &type);
	return _rename_file(symbol, id, new_name);
}

int fix_path(char * path, char * current_path, char * new_path)
{
	if(strlen(path) == 4 && path[2] == ':' && path[3] == '/')
	{
		strcpy(new_path, path);
		return 1;
	}
	else if(strcmp(path, "/") == 0 && strcmp(current_path, "") != 0)
	{
		strcpy(new_path, current_path);
		new_path[4] = '\0';
		return 1;
	}
	else if(strcmp(current_path, "") != 0 || (strlen(path) > 4 && path[2] == ':' && path[3] == '/'))
	{
		char temp[MAX_PATH_BUFFER_LEN];
		strcpy(temp, current_path);
		if(strlen(path) > 4 && path[2] == ':' && path[3] == '/')
		{
			strncpy(temp, path, 4);
			temp[4] = '\0';
			path += 4;
		}
		else if(path[0] == '/')
		{
			temp[4] = '\0';
			path++;
		}
		char name[MAX_FILENAME_BUFFER_LEN];
		char * n = name;
		name[0] = '\0';
		char chr;
		int processed = 1;
		while((chr = *(path++)) != '\0')
			if(chr != '/')
			{
				if(strlen(name) > MAX_FILENAME_LEN)
					return 0;
				*(n++) = chr;
				*n = '\0';
				processed = 0;
			}
			else
			{
				if(strcmp(name, "..") == 0)
					prev_dir(temp, temp);
				else if(strcmp(name, ".") == 0)
					;
				else
				{
					strcat(temp, name);
					strcat(temp, "/");
				}
				n = name;
				name[0] = '\0';
				processed = 1;
			}
		if(!processed)
			if(strcmp(name, "..") == 0)
				prev_dir(temp, temp);
			else if(strcmp(name, ".") == 0)
				;
			else
				strcat(temp, name);
		strcpy(new_path, temp);
		return 1;
	}
	else
		return 0;
}

int is_sub_dir(char * dir, char * sub_dir)
{
	if(strlen(sub_dir) < strlen(dir))
		return 0;
	char temp[MAX_PATH_BUFFER_LEN];
	strncpy(temp, sub_dir, strlen(dir));
	temp[strlen(dir)] = '\0';
	return strcmp(dir, temp) == 0;
}

int copy_file(char * src_path, char * dst_path, char * dst_name)
{
	char temp[MAX_PATH_BUFFER_LEN];
	strcpy(temp, dst_path);	
	strcat(temp, dst_name);
	if(	!exists_df(src_path)
		|| exists_df(temp)
		|| !create_file(dst_path, dst_name))
		return 0;
	IFS1_FILE * src = IFS1_fopen(src_path, FILE_MODE_READ);
	if(src == NULL)
		return 0;
	IFS1_FILE * dst = IFS1_fopen(temp, FILE_MODE_APPEND);
	if(dst == NULL)
	{
		IFS1_fclose(src);
		return 0;
	}
	uint len = flen(src);
	uchar buffer[1024];
	while(len > 0)
	{
		uint copy_len;
		if(len >= 1024)
			copy_len = 1024;
		else
			copy_len = len;
		if(!IFS1_fread(src, buffer, copy_len))
		{
			IFS1_fclose(src);
			IFS1_fclose(dst);
			return 0;
		}
		if(!IFS1_fappend(dst, buffer, copy_len))
		{
			IFS1_fclose(src);
			IFS1_fclose(dst);
			return 0;
		}
		len -= copy_len;
	}
	IFS1_fclose(src);
	IFS1_fclose(dst);
	return 1;
}

int exists_dir(char * path, char * name)
{
	int type;
	char symbol[3];
	uint id = parse_path(path, symbol, &type);
	return _exists_dir(symbol, id, name);
}

int exists_file(char * path, char * name)
{
	int type;
	char symbol[3];
	uint id = parse_path(path, symbol, &type);
	return _exists_file(symbol, id, name);
}

int exists_df(char * path)
{
	if(!is_valid_path(path))
		return 0;
	uint len = strlen(path);
	if(len == 4)
	{
		char symbol[3];
		memcpy(symbol, path, 2);
		symbol[2] = '\0';
		return check_disk(symbol);
	}
	else if(path[len - 1] == '/')
	{
		char temp[MAX_PATH_BUFFER_LEN];
		char name[MAX_DIRNAME_BUFFER_LEN];
		int i;
		strcpy(temp, path);
		temp[len - 1] = '\0';
		len--;
		for(i = len - 1; i >= 0 && temp[i] != '/'; i--);
		strcpy(name, temp + i + 1);
		temp[i + 1] = '\0';
		return exists_dir(temp, name);
	}
	else
	{
		char temp[MAX_PATH_BUFFER_LEN];
		char name[MAX_FILENAME_BUFFER_LEN];
		int i;
		strcpy(temp, path);
		for(i = len - 1; i >= 0 && path[i] != '/'; i--);
		temp[i + 1] = '\0';
		strcpy(name, path + i + 1);
		return exists_file(temp, name);
	}
}

IFS1_FILE * IFS1_fopen(char * path, int mode)
{
	IFS1_FILE * fptr = (IFS1_FILE *)alloc_memory(sizeof(IFS1_FILE));
	if(fptr == NULL)
		return NULL;
	int type;
	char symbol[3];
	uint id = parse_path(path, symbol, &type);
	if(type != BLOCK_TYPE_FILE || id == 0xFFFFFFFF)
		return NULL;
	fptr->mode = mode;
	strcpy(fptr->symbol, symbol);
	fptr->file_block_id = id;
	fptr->next_block_index = 0;
	fptr->next_block_pos = 0;
	struct FileBlock * file_block = (struct FileBlock *)alloc_memory(sizeof(struct FileBlock));	
	if(file_block == NULL)
	{
		free_memory(fptr);
		return NULL;
	}
	if(!get_block(symbol, id, (struct RawBlock *)file_block) || file_block->lock)
	{
		free_memory(fptr->file_block);
		free_memory(fptr);
		return NULL;
	}
	file_block->lock = 1;
	if(!set_block(symbol, id, (struct RawBlock *)file_block))
	{
		free_memory(fptr->file_block);
		free_memory(fptr);
		return NULL;
	}
	fptr->file_block = file_block;
	return fptr;
}

int IFS1_fclose(IFS1_FILE * fptr)
{
	int r = 1;
	fptr->file_block->lock = 0;
	if(!set_block(fptr->symbol, fptr->file_block_id, (struct RawBlock *)(fptr->file_block)))
		r = 0;
	free_memory(fptr->file_block);
	free_memory(fptr);
	return r;
}

int IFS1_fwrite(IFS1_FILE * fptr, uchar * buffer, uint len)
{
	if(fptr->mode & FILE_MODE_WRITE == 0 || len > MAX_FILE_LEN)
		return 0;
	uint ui;
	for(ui = 0; ui < sizeof(fptr->file_block->blockids) / sizeof(uint); ui++)
	{
		uint block_id = fptr->file_block->blockids[ui];
		if(block_id != 0xFFFFFFFF)
		{
			struct DataBlock data_block;
			data_block.used = 0;
			if(!set_block(fptr->symbol, block_id, (struct RawBlock *)&data_block))
				return 0;
			fptr->file_block->blockids[ui] = 0xFFFFFFFF;
		}
	}
	int i;
	uint index = 0;
	for(i = len; i > 0; i -= DATA_BLOCK_DATA_LEN)
	{
		struct DataBlock data_block;
		data_block.used = 1;
		data_block.type = BLOCK_TYPE_DATA;
		if(i >= DATA_BLOCK_DATA_LEN)
			data_block.length = DATA_BLOCK_DATA_LEN;
		else
			data_block.length = i;
		for(ui = 0; ui < sizeof(data_block.reserve); ui++)
			data_block.reserve[ui] = 0;
		memcpy(data_block.data, buffer, data_block.length);
		buffer += data_block.length;
		uint data_block_id = add_block(fptr->symbol, (struct RawBlock *)&data_block);
		fptr->file_block->blockids[index++] = data_block_id;
	}
	fptr->file_block->length = len;
	get_cmos_date_time(&(fptr->file_block->change));
	return set_block(fptr->symbol, fptr->file_block_id, (struct RawBlock *)fptr->file_block);
}

uint IFS1_fread(IFS1_FILE * fptr, uchar * buffer, uint len)
{
	if(fptr->mode & FILE_MODE_READ == 0 && len != 0)
		return 0;
	uint real_len = 0;
	uint block_index = fptr->next_block_index;
	struct DataBlock data_block;
	if(!get_block(fptr->symbol, fptr->file_block->blockids[block_index], (struct RawBlock *)&data_block))
		return 0;
	while(len > 0 && fptr->next_block_index * DATA_BLOCK_DATA_LEN + fptr->next_block_pos < fptr->file_block->length)
	{
		if(block_index != fptr->next_block_index)
		{
			block_index = fptr->next_block_index;
			if(!get_block(fptr->symbol, fptr->file_block->blockids[block_index], (struct RawBlock *)&data_block))
				break;
		}
		*(buffer++) = data_block.data[fptr->next_block_pos++];
		if(fptr->next_block_pos == DATA_BLOCK_DATA_LEN)
		{
			fptr->next_block_index++;
			fptr->next_block_pos = 0;
		}
		len--;
		real_len++;
	}
	return real_len;
}

void IFS1_freset(IFS1_FILE * fptr)
{
	fptr->next_block_index = 0;
	fptr->next_block_pos = 0;
}
/*
int test_fappend(void)
{
	if(!exists_df("DA:/test/"))
		if(!create_dir("DA:/", "test"))
		{
			print_str("Cannot create 'DA:/test/'");
			return 0;
		}
	if(exists_df("DA:/test/fappend.test"))
		if(!del_file("DA:/test/fappend.test"))
		{
			print_str("Cannot delete 'DA:/test/fappend.test'");
			return 0;
		}
	if(!create_file("DA:/test/", "fappend.test"))
	{
		print_str("Cannot create 'DA:/test/fappend.test'");
		return 0;
	}
	FILE * fptr = fopen("DA:/test/fappend.test", FILE_MODE_ALL);
	if(fptr == NULL)
	{
		print_str("Cannot open 'DA:/test/fappend.test'");
		return 0;
	}
	uint ui, ui1;
	uint numbers[256];
	uint sum = 0;
	for(ui = 0; ui < sizeof(numbers) / sizeof(uint); ui++)
	{
		sum += ui;
		numbers[ui] = ui;
	}
	for(ui = 0; ui < 1000; ui++)
		if(!fappend(fptr, (uchar *)numbers, sizeof(numbers)))
		{
			print_str("Cannot append 'DA:/test/fappend.test'");
			fclose(fptr);
			return 0;
		}
	fclose(fptr);
	fptr = fopen("DA:/test/fappend.test", FILE_MODE_ALL);
	if(fptr == NULL)
	{
		print_str("Cannot open 'DA:/test/fappend.test'");
		return 0;
	}
	for(ui = 0; ui < 1000; ui++)
	{
		for(ui1 = 0; ui1 < sizeof(numbers) / sizeof(uint); ui1++)
			numbers[ui1] = 0;
		if(!fread(fptr, (uchar *)numbers, sizeof(numbers)))
		{
			print_str("Cannot read 'DA:/test/fappend.test'");
			return 0;
		}
		uint result = 0;
		for(ui1 = 0; ui1 < sizeof(numbers) / sizeof(uint); ui1++)
			result += numbers[ui1];
		if(sum == result)
			print_str("OK!");
		else
		{
			print_str("ERROR!");
			return 0;
		}
	}
	fclose(fptr);
	//del_file("DA:/test/fappend.test");
	return 1;
}
*/
int IFS1_fappend(IFS1_FILE * fptr, uchar * buffer, uint len)
{
	if(fptr->mode & FILE_MODE_APPEND == 0 || fptr->file_block->length + len > MAX_FILE_LEN)
		return 0;
	uint append_len = len;	
	uint block_index = fptr->file_block->length / DATA_BLOCK_DATA_LEN;
	uint cblock_index = block_index;
	uint block_pos = fptr->file_block->length % DATA_BLOCK_DATA_LEN;
	struct DataBlock data_block;
	//第一个要写入的位置是不是刚好位于下一个Data Block的开始. 如果是则先新建好下一个Data Block.
	//因为下一个Data Block不存在.
	if(block_pos == 0)
	{
		data_block.used = 1;
		data_block.type = BLOCK_TYPE_DATA;
		data_block.length = 0;
		uint new_block_id = add_block(fptr->symbol, (struct RawBlock *)&data_block);
		if(new_block_id == 0xFFFFFFFF)
			return 0;	
		fptr->file_block->blockids[block_index] = new_block_id;
	}

	//获取下一个要写入的Data Block.
	if(!get_block(fptr->symbol, fptr->file_block->blockids[cblock_index], (struct RawBlock *)&data_block))
		return 0;
	
	int is_save = 0;	//Data Block是否已保存.
	int is_next = 0;	//写入位置是否移动到了下一个Data Block.
	while(len > 0)
	{
		//检测是否下一个写入的位置再下一个Data Block.
		if(cblock_index != block_index)
		{
			//检测上一个写完的Data Block是否已经存在.
			if((cblock_index - 1) * DATA_BLOCK_DATA_LEN < fptr->file_block->length)
			{
				//存在. 直接保存.
				if(!set_block(fptr->symbol, fptr->file_block->blockids[cblock_index - 1], (struct RawBlock *)&data_block))
					return 0;
			}
			else
			{
				//不存在. 新建一个Data Block并追加到File Block里.
				data_block.used = 1;
				data_block.type = BLOCK_TYPE_DATA;
				uint new_block_id = add_block(fptr->symbol, (struct RawBlock *)&data_block);
				if(new_block_id == 0xFFFFFFFF)
					return 0;
				fptr->file_block->blockids[cblock_index - 1] = new_block_id;
			}
			is_save = 1;
			data_block.length = 0;
			block_pos = 0;
			block_index++;
		}
		data_block.data[block_pos++] = *(buffer++);
		data_block.length++;
		len--;
		is_save = 0;
		is_next = 0;
		//检查一个Data Block是否已写满
		if(block_pos == DATA_BLOCK_DATA_LEN)
		{
			cblock_index++;
			is_next == 1;
		}
	}
	//检查是否未保存Data Block.
	if(!is_save)
	{
		//获取未保存的Data Block的ID.
		uint id = fptr->file_block->blockids[(is_next ? cblock_index - 1 : cblock_index)];
		
		//检查未保存Data Block是否存在.
		if(id != 0xFFFFFFFF)
		{
			if(!set_block(fptr->symbol, id, (struct RawBlock *)&data_block))
				return 0;
		}
		else
		{
			id = add_block(fptr->symbol, (struct RawBlock *)&data_block);
			if(id == 0xFFFFFFFF)
				return 0;
			fptr->file_block->blockids[(is_next ? cblock_index - 1 : cblock_index)] = id;
		}
	}
	fptr->file_block->length += append_len;
	get_cmos_date_time(&(fptr->file_block->change));
	return set_block(fptr->symbol, fptr->file_block_id, (struct RawBlock *)(fptr->file_block));
}

