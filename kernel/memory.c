/**
	@File:			memory.c
	@Author:		Ihsoh
	@Date:			2014-6-7
	@Description:
		提供内存操作功能。
*/

#include "memory.h"
#include "types.h"
#include "kfuncs.h"
#include "tasks.h"
#include "lock.h"
#include "log.h"

static uint32 memory_limit = 0xFFFFFFFF;
static uint64 used = 0x1000000;
static struct MemoryBlockDescriptor * mbdt_head = NULL;		//Unused
static struct MemoryBlockDescriptor * mbdt_foot = NULL;
static struct MemoryBlockDescriptor * umbdt_head = NULL;	//Used
static struct MemoryBlockDescriptor * umbdt_foot = NULL;

DEFINE_LOCK_IMPL(memory)

/**
	@Function:		copy_memory
	@Access:		Public
	@Description:
		复制内存中的数据。
	@Parameters:
		src, uint8 *, IN
			源地址。
		dst, uint8 *, OUT
			目标地址。
		length, uint32, IN
			复制长度。
	@Return:		
*/
void 
copy_memory(IN uint8 * src,
			OUT uint8 * dst,
			IN uint32 length)
{
	uint32 ui;	
	for(ui = 0; ui < length; ui++)
		dst[ui] = src[ui];
}

/**
	@Function:		clear_memory
	@Access:		Public
	@Description:
		清空内存中的数据。
	@Parameters:
		dst, uint8 *, OUT
			目标地址。
		data, uint8, IN
			填充的数据。
		len, uint32, IN
			长度。
	@Return:	
*/
void 
clear_memory(	OUT uint8 * dst,
				IN uint8 data,
				IN uint32 len)
{
	uint32 ui;
	for(ui = 0; ui < len; ui++)
		dst[ui] = data;
}

/**
	@Function:		get_used_memory_length
	@Access:		Public
	@Description:
		获取已使用内存的长度，单位为Byte。
	@Parameters:
	@Return:
		uint64
			已使用的内存的长度。
*/
uint64 
get_used_memory_length(void)
{
	return used;
}

/**
	@Function:		get_unused_mbd
	@Access:		Private
	@Description:
		获取一个未被使用的内存块描述符。
	@Parameters:
		mbdt, struct MemoryBlockDescriptor *, IN
			未分配内存块描述符表。
		mbd_count, uint32, IN
			未分配内存块描述符表中内存块描述符的总数。
	@Return:
		struct MemoryBlockDescriptor *
			未被使用的内存块描述符。		
*/
static 
struct MemoryBlockDescriptor * 
get_unused_mbd(	IN struct MemoryBlockDescriptor * mbdt, 
				IN uint32 mbd_count)
{
	uint32 ui;
	for(ui = 0; ui < mbd_count; ui++)
		if(mbdt[ui].used == 0)
			return mbdt + ui;
	return NULL;
}

/**
	@Function:		append_mbd
	@Access:		Private
	@Description:
		向内存块描述符表结尾添加一个内存块描述符。
	@Parameters:
		head, struct MemoryBlockDescriptor **, IN OUT
			内存块描述符表头。
		foot, struct MemoryBlockDescriptor **, IN OUT
			内存块描述符表尾。
		mbd, struct MemoryBlockDescriptor *, IN OUT
			内存块描述符。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。		
*/
static
BOOL
append_mbd(	IN OUT struct MemoryBlockDescriptor ** head, 
			IN OUT struct MemoryBlockDescriptor ** foot,
			IN OUT struct MemoryBlockDescriptor * mbd)
{
	if(*head == NULL && *foot == NULL)
	{
		*head = mbd;
		*foot = mbd;
		mbd->prev = NULL;
		mbd->next = NULL;
		return TRUE;
	}
	else if(*head != NULL && *foot != NULL)
	{
		(*foot)->next = mbd;
		mbd->prev = *foot;
		mbd->next = NULL;
		*foot = mbd;
		return TRUE;
	}
	else
		return FALSE;
}

