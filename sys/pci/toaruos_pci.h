/**
	@File:			toaruos_pci.h
	@Author:		toaruos
	@Date:			2015-03-07
	@Description:
		以下代码来自toaruos。
		https://github.com/klange/toaruos。
*/

#ifndef	_TOARUOS_PCI_H_
#define	_TOARUOS_PCI_H_

#include <ilib/ilib.h>

#define PCI_VENDOR_ID            0x00 // 2
#define PCI_DEVICE_ID            0x02 // 2
#define PCI_COMMAND              0x04 // 2
#define PCI_STATUS               0x06 // 2
#define PCI_REVISION_ID          0x08 // 1

#define PCI_PROG_IF              0x09 // 1
#define PCI_SUBCLASS             0x0a // 1
#define PCI_CLASS                0x0b // 1
#define PCI_CACHE_LINE_SIZE      0x0c // 1
#define PCI_LATENCY_TIMER        0x0d // 1
#define PCI_HEADER_TYPE          0x0e // 1
#define PCI_BIST                 0x0f // 1
#define PCI_BAR0                 0x10 // 4
#define PCI_BAR1                 0x14 // 4
#define PCI_BAR2                 0x18 // 4
#define PCI_BAR3                 0x1C // 4
#define PCI_BAR4                 0x20 // 4
#define PCI_BAR5                 0x24 // 4

#define PCI_INTERRUPT_LINE       0x3C // 1

#define PCI_SECONDARY_BUS        0x09 // 1

#define PCI_HEADER_TYPE_DEVICE  0
#define PCI_HEADER_TYPE_BRIDGE  1
#define PCI_HEADER_TYPE_CARDBUS 2

#define PCI_TYPE_BRIDGE 0x0604
#define PCI_TYPE_SATA   0x0106

#define PCI_ADDRESS_PORT 0xCF8
#define PCI_VALUE_PORT   0xCFC

#define PCI_NONE 0xFFFF

typedef void (*pci_func_t)(uint32 device, uint16 vendor_id, uint16 device_id, void * extra);

static inline int32 pci_extract_bus(uint32 device) {
	return (uint8)((device >> 16));
}
static inline int32 pci_extract_slot(uint32 device) {
	return (uint8)((device >> 8));
}
static inline int32 pci_extract_func(uint32 device) {
	return (uint8)(device);
}

static inline uint32 pci_get_addr(uint32 device, int32 field) {
	return 0x80000000 | (pci_extract_bus(device) << 16) | (pci_extract_slot(device) << 11) | (pci_extract_func(device) << 8) | ((field) & 0xFC);
}

static inline uint32 pci_box_device(int32 bus, int32 slot, int32 func) {
	return (uint32)((bus << 16) | (slot << 8) | func);
}

uint32 pci_read_field(uint32 device, int32 field, int32 size);
void pci_write_field(uint32 device, int32 field, int32 size, uint32 value);
uint16 pci_find_type(uint32 dev);
const int8 * pci_vendor_lookup(uint16 vendor_id);
const int8 * pci_device_lookup(uint16 vendor_id, uint16 device_id);
void pci_scan_hit(pci_func_t f, uint32 dev, void * extra);
void pci_scan_func(pci_func_t f, int32 type, int32 bus, int32 slot, int32 func, void * extra);
void pci_scan_slot(pci_func_t f, int32 type, int32 bus, int32 slot, void * extra);
void pci_scan_bus(pci_func_t f, int32 type, int32 bus, void * extra);
void pci_scan(pci_func_t f, int32 type, void * extra);

#endif
