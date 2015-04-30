#include "pci.h"
#include "toaruos_pci.h"
#include <ilib/ilib.h>

uint32 pci_devices_count = 0;
PCIDevice pci_devices[MAX_PCI_DEVICES_COUNT];

static
void
_pci_find_device(	IN uint32 device,
					IN uint16 vendorid,
					IN uint16 deviceid,
					OUT void * extra)
{
	if(pci_devices_count == MAX_PCI_DEVICES_COUNT)
		return;
	PCIDevicePtr pci_device = pci_devices + pci_devices_count;
	pci_device->device = device;
	pci_device->vendorid = vendorid;
	pci_device->deviceid = deviceid;
	pci_device->vendor_name = pci_vendor_lookup(vendorid);
	pci_device->device_name = pci_device_lookup(vendorid, deviceid);
	pci_devices_count++;
	*((uint32 *)extra) = device;
}

BOOL
pci_init(void)
{
	pci_update();
	return TRUE;
}

void
pci_update(void)
{
	pci_devices_count = 0;
	uint32 extra = 0;
	pci_scan(&_pci_find_device, -1, &extra);
}

PCIDevicePtr
pci_get_device(IN uint32 index)
{
	if(index >= pci_devices_count)
		return NULL;
	return pci_devices + index;
}

BOOL
pci_write_to_file(IN const int8 * path)
{
	FILE * fptr = fopen(path, FILE_MODE_WRITE | FILE_MODE_APPEND);
	if(fptr == NULL)
		return FALSE;
	fwrite(fptr, "{\n", 2);
	uint32 ui;
	for(ui = 0; ui < pci_devices_count; ui++)
	{
		PCIDevicePtr pci_device = pci_devices + ui;
		int8 buffer[64 * 1024];
		sprintf_s(	buffer, 
					sizeof(buffer), 
					"  {\n"
					"    Device:\"%X\",\n"
					"    VendorID:\"%X\",\n"
					"    DeviceID:\"%X\",\n"
					"    VendorName:\"%s\",\n"
					"    DeviceName:\"%s\",\n"
					"  },\n", 
					pci_device->device,
					pci_device->vendorid,
					pci_device->deviceid,
					pci_device->vendor_name,
					pci_device->device_name);
		fappend(fptr, buffer, strlen(buffer));
	}
	fappend(fptr, "}\n", 2);
	fclose(fptr);
	return TRUE;
}
