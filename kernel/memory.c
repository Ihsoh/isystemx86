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
	@Function:		MemCopy
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
MemCopy(IN uint8 * src,
		OUT uint8 * dst,
		IN uint32 length)
{
	uint32 ui;	
	for(ui = 0; ui < length; ui++)
		dst[ui] = src[ui];
}

/**
	@Function:		MemClear
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
MemClear(	OUT uint8 * dst,
			IN uint8 data,
			IN uint32 len)
{
	uint32 ui;
	for(ui = 0; ui < len; ui++)
		dst[ui] = data;
}

/**
	@Function:		MemGetUsedLength
	@Access:		Public
	@Description:
		获取已使用内存的长度，单位为Byte。
	@Parameters:
	@Return:
		uint64
			已使用的内存的长度。
*/
uint64 
MemGetUsedLength(void)
{
	return used;
}

/**
	@Function:		_MemGetUnusedMBD
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
_MemGetUnusedMBD(	IN struct MemoryBlockDescriptor * mbdt, 
					IN uint32 mbd_count)
{
	uint32 ui;
	for(ui = 0; ui < mbd_count; ui++)
		if(mbdt[ui].used == 0)
			return mbdt + ui;
	return NULL;
}

/**
	@Function:		_MemAppendMBD
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
_MemAppendMBD(	IN OUT struct MemoryBlockDescriptor ** head, 
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
	@Function:		_MemRemoveMBD
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
_MemRemoveMBD(	IN OUT struct MemoryBlockDescriptor ** head, 
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
	@Function:		MemInit
	@Access:		Public
	@Description:
		初始化内存。
	@Parameters:
	@Return:	
*/
void 
MemInit(void)
{
	uint32 ui;
	uint64 size = get_memory_size();
	memory_limit = (uint32)(size - 1);
	MemClear((uchar *)MBDT_ADDRESS, 0, MBDT_LENGTH);
	MemClear((uchar *)UMBDT_ADDRESS, 0, UMBDT_LENGTH);
	struct MemoryBlockDescriptor * mbd = _MemGetUnusedMBD(MBDT_ADDRESS, MBD_COUNT);
	mbd->used = 1;
	mbd->start = ALLOC_START_ADDRESS;
	mbd->length = memory_limit - ALLOC_START_ADDRESS + 1;
	_MemAppendMBD(&mbdt_head, &mbdt_foot, mbd);
}

/**
	@Function:		MemGetMBDTableInfo
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
MemGetMBDTableInfo(	OUT uint32 * total_mbd_count,
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
	@Function:		MemGetUsedMBDTableInfo
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
MemGetUsedMBDTableInfo(	OUT uint32 * total_umbd_count,
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
	@Function:		MemAlign4KB
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
MemAlign4KB(IN uint32 n)
{
	if(n % KB(4) == 0)
		return n;
	else
		return n + (KB(4) - n % KB(4));
}

/**
	@Function:		_MemAllocWithStart
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
_MemAllocWithStart(	IN uint32 m_start,
					IN uint32 length)
{
	//让需要分配的内存长度4KB对齐.
	//因为一直分配的内存的长度都是4KB对齐
	//并且第一个未分配内存块的起始地址也是4KB对齐的,
	//所以分配得到的内存地址也是4KB对齐的.
	length = MemAlign4KB(length);

	struct MemoryBlockDescriptor * mbd = mbdt_foot;
	while(mbd != NULL)
		if(mbd->start < m_start && m_start + length < mbd->start + mbd->length)
		{
			struct MemoryBlockDescriptor * umbd = _MemGetUnusedMBD(UMBDT_ADDRESS, UMBD_COUNT);
			if(umbd == NULL)
				return NULL;
			uint32 address = mbd->start + mbd->length - length;
			umbd->used = 1;
			umbd->start = address;
			umbd->length = length;
			mbd->length -= length;
			if(!_MemAppendMBD(&umbdt_head, &umbdt_foot, umbd))
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
			struct MemoryBlockDescriptor * umbd = _MemGetUnusedMBD(UMBDT_ADDRESS, MBD_COUNT);
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
				if(!_MemRemoveMBD(&mbdt_head, &mbdt_foot, mbd))
					return NULL;
			}			
			if(!_MemAppendMBD(&umbdt_head, &umbdt_foot, umbd))
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
	@Function:		MemAllocWithStart
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
MemAllocWithStart(	IN uint32 m_start,
					IN uint32 length)
{
	lock();
	void * r = _MemAllocWithStart(m_start, length);
	unlock();
	return r;
}

/**
	@Function:		_MemAlloc
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
_MemAlloc(uint32 length)
{
	return _MemAllocWithStart(ALLOC_START_ADDRESS, length);
}

/**
	@Function:		MemAlloc
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
MemAlloc(uint32 length)
{
	lock();
	void * r = _MemAlloc(length);
	if(r == NULL)
	{
		int8 buffer[1024];
		sprintf_s(	buffer, 
					1024, 
					"Memory manager cannot allocate %d byte(s) of memory.", 
					length);
		Log(LOG_ERROR, buffer);
	}
	unlock();
	return r;
}

/**
	@Function:		_MemFree
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
_MemFree(void * ptr)
{
	uint32 address = (uint32)ptr;
	struct MemoryBlockDescriptor * umbd = umbdt_head;
	while(umbd != NULL)
		if(umbd->start == ptr)
		{
			struct MemoryBlockDescriptor * new_mbd = _MemGetUnusedMBD(MBDT_ADDRESS, MBD_COUNT);
			if(new_mbd == NULL)
				return FALSE;
			MemCopy(umbd, new_mbd, sizeof(struct MemoryBlockDescriptor));
			umbd->used = 0;
			if(_MemRemoveMBD(&umbdt_head, &umbdt_foot, umbd))
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
						if(!_MemAppendMBD(&mbdt_head, &mbdt_foot, new_mbd))
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
	@Function:		MemFree
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
MemFree(void * ptr)
{
	lock();
	BOOL r = _MemFree(ptr);
	if(!r)
	{
		uint32 address = (uint32)ptr;
		int8 buffer[1024];
		sprintf_s(	buffer, 
					1024, 
					"Memory manager cannot release a block of memory, the memory address is %X.", 
					address);
		Log(LOG_ERROR, buffer);
	}
	unlock();
	return r;
}

/**
	@Function:		_MemGetBlockSise
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
_MemGetBlockSise(void * ptr)
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
	@Function:		MemGetBlockSise
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
MemGetBlockSise(void * ptr)
{
	lock();
	uint32 r = _MemGetBlockSise(ptr);
	unlock();
	return r;
}
