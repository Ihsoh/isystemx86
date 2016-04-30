/**
	@File:			pci.c
	@Author:		Ihsoh
	@Date:			2015-03-09
	@Description:
		PCI。
*/

#include "pci.h"
#include "mqueue.h"
#include "system.h"
#include <ilib/string.h>

#define	RETRY_COUNT				0xffffffff

#define	FUNC_INIT				1
#define	FUNC_UPDATE				2
#define	FUNC_GET_DEV			3
#define	FUNC_WRITE_TO_FILE		4

#define	MESSAGE_TID 			-1000

static int32 tid = -1;
static MQueuePtr mqueue = NULL;
static PCIDevice pci_devices[MAX_PCI_DEVICES_COUNT];

/**
	@Function:		pci_init
	@Access:		Public
	@Description:
		初始化PCI。
	@Parameters:
	@Return:
		BOOL
			返回TRUE则初始化成功，否则失败。		
*/
BOOL
pci_init(void)
{
	tid = TskmgrCreateSystemTaskByFile(	SYSTEM_PATH"sys/pci.sys", 
									SYSTEM_PATH"sys/pci.sys", 
									SYSTEM_PATH"sys/");
	if(tid == -1)
		return FALSE;
	if(!TskmgrSetTaskToReady(tid))
		return FALSE;
	uint32 retry = RETRY_COUNT;
	while(	(mqueue = MqGetPtrByName("System-PCI")) == NULL 
			&& --retry != 0);
	if(mqueue == NULL)
		return FALSE;
	MQueueMessage message;
	message.tid = MESSAGE_TID;
	message.message = FUNC_INIT;
	if(!MqAddMessage(mqueue, MQUEUE_OUT, &message))
		return FALSE;
	MQueueMessagePtr messageptr = NULL;
	retry = RETRY_COUNT;
	while(	(messageptr = MqPopMessage(mqueue, MQUEUE_IN)) == NULL
			&& --retry != 0);
	BOOL r = FALSE;
	if(messageptr != NULL)
	{
		r = messageptr->param0.bool_value;
		MemFree(messageptr);
	}
	return r;
}

/**
	@Function:		pci_update
	@Access:		Public
	@Description:
		更新PCI信息。
	@Parameters:
	@Return:		
*/
void
pci_update(void)
{
	if(mqueue == NULL)
		return;
	MQueueMessage message;
	message.tid = MESSAGE_TID;
	message.message = FUNC_UPDATE;
	MqAddMessage(mqueue, MQUEUE_OUT, &message);
}

/**
	@Function:		pci_get_device
	@Access:		Public
	@Description:
		获取PCI设备的信息。
	@Parameters:
		index, uint32, IN
			PCI设备的索引(0 ~ 63)。
	@Return:
		PCIDevicePtr
			返回NULL则失败，否则返回PCI设备的信息。
*/
PCIDevicePtr
pci_get_device(IN uint32 index)
{
	if(mqueue == NULL)
		return NULL;
	MQueueMessage message;
	message.tid = MESSAGE_TID;
	message.message = FUNC_GET_DEV;
	message.param0.uint32_value = index;
	if(!MqAddMessage(mqueue, MQUEUE_OUT, &message))
		return NULL;
	MQueueMessagePtr messageptr = NULL;
	uint32 retry = RETRY_COUNT;
	while(	(messageptr = MqPopMessage(mqueue, MQUEUE_IN)) == NULL
			&& --retry != 0);
	if(messageptr == NULL)
		return NULL;
	PCIDevicePtr p = (PCIDevicePtr)messageptr->param0.uint32_value;
	MemFree(messageptr);
	p = (PCIDevicePtr)TaskmgrConvertLAddrToPAddr(tid, p);
	if(p == NULL)
		return NULL;
	if(index >= 64)
		return NULL;
	pci_devices[index].device = p->device;
	pci_devices[index].vendorid = p->vendorid;
	pci_devices[index].deviceid = p->deviceid;
	pci_devices[index].vendor_name
		= (const int8 *)TaskmgrConvertLAddrToPAddr(tid, p->vendor_name);
	pci_devices[index].device_name
		= (const int8 *)TaskmgrConvertLAddrToPAddr(tid, p->device_name);
	return pci_devices + index;
}

/**
	@Function:		pci_write_to_file
	@Access:		Public
	@Description:
		把所有PCI信息写到文件中。
		PCI信息以JSON的方式组织。
	@Parameters:
		path, const int8 *, IN
			文件路径。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。
*/
BOOL
pci_write_to_file(IN const int8 * path)
{
	if(path == NULL || mqueue == NULL)
		return FALSE;
	MQueueMessage message;
	message.tid = MESSAGE_TID;
	message.message = FUNC_WRITE_TO_FILE;
	if(strlen(path) >= sizeof(message.bsparam0))
		return FALSE;
	UtlCopyString(message.bsparam0, sizeof(message.bsparam0), path);
	if(!MqAddMessage(mqueue, MQUEUE_OUT, &message))
		return FALSE;
	MQueueMessagePtr messageptr = NULL;
	uint32 retry = RETRY_COUNT;
	while(	(messageptr = MqPopMessage(mqueue, MQUEUE_IN)) == NULL
			&& --retry != 0);
	BOOL r = FALSE;
	if(messageptr != NULL)
	{
		r = messageptr->param0.bool_value;
		MemFree(messageptr);
	}
	return r;
}
