/**
	@File:			apic.c
	@Author:		Ihsoh
	@Date:			2015-03-10
	@Description:
		高级可编程中断管理器。
*/

#include "apic.h"
#include "types.h"
#include "madt.h"

#define IA32_APIC_BASE_MSR 			0x1b
#define IA32_APIC_BASE_MSR_BSP 		0x100 // Processor is a BSP
#define IA32_APIC_BASE_MSR_ENABLE	0x800

//#define	IOAPIC_ADDR		0xfec00000
//#define	LAPIC_ADDR		0xfee00000

//I/O APIC寄存器地址。
#define	IOAPIC_REG_ID 			0x0000
#define	IOAPIC_REG_VER			0x0001
#define	IOAPIC_REG_ARB			0x0002

//Local APIC寄存器地址。
#define	LAPIC_REG_ID				0x0020 	//Local APIC ID Register
#define	LAPIC_REG_VER				0x0030	//Local APIC Version Register
#define	LAPIC_REG_TPR				0x0080	//Task Priority Register (TPR)
#define	LAPIC_REG_APR				0x0090	//Arbitration Priority Register1 (APR)
#define	LAPIC_REG_PPR				0x00a0	//Processor Priority Register (PPR)
#define	LAPIC_REG_EOI				0x00b0	//EOI Register
#define	LAPIC_REG_RRD				0x00c0	//Remote Read Register1 (RRD)
#define	LAPIC_REG_LOG_DEST			0x00d0	//Logical Destination Register
#define	LAPIC_REG_DEST_FMT			0x00e0	//Destination Format Register
#define	LAPIC_REG_SPRS_INT_VEC		0x00f0	//Spurious Interrupt Vector Register
#define	LAPIC_ISR_0_31				0x0100 	//In-Service Register (ISR); bits 31:0
#define	LAPIC_ISR_32_63				0x0110 	//In-Service Register (ISR); bits 63:32
#define	LAPIC_ISR_64_95				0x0120 	//In-Service Register (ISR); bits 95:64
#define	LAPIC_ISR_96_127			0x0130 	//In-Service Register (ISR); bits 127:96
#define	LAPIC_ISR_128_159			0x0140	//In-Service Register (ISR); bits 159:128
#define	LAPIC_ISR_160_191			0x0150	//In-Service Register (ISR); bits 191:160
#define	LAPIC_ISR_192_223			0x0160	//In-Service Register (ISR); bits 223:192
#define	LAPIC_ISR_224_255			0x0170	//In-Service Register (ISR); bits 255:224
#define	LAPIC_TMR_0_31				0x0180	//Trigger Mode Register (TMR); bits 31:0
#define	LAPIC_TMR_32_63				0x0190	//Trigger Mode Register (TMR); bits 63:32
#define	LAPIC_TMR_64_95				0x01a0	//Trigger Mode Register (TMR); bits 95:64
#define	LAPIC_TMR_96_127			0x01b0	//Trigger Mode Register (TMR); bits 127:96
#define	LAPIC_TMR_128_159			0x01c0	//Trigger Mode Register (TMR); bits 159:128
#define	LAPIC_TMR_160_191			0x01d0	//Trigger Mode Register (TMR); bits 191:160
#define	LAPIC_TMR_192_223			0x01e0	//Trigger Mode Register (TMR); bits 223:192
#define	LAPIC_TMR_224_255			0x01f0	//Trigger Mode Register (TMR); bits 255:224
#define	LAPIC_IRR_0_31				0x0200 	//Interrupt Request Register (IRR); bits 31:0
#define	LAPIC_IRR_32_63				0x0210 	//Interrupt Request Register (IRR); bits 63:32
#define	LAPIC_IRR_64_95				0x0220 	//Interrupt Request Register (IRR); bits 95:64
#define	LAPIC_IRR_96_127			0x0230 	//Interrupt Request Register (IRR); bits 127:96
#define	LAPIC_IRR_128_159 			0x0240 	//Interrupt Request Register (IRR); bits 159:128
#define	LAPIC_IRR_160_191 			0x0250 	//Interrupt Request Register (IRR); bits 191:160
#define	LAPIC_IRR_192_223			0x0260 	//Interrupt Request Register (IRR); bits 223:192
#define	LAPIC_IRR_224_255			0x0270 	//Interrupt Request Register (IRR); bits 255:224
#define	LAPIC_ERR_STATUS			0x0280	//Error Status Register
#define	LAPIC_LVT_CMCI				0x02f0	//LVT CMCI Register
#define	LAPIC_ICR_0_31				0x0300 	//Interrupt Command Register (ICR); bits 0-31
#define	LAPIC_ICR_32_63				0x0310 	//Interrupt Command Register (ICR); bits 32-63
#define	LAPIC_LVT_TIMER				0x0320 	//LVT Timer Register
#define	LAPIC_LVT_THML_SENSOR		0x0330	//LVT Thermal Sensor Register
#define	LAPIC_LVT_PERF_MNTR_CNTR	0x0340	//LVT Performance Monitoring Counters Register
#define	LAPIC_LVT_LINT0				0x0350	//LVT LINT0 Register
#define	LAPIC_LVT_LINT1				0x0360	//LVT LINT1 Register
#define	LAPIC_LVT_ERR 				0x0370	//LVT Error Register
#define	LAPIC_INIT_COUNT			0x0380	//Initial Count Register (for Timer)
#define	LAPIC_CURRENT_COUNT			0x0390	//Current Count Register (for Timer)
#define	LAPIC_DIVIDE_CONFIG			0x03e0	//Divide Configuration Register (for Timer)		

