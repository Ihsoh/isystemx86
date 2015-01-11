/**
	@File:			386.h
	@Author:		Ihsoh
	@Date:			2014-1-28
	@Description:
*/

#ifndef	_386_H_
#define	_386_H_

#include "types.h"

extern
void
outb(	IN uint16 port, 
		IN uint8 data);

extern
uint8
inb(IN uint16 port);

extern
void
outw(	IN uint16 port,
		IN uint16 data);

extern
uint16
inw(IN uint16 port);

extern
void
set_int(IN uint8 n,
		IN uint32 addr);

extern
BOOL
set_desc_to_gdt(IN uint32 index,
				IN uint8 * desc);

extern
BOOL
get_desc_from_gdt(	IN uint32 index,
					OUT uint8 * desc);

extern
BOOL
set_gate_to_idt(IN uint32 index,
				IN uint8 * gate);

extern
BOOL
get_gate_from_idt(	IN uint32 index,
					OUT uint8 * gate);

extern
void
reset_cpu(void);

extern
void
halt_cpu(void);

#define	sti() { asm volatile ("sti"); }
#define	cli() { asm volatile ("cli"); }

#define	INT_EXIT() { asm volatile ("leave;iret"); }
#define	SAVE_REGS()	{ asm volatile ("pushal"); }
#define	RESUME_REGS() { asm volatile ("popal"); }

#define	ATDR	0x90
#define	ATDW	0x92
#define	ATDWA	0x93
#define	ATCE	0x98
#define	ATCER	0x9a
#define	ATCCO	0x9c
#define	ATCCOR	0x9e

#define	ATLDT		0x82
#define	ATTASKGATE	0x85
#define	AT386TSS	0x89
#define	AT386CGATE	0x8c
#define	AT386IGATE	0x8e
#define	AT386TGATE	0x8f

#define	DPL0		0x00
#define	DPL1		0x20
#define	DPL2		0x40
#define	DPL3		0x60
#define	RPL0		0x00
#define	RPL1		0x01
#define	RPL2		0x02
#define	RPL3		0x03
#define	IOPL0		0x0000
#define	IOPL1		0x1000
#define	IOPL2		0x2000
#define	IOPL3		0x3000

#define	G		0x8000
#define	D32		0x4000
#define	TIL		0x04
#define	VMFL	0x02
#define	IFL		0x0200

#define	PAGE_P		1
#define	PAGE_RW_R	0
#define	PAGE_RW_W	2
#define	PAGE_US_U	0
#define	PAGE_US_S	4

struct Desc
{
	uint16	limitl;
	uint16	basel;
	uint8	basem;
	uint16	attr;
	uint8	baseh;
} __attribute__((packed));

struct Gate
{
	uint16	offsetl;
	uint16	selector;
	uint8	dcount;
	uint8	attr;
	uint16	offseth;
} __attribute__((packed));
	
struct TSS
{
	uint32	back_link;
	uint32	esp0;
	uint32	ss0;
	uint32	esp1;
	uint32	ss1;
	uint32	esp2;
	uint32	ss2;
	uint32	cr3;
	uint32	eip;
	uint32	flags;
	uint32	eax;
	uint32	ecx;
	uint32	edx;
	uint32	ebx;
	uint32	esp;
	uint32	ebp;
	uint32	esi;
	uint32	edi;
	uint32	es;
	uint32	cs;
	uint32	ss;
	uint32	ds;
	uint32	fs;
	uint32	gs;
	uint32	ldt;
	uint16	trap;
	uint16	iobase;
} __attribute__((packed));

struct I387State
{
	uint32	cwd;
	uint32	swd;
	uint32	twd;
	uint32	fip;
	uint32	fcs;
	uint32	foo;
	uint32	fos;
	uint8	st_space[80];
} __attribute__((packed));

#define	NORMAL_C_DESC		((0 << 3) | RPL0)
#define	KERNEL_C_DESC		((1 << 3) | RPL0)
#define	KERNEL_D_DESC		((2 << 3) | RPL0)
#define	KERNEL_STACK_DESC	((3 << 3) | RPL0)
#define	KERNEL_LDR_C_DESC	((4 << 3) | RPL0)
#define	KERNEL_LDR_D_DESC	((5 << 3) | RPL0)
#define	KERNEL_TSS_DESC		((6 << 3) | RPL0)

#define	MAX_DESC_COUNT		8192	//GDT, IDT和LDT表中描述副的最大数量

#endif
