/**
	@File:			ahci.c
	@Author:		Ihsoh
	@Date:			2015-04-24
	@Description:
		AHCI。
*/

#include "ahci.h"
#include "types.h"

#include "pci/pci.h"

#include "screen.h"

#define SATA_SIG_ATA			0x00000101  // SATA drive
#define SATA_SIG_ATAPI			0xEB140101  // SATAPI drive
#define SATA_SIG_SEMB			0xC33C0101  // Enclosure management bridge
#define SATA_SIG_PM				0x96690101  // Port multiplier
#define AHCI_DEV_NULL			0
#define AHCI_DEV_SATA			1
#define AHCI_DEV_SATAPI			4
#define AHCI_DEV_SEMB			2
#define AHCI_DEV_PM				3
#define HBA_PORT_DET_PRESENT	3
#define HBA_PORT_IPM_ACTIVE		1
#define AHCI_BASE				0x400000    // 4M
#define HBA_PxCMD_CR            (1 << 15) /* CR - Command list Running */
#define HBA_PxCMD_FR            (1 << 14) /* FR - FIS receive Running */
#define HBA_PxCMD_FRE           (1 <<  4) /* FRE - FIS Receive Enable */
#define HBA_PxCMD_SUD           (1 <<  1) /* SUD - Spin-Up Device */
#define HBA_PxCMD_ST            (1 <<  0) /* ST - Start (command processing) */
#define ATA_DEV_BUSY			0x80
#define ATA_DEV_DRQ				0x08

#define ATA_DEV_BUSY			0x80
#define ATA_DEV_DRQ				0x08

#define ATA_DEV_BUSY			0x80
#define ATA_DEV_DRQ				0x08
#define HBA_PxIS_TFES			(1 << 30)       /* TFES - Task File Error Status */
#define ATA_CMD_READ_DMA_EX		0x25
#define ATA_CMD_WRITE_DMA_EX	0x35

#define	_MAX_DEVICE_COUNT	4
#define	_MAX_PORT_COUNT		16

typedef struct 
{
	HBA_PORT *	port;
	HBA_MEM *	abar;
} AHCIPort, * AHCIPortPtr;

static uint32 _devices[_MAX_DEVICE_COUNT];
static uint32 _devcnt = 0;
static AHCIPort _ports[_MAX_PORT_COUNT];
static uint32 _prtcnt = 0;

uint32
ahci_port_count(void)
{
	return _prtcnt;
}

HBA_PORT *
ahci_port(uint32 index)
{
	if(index >= _prtcnt)
		return NULL;
	return _ports[index].port;
}

static
HBA_MEM *
_get_abar_by_port(IN HBA_PORT * port)
{
	uint32 ui;
	for(ui = 0; ui < _prtcnt; ui++)
		if(_ports[ui].port == port)
			return _ports[ui].abar;
	return NULL;
}

static
void
_check_all_devices(void)
{
	_devcnt = 0;
	uint32 count = pci_device_count();
	uint32 index;
	for(index = 0; index < count; index++)
	{
		PCIDeviceInfoPtr device = pci_device(index);
		if(	device != NULL
			&& device->header->cfg_hdr.cc[2] == PCI_CLS_MASS_STORAGE_CONTROLLER
			&& device->header->cfg_hdr.cc[1] == 0x06
			&& device->header->cfg_hdr.cc[0] == 0x01)	// Serial ATA (AHCI 1.0)。
			_devices[_devcnt++] = index;
	}
}

static
int32
_check_type(IN HBA_PORT * port)
{
	DWORD ssts = port->ssts;
	BYTE ipm = (ssts >> 8) & 0x0f;
	BYTE det = ssts & 0x0f;

	if(det != HBA_PORT_DET_PRESENT)
		return AHCI_DEV_NULL;
	if(ipm != HBA_PORT_IPM_ACTIVE)
		return AHCI_DEV_NULL;

	switch(port->sig)
	{
		case SATA_SIG_ATAPI:
			return AHCI_DEV_SATAPI;
		case SATA_SIG_SEMB:
			return AHCI_DEV_SEMB;
		case SATA_SIG_PM:
			return AHCI_DEV_PM;
		default:
			return AHCI_DEV_SATA;
	}

	return AHCI_DEV_NULL;
}

static
void
_start_cmd(IN OUT HBA_PORT * port)
{
	// Wait until CR (bit15) is cleared
	while (port->cmd & HBA_PxCMD_CR);

	// Set FRE (bit4) and ST (bit0)
	port->cmd |= HBA_PxCMD_FRE;
	port->cmd |= HBA_PxCMD_ST; 
}