/**
	@Function:		remove_mbd
	@Access:		Private
	@Description:
		移除内存块描述符表里的一个内存块描述符。
	@Parameters:
		head, struct MemoryBlockDescriptor **, IN OUT
			内存块描述符表头。
		foot, struct MemoryBlockDescriptor **, IN OUT
			内存块描述符表尾。
		mbd, struct MemoryBlockDescriptor *, IN OUT
			内存块描述符。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。		
*/
static 
BOOL
remove_mbd(	IN OUT struct MemoryBlockDescriptor ** head, 
			IN OUT struct MemoryBlockDescriptor ** foot,
			IN OUT struct MemoryBlockDescriptor * mbd)
{
	if(*head == mbd && *foot == mbd)
	{
		*head = NULL;
		*foot = NULL;
		return TRUE;
	}
	else if(*head == mbd && *foot != mbd)
	{
		mbd->next->prev = NULL;
		*head = mbd->next;
		return TRUE;
	}
	else if(*head != mbd && *foot == mbd)
	{
		mbd->prev->next = NULL;
		*foot = mbd->prev;	
		return TRUE;
	}
	else if(*head != mbd && *foot != mbd)
	{
		mbd->prev->next = mbd->next;
		mbd->next->prev = mbd->prev;
		return TRUE;
	}
	else
		return FALSE;
}

/**
	@Function:		init_memory
	@Access:		Public
	@Description:
		初始化内存。
	@Parameters:
	@Return:	
*/
void 
init_memory(void)
{
	uint32 ui;
	uint64 size = get_memory_size();
	memory_limit = (uint32)(size - 1);
	clear_memory((uchar *)MBDT_ADDRESS, 0, MBDT_LENGTH);
	clear_memory((uchar *)UMBDT_ADDRESS, 0, UMBDT_LENGTH);
	struct MemoryBlockDescriptor * mbd = get_unused_mbd(MBDT_ADDRESS, MBD_COUNT);
	mbd->used = 1;
	mbd->start = ALLOC_START_ADDRESS;
	mbd->length = memory_limit - ALLOC_START_ADDRESS + 1;
	append_mbd(&mbdt_head, &mbdt_foot, mbd);
}

/**
	@Function:		get_mbdt_info
	@Access:		Public
	@Description:
		获取未被使用的内存块描述符表的信息。
	@Parameters:
		total_mbd_count, uint32 *, OUT
			未被使用的内存块描述符总数。
		used_mbd_count, uint32 *, OUT
			未被使用的内存块描述符数量。
		m_size, uint32 *, OUT
			未被使用的内存大小。
	@Return:
*/
void
get_mbdt_info(	OUT uint32 * total_mbd_count,
				OUT uint32 * used_mbd_count,
				OUT uint32 * m_size)
{
	*total_mbd_count = MBD_COUNT;
	*used_mbd_count = 0;
	*m_size = 0;
	uint32 ui;
	for(ui = 0; ui < MBD_COUNT; ui++)
		if(((struct MemoryBlockDescriptor *)MBDT_ADDRESS)[ui].used)
		{
			*m_size += ((struct MemoryBlockDescriptor *)MBDT_ADDRESS)[ui].length;
			(*used_mbd_count)++;
		}
}

/**
	@Function:		get_umbdt_info
	@Access:		Public
	@Description:
		获取正在被使用的内存块描述符表的信息。
	@Parameters:
		total_mbd_count, uint32 *, OUT
			正在被使用的内存块描述符总数。
		used_mbd_count, uint32 *, OUT
			正在被使用的内存块描述符数量。
		m_size, uint32 *, OUT
			正在被使用的内存大小。
	@Return:
*/
void
get_umbdt_info(	OUT uint32 * total_umbd_count,
				OUT uint32 * used_umbd_count,
				OUT uint32 * m_size)
{
	*total_umbd_count = UMBD_COUNT;
	*used_umbd_count = 0;
	*m_size = ALLOC_START_ADDRESS;
	uint32 ui;
	for(ui = 0; ui < UMBD_COUNT; ui++)
		if(((struct MemoryBlockDescriptor *)UMBDT_ADDRESS)[ui].used)
		{
			*m_size += ((struct MemoryBlockDescriptor *)UMBDT_ADDRESS)[ui].length;
			(*used_umbd_count)++;
		}
}

