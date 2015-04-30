#ifndef	_PCI_H_
#define	_PCI_H_

#include <ilib/ilib.h>

#define	MAX_PCI_DEVICES_COUNT	64

typedef struct
{
	uint32			device;
	uint16			vendorid;
	uint16			deviceid;
	const int8 *	vendor_name;
	const int8 *	device_name;
} PCIDevice, * PCIDevicePtr;

BOOL
pci_init(void);

void
pci_update(void);

PCIDevicePtr
pci_get_device(IN uint32 index);

BOOL
pci_write_to_file(IN const int8 * path);

#endif
