/**
	@File:			pic.c
	@Author:		Ihsoh
	@Date:			2015-03-11
	@Description:
		Intel 8259A。
*/

#include "pic.h"
#include "types.h"
#include "386.h"

#define	PIC_MASTER_CMD		0x20
#define	PIC_MASTER_IMR		(PIC_MASTER_CMD + 1)
#define	PIC_SLAVE_CMD		0xa0
#define	PIC_SLAVE_IMR		(PIC_SLAVE_CMD + 1)

#define	PIC_IRQ_COUNT		16

#define PIT_A		0x40
#define PIT_B		0x41
#define PIT_C		0x42
#define PIT_CONTROL	0x43

#define PIT_MASK	0xFF
#define PIT_SCALE	1193180
#define PIT_SET		0x36

/**
	@Function:		pic_set_phase
	@Access:		Public
	@Description:
		设置PIT周期。
	@Parameters:
		hz, int32, IN
			HZ。
	@Return:
*/
static
void
pic_set_phase(IN int32 hz)
{
	int32 divisor = PIT_SCALE / hz;
	KnlOutByte(PIT_CONTROL, PIT_SET);
	KnlOutByte(PIT_A, divisor & PIT_MASK);
	KnlOutByte(PIT_A, (divisor >> 8) & PIT_MASK);
}

/**
	@Function:		pic_init
	@Access:		Public
	@Description:
		初始化PIC。
	@Parameters:
	@Return:
		BOOL
			返回 TRUE 则成功，否则失败。	
*/
BOOL
pic_init(void)
{
	pic_set_phase(1000);
	return TRUE;
}

/**
	@Function:		pic_mask
	@Access:		Public
	@Description:
		屏蔽指定的外部中断。
	@Parameters:
		irq, uint8, IN
			IRQ Number。
	@Return:
		BOOL
			返回 TRUE 则成功，否则失败。	
*/
BOOL
pic_mask(IN uint8 irq)
{
	if(irq >= PIC_IRQ_COUNT)
		return FALSE;
	if(irq < 8)
		KnlOutByte(PIC_MASTER_IMR, KnlInByte(PIC_MASTER_IMR) | (uint8)(0x01 << irq));
	else
		KnlOutByte(PIC_SLAVE_IMR, KnlInByte(PIC_SLAVE_IMR) | (uint8)(0x01 << (irq - 8)));
	return TRUE;
}

/**
	@Function:		pic_unmask
	@Access:		Public
	@Description:
		启用指定的外部中断。
	@Parameters:
		irq, uint8, IN
			IRQ Number。
	@Return:
		BOOL
			返回 TRUE 则成功，否则失败。	
*/
BOOL
pic_unmask(IN uint8 irq)
{
	if(irq >= PIC_IRQ_COUNT)
		return FALSE;
	if(irq < 8)
		KnlOutByte(PIC_MASTER_IMR, KnlInByte(PIC_MASTER_IMR) & ~(uint8)(0x01 << irq));
	else
		KnlOutByte(PIC_SLAVE_IMR, KnlInByte(PIC_SLAVE_IMR) & ~(uint8)(0x01 << (irq - 8)));
	return TRUE;
}

/**
	@Function:		pic_mask_all
	@Access:		Public
	@Description:
		屏蔽所有的外部中断。
	@Parameters:
	@Return:
		BOOL
			返回 TRUE 则成功，否则失败。	
*/
BOOL
pic_mask_all(void)
{
	uint8 irq;
	for(irq = 0; irq < PIC_IRQ_COUNT; irq++)
		if(!pic_mask(irq))
			return FALSE;
	return TRUE;
}

/**
	@Function:		pic_unmask_all
	@Access:		Public
	@Description:
		启用所有的外部中断。
	@Parameters:
	@Return:
		BOOL
			返回 TRUE 则成功，否则失败。	
*/
BOOL
pic_unmask_all(void)
{
	uint8 irq;
	for(irq = 0; irq < PIC_IRQ_COUNT; irq++)
		if(!pic_unmask(irq))
			return FALSE;
	return TRUE;
}