static
void
_stop_cmd(IN OUT HBA_PORT * port)
{
	// Clear ST (bit0)
	port->cmd &= ~HBA_PxCMD_ST;

	// Clear FRE (bit4)
	port->cmd &= ~HBA_PxCMD_FRE;
 
	// Wait until FR (bit14), CR (bit15) are cleared
	while(1)
	{
		if (port->cmd & HBA_PxCMD_FR)
			continue;
		if (port->cmd & HBA_PxCMD_CR)
			continue;
		break;
	}
 
	
}

static
void
_port_rebase(	IN OUT HBA_PORT * port,
				IN uint32 portno)
{
	_stop_cmd(port);	// Stop command engine
 
	// Command list offset: 1K*portno
	// Command list entry size = 32
	// Command list entry maxim count = 32
	// Command list maxim size = 32*32 = 1K per port
	port->clb = AHCI_BASE + (portno << 10);
	port->clbu = 0;
	memset((void *)(port->clb), 0, 1024);
 
	// FIS offset: 32K+256*portno
	// FIS entry size = 256 bytes per port
	port->fb = AHCI_BASE + (32 << 10) + (portno << 8);
	port->fbu = 0;
	memset((void *)(port->fb), 0, 256);
 
	// Command table offset: 40K + 8K*portno
	// Command table size = 256*32 = 8K per port
	HBA_CMD_HEADER * cmdheader = (HBA_CMD_HEADER *)(port->clb);
	uint32 ui;
	for(ui = 0; ui < 32; ui++)
	{
		cmdheader[ui].prdtl = 8;	// 8 prdt entries per command table
					// 256 bytes per command table, 64+16+48+16*8
		// Command table offset: 40K + 8K*portno + cmdheader_index*256
		cmdheader[ui].ctba = AHCI_BASE + (40 << 10) + (portno << 13) + (ui << 8);
		cmdheader[ui].ctbau = 0;
		memset((void *)cmdheader[ui].ctba, 0, 256);
	}
 
	_start_cmd(port);	// Start command engine
}

static
void
_probe_port(void)
{
	uint32 devidx;
	for(devidx = 0; devidx < _devcnt; devidx++)
	{
		PCIDeviceInfoPtr device = pci_device(_devices[devidx]);
		if(device == NULL)
			return;
		HBA_MEM * abar = (HBA_MEM *)device->header->cfg_hdr.abar;
		DWORD pi = abar->pi;
		uint32 ui;
		for(ui = 0; ui < 32; ui++)
			if((pi >> ui) & 0x1)
			{
				int32 dt = _check_type((HBA_PORT *)&abar->ports[ui]);
				switch(dt)
				{
					case AHCI_DEV_SATA:
					{
						AHCIPortPtr port = &_ports[_prtcnt++];
						port->abar = abar;
						port->port = (HBA_PORT *)&abar->ports[ui];
						_port_rebase(port->port, ui);
						if(_prtcnt == _MAX_PORT_COUNT)
							return;
						break;
					}
					case AHCI_DEV_SATAPI:
					{
						break;
					}
					case AHCI_DEV_SEMB:
					{
						break;
					}
					case AHCI_DEV_PM:
					{
						break;
					}
				}
			}
	}
}

static
int32
_find_cmdslot(IN HBA_PORT * port)
{
	// If not set in SACT and CI, the slot is free
	DWORD slots = (port->sact | port->ci);
	HBA_MEM * abar = _get_abar_by_port(port);
	int32 n = (abar->cap & 0x0f00) >> 8;
	int32 i;
	for(i = 0; i < n; i++)
	{
		if ((slots & 1) == 0)
			return i;
		slots >>= 1;
	}
	return -1;
}

