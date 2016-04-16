/**
	@File:			pci.h
	@Author:		Ihsoh
	@Date:			2016-01-05
	@Description:
		对PCI总线进行操作。
*/

#ifndef	_PCI_PCI_H_
#define	_PCI_PCI_H_

#include "../types.h"

// PCI类型代码。
#define	PCI_CLS_MASS_STORAGE_CONTROLLER				0x01
#define	PCI_CLS_NETWORK_CONTROLLER					0x02
#define	PCI_CLS_DISPLAY_CONTROLLER					0x03
#define	PCI_CLS_MULTIMEDIA_CONTROLLER				0x04
#define	PCI_CLS_MEMORY_CONTROLLER					0x05
#define	PCI_CLS_BRIDGE_DEVICE						0x06
#define	PCI_CLS_SIMPLE_COMMUNICATION_CONTROLLERS	0x07
#define	PCI_CLS_BASE_SYSTEM_PERIPHERALS				0x08
#define	PCI_CLS_INPUT_DEVICES						0x09
#define	PCI_CLS_DOCKING_STATIONS					0x0a
#define	PCI_CLS_PROCESSORS							0x0b
#define	PCI_CLS_SERIAL_BUS_CONTROLLERS				0x0c
#define	PCI_CLS_WIRELESS_CONTROLLERS				0x0d
#define	PCI_CLS_INTELLIGENT_IO_CONTROLLERS			0x0e
#define	PCI_CLS_SATELLITE_COMMUNICATION_CONTROLLERS	0x0f
#define	PCI_CLS_ENCRYPTION_DECRYPTION_CONTROLLERS	0x10
#define	PCI_CLS_DAT_ACQ_AND_SIG_PROC_CONTROLLERS	0x11

#define	PCI_HEADER_SIZE		256

typedef struct
{
	uint32		id;			// 00 ~ 03，Identifiers。
	uint16		cmd;		// 04 ~ 05，Command Register。
	uint16		sts;		// 06 ~ 07，Device Status。
	uint8		rid;		// 08 ~ 08，Revision ID。
	uint8		cc[3];		// 09 ~ 0B，Class Codes。
	uint8		cls;		// 0C ~ 0C，Cache Line Size。
	uint8		mlt;		// 0D ~ 0D，Master Latency Timer。
	uint8		htype;		// 0E ~ 0E，Header Type。
	uint8		bist;		// 0F ~ 0F，Built In Self Test (Optional)。
	uint32		bars[5];	// 10 ~ 23，Other Base Address Registres (Optional) <BAR0-4>。
	uint32		abar;		// 24 ~ 27，AHCI Base Address <BAR5>。
	uint32		cbcp;		// 28 ~ 2B，CardBus CIS Pointer。
	uint32		ss;			// 2C ~ 2F，Subsystem Identifiers。
	uint32		erom;		// 30 ~ 33，Expansion ROM Base Address (Optional)。
	uint8		cap;		// 34 ~ 34，Capabilities Pointer。
	uint8		unused0[7];
	uint16		intr;		// 3C ~ 3D，Interrupt Information。
	uint8		mgnt;		// 3E ~ 3E，Min Grant (Optional) 。
	uint8		mlat;		// 3F ~ 3F，Max Latency (Optional)。
} PCIHeaderCH, * PCIHeaderCHPtr;

typedef struct
{
	PCIHeaderCH		cfg_hdr;
	uint8			cfg_local[PCI_HEADER_SIZE - sizeof(PCIHeaderCH)];
} PCIHeader, * PCIHeaderPtr;

#define	PCI_CONFIG_ADDRESS		0xcf8
#define	PCI_CONFIG_DATA			0xcfc


#define	PCI_MAX_BUS_COUNT		0x100
#define	PCI_MAX_SLOT_COUNT		0x20
#define	PCI_MAX_DEVICE_COUNT	32

typedef struct
{
	uint8			bus;
	uint8			slot;
	PCIHeaderPtr 	header;
} PCIDeviceInfo, * PCIDeviceInfoPtr;



extern
BOOL
PciInitDevices(void);

extern
void
PciFreeDevices(void);

extern
uint32
PciGetDeviceCount(void);

extern
PCIDeviceInfoPtr
PciGetDevice(IN uint32 index);

#endif