#define	LTIMER_DIVIDE_BY_1			0x0000000b	//定时器以1递减。
#define	LTIMER_DIVIDE_BY_2 			0x00000000	//定时器以2递减。
#define	LTIMER_DIVIDE_BY_4 			0x00000001	//定时器以4递减。
#define	LTIMER_DIVIDE_BY_8 			0x00000002	//定时器以8递减。
#define	LTIMER_DIVIDE_BY_16			0x00000003	//定时器以16递减。
#define	LTIMER_DIVIDE_BY_32			0x00000008	//定时器以32递减。
#define	LTIMER_DIVIDE_BY_64			0x00000009	//定时器以64递减。
#define	LTIMER_DIVIDE_BY_128		0x0000000a	//定时器以128递减。

#define	LTIMER_PERIODIC				0x00020000	//定时器以 Periodic(周期的) 模式运行。
												//应用该模式后，定时器的计数器递减为0时，
												//引发中断，并且重置计数器为 LAPIC_INIT_COUNT 寄存器的值。

#define	LTIMER_DEFAULT_COUNT		(0x10000 * 16)	//默认定时器的计数器的初始化数值。

static uint8 apic_ver = 0;
static uint8 apic_irq_count = 0;
static BOOL apic_ok = FALSE;
static uint32 lapic_address = 0;
static uint32 ioapic_address = 0;

/**
	@Function:		ioapic_write_register
	@Access:		Private
	@Description:
		写 I/O APIC 的寄存器。
	@Parameters:
		reg, uint32, IN
			需要写入值的寄存器。
		value, uint32, IN
			要写入的值。
	@Return:	
*/
static
void
ioapic_write_register(	IN uint32 reg,
						IN uint32 value)
{
	uint32 volatile * ioapic = (uint32 *)ioapic_address;
	ioapic[0] = reg & 0xff;
	ioapic[4] = value;
}

/**
	@Function:		ioapic_read_register
	@Access:		Private
	@Description:
		读 I/O APIC 的寄存器。
	@Parameters:
		reg, uint32, IN
			需要读取值的寄存器。
	@Return:
		uint32
			指定的寄存器的值。	
*/
static
uint32
ioapic_read_register(IN uint32 reg)
{
	uint32 volatile * ioapic = (uint32 *)ioapic_address;
	ioapic[0] = reg & 0xff;
	return ioapic[4];
}

/**
	@Function:		lapic_write_register
	@Access:		Private
	@Description:
		写 Local APIC 的寄存器。
	@Parameters:
		reg, uint32, IN
			需要写入值的寄存器。
		value, uint32, IN
			要写入的值。
	@Return:	
*/
static
void
lapic_write_register(	IN uint32 reg,
						IN uint32 value)
{
	uint8 * lapic = (uint8 *)lapic_address;
	*(uint32 *)(lapic + reg) = value;
}

/**
	@Function:		lapic_read_register
	@Access:		Private
	@Description:
		读 Local APIC 的寄存器。
	@Parameters:
		reg, uint32, IN
			需要读取值的寄存器。
	@Return:
		uint32
			指定的寄存器的值。	
*/
static
uint32
lapic_read_register(IN uint32 reg)
{
	uint8 * lapic = (uint8 *)lapic_address;
	return *(uint32 *)(lapic + reg);
}

