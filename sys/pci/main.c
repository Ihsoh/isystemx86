#include <ilib.h>
#include "pci.h"

#define	FUNC_INIT				1
#define	FUNC_UPDATE				2
#define	FUNC_GET_DEV			3
#define	FUNC_WRITE_TO_FILE		4

int main(int argc, char * argv[])
{
	uint mqid = il_create_mqueue("System-PCI");
	if(mqid == 0)
		return -1;
	MQueueMessage message;
	while(1)
		if(il_pop_message_s(mqid, &message))
			switch(message.message)
			{
				case FUNC_INIT:
				{
					message.param0.bool_value = pci_init();
					il_push_message_s(mqid, &message);
					break;
				}
				case FUNC_UPDATE:
				{
					pci_update();
					break;
				}
				case FUNC_GET_DEV:
				{
					uint32 index = message.param0.uint32_value;
					PCIDevicePtr p = pci_get_device(index);
					message.param0.uint32_value = (uint32)p;
					il_push_message_s(mqid, &message);
					break;
				}
				case FUNC_WRITE_TO_FILE:
				{
					message.param0.bool_value = pci_write_to_file(message.bsparam0);
					il_push_message_s(mqid, &message);
					break;
				}
				default:
					break;
			}
	return 0;
}