/**
	@Function:		align_4kb
	@Access:		Public
	@Description:
		使一个值 4KB 对齐。
	@Parameters:
		n, uint32, IN
			值。
	@Return:
		uint32
			4KB 对齐之后的值。		
*/
uint32
align_4kb(IN uint32 n)
{
	if(n % KB(4) == 0)
		return n;
	else
		return n + (KB(4) - n % KB(4));
}

/**
	@Function:		_alloc_memory_with_start
	@Access:		Private
	@Description:
		分配一段内存。该内存的起始地址必须大于一个指定的地址
		或等于该指定的地址。
	@Parameters:
		m_start, uint32, IN
			指定的地址。
		length, uint32, IN
			长度。
	@Return:
		void *
			分配的内存的地址。返回 NULL 则失败。		
*/
static
void *
_alloc_memory_with_start(	IN uint32 m_start,
							IN uint32 length)
{
	//让需要分配的内存长度4KB对齐.
	//因为一直分配的内存的长度都是4KB对齐
	//并且第一个未分配内存块的起始地址也是4KB对齐的,
	//所以分配得到的内存地址也是4KB对齐的.
	length = align_4kb(length);

	struct MemoryBlockDescriptor * mbd = mbdt_foot;
	while(mbd != NULL)
		if(mbd->start < m_start && m_start + length < mbd->start + mbd->length)
		{
			struct MemoryBlockDescriptor * umbd = get_unused_mbd(UMBDT_ADDRESS, UMBD_COUNT);
			if(umbd == NULL)
				return NULL;
			uint32 address = mbd->start + mbd->length - length;
			umbd->used = 1;
			umbd->start = address;
			umbd->length = length;
			mbd->length -= length;
			if(!append_mbd(&umbdt_head, &umbdt_foot, umbd))
				return NULL;
			else
			{
				used += length;
				return address;			
			}
		}
		else if(m_start <= mbd->start && length <= mbd->length)
		{
			uint32 address;
			struct MemoryBlockDescriptor * umbd = get_unused_mbd(UMBDT_ADDRESS, MBD_COUNT);
			if(umbd == NULL)
				return NULL;
			address = (uint)(mbd->start);
			umbd->used = 1;
			umbd->start = mbd->start;
			umbd->length = length;
			mbd->length -= length;
			mbd->start += length;
			if(mbd->length == 0)
			{
				mbd->used = 0;
				if(!remove_mbd(&mbdt_head, &mbdt_foot, mbd))
					return NULL;
			}			
			if(!append_mbd(&umbdt_head, &umbdt_foot, umbd))
				return NULL;
			else
			{
				used += length;
				return address;
			}
		}
		else
			mbd = mbd->prev;
	return NULL;
}

/**
	@Function:		alloc_memory_with_start
	@Access:		Public
	@Description:
		分配一段内存。该内存的起始地址必须大于一个指定的地址
		或等于该指定的地址。
	@Parameters:
		m_start, uint32, IN
			指定的地址。
		length, uint32, IN
			长度。
	@Return:
		void *
			分配的内存的地址。返回 NULL 则失败。
*/
void *
alloc_memory_with_start(IN uint32 m_start,
						IN uint32 length)
{
	lock();
	void * r = _alloc_memory_with_start(m_start, length);
	unlock();
	return r;
}

/**
	@Function:		_alloc_memory
	@Access:		Private
	@Description:
		分配一段内存。
	@Parameters:
		length, uint32, IN
			长度。
	@Return:
		void *
			分配的内存的地址。返回 NULL 则失败。	
*/
static
void * 
_alloc_memory(uint length)
{
	return _alloc_memory_with_start(ALLOC_START_ADDRESS, length);
}

