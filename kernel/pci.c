/**
	@File:			pci.c
	@Author:		Ihsoh
	@Date:			2015-03-07
	@Description:
		PCI。
*/

#include "pci.h"
#include "types.h"

BOOL
pci_init(void)
{

	return TRUE;
}

uint16
pci_config_read_word(	IN uint32 bus,
						IN uint32 slot,
						IN uint32 func,
						IN uint32 offset)
{

}