BOOL
ahci_read(	IN OUT HBA_PORT * port,
			IN DWORD startl,
			IN DWORD starth,
			IN DWORD count,
			OUT WORD * buf)
{
	port->is = (DWORD)-1;
	int32 spin = 0;
	int32 slot = _find_cmdslot(port);
	if(slot == -1)
		return FALSE;

	HBA_CMD_HEADER * cmdheader = (HBA_CMD_HEADER *)port->clb;
	cmdheader += slot;
	cmdheader->cfl = sizeof(FIS_REG_H2D) / sizeof(DWORD);
	cmdheader->w = 0;
	cmdheader->c = 1;
	cmdheader->p = 1;
	cmdheader->prdtl = (WORD)((count - 1) >> 4) + 1;

	HBA_CMD_TBL * cmdtbl = (HBA_CMD_TBL *)cmdheader->ctba;
	memset(	cmdtbl,
			0,
			sizeof(HBA_CMD_TBL) + (cmdheader->prdtl - 1) * sizeof(HBA_PRDT_ENTRY));

	int32 i;
	for(i = 0; i < cmdheader->prdtl - 1; i++)
	{
		cmdtbl->prdt_entry[i].dba = (DWORD)buf;
		cmdtbl->prdt_entry[i].dbc = 8 * 1024;
		cmdtbl->prdt_entry[i].i = 0;
		buf += 4 * 1024;
		count -= 16;
	}

	cmdtbl->prdt_entry[i].dba = (DWORD)buf;
	cmdtbl->prdt_entry[i].dbc = count << 9;
	cmdtbl->prdt_entry[i].i = 0;

	FIS_REG_H2D * cmdfis = (FIS_REG_H2D *)&cmdtbl->cfis;

	cmdfis->fis_type = FIS_TYPE_REG_H2D;
	cmdfis->c = 1;
	cmdfis->command = ATA_CMD_READ_DMA_EX;

	cmdfis->lba0 = (BYTE)startl;
	cmdfis->lba1 = (BYTE)(startl >> 8);
	cmdfis->lba2 = (BYTE)(startl >> 16);
	cmdfis->device = 1 << 6;

	cmdfis->lba3 = (BYTE)(startl >> 24);
	cmdfis->lba4 = (BYTE)starth;
	cmdfis->lba5 = (BYTE)(starth >> 8);

	cmdfis->countl = (BYTE)count;
	cmdfis->counth = (BYTE)(count >> 8);

	while((port->tfd & (ATA_DEV_BUSY | ATA_DEV_DRQ)) && spin < 1000000)
		spin++;
	if(spin == 1000000)
		return FALSE;

	port->ci = 1 << slot;

	while(1)
	{
		if((port->ci & (1 << slot)) == 0)
			break;
		if(port->is & HBA_PxIS_TFES)
			return FALSE; 
	}

	if(port->is & HBA_PxIS_TFES)
		return FALSE;

	while(port->ci != 0);

	return TRUE;
}

BOOL
ahci_write(	IN OUT HBA_PORT * port,
			IN DWORD startl,
			IN DWORD starth,
			IN DWORD count,
			IN WORD * buf)
{
	port->is = (DWORD)-1;
	int32 spin = 0;
	int32 slot = _find_cmdslot(port);
	if(slot == -1)
		return FALSE;

	HBA_CMD_HEADER * cmdheader = (HBA_CMD_HEADER *)port->clb;
	cmdheader += slot;
	cmdheader->cfl = sizeof(FIS_REG_H2D) / sizeof(DWORD);
	cmdheader->w = 1;
	cmdheader->c = 1;
	cmdheader->p = 1;
	cmdheader->prdtl = (WORD)((count - 1) >> 4) + 1;

	HBA_CMD_TBL * cmdtbl = (HBA_CMD_TBL *)cmdheader->ctba;
	memset(	cmdtbl,
			0,
			sizeof(HBA_CMD_TBL) + (cmdheader->prdtl - 1) * sizeof(HBA_PRDT_ENTRY));

	int32 i;
	for(i = 0; i < cmdheader->prdtl - 1; i++)
	{
		cmdtbl->prdt_entry[i].dba = (DWORD)buf;
		cmdtbl->prdt_entry[i].dbc = 8 * 1024;
		cmdtbl->prdt_entry[i].i = 0;
		buf += 4 * 1024;
		count -= 16;
	}

	cmdtbl->prdt_entry[i].dba = (DWORD)buf;
	cmdtbl->prdt_entry[i].dbc = count << 9;
	cmdtbl->prdt_entry[i].i = 0;

	FIS_REG_H2D * cmdfis = (FIS_REG_H2D *)&cmdtbl->cfis;

	cmdfis->fis_type = FIS_TYPE_REG_H2D;
	cmdfis->c = 1;
	cmdfis->command = ATA_CMD_WRITE_DMA_EX;

	cmdfis->lba0 = (BYTE)startl;
	cmdfis->lba1 = (BYTE)(startl >> 8);
	cmdfis->lba2 = (BYTE)(startl >> 16);
	cmdfis->device = 1 << 6;

	cmdfis->lba3 = (BYTE)(startl >> 24);
	cmdfis->lba4 = (BYTE)starth;
	cmdfis->lba5 = (BYTE)(starth >> 8);

	cmdfis->countl = (BYTE)count;
	cmdfis->counth = (BYTE)(count >> 8);

	while((port->tfd & (ATA_DEV_BUSY | ATA_DEV_DRQ)) && spin < 1000000)
		spin++;
	if(spin == 1000000)
		return FALSE;

	port->ci = 1 << slot;

	while(1)
	{
		if((port->ci & (1 << slot)) == 0)
			break;
		if(port->is & HBA_PxIS_TFES)
			return FALSE; 
	}

	if(port->is & HBA_PxIS_TFES)
		return FALSE;

	while(port->ci != 0);

	return TRUE;
}

BOOL
ahci_init(void)
{
	_check_all_devices();
	_probe_port();
	return TRUE;
}