/**
	@Function:		alloc_memory
	@Access:		Public
	@Description:
		分配一段内存。
	@Parameters:
		length, uint32, IN
			长度。
	@Return:
		void *
			分配的内存的地址。返回 NULL 则失败。	
*/
void * 
alloc_memory(uint length)
{
	lock();
	void * r = _alloc_memory(length);
	if(r == NULL)
	{
		int8 buffer[1024];
		sprintf_s(	buffer, 
					1024, 
					"Memory manager cannot allocate %d byte(s) of memory.", 
					length);
		log(LOG_ERROR, buffer);
	}
	unlock();
	return r;
}

/**
	@Function:		_free_memory
	@Access:		Private
	@Description:
		释放内存。
	@Parameters:
		ptr, void *, IN
			要释放的内存的地址。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。		
*/
static
BOOL
_free_memory(void * ptr)
{
	uint32 address = (uint32)ptr;
	struct MemoryBlockDescriptor * umbd = umbdt_head;
	while(umbd != NULL)
		if(umbd->start == ptr)
		{
			struct MemoryBlockDescriptor * new_mbd = get_unused_mbd(MBDT_ADDRESS, MBD_COUNT);
			if(new_mbd == NULL)
				return FALSE;
			copy_memory(umbd, new_mbd, sizeof(struct MemoryBlockDescriptor));
			umbd->used = 0;
			if(remove_mbd(&umbdt_head, &umbdt_foot, umbd))
			{
				if(mbdt_head == NULL && mbdt_foot == NULL)
				{
					mbdt_head = new_mbd;
					mbdt_foot = new_mbd;
				}
				else if(mbdt_head != NULL && mbdt_foot != NULL)
				{
					struct MemoryBlockDescriptor * mbd = mbdt_head;
					while(mbd != NULL)
						if(new_mbd->start + new_mbd->length == mbd->start)
						{
							mbd->start = new_mbd->start;
							mbd->length += new_mbd->length;
							new_mbd->used = 0;
							break;
						}
						else if(mbd->start + mbd->length == new_mbd->start)
						{
							mbd->length += new_mbd->length;
							new_mbd->used = 0;
							break;
						}
						else
							mbd = mbd->next;
					if(mbd == NULL)
						if(!append_mbd(&mbdt_head, &mbdt_foot, new_mbd))
							return FALSE;
				}
				else
					return FALSE;
				used -= umbd->length;
				return TRUE;
			}
			else
				return FALSE;
		}
		else
			umbd = umbd->next;
	return FALSE;
}

/**
	@Function:		free_memory
	@Access:		Public
	@Description:
		释放内存。
	@Parameters:
		ptr, void *, IN
			要释放的内存的地址。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。		
*/
BOOL
free_memory(void * ptr)
{
	lock();
	BOOL r = _free_memory(ptr);
	if(!r)
	{
		uint32 address = (uint32)ptr;
		int8 buffer[1024];
		sprintf_s(	buffer, 
					1024, 
					"Memory manager cannot release a block of memory, the memory address is %X.", 
					address);
		log(LOG_ERROR, buffer);
	}
	unlock();
	return r;
}

/**
	@Function:		_get_memory_block_size
	@Access:		Private
	@Description:
		获取内存块的大小。
	@Parameters:
		ptr, void *, IN
			内存的地址。
	@Return:
		uint32
			内存块的大小。		
*/
static
uint32
_get_memory_block_size(void * ptr)
{
	struct MemoryBlockDescriptor * umbdt = umbdt_head;
	while(umbdt != NULL)
	{
		if(umbdt->start == (uint32)ptr)
			return umbdt->length;
		else
			umbdt = umbdt->next;
	}
	return 0;
}

/**
	@Function:		get_memory_block_size
	@Access:		Public
	@Description:
		获取内存块的大小。
	@Parameters:
		ptr, void *, IN
			内存的地址。
	@Return:
		uint32
			内存块的大小。		
*/
uint32
get_memory_block_size(void * ptr)
{
	lock();
	uint32 r = _get_memory_block_size(ptr);
	unlock();
	return r;
}
