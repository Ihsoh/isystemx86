/**
	@File:			madt.c
	@Author:		Ihsoh
	@Date:			2015-03-10
	@Description:
		MADT。
*/

#include "madt.h"
#include "types.h"
#include "rsdt.h"
#include "madt.h"
#include "memory.h"

#include "fs/ifs1/fs.h"

#define	MAX_MADT_ENTRY_COUNT	32

static struct MADT * madt = NULL;
static uint32 type0_count = 0;
static uint32 type1_count = 0;
static uint32 type2_count = 0;
static struct MADTEntryType0 type0_entries[MAX_MADT_ENTRY_COUNT];
static struct MADTEntryType1 type1_entries[MAX_MADT_ENTRY_COUNT];
static struct MADTEntryType2 type2_entries[MAX_MADT_ENTRY_COUNT];

/**
	@Function:		release_resource
	@Access:		Private
	@Description:
		释放该代码文件模块所占用资源。
	@Parameters:
	@Return:
*/
static
void
release_resource(void)
{
	if(madt != NULL)
	{
		free_memory(madt);
		madt = NULL;
	}
	type0_count = 0;
	type1_count = 0;
	type2_count = 0;
}

/**
	@Function:		madt_init
	@Access:		Public
	@Description:
		初始化 MADT 模块。该模块会读取 MADT(名称为"APIC")。
		MADT 包括：
		  1. Processor Local APIC 的 Entry。
		  2. I/O APIC 的 Entry。
		  3. Interrupt Source Override 的 Entry。
	@Parameters:
	@Return:
		BOOL
			返回 TRUE 则成功，否则失败。
*/
BOOL
madt_init(void)
{
	if(!rsdt_init())
		return FALSE;
	struct ACPISDTHeader * madt_header = rsdt_find_sdt("APIC");
	if(madt_header == NULL)
		return FALSE;
	madt = (struct MADT *)alloc_memory(madt_header->length);
	if(madt == NULL)
		return FALSE;
	memcpy(madt, madt_header, madt_header->length);
	struct MADTEntryHeader * eh = &(madt->first_entry_header);
	int32 len = madt->header.length - sizeof(struct ACPISDTHeader) - 2 * 4;
	while(len > 0)
	{
		switch(eh->type)
		{
			case 0:
				if(type0_count == MAX_MADT_ENTRY_COUNT)
				{
					release_resource();
					return FALSE;
				}
				memcpy(	type0_entries + type0_count, 
						(uint8 *)eh + 2, 
						sizeof(struct MADTEntryType0));
				type0_count++;
				break;
			case 1:
				if(type1_count == MAX_MADT_ENTRY_COUNT)
				{
					release_resource();
					return FALSE;
				}
				memcpy(	type1_entries + type1_count, 
						(uint8 *)eh + 2, 
						sizeof(struct MADTEntryType1));
				type1_count++;
				break;
			case 2:
				if(type2_count == MAX_MADT_ENTRY_COUNT)
				{
					release_resource();
					return FALSE;
				}
				memcpy(	type2_entries + type2_count, 
						(uint8 *)eh + 2, 
						sizeof(struct MADTEntryType2));
				type2_count++;
				break;
			default:
				release_resource();
				return FALSE;
		}
		len -= eh->length;
		eh = (uint8 *)eh + eh->length;
	}
	if(len < 0)
	{
		release_resource();
		return FALSE;
	}
	return TRUE;
}

/**
	@Function:		madt_get_type0_count
	@Access:		Public
	@Description:
		获取 Processor Local APIC 的 Entry 的数量。
	@Parameters:
	@Return:
		BOOL
			Processor Local APIC 的 Entry 的数量。
*/
uint32
madt_get_type0_count(void)
{
	if(madt == NULL)
		return 0;
	return type0_count;
}

/**
	@Function:		madt_get_type1_count
	@Access:		Public
	@Description:
		获取 I/O APIC 的 Entry 的数量。
	@Parameters:
	@Return:
		BOOL
			I/O APIC 的 Entry 的数量。
*/
uint32
madt_get_type1_count(void)
{
	if(madt == NULL)
		return 0;
	return type1_count;
}

