/**
	@File:			toaruos_pci.c
	@Author:		toaruos
	@Date:			2015-03-07
	@Description:
		以下代码来自toaruos。
		https://github.com/klange/toaruos。
*/

#include "toaruos_pci.h"
#include "pci_list.h"
#include "386.h"
#include <ilib/ilib.h>

void pci_write_field(uint32 device, int32 field, int32 size, uint32 value) {
	outl(PCI_ADDRESS_PORT, pci_get_addr(device, field));
	outl(PCI_VALUE_PORT, value);
}

uint32 pci_read_field(uint32 device, int32 field, int32 size) {
	outl(PCI_ADDRESS_PORT, pci_get_addr(device, field));

	if (size == 4) {
		uint32 t = inl(PCI_VALUE_PORT);
		return t;
	} else if (size == 2) {
		uint32 t = inw(PCI_VALUE_PORT + (field & 2));
		return t;
	} else if (size == 1) {
		uint32 t = inb(PCI_VALUE_PORT + (field & 3));
		return t;
	}
	return 0xFFFF;
}

uint16 pci_find_type(uint32 dev) {
	return (pci_read_field(dev, PCI_CLASS, 1) << 8) | pci_read_field(dev, PCI_SUBCLASS, 1);
}

const int8 * pci_vendor_lookup(uint16 vendor_id) {
	uint32 i;
	for (i = 0; i < PCI_VENTABLE_LEN; ++i) {
		if (PciVenTable[i].VenId == vendor_id) {
			return PciVenTable[i].VenFull;
		}
	}
	return "";
}

const int8 * pci_device_lookup(uint16 vendor_id, uint16 device_id) {
	uint32 i;
	for (i = 0; i < PCI_DEVTABLE_LEN; ++i) {
		if (PciDevTable[i].VenId == vendor_id && PciDevTable[i].DevId == device_id) {
			return PciDevTable[i].ChipDesc;
		}
	}
	return "";
}

void pci_scan_hit(pci_func_t f, uint32 dev, void * extra) {
	int32 dev_vend = (int32)pci_read_field(dev, PCI_VENDOR_ID, 2);
	int32 dev_dvid = (int32)pci_read_field(dev, PCI_DEVICE_ID, 2);

	f(dev, dev_vend, dev_dvid, extra);
}

void pci_scan_func(pci_func_t f, int32 type, int32 bus, int32 slot, int32 func, void * extra) {
	uint32 dev = pci_box_device(bus, slot, func);
	if (type == -1 || type == pci_find_type(dev)) {
		pci_scan_hit(f, dev, extra);
	}
	if (pci_find_type(dev) == PCI_TYPE_BRIDGE) {
		pci_scan_bus(f, type, pci_read_field(dev, PCI_SECONDARY_BUS, 1), extra);
	}
}

void pci_scan_slot(pci_func_t f, int32 type, int32 bus, int32 slot, void * extra) {
	uint32 dev = pci_box_device(bus, slot, 0);
	if (pci_read_field(dev, PCI_VENDOR_ID, 2) == PCI_NONE) {
		return;
	}
	pci_scan_func(f, type, bus, slot, 0, extra);
	if (!pci_read_field(dev, PCI_HEADER_TYPE, 1)) {
		return;
	}
	int32 func;
	for (func = 1; func < 8; func++) {
		uint32 dev = pci_box_device(bus, slot, func);
		if (pci_read_field(dev, PCI_VENDOR_ID, 2) != PCI_NONE) {
			pci_scan_func(f, type, bus, slot, func, extra);
		}
	}
}

void pci_scan_bus(pci_func_t f, int32 type, int32 bus, void * extra) {
	int32 slot;
	for (slot = 0; slot < 32; ++slot) {
		pci_scan_slot(f, type, bus, slot, extra);
	}
}

void pci_scan(pci_func_t f, int32 type, void * extra) {
	pci_scan_bus(f, type, 0, extra);

	if (!pci_read_field(0, PCI_HEADER_TYPE, 1)) {
		return;
	}
	int32 func;
	for (func = 1; func < 8; ++func) {
		uint32 dev = pci_box_device(0, 0, func);
		if (pci_read_field(dev, PCI_VENDOR_ID, 2) != PCI_NONE) {
			pci_scan_bus(f, type, func, extra);
		} else {
			break;
		}
	}
}