/**
	@Function:		apic_init
	@Access:		Public
	@Description:
		初始化 APIC。
	@Parameters:
	@Return:
		BOOL
			返回 TRUE 则成功，否则失败。
*/
BOOL
apic_init(void)
{
	if(!madt_init())
		return FALSE;
	lapic_address = madt_get()->lcaddr;
	struct MADTEntryType1 * e = madt_get_type1_entry(0);
	if(e == NULL)
		return FALSE;
	ioapic_address = e->ioapic_address;
	uint32 ioapicver = ioapic_read_register(IOAPIC_REG_VER);
	apic_ver = (uint8)ioapicver;
	apic_irq_count = (uint8)((ioapicver >> 16) & 0xff) + 1;
	uint8 irq;
	for(irq = 0; irq < apic_irq_count; irq++)
		if(	irq == 0
			|| irq == 1
			|| irq == 12
			|| irq == 13
			|| irq == 14)
		{
			uint32 lindex = 0x10 + irq * 2;
			uint32 hindex = 0x10 + irq * 2 + 1;
			uint32 l = 0;
			uint32 h = 0;
			uint8 vector = 0;
			if(irq < 8)
				vector = 0x40 + irq;
			else
				vector = 0x70 + (irq - 8);
			l = l
				| vector 			// Interrupt Vector
				| (0x00 << 8)		// Delivery Mode, Fixed
				| (0x00 << 11)		// Destination Mode, Physical Mode
									// Delivery Status, Readonly
				| (0x00 << 13)		// Interrupt Input Pin Polarity, High active
									// Remote IRR, Readonly
				| (0x00 << 15)		// Trigger Mode, Edge sensitive
				| (0x00 << 16);		// Interrupt Mask, Not Masked
			h =						// Reserved
				(0x00 << 24);		// Destination Field, APIC ID = 0
			ioapic_write_register(lindex, l);
			ioapic_write_register(hindex, h);
		}
	return TRUE;
}

/**
	@Function:		apic_set_base
	@Access:		Private
	@Description:
		为 Local APIC 寄存器设置物理地址。
	@Parameters:
		apic, uint32, IN
			设置的值。
	@Return:
*/
static
void
apic_set_base(IN uint32 apic)
{
	uint32 edx = 0;
	uint32 eax = (apic & 0xfffff100) | IA32_APIC_BASE_MSR_ENABLE;
	set_msr(IA32_APIC_BASE_MSR, eax, edx);
}

/**
	@Function:		apic_get_base
	@Access:		Private
	@Description:
		获取 Local APIC 寄存器页的物理地址。
	@Parameters:
	@Return:
		uint32
			获取的值。
*/
static
uint32
apic_get_base(void)
{
	uint32 eax, edx;
	get_msr(IA32_APIC_BASE_MSR, &eax, &edx);
	return eax & 0xfffff000;
}

/**
	@Function:		apic_enable
	@Access:		Public
	@Description:
		启用 APIC。
	@Parameters:
	@Return:
		BOOL
			返回 TRUE 则成功，否则失败。
*/
BOOL
apic_enable(void)
{
	apic_set_base(apic_get_base());
	lapic_write_register(	LAPIC_REG_SPRS_INT_VEC, 
							lapic_read_register(LAPIC_REG_SPRS_INT_VEC) | 0x100);
	lapic_write_register(LAPIC_INIT_COUNT, LTIMER_DEFAULT_COUNT);
	lapic_write_register(LAPIC_DIVIDE_CONFIG, LTIMER_DIVIDE_BY_16);
	lapic_write_register(LAPIC_LVT_TIMER, LTIMER_PERIODIC | 0x40);
	apic_ok = TRUE;
	return TRUE;
}


BOOL
apic_stop_timer(void)
{
	if(!apic_ok)
		return FALSE;
	lapic_write_register(LAPIC_LVT_TIMER, 0);
	return TRUE;
}

/**
	@Function:		apic_is_enable
	@Access:		Public
	@Description:
		确认 APIC 是否已经启用。
	@Parameters:
	@Return:
		BOOL
			返回 TRUE 则已启用，否则未启用。
*/
BOOL
apic_is_enable(void)
{
	return apic_ok;
}

/**
	@Function:		apic_eoi
	@Access:		Public
	@Description:
		End Of Interrupt。
	@Parameters:
	@Return:
		BOOL
			返回 TRUE 则成功，否则失败。
*/
BOOL
apic_eoi(void)
{
	if(apic_ok)
	{
		lapic_write_register(LAPIC_REG_EOI, 0);
		return TRUE;
	}
	return FALSE;
}