/**
	@Function:		madt_get_type2_count
	@Access:		Public
	@Description:
		获取 Interrupt Source Override 的 Entry 的数量。
	@Parameters:
	@Return:
		BOOL
			Interrupt Source Override 的 Entry 的数量。
*/
uint32
madt_get_type2_count(void)
{
	if(madt == NULL)
		return 0;
	return type2_count;
}

/**
	@Function:		madt_get_type0_entry
	@Access:		Public
	@Description:
		通过索引获取指定的 Processor Local APIC 的 Entry。
	@Parameters:
		index, uint32, IN
			索引。
	@Return:
		struct MADTEntryType0 *
			Processor Local APIC 的 Entry。
*/
struct MADTEntryType0 *
madt_get_type0_entry(IN uint32 index)
{
	if(madt == NULL || index >= type0_count)
		return NULL;
	return type0_entries + index;
}

/**
	@Function:		madt_get_type1_entry
	@Access:		Public
	@Description:
		通过索引获取指定的 I/O APIC 的 Entry。
	@Parameters:
		index, uint32, IN
			索引。
	@Return:
		struct MADTEntryType1 *
			I/O APIC 的 Entry。
*/
struct MADTEntryType1 *
madt_get_type1_entry(IN uint32 index)
{
	if(madt == NULL || index >= type1_count)
		return NULL;
	return type1_entries + index;
}

/**
	@Function:		madt_get_type2_entry
	@Access:		Public
	@Description:
		通过索引获取指定的 Interrupt Source Override 的 Entry。
	@Parameters:
		index, uint32, IN
			索引。
	@Return:
		struct MADTEntryType2 *
			Interrupt Source Override 的 Entry。
*/
struct MADTEntryType2 *
madt_get_type2_entry(IN uint32 index)
{
	if(madt == NULL || index >= type2_count)
		return NULL;
	return type2_entries + index;
}

/**
	@Function:		madt_get
	@Access:		Public
	@Description:
		获取 MADT。
	@Parameters:
	@Return:
		struct MADT *
			MADT。
*/
struct MADT *
madt_get(void)
{
	return madt;
}

/**
	@Function:		madt_write_to_file
	@Access:		Public
	@Description:
		把 MADT 保存到指定文件。
	@Parameters:
		path, const int8 *, IN
			文件路径。
	@Return:
		BOOL
			返回 TRUE 则成功，否则失败。
*/
BOOL
madt_write_to_file(IN const int8 * path)
{
	if(madt == NULL)
		return FALSE;
	FileObject * fptr = Ifs1OpenFile(path, FILE_MODE_WRITE | FILE_MODE_APPEND);
	if(fptr == NULL)
		return FALSE;
	Ifs1WriteFile(fptr, "", 0);
	int8 buffer[1024];
	uint32 ui;
	uint32 count = madt_get_type0_count();
	for(ui = 0; ui < count; ui++)
	{
		struct MADTEntryType0 * e = madt_get_type0_entry(ui);
		sprintf_s(	buffer, 
					sizeof(buffer), 
					"Type0Entry: %d, %d, %d\n",
					e->acpi_processor_id,
					e->apic_id,
					e->flags);
		Ifs1AppendFile(fptr, buffer, strlen(buffer));
	}
	count = madt_get_type1_count();
	for(ui = 0; ui < count; ui++)
	{
		struct MADTEntryType1 * e = madt_get_type1_entry(ui);
		sprintf_s(	buffer, 
					sizeof(buffer), 
					"Type1Entry: %d, %X, %X\n",
					e->ioapic_id,
					e->ioapic_address,
					e->global_sys_int_base);
		Ifs1AppendFile(fptr, buffer, strlen(buffer));
	}
	count = madt_get_type2_count();
	for(ui = 0; ui < count; ui++)
	{
		struct MADTEntryType2 * e = madt_get_type2_entry(ui);
		sprintf_s(	buffer, 
					sizeof(buffer), 
					"Type2Entry: %d, %d, %X, %X\n",
					e->bus_source,
					e->irq_source,
					e->global_sys_int,
					e->flags);
		Ifs1AppendFile(fptr, buffer, strlen(buffer));
	}
	Ifs1CloseFile(fptr);
	return TRUE;
}
