/**
	@File:			file_list.c
	@Author:		Ihsoh
	@Date:			2020-05-04
	@Description:
		文件列表控件 - File List。
*/

#include "file_list.h"
#include "control.h"
#include "list.h"
#include "../types.h"
#include "../image.h"
#include "../enfont.h"
#include "../klib.h"
#include "../window.h"
#include "../kernel.h"
#include "../disk.h"
#include "../memory.h"

#include "../utils/sfstr.h"

#include "../../libs/dslib/list.h"
#include "../../libs/dslib/linked_list.h"

#include <ilib/string.h>

/**
	@Function:		_Event
	@Access:		Private
	@Description:
		List的事件函数。
	@Parameters:
		id, uint32, IN
			控件ID。
		type, uint32, IN
			事件类型。
		params, void *, IN
			指向参数的指针。为NULL时表示没有参数。
	@Return:
*/
static
void
_Event(	IN uint32 id,
		IN uint32 type,
		IN void * param)
{
	if(!IS_CONTROL_ID(id))
	{
		ListPtr list = (ListPtr)id;
		FileListPtr file_list = (FileListPtr)list->vpext;
		if(file_list == NULL)
			return;
		ControlEvent event = file_list->event;
		if(event == NULL)
			return;
		uint32 list_item_index = *(uint32*)param;
		uint32 index = file_list->top + list_item_index;
		if(index >= file_list->count)
			return;
		event((uint32)file_list, type, file_list->items + index);
	}
}

BOOL
CtrlFileListInit(	OUT FileListPtr file_list,
					IN uint32 id,
					IN uint32 max_count,
					IN uint32 x,
					IN uint32 y,
					IN uint32 color,
					IN uint32 bgcolor,
					IN uint32 colorh,
					IN uint32 bgcolorh,
					IN ControlEvent event)
{
	if(file_list == NULL)
		return FALSE;
	if(id == 0)
		file_list->id = (uint32)file_list;
	else
		file_list->id = id;
	file_list->type = CONTROL_FILE_LIST;
	file_list->uwid = -1;
	file_list->uwcid = -1;
	file_list->x = x;
	file_list->y = y;
	file_list->max_count = max_count;
	UtlCopyString(
		file_list->path,
		sizeof(file_list->path),
		""
	);
	file_list->items = NULL;
	file_list->count = 0;
	file_list->top = 0;
	file_list->event = event;

	CtrlListInit(
		&(file_list->list),
		&(file_list->list),
		max_count,
		x, y,
		"###",
		color, bgcolor,
		colorh, bgcolorh,
		_Event
	);

	file_list->list.vpext = file_list;
}

BOOL
CtrlFileListRelease(IN OUT FileListPtr file_list)
{
	if(file_list == NULL)
		return FALSE;
	if(file_list->items != NULL)
	{
		MemFree(file_list->items);
		file_list->items = NULL;
	}
	return TRUE;
}

BOOL
CtrlFileListUpdate(	IN OUT FileListPtr file_list,
					OUT ImagePtr image,
					IN WindowEventParamsPtr params,
					BOOL top)
{
	if(file_list == NULL || image == NULL || params == NULL)
		return FALSE;
	if(!top)
		return TRUE;
	return LIST(&(file_list->list), image, params, top);
}

BOOL
CtrlFileListSetPath(
	IN OUT FileListPtr file_list,
	IN int8 * path)
{
	if(file_list == NULL
		|| path == NULL)
		return FALSE;

	DSLLinkedList lnklst;
	dsl_lnklst_init(&lnklst);
	int32 item_count = Ifs1GetItemList(path, &lnklst);
	if(item_count == -1)
		return FALSE;
	file_list->count = item_count;

	file_list->top = 0;
	UtlCopyString(
		file_list->path,
		sizeof(file_list->path),
		path
	);

	if(file_list->items != NULL)
	{
		MemFree(file_list->items);
		file_list->items = NULL;
	}
	file_list->items = (FileListItemPtr)MemAlloc(item_count * sizeof(FileListItem));

	DSLLinkedListNodePtr item = lnklst.head;
	int i = 0;
	while(item)
	{
		FileListItemPtr file_list_item = file_list->items + i++;

		struct RawBlock * block = item->value.value.object_value;
		if(block->type == BLOCK_TYPE_FILE)
		{
			struct FileBlock * file = (struct FileBlock *)block;
			UtlCopyString(
				file_list_item->name,
				sizeof(file_list_item->name),
				file->filename
			);
			file_list_item->type = FILE_LIST_ITEM_TYPE_FILE;
		}
		else if(block->type == BLOCK_TYPE_DIR)
		{
			struct DirBlock * dir = (struct DirBlock *)block;
			UtlCopyString(
				file_list_item->name,
				sizeof(file_list_item->name),
				dir->dirname
			);
			file_list_item->type = FILE_LIST_ITEM_TYPE_DIR;
		}
		else if(block->type == BLOCK_TYPE_SLINK)
		{
			struct SLinkBlock * slink = (struct SLinkBlock *)block;
			UtlCopyString(
				file_list_item->name,
				sizeof(file_list_item->name),
				slink->filename
			);
			file_list_item->type = FILE_LIST_ITEM_TYPE_SLINK;
		}
		else
		{
			UtlCopyString(
				file_list_item->name,
				sizeof(file_list_item->name),
				"????"
			);
			file_list_item->type = FILE_LIST_ITEM_TYPE_UNKNOWN;
		}

		item = item->next;
	}

	dsl_lnklst_delete_all_object_node(&lnklst);
	dsl_lnklst_release(&lnklst);
}

BOOL
CtrlFileListSetTop(
	IN OUT FileListPtr file_list,
	IN uint32 top)
{
	if(file_list == NULL
		|| top >= file_list->count)
		return FALSE;

	file_list->top = top;

	uint32 i;
	for (i = 0; i < file_list->max_count; i++)
	{
		uint32 index = file_list->top + i;
		if (index < file_list->count)
		{
			int8 line[256];

			FileListItemPtr file_list_item = file_list->items + index;
			switch (file_list_item->type)
			{
				case FILE_LIST_ITEM_TYPE_FILE:
				{
					UtlCopyString(line, sizeof(line), "<FILE> ");
					break;
				}
				case FILE_LIST_ITEM_TYPE_DIR:
				{
					UtlCopyString(line, sizeof(line), "<DIR> ");
					break;
				}
				case FILE_LIST_ITEM_TYPE_SLINK:
				{
					UtlCopyString(line, sizeof(line), "<SLINK> ");
					break;
				}
				default:
				{
					UtlCopyString(line, sizeof(line), "<????> ");
					break;
				}
			}

			UtlConcatString(line, sizeof(line), file_list_item->name);

			CtrlListSetText(&(file_list->list), i, line);
			CtrlListEnableItem(&(file_list->list), i);
		}
		else
		{
			CtrlListSetText(&(file_list->list), i, "");
			CtrlListDisableItem(&(file_list->list), i);
		}
		
	}
}
