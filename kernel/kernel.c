/**
	@File:			kernel.c
	@Author:		Ihsoh
	@Date:			2014-1-24
	@Description:
		内核。
*/

#include "kernel.h"
#include "types.h"
#include "screen.h"
#include "386.h"
#include "die.h"
#include "keyboard.h"
#include "console.h"
#include "disk.h"
#include "memory.h"
#include "tasks.h"
#include "vesa.h"
#include "sparams.h"
#include "paging.h"
#include "system.h"
#include "lock.h"
#include "log.h"
#include "acpi.h"
#include "mqueue.h"
#include "pci.h"
#include "pic.h"
#include "interrupts.h"
#include "config.h"
#include "cmlock.h"
#include "ahci.h"
#include "serial.h"
#include "dma.h"
#include "kmpool.h"
#include "knlldr.h"
#include "sse.h"
#include "timer.h"
#include "gui.h"
#include "so.h"

#include "syscall/fs.h"
#include "syscall/keyboard.h"
#include "syscall/mouse.h"
#include "syscall/screen.h"
#include "syscall/system.h"

#include "fs/ifs1/fs.h"

#include "pci/pci.h"

#include "sb/sb16.h"

#include <dslib/dslib.h>
#include <jsonlib/jsonlib.h>
#include <pathlib/pathlib.h>
#include <mempoollib/mempoollib.h>

#define	INTERRUPT_PROCEDURE_STACK_SIZE	(MB(1))
#define SYSCALL_PROCEDURE_STACK_SIZE	(MB(1))

static uint32	gdt_addr		= 0;	//GDT的物理地址。
static uint32	idt_addr		= 0;	//IDT的物理地址。
static int32	current_tid;			//当前正在运行的任务的TID。
static BOOL		is_kernel_task	= TRUE;	//当前的任务是否是内核任务。

//内核各个任务的TSS
static struct TSS divby0_tss;		//处理除以0引发的异常的任务的TSS。
static struct TSS bndchk_tss;		//处理边界检查指令引发的异常的任务的TSS。
static struct TSS invalidopc_tss;	//处理执行了错误的指令码引发的异常的任务的TSS。
static struct TSS df_tss;			//处理双重故障的任务的TSS。
static struct TSS invalidtss_tss;	//处理由于错误的TSS所引发的异常的任务的TSS。
static struct TSS invalidseg_tss;	//处理段不存在异常的任务的TSS。
static struct TSS invalidstck_tss;	//处理堆栈故障的任务的TSS。
static struct TSS noimpl_tss;		//处理调用了未实现的中断的情况的任务的TSS。
static struct TSS gp_tss;			//处理通用保护异常的任务的TSS。
static struct TSS pf_tss;			//处理页故障的任务的TSS。
static struct TSS mf_tss;			//处理x87 FPU浮点错误（数学错误）的故障的任务的TSS。
static struct TSS ac_tss;			//处理对齐检查故障的任务的TSS。
static struct TSS mc_tss;			//处理机器检查故障的任务的TSS。
static struct TSS xf_tss;			//处理SIMD浮点异常的任务的TSS。

static struct TSS timer_tss;					//定时器的任务的TSS。
static struct TSS mouse_tss;					//鼠标的任务的TSS。
static struct TSS keyboard_tss;					//键盘的任务的TSS。
static struct TSS ide_tss;						//第一IDE的任务的TSS。
static struct TSS ide1_tss;						//第二IDE的任务的TSS。
static struct TSS fpu_tss;						//FPU的任务的TSS。
static struct TSS peripheral0_tss;				//外设0的任务额TSS。
static struct TSS peripheral1_tss;				//外设1的任务额TSS。
static struct TSS peripheral2_tss;				//外设2的任务额TSS。
static struct TSS peripheral3_tss;				//外设3的任务额TSS。
static struct TSS scall_tss[MAX_TASK_COUNT];	//各个系统调用的任务的TSS。

static uint8 *	divby0_stack		= NULL;
static uint8 *	bndchk_stack		= NULL;
static uint8 *	invalidopc_stack	= NULL;
static uint8 *	df_stack			= NULL;
static uint8 *	invalidtss_stack	= NULL;
static uint8 *	invalidseg_stack	= NULL;
static uint8 *	invalidstck_stack	= NULL;
static uint8 *	noimpl_stack		= NULL;
static uint8 *	gp_stack			= NULL;
static uint8 *	pf_stack			= NULL;
static uint8 *	mf_stack			= NULL;
static uint8 *	ac_stack			= NULL;
static uint8 *	mc_stack			= NULL;
static uint8 *	xf_stack			= NULL;

static BOOL		mouse_loop_was_enabled 		= FALSE;	//!!!警告!!!
														//在使用APIC模式时，不知道为什么
														//在执行_KnlEnterSystem()附近的代码
														//时会引发一次鼠标中断，导致鼠标驱动
														//程序错误的递增了mouse_count。所以
														//在调用完毕_KnlEnterSystem()之后才将
														//mouse_loop_was_enabled设为TRUE。
static BOOL 	keyboard_loop_was_enabled	= FALSE;
static BOOL		use_rtc_for_task_scheduler	= FALSE;
static uint8	rtc_rate					= 15;

static CASCTEXT	_func_name					= "N/A";

static uint32 _ide0_signal				= 0;	// 第一IDE的信号。
static uint32 _ide1_signal				= 0;	// 第二IDE的信号。

#include "knlstatic.h"

extern void _irq0(void);
extern void _irq1(void);
extern void _irq2(void);
extern void _irq3(void);
extern void _irq4(void);
extern void _irq5(void);
extern void _irq6(void);
extern void _irq7(void);
extern void _irq8(void);
extern void _irq9(void);
extern void _irq10(void);
extern void _irq11(void);
extern void _irq12(void);
extern void _irq13(void);
extern void _irq14(void);
extern void _irq15(void);

/**
	@Function:		main
	@Access:		Public
	@Description:
		内核入口函数。
	@Parameters:
	@Return:	
*/
void
main(void)
{	
	gdt_addr = get_gdt_addr();
	idt_addr = get_idt_addr();
	
	MemInit();
	
	//必须在这里禁用所有锁
	KnlDisableCommonLock();
	disable_memory_lock();
	disable_paging_lock();
	disable_kernel_lock();
	disable_tasks_lock();
	disable_console_lock();
	disable_ifs1_lock();
	disable_gui_lock();

	// 启用SSE。
	// In order to allow SSE instructions to be executed without generating a #UD, 
	// we need to alter the CR0 and CR4 registers.
	// clear the CR0.EM bit (bit 2) [ CR0 &= ~(1 << 2) ]
	// set the CR0.MP bit (bit 1) [ CR0 |= (1 << 1) ]
	// set the CR4.OSFXSR bit (bit 9) [ CR4 |= (1 << 9) ]
	// set the CR4.OSXMMEXCPT bit (bit 10) [ CR4 |= (1 << 10) ]
	asm volatile (
		"mov 	%cr0, %eax\n\t"
		"and 	$0xfffb, %ax\n\t"
		"or 	$0x2, %ax\n\t"
		"mov	%eax, %cr0\n\t"
		"mov	%cr4, %eax\n\t"
		"or		$0x3 << 9, %ax\n\t"
		"mov	%eax, %cr4\n\t"
		);

	VesaInit();
	PgInit();

	KnlInitInterrupts();
	_KnlInitUnimplementedInterruptException();
	_KnlInitGlobalProtectionException();
	_KnlInitPageFaultException();
	_KnlInitDividingByZeroException();
	_KnlInitBoundCheckException();
	_KnlInitInvalidOpcodeException();
	_KnlInitDoubleFaultException();
	_KnlInitInvalidTSSException();
	_KnlInitInvalidSegmentException();
	_KnlInitInvalidStackException();
	_KnlInitMathFaultException();
	_KnlInitAlignCheckException();
	_KnlInitMachineCheckException();
	_KnlInitSIMDFloatPointException();
	_KnlInitInterrupt();

	// 初始化PCI。
	PciInitDevices();

	DskInit("VA");
	DskInit("VB");
	DskInit("HD");
	_KnlInitIde();
	_KnlInitIde1();

	// 初始化外部库。DSLIB，JSONLIB，PATHLIB, MEMPOOLLIB。 
	_KnlLibInitDSL();
	_KnlLibInitJson();
	_KnlLibInitPath();
	_KnlLibInitMemPool();

	kmpool_init();

	config_init();

	Ifs1Init();
	ConInit();

	TmrInit();
	EnfntInit();
	ScrInit();
	_KnlInitTimer();
	_KnlInitMouse();
	MouseInit();
	
	_KnlInitKeyboard();
	_KnlInitFpu();
	_KnlInitPeripheral0();
	TskmgrInit();
	_KnlInitSystemCall();
	
	init_keyboard_driver();
	CpuInit();
	PgEnablePaging();
	LogInit();
	MqInit();
	AcpiInit();
	SrlInit();
	GuiInit();

	//检测 APIC 是否初始化成功。
	//如果初始化失败则使用 PIC。
	BOOL enable_apic = TRUE;
	config_system_get_bool("EnableAPIC", &enable_apic);
	if(enable_apic && ApicInit())
	{
		PicMaskAll();
		ApicEnable();
	}
	else
	{
		PicInit();
		PicUnmaskAll();
	}

	Ifs1InitBlocks();
	KnlInitElfSharedObject();

	/*
	uint32 vidtr_addr = (uint32)KNLLDR_HEADER_ITEM(KNLLDR_HEADER_VIDTR);
	asm volatile(
		"pushal\n\t"
		"movl	%0, %%eax\n\t"
		"lidt	(%%eax)\n\t"
		"popal\n\t"
		:
		:"m"(vidtr_addr));
	uint32 delay;
	for(delay = 0; delay < 0x0fffffff; delay++)
		delay = delay;
	*/
	sti();

	//在这里启用所有锁
	KnlEnableCommonLock();
	enable_memory_lock();	
	enable_paging_lock();
	enable_kernel_lock();
	enable_tasks_lock();
	enable_console_lock();
	//enable_ifs1_lock();
	enable_gui_lock();

	DmaInit();
	pci_init();
	_KnlInitDiskVA();

	_KnlEnterSystem();

	config_system_get_bool(	"UseRTCForTaskScheduler", 
							&use_rtc_for_task_scheduler);
	if(enable_apic && ApicIsEnabled())
		if(use_rtc_for_task_scheduler)
		{
			double temp;
			if(config_system_get_number("RTCRate", &temp))
				rtc_rate = (uint8)temp;
			CmosSetRtcRate(rtc_rate);
			CmosEnableRtc();
		}
		else
			ApicStartTimer();
	mouse_loop_was_enabled = TRUE;
	keyboard_loop_was_enabled = TRUE;

	COMMON_LOCK();
	int32 sys_screen_tid = TskmgrCreateSystemTaskByFile(
		SYSTEM_PATH"sys/screen.sys",
		SYSTEM_PATH"sys/screen.sys",
		SYSTEM_PATH"sys/");
	TskmgrGetTaskInfoPtr(sys_screen_tid)->priority = TASK_PRIORITY_HIGH;
	TskmgrSetTaskToReady(sys_screen_tid);

	int32 sys_timer_tid = TskmgrCreateSystemTaskByFile(
		SYSTEM_PATH"sys/timer.sys",
		SYSTEM_PATH"sys/timer.sys",
		SYSTEM_PATH"sys/");
	TskmgrGetTaskInfoPtr(sys_timer_tid)->priority = TASK_PRIORITY_LOW;
	TskmgrSetTaskToReady(sys_timer_tid);
	COMMON_UNLOCK();

	ConEnterConsole();
}

DEFINE_LOCK_IMPL(kernel)

/**
	@Function:		KnlIsCurrentlyKernelTask
	@Access:		Public
	@Description:
		获取当前任务是否为内核任务。
	@Parameters:
	@Return:
		BOOL
			如果是内核任务则返回TRUE，否则返回FALSE。
*/
BOOL
KnlIsCurrentlyKernelTask(void)
{
	return is_kernel_task;
}

/**
	@Function:		KnlGetCurrentTaskId
	@Access:		Public
	@Description:
		获取当前的用户任务的TID。
	@Parameters:
	@Return:
		int32
			任务的TID。
*/
int32
KnlGetCurrentTaskId(void)
{
	return current_tid;
}

/**
	@Function:		_KnlResetAllExceptions
	@Access:		Private
	@Description:
		重置所有异常处理程序。
		把所有异常处理程序的 TSS 的描述符的 ATTR 字段设为
		AT386TSS + DPL0，就是去掉 BUSY 状态。
	@Parameters:
	@Return:
*/
static
void
_KnlResetAllExceptions(void)
{
	struct Desc tss_desc;

	#define RESET_EXCEPTION(__desc_idx, __tss, __eip, __esp)	\
		KnlGetDescFromGDT(__desc_idx, &tss_desc);	\
		tss_desc.attr = AT386TSS + DPL0;	\
		KnlSetDescToGDT(__desc_idx, &tss_desc);	\
		_KnlFillTSS(__tss, __eip, __esp);

	// 除以0。
	RESET_EXCEPTION(21, &divby0_tss, &_KnlDividingByZeroExceptionInterrupt, divby0_stack);

	// 边界检查。
	RESET_EXCEPTION(22, &bndchk_tss, &_KnlBoundCheckExceptionInterrupt, bndchk_stack);
	
	// 错误的操作码。
	RESET_EXCEPTION(23, &invalidopc_tss, &_KnlInvalidOpcodeExceptionInterrupt, invalidopc_stack);
	
	// 错误的 TSS。
	RESET_EXCEPTION(24, &invalidtss_tss, &_KnlInvalidTSSExceptionInterrupt, invalidtss_stack);
	
	// 段不存在故障。
	RESET_EXCEPTION(25, &invalidseg_tss, &_KnlInvalidSegmentExceptionInterrupt, invalidseg_stack);

	// 堆栈段故障。
	RESET_EXCEPTION(26, &invalidstck_tss, &_KnlInvalidStackExceptionInterrupt, invalidstck_stack);

	// 未实现中断。
	RESET_EXCEPTION(27, &noimpl_tss, &_KnlUnimplementedInterrupt, noimpl_stack);

	// 通用保护异常。
	RESET_EXCEPTION(18, &gp_tss, &_KnlGlobalProtectionExceptionInterrupt, gp_stack);

	// 页故障。
	RESET_EXCEPTION(20, &pf_tss, &_KnlPageFaultExceptionInterrupt, pf_stack);

	// x87 FPU浮点错误（数学错误）的故障。
	RESET_EXCEPTION(29, &mf_tss, &_KnlMathFaultExceptionInterrupt, mf_stack);

	// 对齐检查故障。
	RESET_EXCEPTION(158, &ac_tss, &_KnlAlignCheckExceptionInterrupt, ac_stack);

	// SIMD浮点异常。
	RESET_EXCEPTION(160, &xf_tss, &_KnlSIMDFloatPointExceptionInterrupt, xf_stack);
}

/**
	@Function:		_KnlInitDiskVA
	@Access:		Private
	@Description:
		初始化虚拟磁盘 VA。
	@Parameters:
	@Return:
*/
static
void
_KnlInitDiskVA(void)
{
	Ifs1Format("VA");
	Ifs1CreateDir("VA:/", "data");
	
	Ifs1CreateFile("VA:/data/", "pci.dat");
	pci_write_to_file("VA:/data/pci.dat");
	
	Ifs1CreateFile("VA:/data/", "cpu.dat");
	CpuWriteInfoToFile("VA:/data/cpu.dat");

	Ifs1CreateFile("VA:/data/", "madt.dat");
	MadtWriteToFile("VA:/data/madt.dat");
}

/**
	@Function:		_KnlLibMalloc
	@Access:		Private
	@Description:
		为库准备的释放 malloc 函数。
	@Parameters:
		num_bytes, uint32, IN
			分配大小。
	@Return:
		void *
			分配的内存的指针。返回 NULL 则失败。
*/
static
void *
_KnlLibMalloc(IN uint32 num_bytes)
{
	return MemAlloc(num_bytes);
}

/**
	@Function:		_KnlLibCalloc
	@Access:		Private
	@Description:
		为库准备的释放 calloc 函数。
	@Parameters:
		n, uint32, IN
			分配数量。
		size, uint32, IN
			分配大小。
	@Return:
		void *
			分配的内存的指针。返回 NULL 则失败。
*/
static
void *
_KnlLibCalloc(	IN uint32 n, 
				IN uint32 size)
{
	return MemAlloc(n * size);
}

/**
	@Function:		_KnlLibFree
	@Access:		Private
	@Description:
		为 DSL 库准备的释放 free 函数。
	@Parameters:
		ptr, void *, IN
			要释放的内存的指针。
	@Return:
*/
static
void
_KnlLibFree(IN void * ptr)
{
	MemFree(ptr);
}

/**
	@Function:		_KnlLibInitDSL
	@Access:		Private
	@Description:
		初始化 DSL 库。
	@Parameters:
	@Return:
		BOOL
			返回 TRUE 则成功，否则失败。
*/
static
BOOL
_KnlLibInitDSL(void)
{
	DSLEnvironment env;
	env.dsl_malloc = _KnlLibMalloc;
	env.dsl_calloc = _KnlLibCalloc;
	env.dsl_free = _KnlLibFree;
	return dsl_init(&env);
}

/**
	@Function:		_KnlLibInitJson
	@Access:		Private
	@Description:
		初始化 JSON 库。
	@Parameters:
	@Return:
		BOOL
			返回 TRUE 则成功，否则失败。
*/
static
BOOL
_KnlLibInitJson(void)
{
	JSONLEnvironment env;
	env.jsonl_malloc = _KnlLibMalloc;
	env.jsonl_calloc = _KnlLibCalloc;
	env.jsonl_free = _KnlLibFree;
	return jsonl_init(&env);
}

/**
	@Function:		_KnlLibInitPath
	@Access:		Private
	@Description:
		初始化 PATH 库。
	@Parameters:
	@Return:
		BOOL
			返回 TRUE 则成功，否则失败。
*/
static
BOOL
_KnlLibInitPath(void)
{
	PATHLEnvironment env;
	env.pathl_malloc = _KnlLibMalloc;
	env.pathl_calloc = _KnlLibCalloc;
	env.pathl_free = _KnlLibFree;
	return pathl_init(&env);
}

/**
	@Function:		_KnlLibInitMemPool
	@Access:		Private
	@Description:
		初始化 MEMPOOL 库。
	@Parameters:
	@Return:
		BOOL
			返回 TRUE 则成功，否则失败。
*/
static
BOOL
_KnlLibInitMemPool(void)
{
	MEMPOOLLEnvironment env;
	env.mempooll_malloc = _KnlLibMalloc;
	env.mempooll_calloc = _KnlLibCalloc;
	env.mempooll_free = _KnlLibFree;
	return mempooll_init(&env);
}

/**
	@Function:		_KnlIrqAck
	@Access:		Private
	@Description:
		发送 EOI。
	@Parameters:
		no, uint32, IN
			IRQ Number。
	@Return:	
*/
static
void
_KnlIrqAck(IN uint32 no)
{
	if(ApicIsEnabled())
		ApicEOI();
	else
	{
		if(no >= 8)
			KnlOutByte(0xa0, 0x20);
		KnlOutByte(0x20, 0x20);
	}
}

/**
	@Function:		_KnlInitInterrupt
	@Access:		Private
	@Description:
		初始化中断。
	@Parameters:
	@Return:	
*/
static
void
_KnlInitInterrupt(void)
{
	uint32 fpu_error_int_addr = (uint32)&_KnlFpuErrorInterrupt;
	KnlSetInterrupt(0x07, fpu_error_int_addr);
	uint32 system_call_int_addr = (uint32)&_KnlSystemCallInterrupt;
	KnlSetInterrupt(0xa0, system_call_int_addr);
}

/**
	@Function:		_KnlFpuErrorInterrupt
	@Access:		Private
	@Description:
		FPU 错误的中断处理函数。
	@Parameters:
	@Return:		
*/
static
void
_KnlFpuErrorInterrupt(void)
{
	asm volatile ("clts");
	INT_EXIT();
}

/**
	@Function:		_KnlFillTSS
	@Access:		Public
	@Description:
		填充 TSS 结构体。
	@Parameters:
		tss, struct TSS *, OUT
			参数1的说明
		eip, uint32, IN
			TSS 的 EIP 域。
		esp, uint32, IN
			TSS 的 ESP 域。
	@Return:	
*/
static
void
_KnlFillTSS(OUT struct TSS * tss, 
			IN uint32 eip,
			IN uint32 esp)
{
	tss->back_link = 0;
	tss->esp0 = esp + INTERRUPT_PROCEDURE_STACK_SIZE;
	tss->ss0 = KERNEL_D_DESC;
	tss->esp1 = 0;
	tss->ss1 = 0;
	tss->esp2 = 0;
	tss->ss2 = 0;
	tss->cr3 = PgGetKernelCR3();
	tss->eip = eip;
	tss->flags = 0x0; //x200;
	tss->eax = 0;
	tss->ecx = 0;
	tss->edx = 0;
	tss->ebx = 0;
	tss->esp = esp + INTERRUPT_PROCEDURE_STACK_SIZE;
	tss->ebp = 0;
	tss->esi = 0;
	tss->edi = 0;
	tss->es = KERNEL_D_DESC;
	tss->cs = KERNEL_C_DESC;
	tss->ss = KERNEL_D_DESC;
	tss->ds = KERNEL_D_DESC;
	tss->fs = KERNEL_D_DESC;
	tss->gs = KERNEL_D_DESC;
	tss->ldt = 0;
	tss->trap = 0;
	tss->iobase = sizeof(struct TSS);
}

static uint32 _timer_stack = 0;

/**
	@Function:		_KnlInitTimer
	@Access:		Private
	@Description:
		初始化定时器的中断程序。
	@Parameters:
	@Return:		
*/
static
void
_KnlInitTimer(void)
{
	struct die_info info;
	struct TSS * tss = &timer_tss;
	uint8 * stack = (uint8 *)MemAlloc(INTERRUPT_PROCEDURE_STACK_SIZE);
	_timer_stack = (uint32)stack;
	if(tss == NULL)
	{
		fill_info(info, DC_INIT_TIMER, DI_INIT_TIMER);
		die(&info);
	}
	struct Desc tss_desc;
	struct Gate task_gate;		

	uint32 temp = (uint32)tss;
	tss_desc.limitl = sizeof(struct TSS) - 1;
	tss_desc.basel = (uint16)(temp & 0xFFFF);
	tss_desc.basem = (uint8)((temp >> 16) & 0xFF);
	tss_desc.baseh = (uint8)((temp >> 24) & 0xFF);
	tss_desc.attr = AT386TSS + DPL0;
	KnlSetDescToGDT(10, (uint8 *)&tss_desc);

	task_gate.offsetl = 0;
	task_gate.offseth = 0;
	task_gate.dcount = 0;
	task_gate.selector = (10 << 3) | RPL0;
	//task_gate.attr = ATTASKGATE | DPL0;
	task_gate.attr = ATTASKGATE | DPL3;
	
	// IRQ 0
	//KnlSetGateToIDT(0x40, (uint8 *)&task_gate);
	KnlSetDescToGDT(161, (uint8 *)&task_gate);
	KnlSetInterruptGate(0x40, _irq0);

	// IRQ 8
	KnlSetGateToIDT(0x70, (uint8 *)&task_gate);
	

	_KnlFillTSS(tss, (uint32)_KnlTimerInterrupt, (uint32)stack);
}

/**
	@Function:		_KnlInitMouse
	@Access:		Private
	@Description:
		初始化鼠标的中断程序。
	@Parameters:
	@Return:	
*/
static
void
_KnlInitMouse(void)
{
	struct die_info info;
	struct TSS * tss = &mouse_tss;
	uint8 * stack = (uint8 *)MemAlloc(INTERRUPT_PROCEDURE_STACK_SIZE);
	if(tss == NULL)
	{
		fill_info(info, DC_INIT_MOUSE, DI_INIT_MOUSE);
		die(&info);
	}
	struct Desc tss_desc;
	struct Gate task_gate;		

	uint32 temp = (uint32)tss;
	tss_desc.limitl = sizeof(struct TSS) - 1;
	tss_desc.basel = (uint16)(temp & 0xFFFF);
	tss_desc.basem = (uint8)((temp >> 16) & 0xFF);
	tss_desc.baseh = (uint8)((temp >> 24) & 0xFF);
	tss_desc.attr = AT386TSS + DPL0;
	KnlSetDescToGDT(19, (uint8 *)&tss_desc);

	task_gate.offsetl = 0;
	task_gate.offseth = 0;
	task_gate.dcount = 0;
	task_gate.selector = (19 << 3) | RPL0;
	//task_gate.attr = ATTASKGATE | DPL0;
	task_gate.attr = ATTASKGATE | DPL3;

	// IRQ12
	//KnlSetGateToIDT(0x74, (uint8 *)&task_gate);
	KnlSetDescToGDT(163, (uint8 *)&task_gate);
	KnlSetInterruptGate(0x74, _irq12);

	_KnlFillTSS(tss, (uint32)_KnlMouseInterrupt, (uint32)stack);
}

/**
	@Function:		_KnlInitKeyboard
	@Access:		Private
	@Description:
		初始化键盘的中断程序。
	@Parameters:
	@Return:		
*/
static
void
_KnlInitKeyboard(void)
{
	struct die_info info;
	struct TSS * tss = &keyboard_tss;
	uint8 * stack = (uint8 *)MemAlloc(INTERRUPT_PROCEDURE_STACK_SIZE);
	if(tss == NULL)
	{
		fill_info(info, DC_INIT_KEYBOARD, DI_INIT_KEYBOARD);
		die(&info);
	}
	struct Desc tss_desc;
	struct Gate task_gate;		

	uint32 temp = (uint32)tss;
	tss_desc.limitl = sizeof(struct TSS) - 1;
	tss_desc.basel = (uint16)(temp & 0xFFFF);
	tss_desc.basem = (uint8)((temp >> 16) & 0xFF);
	tss_desc.baseh = (uint8)((temp >> 24) & 0xFF);
	tss_desc.attr = AT386TSS + DPL0;
	KnlSetDescToGDT(12, (uint8 *)&tss_desc);

	task_gate.offsetl = 0;
	task_gate.offseth = 0;
	task_gate.dcount = 0;
	task_gate.selector = (12 << 3) | RPL0;
	//task_gate.attr = ATTASKGATE | DPL0;
	task_gate.attr = ATTASKGATE | DPL3;
	
	// IRQ1
	// KnlSetGateToIDT(0x41, (uint8 *)&task_gate);
	KnlSetInterruptGate(0x41, _irq1);
	KnlSetDescToGDT(162, (uint8 *)&task_gate);

	_KnlFillTSS(tss, (uint32)_KnlKeyboardInterrupt, (uint32)stack);
}

/**
	@Function:		_KnlInitIde
	@Access:		Private
	@Description:
		初始化第一IDE的中断程序。
	@Parameters:
	@Return:	
*/
static
void
_KnlInitIde(void)
{
	struct die_info info;
	struct TSS * tss = &ide_tss;
	uint8 * stack = (uint8 *)MemAlloc(INTERRUPT_PROCEDURE_STACK_SIZE);
	if(tss == NULL)
	{
		fill_info(info, DC_INIT_IDE, DI_INIT_IDE);
		die(&info);
	}
	struct Desc tss_desc;
	struct Gate task_gate;		

	uint32 temp = (uint32)tss;
	tss_desc.limitl = sizeof(struct TSS) - 1;
	tss_desc.basel = (uint16)(temp & 0xFFFF);
	tss_desc.basem = (uint8)((temp >> 16) & 0xFF);
	tss_desc.baseh = (uint8)((temp >> 24) & 0xFF);
	tss_desc.attr = AT386TSS + DPL0;
	KnlSetDescToGDT(13, (uint8 *)&tss_desc);

	task_gate.offsetl = 0;
	task_gate.offseth = 0;
	task_gate.dcount = 0;
	task_gate.selector = (13 << 3) | RPL0;
	//task_gate.attr = ATTASKGATE | DPL0;
	task_gate.attr = ATTASKGATE | DPL3;
	
	// IRQ14
	//KnlSetGateToIDT(0x76, (uint8 *)&task_gate);
	KnlSetDescToGDT(164, (uint8 *)&task_gate);
	KnlSetInterruptGate(0x76, _irq14);

	_KnlFillTSS(tss, (uint32)_KnlIdeInterrupt, (uint32)stack);
}

/**
	@Function:		_KnlInitIde1
	@Access:		Private
	@Description:
		初始化第二IDE的中断程序。
	@Parameters:
	@Return:	
*/
static
void
_KnlInitIde1(void)
{
	struct die_info info;
	struct TSS * tss = &ide1_tss;
	uint8 * stack = (uint8 *)MemAlloc(INTERRUPT_PROCEDURE_STACK_SIZE);
	if(tss == NULL)
	{
		fill_info(info, DC_INIT_IDE, DI_INIT_IDE);
		die(&info);
	}
	struct Desc tss_desc;
	struct Gate task_gate;		

	uint32 temp = (uint32)tss;
	tss_desc.limitl = sizeof(struct TSS) - 1;
	tss_desc.basel = (uint16)(temp & 0xFFFF);
	tss_desc.basem = (uint8)((temp >> 16) & 0xFF);
	tss_desc.baseh = (uint8)((temp >> 24) & 0xFF);
	tss_desc.attr = AT386TSS + DPL0;
	KnlSetDescToGDT(14, (uint8 *)&tss_desc);

	task_gate.offsetl = 0;
	task_gate.offseth = 0;
	task_gate.dcount = 0;
	task_gate.selector = (14 << 3) | RPL0;
	//task_gate.attr = ATTASKGATE | DPL0;
	task_gate.attr = ATTASKGATE | DPL3;
	
	// IRQ15
	//KnlSetGateToIDT(0x77, (uint8 *)&task_gate);
	KnlSetDescToGDT(166, (uint8 *)&task_gate);
	KnlSetInterruptGate(0x77, _irq15);

	_KnlFillTSS(tss, (uint32)_KnlIde1Interrupt, (uint32)stack);
}

/**
	@Function:		_KnlInitFpu
	@Access:		Private
	@Description:
		初始化 FPU 的中断程序。
	@Parameters:
	@Return:
*/
static
void
_KnlInitFpu(void)
{
	struct die_info info;
	struct TSS * tss = &fpu_tss;
	uint8 * stack = (uint8 *)MemAlloc(INTERRUPT_PROCEDURE_STACK_SIZE);
	if(tss == NULL)
	{
		fill_info(info, DC_INIT_FPU, DI_INIT_FPU);
		die(&info);
	}
	struct Desc tss_desc;
	struct Gate task_gate;

	uint32 temp = (uint32)tss;
	tss_desc.limitl = sizeof(struct TSS) - 1;
	tss_desc.basel = (uint16)(temp & 0xFFFF);
	tss_desc.basem = (uint8)((temp >> 16) & 0xFF);
	tss_desc.baseh = (uint8)((temp >> 24) & 0xFF);
	tss_desc.attr = AT386TSS + DPL0;
	KnlSetDescToGDT(16, (uint8 *)&tss_desc);

	task_gate.offsetl = 0;
	task_gate.offseth = 0;
	task_gate.dcount = 0;
	task_gate.selector = (16 << 3) | RPL0;
	//task_gate.attr = ATTASKGATE | DPL0;
	task_gate.attr = ATTASKGATE | DPL3;

	// IRQ13
	//KnlSetGateToIDT(0x75, (uint8 *)&task_gate);
	KnlSetDescToGDT(165, (uint8 *)&task_gate);
	KnlSetInterruptGate(0x75, _irq13);
	
	_KnlFillTSS(tss, (uint32)_KnlFpuInterrupt, (uint32)stack);
}

/**
	@Function:		_KnlInitPeripheral0
	@Access:		Private
	@Description:
		初始化外部设备0的中断程序。
	@Parameters:
	@Return:
*/
static
void
_KnlInitPeripheral0(void)
{
	struct die_info info;
	struct TSS * tss = &peripheral0_tss;
	uint8 * stack = (uint8 *)MemAlloc(INTERRUPT_PROCEDURE_STACK_SIZE);
	if(tss == NULL)
	{
		fill_info(info, DC_INIT_PERIPHERAL0, DI_INIT_PERIPHERAL0);
		die(&info);
	}
	struct Desc tss_desc;
	struct Gate task_gate;

	uint32 temp = (uint32)tss;
	tss_desc.limitl = sizeof(struct TSS) - 1;
	tss_desc.basel = (uint16)(temp & 0xFFFF);
	tss_desc.basem = (uint8)((temp >> 16) & 0xFF);
	tss_desc.baseh = (uint8)((temp >> 24) & 0xFF);
	tss_desc.attr = AT386TSS + DPL0;
	KnlSetDescToGDT(167, (uint8 *)&tss_desc);

	task_gate.offsetl = 0;
	task_gate.offseth = 0;
	task_gate.dcount = 0;
	task_gate.selector = (167 << 3) | RPL0;
	//task_gate.attr = ATTASKGATE | DPL0;
	task_gate.attr = ATTASKGATE | DPL3;

	// IRQ5
	//KnlSetGateToIDT(0x45, (uint8 *)&task_gate);
	KnlSetDescToGDT(168, (uint8 *)&task_gate);
	KnlSetInterruptGate(0x45, _irq5);
	
	_KnlFillTSS(tss, (uint32)_KnlPeripheral0Interrupt, (uint32)stack);
}

/**
	@Function:		_KnlFreeSystemCallTSS
	@Access:		Private
	@Description:
		释放系统调用的 TSS。清除系统调用的 TSS 的 BUSY 状态。
	@Parameters:
	@Return:
*/
static
void
_KnlFreeSystemCallTSS(void)
{
	uint32 ui;
	for(ui = 0; ui < MAX_TASK_COUNT; ui++)
	{
		struct Desc system_call_tss_desc;
		KnlGetDescFromGDT(30 + ui * 2, (uint8 *)&system_call_tss_desc);
		system_call_tss_desc.attr = AT386TSS + DPL3; 
		KnlSetDescToGDT(30 + ui * 2, (uint8 *)&system_call_tss_desc);
	}
}

/**
	@Function:		_KnlFreeTaskTSS
	@Access:		Private
	@Description:
		释放所有任务的 TSS。清除所有任务的 TSS 的 BUSY 状态。
	@Parameters:
	@Return:	
*/
static
void
_KnlFreeTaskTSS(void)
{
	uint32 ui;
	for(ui = 0; ui < MAX_TASK_COUNT; ui++)
	{
		struct Desc tss_desc;
		KnlGetDescFromGDT(400 + ui * 5 + 0, (uint8 *)&tss_desc);
		tss_desc.attr = AT386TSS + DPL3;
		KnlSetDescToGDT(400 + ui * 5 + 0, (uint8 *)&tss_desc);
	}
}

static int32	counter						= 1;
static int32	clock_counter				= 100;
static int32	is_system_call				= 0;
static BOOL		switch_to_kernel			= FALSE;
static BOOL		kt_jk_lock					= TRUE;
static BOOL		will_reset_all_exceptions	= FALSE;

static BOOL				kernel_init_i387	= FALSE;
static struct I387State	kernel_i387_state;
static BOOL				kernel_task_ran		= FALSE;
static SSEState			kernel_sse_state;

/**
	@Function:		_KnlTimerInterrupt
	@Access:		Private
	@Description:
		定时器的中断程序。
		该版本的任务调度无法在有任务进行系统调用时，
		进行任务切换。
	@Parameters:
	@Return:	
*/
static
void
_KnlTimerInterrupt(void)
{
	while(1)
	{
		lock();

		_func_name = __FUNCTION__;

		TmrIncTicks();

		if(!use_rtc_for_task_scheduler && ApicIsEnabled())
			ApicStopTimer();
		kt_jk_lock = FALSE;
		if(will_reset_all_exceptions)
		{
			_KnlResetAllExceptions();
			will_reset_all_exceptions = FALSE;
		}
		if(--clock_counter == 0)
		{
			ConUpdateClock();
			clock_counter = 100;
		}
		int32 running_tid = TaskmgrGetRunningTaskID();
		int32 tid = TskmgrGetNextTaskID();
		current_tid = tid;
		int32 task_count = TaskmgrGetTaskCount();
		if(counter >= task_count || tid == -1 || switch_to_kernel)
		{
			switch_to_kernel = FALSE;
			counter = 0;
			is_kernel_task = TRUE;
			struct Task task;

			//如果在执行内核任务之前有其他任务，
			//则保存任务的I387的状态。
			if(running_tid != -1)
			{
				TskmgrGetTaskInfo(running_tid, &task);
				asm volatile ("fnsave %0"::"m"(task.i387_state));
				asm volatile ("fxsave %0"::"m"(task.sse_state));
				TskmgrSetTaskInfo(running_tid, &task);
			}

			//加载内核的I387的状态，如果未初始化则先初始化。
			if(kernel_init_i387)
			{
				if(!kernel_task_ran)
				{
					asm volatile ("frstor %0"::"m"(kernel_i387_state));
					asm volatile ("fxrstor %0"::"m"(kernel_sse_state));
				}
			}
			else
			{
				asm volatile ("fninit");
				kernel_init_i387 = TRUE;
			}

			kernel_task_ran = TRUE;

			// 释放内核的TSS。
			struct Desc kernel_tss_desc;
			KnlGetDescFromGDT(6, (uint8 *)&kernel_tss_desc);
			kernel_tss_desc.attr = AT386TSS + DPL0; 
			KnlSetDescToGDT(6, (uint8 *)&kernel_tss_desc);

			// 释放所有系统调用的TSS。
			_KnlFreeSystemCallTSS();

			if(!use_rtc_for_task_scheduler && ApicIsEnabled())
				ApicStartTimer();
			CmosEndOfRtc();
			unlock_without_sti();
			if(use_rtc_for_task_scheduler)
				_KnlIrqAck(8);
			else
				_KnlIrqAck(0);
			asm volatile ("ljmp	$56, $0;");
		}
		else
		{
			counter++;
			is_kernel_task = FALSE;
			struct Task task;

			if(kernel_task_ran)
			{
				//刚才执行的是内核任务，所以保存内核的I387状态。
				asm volatile ("fnsave %0"::"m"(kernel_i387_state));
				asm volatile ("fxsave %0"::"m"(kernel_sse_state));
				kernel_task_ran = FALSE;
			}
			else
				if(running_tid != -1)
				{
					//刚才执行的不是内核任务，保存上一个任务的I387状态。
					TskmgrGetTaskInfo(running_tid, &task);
					asm volatile ("fnsave %0"::"m"(task.i387_state));
					asm volatile ("fxsave %0"::"m"(task.sse_state));
					TskmgrSetTaskInfo(running_tid, &task);
				}

			TskmgrGetTaskInfo(tid, &task);
			if(task.init_i387)
			{
				asm volatile ("frstor %0"::"m"(task.i387_state));
				asm volatile ("fxrstor %0"::"m"(task.sse_state));
			}
			else
			{
				asm volatile ("fninit");
				task.init_i387 = 1;
				TskmgrSetTaskInfo(tid, &task);
			}
			TaskmgrSetTaskRunningStatus(tid);
			_KnlFreeSystemCallTSS();
			if(!use_rtc_for_task_scheduler && ApicIsEnabled())
				ApicStartTimer();
			CmosEndOfRtc();
			unlock_without_sti();
			if(use_rtc_for_task_scheduler)
				_KnlIrqAck(8);
			else
				_KnlIrqAck(0);

			if(task.is_system_call)
			{
				#define SYSTEM_CALL(tid_s)	\
					asm volatile (	".set _TMR_TARGET"tid_s", (((30 + "tid_s" * 2 + 1) << 3) | 3)\n\t"	\
									"ljmp	$_TMR_TARGET"tid_s", $0\n\t");

				#define SYSTEM_CALL_CASE(tid, tid_s)	\
					case tid:	\
						SYSTEM_CALL(tid_s);	\
						break;

				switch(tid)
				{
					SYSTEM_CALL_CASE(0, "0")
					SYSTEM_CALL_CASE(1, "1")
					SYSTEM_CALL_CASE(2, "2")
					SYSTEM_CALL_CASE(3, "3")
					SYSTEM_CALL_CASE(4, "4")
					SYSTEM_CALL_CASE(5, "5")
					SYSTEM_CALL_CASE(6, "6")
					SYSTEM_CALL_CASE(7, "7")
					SYSTEM_CALL_CASE(8, "8")
					SYSTEM_CALL_CASE(9, "9")
					SYSTEM_CALL_CASE(10, "10")
					SYSTEM_CALL_CASE(11, "11")
					SYSTEM_CALL_CASE(12, "12")
					SYSTEM_CALL_CASE(13, "13")
					SYSTEM_CALL_CASE(14, "14")
					SYSTEM_CALL_CASE(15, "15")
					SYSTEM_CALL_CASE(16, "16")
					SYSTEM_CALL_CASE(17, "17")
					SYSTEM_CALL_CASE(18, "18")
					SYSTEM_CALL_CASE(19, "19")
					SYSTEM_CALL_CASE(20, "20")
					SYSTEM_CALL_CASE(21, "21")
					SYSTEM_CALL_CASE(22, "22")
					SYSTEM_CALL_CASE(23, "23")
					SYSTEM_CALL_CASE(24, "24")
					SYSTEM_CALL_CASE(25, "25")
					SYSTEM_CALL_CASE(26, "26")
					SYSTEM_CALL_CASE(27, "27")
					SYSTEM_CALL_CASE(28, "28")
					SYSTEM_CALL_CASE(29, "29")
					SYSTEM_CALL_CASE(30, "30")
					SYSTEM_CALL_CASE(31, "31")
					SYSTEM_CALL_CASE(32, "32")
					SYSTEM_CALL_CASE(33, "33")
					SYSTEM_CALL_CASE(34, "34")
					SYSTEM_CALL_CASE(35, "35")
					SYSTEM_CALL_CASE(36, "36")
					SYSTEM_CALL_CASE(37, "37")
					SYSTEM_CALL_CASE(38, "38")
					SYSTEM_CALL_CASE(39, "39")
					SYSTEM_CALL_CASE(40, "40")
					SYSTEM_CALL_CASE(41, "41")
					SYSTEM_CALL_CASE(42, "42")
					SYSTEM_CALL_CASE(43, "43")
					SYSTEM_CALL_CASE(44, "44")
					SYSTEM_CALL_CASE(45, "45")
					SYSTEM_CALL_CASE(46, "46")
					SYSTEM_CALL_CASE(47, "47")
					SYSTEM_CALL_CASE(48, "48")
					SYSTEM_CALL_CASE(49, "49")
					SYSTEM_CALL_CASE(50, "50")
					SYSTEM_CALL_CASE(51, "51")
					SYSTEM_CALL_CASE(52, "52")
					SYSTEM_CALL_CASE(53, "53")
					SYSTEM_CALL_CASE(54, "54")
					SYSTEM_CALL_CASE(55, "55")
					SYSTEM_CALL_CASE(56, "56")
					SYSTEM_CALL_CASE(57, "57")
					SYSTEM_CALL_CASE(58, "58")
					SYSTEM_CALL_CASE(59, "59")
					SYSTEM_CALL_CASE(60, "60")
					SYSTEM_CALL_CASE(61, "61")
					SYSTEM_CALL_CASE(62, "62")
					SYSTEM_CALL_CASE(63, "63")
				}

				#undef	SYSTEM_CALL
				#undef	SYSTEM_CALL_CASE
			}
			else
			{
				struct Desc tss_desc;
				struct Gate task_gate;
				KnlGetDescFromGDT(400 + tid * 5 + 0, (uint8 *)&tss_desc);
				KnlGetDescFromGDT(400 + tid * 5 + 1, (uint8 *)&task_gate);
				tss_desc.attr = AT386TSS + DPL3;
				KnlSetDescToGDT(400 + tid * 5 + 0, (uint8 *)&tss_desc);
				KnlSetDescToGDT(11, (uint8 *)&task_gate);
				asm volatile ("ljmp	$88, $0;");
			}
		}
	}
}

static int32	old_mouse_x = 0;
static int32	old_mouse_y = 0;
static int32	mouse_x = 0;
static int32	mouse_y = 0;
static BOOL		mouse_left_button_down = 0;
static BOOL		mouse_right_button_down = 0;
static int32	mouse_count = 0;
static int32 	x_sign = 0;
static int32	y_sign = 0;

#define MOUSE_DEVICE	0x60
#define MOUSE_STATUS	0x64
#define MOUSE_ABIT		0x02
#define MOUSE_BBIT		0x01
#define MOUSE_WRITE		0xD4
#define MOUSE_F_BIT		0x20
#define MOUSE_V_BIT		0x08

/**
	@Function:		_KnlMouseInterrupt
	@Access:		Private
	@Description:
		鼠标的中断程序。
	@Parameters:
	@Return:		
*/
static
void
_KnlMouseInterrupt(void)
{
	while(1)
	{
		lock();
		uint8 status = KnlInByte(MOUSE_STATUS);
		while(status & MOUSE_BBIT)
		{
			if(status & MOUSE_F_BIT)
			{
				int32 max_screen_width = VesaGetWidth();
				int32 max_screen_height = VesaGetHeight();
				uint8 data = KnlInByte(MOUSE_DEVICE);

				if(mouse_loop_was_enabled)
					switch(mouse_count)
					{
						case 0:
							if(!(data & MOUSE_V_BIT))
								goto end;
							if(data & 0x01)
								mouse_left_button_down = TRUE;
							else
								mouse_left_button_down = FALSE;
							if(data & 0x02)
								mouse_right_button_down = TRUE;
							else
								mouse_right_button_down = FALSE;
							x_sign = data & 0x10 ? 0xffffff00 : 0;
							y_sign = data & 0x20 ? 0xffffff00 : 0;
							mouse_count++;
							break;
						case 1:
							mouse_x += (x_sign | data);
							if(mouse_x < 0)
								mouse_x = 0;
							else if(mouse_x >= max_screen_width)
								mouse_x = max_screen_width - 1;
							mouse_count++;
							break;
						case 2:
							mouse_y += -(y_sign | data);
							if(mouse_y < 0)
								mouse_y = 0;
							else if(mouse_y >= max_screen_height)
								mouse_y = max_screen_height - 1;
							mouse_count = 0;
							break;
					}
			}
			status = KnlInByte(MOUSE_STATUS);
		}
end:
		unlock_without_sti();
		_KnlIrqAck(12);
		asm volatile ("iret");
	}
}

/**
	@Function:		KnlGetMousePosition
	@Access:		Public
	@Description:
		获取鼠标的位置。
	@Parameters:
		x, int32 *, OUT
			鼠标的 X 坐标。
		y, int32 *, OUT
			鼠标的 Y 坐标。
	@Return:	
*/
void
KnlGetMousePosition(OUT int32 * x,
					OUT int32 * y)
{
	if(mouse_count == 0)
	{
		*x = mouse_x;
		*y = mouse_y;
		old_mouse_x = mouse_x;
		old_mouse_y = mouse_y;
	}
	else
	{
		*x = old_mouse_x;
		*y = old_mouse_y;
	}
}

/**
	@Function:		KnlIsMouseLeftButtonDown
	@Access:		Public
	@Description:
		获取鼠标的左键是否被按下。
	@Parameters:
	@Return:
		BOOL
			返回TRUE则被按下，否则没被按下。
*/
BOOL
KnlIsMouseLeftButtonDown(void)
{
	if(mouse_count == 0)
		return mouse_left_button_down;
	else
		return FALSE;
}

/**
	@Function:		KnlIsMouseRightButtonDown
	@Access:		Public
	@Description:
		获取鼠标的右键是否被按下。
	@Parameters:
	@Return:
		BOOL
			返回TRUE则被按下，否则没被按下。
*/
BOOL
KnlIsMouseRightButtonDown(void)
{
	if(mouse_count == 0)
		return mouse_right_button_down;
	else
		return FALSE;
}

#define KEYBOARD_DEVICE  0x60
#define KEYBOARD_PENDING 0x64

/**
	@Function:		_KnlKeyboardInterrupt
	@Access:		Private
	@Description:
		键盘的中断程序。
	@Parameters:
	@Return:
*/
static
void
_KnlKeyboardInterrupt(void)
{
	while(1)
	{
		lock();
		uint8 scan_code = 0;
		while(KnlInByte(KEYBOARD_PENDING) & 2);
		scan_code = KnlInByte(KEYBOARD_DEVICE);
		if(keyboard_loop_was_enabled)
			tran_key(scan_code);
		unlock_without_sti();
		_KnlIrqAck(1);
		asm volatile ("iret;");
	}
}

/**
	@Function:		_KnlIdeInterrupt
	@Access:		Private
	@Description:
		第一IDE的中断程序。
	@Parameters:
	@Return:	
*/
static
void
_KnlIdeInterrupt(void)
{
	while(1)
	{
		lock();
		if(!use_rtc_for_task_scheduler && ApicIsEnabled())
			ApicStopTimer();
		_ide0_signal++;
		if(!use_rtc_for_task_scheduler && ApicIsEnabled())
			ApicStartTimer();
		unlock_without_sti();
		_KnlIrqAck(14);
		asm volatile ("iret;");
	}
}

/**
	@Function:		_KnlIde1Interrupt
	@Access:		Private
	@Description:
		第二IDE的中断程序。
	@Parameters:
	@Return:	
*/
static
void
_KnlIde1Interrupt(void)
{
	while(1)
	{
		lock();
		if(!use_rtc_for_task_scheduler && ApicIsEnabled())
			ApicStopTimer();
		_ide1_signal++;
		if(!use_rtc_for_task_scheduler && ApicIsEnabled())
			ApicStartTimer();
		unlock_without_sti();
		_KnlIrqAck(15);
		asm volatile ("iret;");
	}
}

/**
	@Function:		_KnlFpuInterrupt
	@Access:		Private
	@Description:
		FPU 的中断程序。
	@Parameters:
	@Return:
*/
static
void
_KnlFpuInterrupt(void)
{
	while(1)
	{
		lock();
		if(!use_rtc_for_task_scheduler && ApicIsEnabled())
			ApicStopTimer();
		KnlOutByte(0xf0, 0x00);
		if(!use_rtc_for_task_scheduler && ApicIsEnabled())
			ApicStartTimer();
		unlock_without_sti();
		_KnlIrqAck(13);
		asm volatile ("iret;");
	}
}

static PeripheralInterrupt _peripheral_interrupts[4] = { NULL, NULL, NULL, NULL };

/**
	@Function:		KnlSetPeripheralInterrupt
	@Access:		Public
	@Description:
		设置外设中断。
	@Parameters:
		index, uint32, IN
			索引，0 ~ MAX_PERIPHERAL_COUNT - 1。
		proc, PeripheralInterrupt, IN
			外设中断处理过程。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。
*/
BOOL
KnlSetPeripheralInterrupt(
	IN uint32 index,
	IN PeripheralInterrupt proc)
{
	if (index >= MAX_PERIPHERAL_COUNT
		|| proc == NULL
		|| _peripheral_interrupts[index] != NULL)
	{
		return FALSE;
	}
	_peripheral_interrupts[index] = proc;
	return TRUE;
}

/**
	@Function:		_KnlPeripheral0Interrupt
	@Access:		Private
	@Description:
		外部设备0的中断程序（IRQ5）。
	@Parameters:
	@Return:
*/
static
void
_KnlPeripheral0Interrupt(void)
{
	while (1)
	{
		if (_peripheral_interrupts[0] != NULL)
		{
			_peripheral_interrupts[0](0, 5);
		}
		_KnlIrqAck(5);
		asm volatile ("iret;");
	}
}

/**
	@Function:		_KnlPeripheral1Interrupt
	@Access:		Private
	@Description:
		外部设备1的中断程序（IRQ9）。
	@Parameters:
	@Return:
*/
static
void
_KnlPeripheral1Interrupt(void)
{
	while (1)
	{
		if (_peripheral_interrupts[1] != NULL)
		{
			_peripheral_interrupts[1](1, 9);
		}
		_KnlIrqAck(9);
		asm volatile ("iret;");
	}
}

/**
	@Function:		_KnlPeripheral2Interrupt
	@Access:		Private
	@Description:
		外部设备2的中断程序（IRQ10）。
	@Parameters:
	@Return:
*/
static
void
_KnlPeripheral2Interrupt(void)
{
	while (1)
	{
		if (_peripheral_interrupts[2] != NULL)
		{
			_peripheral_interrupts[2](2, 10);
		}
		_KnlIrqAck(10);
		asm volatile ("iret;");
	}
}

/**
	@Function:		_KnlPeripheral3Interrupt
	@Access:		Private
	@Description:
		外部设备3的中断程序（IRQ11）。
	@Parameters:
	@Return:
*/
static
void
_KnlPeripheral3Interrupt(void)
{
	while (1)
	{
		if (_peripheral_interrupts[3] != NULL)
		{
			_peripheral_interrupts[3](3, 11);
		}
		_KnlIrqAck(11);
		asm volatile ("iret;");
	}
}

static uint8 * scall_stacks[MAX_TASK_COUNT];

/**
	@Function:		_KnlInitSystemCall
	@Access:		Private
	@Description:
		初始化系统调用。
	@Parameters:
	@Return:
*/
static
void
_KnlInitSystemCall(void)
{
	uint32 ui;
	for(ui = 0; ui < MAX_TASK_COUNT; ui++)
		scall_stacks[ui] = NULL;
}

/**
	@Function:		KnlResetSystemCall
	@Access:		Public
	@Description:
		重置系统调用。
	@Parameters:
		tid, int32, IN
			任务ID。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。
*/
BOOL
KnlResetSystemCall(IN int32 tid)
{
	if(	tid < 0
		|| tid >= MAX_TASK_COUNT)
		return FALSE;
	struct TSS * tss = &(scall_tss[tid]);
	uint8 * stack = (uint8 *)MemAlloc(SYSCALL_PROCEDURE_STACK_SIZE);
	if(stack == NULL)
		return FALSE;
	scall_stacks[tid] = stack;

	struct Desc tss_desc;
	struct Gate task_gate;

	uint32 temp = (uint32)tss;
	tss_desc.limitl = sizeof(struct TSS) - 1;
	tss_desc.basel = (uint16)(temp & 0xFFFF);
	tss_desc.basem = (uint8)((temp >> 16) & 0xFF);
	tss_desc.baseh = (uint8)((temp >> 24) & 0xFF);
	tss_desc.attr = AT386TSS + DPL3;
	KnlSetDescToGDT(30 + tid * 2 + 0, (uint8 *)&tss_desc);

	task_gate.offsetl = 0;
	task_gate.offseth = 0;
	task_gate.dcount = 0;
	task_gate.selector = ((30 + tid * 2 + 0) << 3) | RPL3;
	task_gate.attr = ATTASKGATE | DPL3;
	KnlSetDescToGDT(30 + tid * 2 + 1, (uint8 *)&task_gate);

	_KnlFillTSS(tss, (uint32)_KnlProcessSystemCall, (uint32)stack);

	return TRUE;
}

/**
	@Function:		KnlFreeSystemCall
	@Access:		Public
	@Description:
		释放系统调用。
	@Parameters:
		tid, int32, IN
			任务ID。
	@Return:
*/
void
KnlFreeSystemCall(IN int32 tid)
{
	if(	tid < 0
		|| tid >= MAX_TASK_COUNT)
		return;
	MemFree(scall_stacks[tid]);
}

/**
	@Function:		_KnlProcessSystemCall
	@Access:		Private
	@Description:
		系统调用。由系统调用中断程序调用。
	@Parameters:
	@Return:
*/
static
void
_KnlProcessSystemCall(void)
{
	// 该区域处于关中断状态 {
	uint32 eax, ecx, edx;

	asm volatile (
		"pushl	%%eax\n\t"
		"pushl	%%ecx\n\t"
		"pushl	%%edx\n\t"
		"popl	%0\n\t"
		"popl	%1\n\t"
		"popl	%2\n\t"
		:
		:"m"(edx), "m"(ecx), "m"(eax));

	_func_name = __FUNCTION__;

	struct Task * task = TskmgrGetTaskInfoPtr(ecx);
	task->is_system_call = TRUE;

	is_system_call++;

	uint32 base = (uint32)TaskmgrConvertLAddrToPAddr(ecx, 0x01300000); 
	struct SParams * sparams = TaskmgrConvertLAddrToPAddr(ecx, edx); 
	// }

	UNLOCK_TASK();

	// 该区域处于开中断状态 {
	switch(eax >> 16)
	{
		case SCALL_SCREEN:
			_ScScrProcess(eax & 0xffff, base, sparams);
			break;
		case SCALL_KEYBOARD:
			_ScKbdProcess(eax & 0xffff, base, sparams);
			break;
		case SCALL_FS:
			_ScFsProcess(eax & 0xffff, base, sparams);
			break;
		case SCALL_SYSTEM:
			_ScSysProcess(eax & 0xffff, base, sparams);
			break;
		case SCALL_MOUSE:
			_ScMouseProcess(eax & 0xffff, base, sparams);
			break;
		case SCALL_GUI:
			_ScGuiProcess(eax & 0xffff, base, sparams);
			break;
	}
	// }

	LOCK_TASK();

	// 该区域处于关中断状态 {

	is_system_call--;

	task->is_system_call = FALSE;

	asm volatile ("iret");
	// }
}

/**
	@Function:		_KnlSystemCallInterrupt
	@Access:		Private
	@Description:
		系统调用中断程序。
	@Parameters:
	@Return:	
*/
static
void
_KnlSystemCallInterrupt(void)
{
	uint32 edx, eax;
	asm volatile (
		"pushl	%%eax\n\t"
		"pushl	%%edx\n\t"
		"popl	%0\n\t"
		"popl	%1\n\t"

		/*"pushw	%%si\n\t"
		"pushw	%%ds\n\t"
		"pushw	%%es\n\t"
		"pushw	%%fs\n\t"
		"pushw	%%gs\n\t"
		"movw	$0x4b, %%si\n\t"
		"movw	%%si, %%ds\n\t"
		"movw	%%si, %%es\n\t"
		"movw	%%si, %%fs\n\t"
		"movw	%%si, %%gs\n\t"*/
		:"=m"(edx), "=m"(eax));

	// 获取发生系统调用的任务的ID。
	int32 tid = *(int32 *)(0x01000000 + 0);
	struct SParams * sparams = (struct SParams *)edx;
	sparams->tid = tid;

	//获取指定的TSS和Stack。
	struct TSS * system_call_tss = &(scall_tss[tid]);
	uint8 * system_call_stack = scall_stacks[tid];

	//重置System Call的TSS。
	system_call_tss->eip = (uint32)_KnlProcessSystemCall;
	system_call_tss->esp0 = (uint32)(system_call_stack + SYSCALL_PROCEDURE_STACK_SIZE);
	system_call_tss->esp = (uint32)(system_call_stack + SYSCALL_PROCEDURE_STACK_SIZE);
	system_call_tss->flags = 0x0;
	system_call_tss->edx = edx;
	system_call_tss->ecx = tid;
	system_call_tss->eax = eax;

	/*asm volatile (
		"popw	%gs\n\t"
		"popw	%fs\n\t"
		"popw	%es\n\t"
		"popw	%ds\n\t"
		"popw	%si\n\t");*/

	#define SYSTEM_CALL(tid_s)	\
		asm volatile (	".set _SYSCALL_TARGET"tid_s", (((30 + "tid_s" * 2 + 1) << 3) | 3)\n\t"	\
						"lcall	$_SYSCALL_TARGET"tid_s", $0\n\t");

	#define SYSTEM_CALL_CASE(tid, tid_s)	\
		case tid:	\
			SYSTEM_CALL(tid_s);	\
			break;

	switch(tid)
	{
		SYSTEM_CALL_CASE(0, "0")
		SYSTEM_CALL_CASE(1, "1")
		SYSTEM_CALL_CASE(2, "2")
		SYSTEM_CALL_CASE(3, "3")
		SYSTEM_CALL_CASE(4, "4")
		SYSTEM_CALL_CASE(5, "5")
		SYSTEM_CALL_CASE(6, "6")
		SYSTEM_CALL_CASE(7, "7")
		SYSTEM_CALL_CASE(8, "8")
		SYSTEM_CALL_CASE(9, "9")
		SYSTEM_CALL_CASE(10, "10")
		SYSTEM_CALL_CASE(11, "11")
		SYSTEM_CALL_CASE(12, "12")
		SYSTEM_CALL_CASE(13, "13")
		SYSTEM_CALL_CASE(14, "14")
		SYSTEM_CALL_CASE(15, "15")
		SYSTEM_CALL_CASE(16, "16")
		SYSTEM_CALL_CASE(17, "17")
		SYSTEM_CALL_CASE(18, "18")
		SYSTEM_CALL_CASE(19, "19")
		SYSTEM_CALL_CASE(20, "20")
		SYSTEM_CALL_CASE(21, "21")
		SYSTEM_CALL_CASE(22, "22")
		SYSTEM_CALL_CASE(23, "23")
		SYSTEM_CALL_CASE(24, "24")
		SYSTEM_CALL_CASE(25, "25")
		SYSTEM_CALL_CASE(26, "26")
		SYSTEM_CALL_CASE(27, "27")
		SYSTEM_CALL_CASE(28, "28")
		SYSTEM_CALL_CASE(29, "29")
		SYSTEM_CALL_CASE(30, "30")
		SYSTEM_CALL_CASE(31, "31")
		SYSTEM_CALL_CASE(32, "32")
		SYSTEM_CALL_CASE(33, "33")
		SYSTEM_CALL_CASE(34, "34")
		SYSTEM_CALL_CASE(35, "35")
		SYSTEM_CALL_CASE(36, "36")
		SYSTEM_CALL_CASE(37, "37")
		SYSTEM_CALL_CASE(38, "38")
		SYSTEM_CALL_CASE(39, "39")
		SYSTEM_CALL_CASE(40, "40")
		SYSTEM_CALL_CASE(41, "41")
		SYSTEM_CALL_CASE(42, "42")
		SYSTEM_CALL_CASE(43, "43")
		SYSTEM_CALL_CASE(44, "44")
		SYSTEM_CALL_CASE(45, "45")
		SYSTEM_CALL_CASE(46, "46")
		SYSTEM_CALL_CASE(47, "47")
		SYSTEM_CALL_CASE(48, "48")
		SYSTEM_CALL_CASE(49, "49")
		SYSTEM_CALL_CASE(50, "50")
		SYSTEM_CALL_CASE(51, "51")
		SYSTEM_CALL_CASE(52, "52")
		SYSTEM_CALL_CASE(53, "53")
		SYSTEM_CALL_CASE(54, "54")
		SYSTEM_CALL_CASE(55, "55")
		SYSTEM_CALL_CASE(56, "56")
		SYSTEM_CALL_CASE(57, "57")
		SYSTEM_CALL_CASE(58, "58")
		SYSTEM_CALL_CASE(59, "59")
		SYSTEM_CALL_CASE(60, "60")
		SYSTEM_CALL_CASE(61, "61")
		SYSTEM_CALL_CASE(62, "62")
		SYSTEM_CALL_CASE(63, "63")
	}

	#undef	SYSTEM_CALL
	#undef	SYSTEM_CALL_CASE

	INT_EXIT();
}

/**
	@Function:		_KnlKillTaskAndJumpToKernel
	@Access:		Private
	@Description:
		杀死任务后等待任务调度器执行，
		并且任务调度器将执行内核任务。
	@Parameters:
		tid, uint32, IN
			要杀死的任务任务ID。
	@Return:
*/
static
void
_KnlKillTaskAndJumpToKernel(IN uint32 tid)
{
	is_kernel_task = TRUE;

	// 检测screen.sys任务是不是被杀死了。
	if(strcmp(TskmgrGetTaskInfoPtr(tid)->name, SYSTEM_PATH"sys/screen.sys") == 0)
	{
		int32 sys_screen_tid = TskmgrCreateSystemTaskByFile(
			SYSTEM_PATH"sys/screen.sys",
			SYSTEM_PATH"sys/screen.sys",
			SYSTEM_PATH"sys/");
		TskmgrGetTaskInfoPtr(sys_screen_tid)->priority = TASK_PRIORITY_HIGH;
		TskmgrSetTaskToReady(sys_screen_tid);
	}

	// 检测pci.sys任务是不是被杀死了。
	if(strcmp(TskmgrGetTaskInfoPtr(tid)->name, SYSTEM_PATH"sys/pci.sys") == 0)
	{
		int32 sys_pci_tid = TskmgrCreateSystemTaskByFile(
			SYSTEM_PATH"sys/pci.sys",
			SYSTEM_PATH"sys/pci.sys",
			SYSTEM_PATH"sys/");
		TskmgrSetTaskToReady(sys_pci_tid);
	}

	// 检测timer.sys任务是不是被杀死了。
	if(strcmp(TskmgrGetTaskInfoPtr(tid)->name, SYSTEM_PATH"sys/timer.sys") == 0)
	{
		int32 sys_pci_tid = TskmgrCreateSystemTaskByFile(
			SYSTEM_PATH"sys/timer.sys",
			SYSTEM_PATH"sys/timer.sys",
			SYSTEM_PATH"sys/");
		TskmgrGetTaskInfoPtr(sys_pci_tid)->priority = TASK_PRIORITY_LOW;
		TskmgrSetTaskToReady(sys_pci_tid);
	}

	int32 wait_app_tid = ConGetCurrentApplicationTid();
	if(tid == wait_app_tid)
		ConSetCurrentApplicationTid(-1);
	
	TskmgrKillSystemTask(tid);

	switch_to_kernel = TRUE;			//强制让任务调度器执行内核任务。
	kt_jk_lock = TRUE;					//unlock()之后等待任务调度器的执行。
	will_reset_all_exceptions = TRUE;	//需要重设所有异常处理程序的状态。
	unlock();
	if(!use_rtc_for_task_scheduler)
		ApicStartTimer();
	while(kt_jk_lock);					//任务调度器会把kt_jk_lock设置为FALSE。
	asm volatile ("cli");
}

#define GET_ERROR_CODE(__errcode)	\
{	\
	asm volatile (	\
		"pushal\n\t"	\
		"movl	4(%%ebp), %%eax\n\t"	\
		"movl	%%eax, %0\n\t"	\
		"popal\n\t"	\
	:"=m"(*(__errcode)));	\
}

#define PARSE_ERROR_CODE(__errcode, __ext, __idt, __ti, __sel)	\
{	\
	*(__ext) = __errcode & 0x00000001;	\
	*(__idt) = __errcode & 0x00000002;	\
	*(__ti) = __errcode & 0x00000004;	\
	*(__sel) = (__errcode >> 3) & 0x1fff;	\
}

/*
	中断号 	标识 	意思 				类型 		错误码 	引起的条件
	======================================================================================
	0 		#DE 	除出错				故障 		无 		DIV或IDIV指令。
	1 		#DB 	调试					故障/陷阱	无 		任何代码或数据引用，或是指令INT 1指令。
	2 		-- 		NMI中断 				中断 		无 		非屏蔽外部中断。
	3 		#BP 	断电 				陷阱 		无 		INT 3指令。
	4 		#OF 	溢出 				陷阱 		无 		INTO指令。
	5 		#BR 	边界范围超出 			故障 		无 		BOUND指令。
	6 		#UD 	无效操作码 			故障 		无 		UD2指令或保留的操作码。
	7 		#NM 	设备不存在			故障 		无 		浮点或WAIT/FWAIT指令。
	8 		#DF 	双重故障 			异常终止 	有（0）	任何可产生的异常、NMI或INTR指令。
	9 		-- 		协处理器段超越 		故障 		无 		浮点指令（386以后的机器不产生该异常）。
	10 		#TS 	无效的任务状态段TSS 	故障 		有 		任务交换或访问TSS。
	11 		#NP 	段不存在 			故障 		有 		加载段寄存器或访问系统段。
	12 		#SS 	堆栈段错误 			故障 		有 		堆栈操作和SS寄存器加载。
	13 		#GP 	一般保护错误 			故障 		有 		任何内存引用和其他保护检查。
	14 		#PF 	页面错误 			故障 		有 		任何内存引用。
	15 		-- 		（Intel保留） 					无
	16 		#MF 	x87 FPU 浮点错误 	故障 		无 		x87 FPU浮点或WAIT/FWAIT指令。
	17 		#AC 	对齐检查 			故障 		有（0）	对内存中任何数据的引用。
	18 		#MC 	机器检查 			异常终止 	无 		错误码（若有）和产生源与CPU类型有关（奔腾处理器引进）。
	19 		#XF 	SIMD浮点异常 		故障 		无 		SSE和SSE2浮点指令。
	20-31 	-- 		（Intel保留）
	32-255 	-- 		用户自定义			中断 				外部中断或INT n指令。
*/

/*================================================================================
							处理除数为0的故障, 0x00
================================================================================*/

/**
	@Function:		_KnlInitDividingByZeroException
	@Access:		Private
	@Description:
		设置除以0时引发的异常处理程序。
	@Parameters:
	@Return:
*/
static
void
_KnlInitDividingByZeroException(void)
{
	struct die_info info;
	struct TSS * tss = &divby0_tss;
	uint8 * stack = (uint8 *)MemAlloc(INTERRUPT_PROCEDURE_STACK_SIZE);
	divby0_stack = stack;
	if(tss == NULL)
	{
		fill_info(info, DC_INIT_DIV_BY_0, DI_INIT_DIV_BY_0);
		die(&info);
	}
	struct Desc tss_desc;	
	struct Gate task_gate;	

	uint32 temp = (uint32)tss;
	tss_desc.limitl = sizeof(struct TSS) - 1;
	tss_desc.basel = (uint16)(temp & 0xFFFF);
	tss_desc.basem = (uint8)((temp >> 16) & 0xFF);
	tss_desc.baseh = (uint8)((temp >> 24) & 0xFF);
	tss_desc.attr = AT386TSS + DPL0;
	KnlSetDescToGDT(21, (uint8 *)&tss_desc);

	task_gate.offsetl = 0;
	task_gate.offseth = 0;
	task_gate.dcount = 0;
	task_gate.selector = (21 << 3) | RPL0;
	task_gate.attr = ATTASKGATE | DPL0;
	KnlSetGateToIDT(0x00, (uint8 *)&task_gate);

	_KnlFillTSS(tss, (uint32)_KnlDividingByZeroExceptionInterrupt, (uint32)stack);
}

/**
	@Function:		_KnlDividingByZeroExceptionInterrupt
	@Access:		Private
	@Description:
		处理除以0时引发的异常的中断过程。
	@Parameters:
	@Return:
*/
static
void
_KnlDividingByZeroExceptionInterrupt(void)
{
	while(1)
	{
		if(is_kernel_task)
		{
			struct die_info info;
			fill_info(info, DC_DIV_BY_0, DI_DIV_BY_0);
			Log(LOG_ERROR, DI_DIV_BY_0);
			die(&info);
		}
		else
		{
			lock();
			if(!use_rtc_for_task_scheduler)
				ApicStopTimer();
			struct Task * task = TskmgrGetTaskInfoPtr(current_tid);

			int8 buffer[1024];
			sprintf_s(	buffer,
						1024,
						"A task causes a exception of dividing by zero"
						", the id is %d, the name is '%s'\n",
						current_tid,
						task->name);
			Log(LOG_ERROR, buffer);

			ScrPrintStringWithProperty(buffer, CC_RED);
			ScrPrintString("\n");

			_KnlKillTaskAndJumpToKernel(current_tid);
		}
	}
}

/*================================================================================
							处理边界检测的故障, 0x05
================================================================================*/

/**
	@Function:		_KnlInitBoundCheckException
	@Access:		Private
	@Description:
		设置边界检查时引发的异常处理程序。
	@Parameters:
	@Return:
*/
static
void
_KnlInitBoundCheckException(void)
{
	struct die_info info;
	struct TSS * tss = &bndchk_tss;
	uint8 * stack = (uint8 *)MemAlloc(INTERRUPT_PROCEDURE_STACK_SIZE);
	bndchk_stack = stack;
	if(tss == NULL)
	{
		fill_info(info, DC_INIT_BOUND_CHK, DI_INIT_BOUND_CHK);
		die(&info);
	}
	struct Desc tss_desc;	
	struct Gate task_gate;	

	uint32 temp = (uint32)tss;
	tss_desc.limitl = sizeof(struct TSS) - 1;
	tss_desc.basel = (uint16)(temp & 0xFFFF);
	tss_desc.basem = (uint8)((temp >> 16) & 0xFF);
	tss_desc.baseh = (uint8)((temp >> 24) & 0xFF);
	tss_desc.attr = AT386TSS + DPL0;
	KnlSetDescToGDT(22, (uint8 *)&tss_desc);

	task_gate.offsetl = 0;
	task_gate.offseth = 0;
	task_gate.dcount = 0;
	task_gate.selector = (22 << 3) | RPL0;
	task_gate.attr = ATTASKGATE | DPL0;
	KnlSetGateToIDT(0x05, (uint8 *)&task_gate);

	_KnlFillTSS(tss, (uint32)_KnlBoundCheckExceptionInterrupt, (uint32)stack);
}

/**
	@Function:		_KnlBoundCheckExceptionInterrupt
	@Access:		Private
	@Description:
		处理边界检查时引发的异常的中断过程。
	@Parameters:
	@Return:
*/
static
void
_KnlBoundCheckExceptionInterrupt(void)
{
	while(1)
	{
		if(is_kernel_task)
		{
			struct die_info info;
			fill_info(info, DC_BOUND_CHK, DI_BOUND_CHK);
			die(&info);
		}
		else
		{
			lock();
			if(!use_rtc_for_task_scheduler)
				ApicStopTimer();
			struct Task * task = TskmgrGetTaskInfoPtr(current_tid);

			int8 buffer[1024];
			sprintf_s(	buffer,
						1024,
						"A task causes a exception of bound check"
						", the id is %d, the name is '%s'\n",
						current_tid,
						task->name);
			Log(LOG_ERROR, buffer);

			ScrPrintStringWithProperty(buffer, CC_RED);
			ScrPrintString("\n");

			_KnlKillTaskAndJumpToKernel(current_tid);
		}
	}
}

/*================================================================================
							处理非法的操作码的故障, 0x06
================================================================================*/

/**
	@Function:		_KnlInitInvalidOpcodeException
	@Access:		Private
	@Description:
		设置检测到错误的操作码时引发的异常处理程序。
	@Parameters:
	@Return:
*/
static
void
_KnlInitInvalidOpcodeException(void)
{
	struct die_info info;
	struct TSS * tss = &invalidopc_tss;
	uint8 * stack = (uint8 *)MemAlloc(INTERRUPT_PROCEDURE_STACK_SIZE);
	invalidopc_stack = stack;
	if(tss == NULL)
	{
		fill_info(info, DC_INIT_INVALID_OPC, DI_INIT_INVALID_OPC);
		die(&info);
	}
	struct Desc tss_desc;	
	struct Gate task_gate;	

	uint32 temp = (uint32)tss;
	tss_desc.limitl = sizeof(struct TSS) - 1;
	tss_desc.basel = (uint16)(temp & 0xFFFF);
	tss_desc.basem = (uint8)((temp >> 16) & 0xFF);
	tss_desc.baseh = (uint8)((temp >> 24) & 0xFF);
	tss_desc.attr = AT386TSS + DPL0;
	KnlSetDescToGDT(23, (uint8 *)&tss_desc);

	task_gate.offsetl = 0;
	task_gate.offseth = 0;
	task_gate.dcount = 0;
	task_gate.selector = (23 << 3) | RPL0;
	task_gate.attr = ATTASKGATE | DPL0;
	KnlSetGateToIDT(0x06, (uint8 *)&task_gate);

	_KnlFillTSS(tss, (uint32)_KnlInvalidOpcodeExceptionInterrupt, (uint32)stack);
}

/**
	@Function:		_KnlInvalidOpcodeExceptionInterrupt
	@Access:		Private
	@Description:
		处理检测到错误的操作码引发的异常的中断过程。
	@Parameters:
	@Return:
*/
static
void
_KnlInvalidOpcodeExceptionInterrupt(void)
{
	while(1)
	{
		if(is_kernel_task)
		{
			struct die_info info;
			fill_info(info, DC_INVALID_OPC, DI_INVALID_OPC);
			die(&info);
		}
		else
		{
			lock();
			if(!use_rtc_for_task_scheduler)
				ApicStopTimer();
			struct Task * task = TskmgrGetTaskInfoPtr(current_tid);

			int8 buffer[1024];
			sprintf_s(	buffer,
						1024,
						"A task causes a exception of invalid opcode"
						", the id is %d, the name is '%s'\n",
						current_tid,
						task->name);
			Log(LOG_ERROR, buffer);

			ScrPrintStringWithProperty(buffer, CC_RED);
			ScrPrintString("\n");

			_KnlKillTaskAndJumpToKernel(current_tid);
		}
	}
}

/*================================================================================
							处理双重故障, 0x08
================================================================================*/

/**
	@Function:		_KnlInitDoubleFaultException
	@Access:		Private
	@Description:
		设置检测到双重故障时引发的异常处理程序。
	@Parameters:
	@Return:
*/
static
void
_KnlInitDoubleFaultException(void)
{
	struct die_info info;
	struct TSS * tss = &df_tss;
	uint8 * stack = (uint8 *)MemAlloc(INTERRUPT_PROCEDURE_STACK_SIZE);
	df_stack = stack;
	if(tss == NULL)
	{
		fill_info(info, DC_INIT_DOUBLE_FAULT, DI_INIT_DOUBLE_FAULT);
		die(&info);
	}
	struct Desc tss_desc;	
	struct Gate task_gate;	

	uint32 temp = (uint32)tss;
	tss_desc.limitl = sizeof(struct TSS) - 1;
	tss_desc.basel = (uint16)(temp & 0xFFFF);
	tss_desc.basem = (uint8)((temp >> 16) & 0xFF);
	tss_desc.baseh = (uint8)((temp >> 24) & 0xFF);
	tss_desc.attr = AT386TSS + DPL0;
	KnlSetDescToGDT(28, (uint8 *)&tss_desc);

	task_gate.offsetl = 0;
	task_gate.offseth = 0;
	task_gate.dcount = 0;
	task_gate.selector = (28 << 3) | RPL0;
	task_gate.attr = ATTASKGATE | DPL0;
	KnlSetGateToIDT(0x08, (uint8 *)&task_gate);

	_KnlFillTSS(tss, (uint32)_KnlDoubleFaultExceptionInterrupt, (uint32)stack);
}

/**
	@Function:		_KnlDoubleFaultExceptionInterrupt
	@Access:		Private
	@Description:
		处理检测到双重故障引发的异常的中断过程。
	@Parameters:
	@Return:
*/
static
void
_KnlDoubleFaultExceptionInterrupt(void)
{
	if(is_kernel_task)
	{
		struct die_info info;
		fill_info(info, DC_DOUBLE_FAULT_KNL, DI_DOUBLE_FAULT_KNL);
		die(&info);
	}
	else
	{
		struct Task * task = TskmgrGetTaskInfoPtr(current_tid);
		int8 buffer[1024];
		sprintf_s(	buffer,
					1024,
					DI_DOUBLE_FAULT_TSK,
					current_tid,
					task->name);
		struct die_info info;
		fill_info(info, DC_DOUBLE_FAULT_TSK, buffer);
		die(&info);
	}
}

/*================================================================================
							处理非法的TSS的中断程序, 0x0a
================================================================================*/

/**
	@Function:		_KnlInitInvalidTSSException
	@Access:		Private
	@Description:
		初始化检测到非法的TSS的中断程序。
	@Parameters:
	@Return:
*/
static
void
_KnlInitInvalidTSSException(void)
{
	struct die_info info;
	struct TSS * tss = &invalidtss_tss;
	uint8 * stack = (uint8 *)MemAlloc(INTERRUPT_PROCEDURE_STACK_SIZE);
	invalidtss_stack = stack;
	if(tss == NULL)
	{
		fill_info(info, DC_INIT_INVALID_TSS, DI_INIT_INVALID_TSS);
		die(&info);
	}
	struct Desc tss_desc;	
	struct Gate task_gate;	

	uint32 temp = (uint32)tss;
	tss_desc.limitl = sizeof(struct TSS) - 1;
	tss_desc.basel = (uint16)(temp & 0xFFFF);
	tss_desc.basem = (uint8)((temp >> 16) & 0xFF);
	tss_desc.baseh = (uint8)((temp >> 24) & 0xFF);
	tss_desc.attr = AT386TSS + DPL0;
	KnlSetDescToGDT(24, (uint8 *)&tss_desc);

	task_gate.offsetl = 0;
	task_gate.offseth = 0;
	task_gate.dcount = 0;
	task_gate.selector = (24 << 3) | RPL0;
	task_gate.attr = ATTASKGATE | DPL0;
	KnlSetGateToIDT(0x0a, (uint8 *)&task_gate);

	_KnlFillTSS(tss, (uint32)_KnlInvalidTSSExceptionInterrupt, (uint32)stack);
}

/**
	@Function:		_KnlInvalidTSSExceptionInterrupt
	@Access:		Private
	@Description:
		检测到非法的TSS的中断程序
	@Parameters:
	@Return:		
*/
static
void
_KnlInvalidTSSExceptionInterrupt(void)
{
	while(1)
	{
		if(is_kernel_task)
		{
			struct die_info info;
			fill_info(info, DC_INVALID_TSS, DI_INVALID_TSS);
			die(&info);
		}
		else
		{
			lock();
			if(!use_rtc_for_task_scheduler)
				ApicStopTimer();
			struct Task * task = TskmgrGetTaskInfoPtr(current_tid);

			uint32 errcode = 0;
			GET_ERROR_CODE(&errcode);
			uint32 ext = 0, idt = 0, ti = 0, selector = 0;
			PARSE_ERROR_CODE(errcode, &ext, &idt, &ti, &selector);
			
			int8 buffer[1024];
			sprintf_s(	buffer,
						1024,
						"A task causes a exception of invalid tss"
						"(Selector: %X, Table: %s, EXT: %s)"
						", the id is %d, the name is '%s'\n",
						selector,
						idt ? "IDT" : (ti ? "LDT" : "GDT"),
						ext ? "True" : "False",
						current_tid,
						task->name);
			Log(LOG_ERROR, buffer);

			ScrPrintStringWithProperty(buffer, CC_RED);
			ScrPrintString("\n");

			_KnlKillTaskAndJumpToKernel(current_tid);
		}
	}
}

/*================================================================================
							段不存在故障, 0x0b
================================================================================*/

/**
	@Function:		_KnlInitInvalidSegmentException
	@Access:		Private
	@Description:
		初始化处理段不存在故障的中断程序。
	@Parameters:
	@Return:	
*/
static
void
_KnlInitInvalidSegmentException(void)
{
	struct die_info info;
	struct TSS * tss = &invalidseg_tss;
	uint8 * stack = (uint8 *)MemAlloc(INTERRUPT_PROCEDURE_STACK_SIZE);
	invalidseg_stack = stack;
	if(tss == NULL)
	{
		fill_info(info, DC_INIT_INVALID_SEG, DI_INIT_INVALID_SEG);
		die(&info);
	}
	struct Desc tss_desc;
	struct Gate task_gate;	

	uint32 temp = (uint32)tss;
	tss_desc.limitl = sizeof(struct TSS) - 1;
	tss_desc.basel = (uint16)(temp & 0xFFFF);
	tss_desc.basem = (uint8)((temp >> 16) & 0xFF);
	tss_desc.baseh = (uint8)((temp >> 24) & 0xFF);
	tss_desc.attr = AT386TSS + DPL0;
	KnlSetDescToGDT(25, (uint8 *)&tss_desc);

	task_gate.offsetl = 0;
	task_gate.offseth = 0;
	task_gate.dcount = 0;
	task_gate.selector = (25 << 3) | RPL0;
	task_gate.attr = ATTASKGATE | DPL0;
	KnlSetGateToIDT(0x0b, (uint8 *)&task_gate);

	_KnlFillTSS(tss, (uint32)_KnlInvalidSegmentExceptionInterrupt, (uint32)stack);
}

/**
	@Function:		_KnlInvalidSegmentExceptionInterrupt
	@Access:		Private
	@Description:
		处理段不存在故障的中断程序。
	@Parameters:
	@Return:		
*/
static
void
_KnlInvalidSegmentExceptionInterrupt(void)
{
	while(1)
	{
		if(is_kernel_task)
		{
			struct die_info info;
			fill_info(info, DC_INVALID_SEG, DI_INVALID_SEG);
			die(&info);
		}
		else
		{
			lock();
			if(!use_rtc_for_task_scheduler)
				ApicStopTimer();
			struct Task * task = TskmgrGetTaskInfoPtr(current_tid);

			uint32 errcode = 0;
			GET_ERROR_CODE(&errcode);
			uint32 ext = 0, idt = 0, ti = 0, selector = 0;
			PARSE_ERROR_CODE(errcode, &ext, &idt, &ti, &selector);

			int8 buffer[1024];
			sprintf_s(	buffer,
						1024,
						"A task causes a exception of non-existent segment"
						"(Selector: %X, Table: %s, EXT: %s)"
						", the id is %d, the name is '%s'\n",
						selector,
						idt ? "IDT" : (ti ? "LDT" : "GDT"),
						ext ? "True" : "False",
						current_tid,
						task->name);
			Log(LOG_ERROR, buffer);

			ScrPrintStringWithProperty(buffer, CC_RED);
			ScrPrintString("\n");

			_KnlKillTaskAndJumpToKernel(current_tid);
		}
	}
}

/*================================================================================
							堆栈段故障, 0x0c
================================================================================*/

/**
	@Function:		_KnlInitInvalidStackException
	@Access:		Private
	@Description:
		初始化处理堆栈段故障的中断程序。
	@Parameters:
	@Return:
*/
static
void
_KnlInitInvalidStackException(void)
{
	struct die_info info;
	struct TSS * tss = &invalidstck_tss;
	uint8 * stack = (uint8 *)MemAlloc(INTERRUPT_PROCEDURE_STACK_SIZE);
	invalidstck_stack = stack;
	if(tss == NULL)
	{
		fill_info(info, DC_INIT_INVALID_STACK, DI_INIT_INVALID_STACK);
		die(&info);
	}
	struct Desc tss_desc;
	struct Gate task_gate;	

	uint32 temp = (uint32)tss;
	tss_desc.limitl = sizeof(struct TSS) - 1;
	tss_desc.basel = (uint16)(temp & 0xFFFF);
	tss_desc.basem = (uint8)((temp >> 16) & 0xFF);
	tss_desc.baseh = (uint8)((temp >> 24) & 0xFF);
	tss_desc.attr = AT386TSS + DPL0;
	KnlSetDescToGDT(26, (uint8 *)&tss_desc);

	task_gate.offsetl = 0;
	task_gate.offseth = 0;
	task_gate.dcount = 0;
	task_gate.selector = (26 << 3) | RPL0;
	task_gate.attr = ATTASKGATE | DPL0;
	KnlSetGateToIDT(0x0c, (uint8 *)&task_gate);

	_KnlFillTSS(tss, (uint32)_KnlInvalidStackExceptionInterrupt, (uint32)stack);
}

/**
	@Function:		_KnlInvalidStackExceptionInterrupt
	@Access:		Private
	@Description:
		处理堆栈段故障的中断程序。
	@Parameters:
	@Return:		
*/
static
void
_KnlInvalidStackExceptionInterrupt(void)
{
	while(1)
	{
		if(is_kernel_task)
		{
			struct die_info info;
			fill_info(info, DC_INVALID_STACK, DI_INVALID_STACK);
			die(&info);
		}
		else
		{
			lock();
			if(!use_rtc_for_task_scheduler)
				ApicStopTimer();
			struct Task * task = TskmgrGetTaskInfoPtr(current_tid);

			uint32 errcode = 0;
			GET_ERROR_CODE(&errcode);
			uint32 ext = 0, idt = 0, ti = 0, selector = 0;
			PARSE_ERROR_CODE(errcode, &ext, &idt, &ti, &selector);

			int8 buffer[1024];
			sprintf_s(	buffer,
						1024,
						"A task causes a exception of invalid stack"
						"(Selector: %X, Table: %s, EXT: %s)"
						", the id is %d, the name is '%s'\n",
						selector,
						idt ? "IDT" : (ti ? "LDT" : "GDT"),
						ext ? "True" : "False",
						current_tid,
						task->name);
			Log(LOG_ERROR, buffer);

			ScrPrintStringWithProperty(buffer, CC_RED);
			ScrPrintString("\n");

			_KnlKillTaskAndJumpToKernel(current_tid);
		}
	}
}

/*================================================================================
							通用保护异常, 0x0d
================================================================================*/

/**
	@Function:		_KnlInitGlobalProtectionException
	@Access:		Private
	@Description:
		初始化通用保护的中断程序。
	@Parameters:
	@Return:		
*/
static
void
_KnlInitGlobalProtectionException(void)
{
	struct die_info info;
	struct TSS * tss = &gp_tss;
	uint8 * stack = (uint8 *)MemAlloc(INTERRUPT_PROCEDURE_STACK_SIZE);
	gp_stack = stack;
	if(tss == NULL)
	{
		fill_info(info, DC_INIT_GP, DI_INIT_GP);
		die(&info);
	}
	struct Desc tss_desc;	
	struct Gate task_gate;	

	uint32 temp = (uint32)tss;
	tss_desc.limitl = sizeof(struct TSS) - 1;
	tss_desc.basel = (uint16)(temp & 0xFFFF);
	tss_desc.basem = (uint8)((temp >> 16) & 0xFF);
	tss_desc.baseh = (uint8)((temp >> 24) & 0xFF);
	tss_desc.attr = AT386TSS + DPL0;
	KnlSetDescToGDT(18, (uint8 *)&tss_desc);

	task_gate.offsetl = 0;
	task_gate.offseth = 0;
	task_gate.dcount = 0;
	task_gate.selector = (18 << 3) | RPL0;
	task_gate.attr = ATTASKGATE | DPL0;
	KnlSetGateToIDT(0x0d, (uint8 *)&task_gate);

	_KnlFillTSS(tss, (uint32)_KnlGlobalProtectionExceptionInterrupt, (uint32)stack);
}

/**
	@Function:		_KnlGlobalProtectionExceptionInterrupt
	@Access:		Private
	@Description:
		通用保护的中断程序。
	@Parameters:
	@Return:	
*/
static
void
_KnlGlobalProtectionExceptionInterrupt(void)
{
	while(1)
	{
		if(is_kernel_task)
		{
			struct die_info info;
			fill_info(info, DC_GP_INT, DI_GP_INT);
			die(&info);
		}
		else
		{
			lock();
			if(!use_rtc_for_task_scheduler)
				ApicStopTimer();
			struct Task * task = TskmgrGetTaskInfoPtr(current_tid);
			struct TSS * syscall_tss = scall_tss + current_tid;
			struct Desc syscall_tss_desc;
			KnlGetDescFromGDT(30 + current_tid * 2, &syscall_tss_desc);

			uint32 errcode = 0;
			GET_ERROR_CODE(&errcode);
			uint32 ext = 0, idt = 0, ti = 0, selector = 0;
			PARSE_ERROR_CODE(errcode, &ext, &idt, &ti, &selector);

			int8 buffer[KB(64)];
			int8 buffer00[KB(64)];
			sprintf_s(	buffer,
						1024,
						"Application Crash\n"
						"######################################################################"
						"\n"
						"A task causes a global protected exception"
						"(Selector: %X, Table: %s, EXT: %s)"
						", the id is %d, the name is '%s'\n",
						selector,
						idt ? "IDT" : (ti ? "LDT" : "GDT"),
						ext ? "True" : "False",
						current_tid,
						task->name);

			sprintf_s(	buffer00,
						1024,
						"Task infomation:\n"
						"    System Call: %s\n"
						"    Function: %s\n"
						"    Timer Stack: %X\n"
						"    Timer Stack Bottom: %X\n"
						"TSS infomation:\n"
						"    ES: %X SS: %X DS: %X\n"
						"    DS: %X FS: %X GS: %X\n"
						"    CS: %X EIP: %X\n"
						"    ESP: %X\n"
						"System Call TSS infomation:\n"
						"    TSS Descriptor Attr: %X\n"
						"    Link: %X\n"
						"    ES: %X SS: %X DS: %X\n"
						"    DS: %X FS: %X GS: %X\n"
						"    CS: %X EIP: %X\n"
						"    ESP: %X\n"
						"Timer TSS infomation:\n"
						"    Link: %X\n"
						"    ES: %X SS: %X DS: %X\n"
						"    DS: %X FS: %X GS: %X\n"
						"    CS: %X EIP: %X\n"
						"    ESP: %X\n"
						"^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^",
						task->is_system_call ? "True" : "False",
						_func_name,
						_timer_stack,
						_timer_stack + INTERRUPT_PROCEDURE_STACK_SIZE,

						task->tss.es, task->tss.ss, task->tss.ds,
						task->tss.ds, task->tss.fs, task->tss.gs,
						task->tss.cs, task->tss.eip,
						task->tss.esp,

						syscall_tss_desc.attr,
						syscall_tss->back_link,
						syscall_tss->es, syscall_tss->ss, syscall_tss->ds,
						syscall_tss->ds, syscall_tss->fs, syscall_tss->gs,
						syscall_tss->cs, syscall_tss->eip,
						syscall_tss->esp,

						timer_tss.back_link,
						timer_tss.es, timer_tss.ss, timer_tss.ds,
						timer_tss.ds, timer_tss.fs, timer_tss.gs,
						timer_tss.cs, timer_tss.eip,
						timer_tss.esp);
			UtlConcatString(buffer, sizeof(buffer), buffer00);
			Log(LOG_ERROR, buffer);

			ScrPrintStringWithProperty(buffer, CC_RED);
			ScrPrintString("\n");

			_KnlKillTaskAndJumpToKernel(current_tid);
		}
	}
}

/*================================================================================
							处理页故障, 0x0e
================================================================================*/

/**
	@Function:		_KnlInitPageFaultException
	@Access:		Private
	@Description:
		初始化页失败的中断程序。
	@Parameters:
	@Return:	
*/
static
void
_KnlInitPageFaultException(void)
{
	struct die_info info;
	struct TSS * tss = &pf_tss;
	uint8 * stack = (uint8 *)MemAlloc(INTERRUPT_PROCEDURE_STACK_SIZE);
	pf_stack = stack;
	if(tss == NULL)
	{
		fill_info(info, DC_INIT_PF, DI_INIT_PF);
		die(&info);
	}
	struct Desc tss_desc;	
	struct Gate task_gate;	

	uint32 temp = (uint32)tss;
	tss_desc.limitl = sizeof(struct TSS) - 1;
	tss_desc.basel = (uint16)(temp & 0xFFFF);
	tss_desc.basem = (uint8)((temp >> 16) & 0xFF);
	tss_desc.baseh = (uint8)((temp >> 24) & 0xFF);
	tss_desc.attr = AT386TSS + DPL0;
	KnlSetDescToGDT(20, (uint8 *)&tss_desc);

	task_gate.offsetl = 0;
	task_gate.offseth = 0;
	task_gate.dcount = 0;
	task_gate.selector = (20 << 3) | RPL0;
	task_gate.attr = ATTASKGATE | DPL0;
	KnlSetGateToIDT(0x0e, (uint8 *)&task_gate);

	_KnlFillTSS(tss, (uint32)_KnlPageFaultExceptionInterrupt, (uint32)stack);
}

/**
	@Function:		_KnlPageFaultExceptionInterrupt
	@Access:		Private
	@Description:
		页失败的中断程序。
	@Parameters:
	@Return:		
*/
static
void
_KnlPageFaultExceptionInterrupt(void)
{
	while(1)
	{
		if(is_kernel_task)
		{
			struct die_info info;
			fill_info(info, DC_PF_INT, DI_PF_INT);
			die(&info);
		}
		else
		{
			lock();
			if(!use_rtc_for_task_scheduler)
				ApicStopTimer();
			struct Task * task = TskmgrGetTaskInfoPtr(current_tid);

			// 获取导致异常的线性地址。
			uint32 badaddr = 0;
			asm volatile (
				"pushal\n\t"
				"movl	%%cr2, %%eax\n\t"
				"movl	%%eax, %0\n\t"
				"popal\n\t"
			:"=m"(badaddr));

			// 获取错误码。
			uint32 errcode = 0;
			GET_ERROR_CODE(&errcode);
			
			// P: 错误码的位0。
			// 异常是由于页面不存在或违反访问特权而引起的。
			// P=0，表示页不存在。
			// P=1，表示违反页级保护权限。
			uint32 p = errcode & 0x00000001;

			// W/R: 错误码的位1。
			// 异常是由于内存读或写操作引起的。
			// W/R=0，表示由读操作引起。
			// W/R=1，表示由写操作引起。
			uint32 wr = errcode & 0x0000002;
			
			// U/S: 错误码的位2。
			// 发生异常时CPU执行的代码级别。
			// U/S=0，表示CPU正在执行超级用户代码。
			// U/S=1，表示CPU正在执行一般用户代码。
			uint32 us = errcode & 0x0000004;

			int8 buffer[1024];
			sprintf_s(	buffer,
						1024,
						"A task causes a error of page fault(0x%X, %s, %s, %s), the id is %d, the name is '%s'\n",
						badaddr,
						p ? "Violating page level protected privilege" : "Page does not exist",
						wr ? "Write" : "Read",
						us ? "User" : "System",
						current_tid,
						task->name);
			Log(LOG_ERROR, buffer);

			ScrPrintStringWithProperty(buffer, CC_RED);
			ScrPrintString("\n");
			_KnlKillTaskAndJumpToKernel(current_tid);
		}
	}
}

/*================================================================================
					x87 FPU浮点错误（数学错误）故障, 0x10
================================================================================*/

/**
	@Function:		_KnlInitMathFaultException
	@Access:		Private
	@Description:
		初始化处理x87 FPU浮点错误（数学错误）故障的中断处理程序。
	@Parameters:
	@Return:	
*/
static
void
_KnlInitMathFaultException(void)
{
	struct die_info info;
	struct TSS * tss = &mf_tss;
	uint8 * stack = (uint8 *)MemAlloc(INTERRUPT_PROCEDURE_STACK_SIZE);
	mf_stack = stack;
	if(tss == NULL)
	{
		fill_info(info, DC_INIT_MF, DI_INIT_MF);
		die(&info);
	}
	struct Desc tss_desc;	
	struct Gate task_gate;	

	uint32 temp = (uint32)tss;
	tss_desc.limitl = sizeof(struct TSS) - 1;
	tss_desc.basel = (uint16)(temp & 0xFFFF);
	tss_desc.basem = (uint8)((temp >> 16) & 0xFF);
	tss_desc.baseh = (uint8)((temp >> 24) & 0xFF);
	tss_desc.attr = AT386TSS + DPL0;
	KnlSetDescToGDT(29, (uint8 *)&tss_desc);

	task_gate.offsetl = 0;
	task_gate.offseth = 0;
	task_gate.dcount = 0;
	task_gate.selector = (29 << 3) | RPL0;
	task_gate.attr = ATTASKGATE | DPL0;
	KnlSetGateToIDT(0x10, (uint8 *)&task_gate);

	_KnlFillTSS(tss, (uint32)_KnlMathFaultExceptionInterrupt, (uint32)stack);
}

/**
	@Function:		_KnlMathFaultExceptionInterrupt
	@Access:		Private
	@Description:
		处理x87 FPU浮点错误（数学错误）故障的中断处理程序。
	@Parameters:
	@Return:		
*/
static
void
_KnlMathFaultExceptionInterrupt(void)
{
	while(1)
	{
		if(is_kernel_task)
		{
			struct die_info info;
			fill_info(info, DC_MF, DI_MF);
			die(&info);
		}
		else
		{
			lock();
			if(!use_rtc_for_task_scheduler)
				ApicStopTimer();
			struct Task * task = TskmgrGetTaskInfoPtr(current_tid);

			int8 buffer[1024];
			sprintf_s(	buffer,
						1024,
						"A task causes a exception of FPU float point error, the id is %d, the name is '%s'\n",
						current_tid,
						task->name);
			Log(LOG_ERROR, buffer);

			ScrPrintStringWithProperty(buffer, CC_RED);
			ScrPrintString("\n");
			_KnlKillTaskAndJumpToKernel(current_tid);
		}
	}
}

/*================================================================================
								对齐检查故障, 0x11
================================================================================*/

/**
	@Function:		_KnlInitAlignCheckException
	@Access:		Private
	@Description:
		初始化处理对齐检查故障的中断处理程序。
	@Parameters:
	@Return:	
*/
static
void
_KnlInitAlignCheckException(void)
{
	struct die_info info;
	struct TSS * tss = &ac_tss;
	uint8 * stack = (uint8 *)MemAlloc(INTERRUPT_PROCEDURE_STACK_SIZE);
	ac_stack = stack;
	if(tss == NULL)
	{
		fill_info(info, DC_INIT_AC, DI_INIT_AC);
		die(&info);
	}
	struct Desc tss_desc;	
	struct Gate task_gate;	

	uint32 temp = (uint32)tss;
	tss_desc.limitl = sizeof(struct TSS) - 1;
	tss_desc.basel = (uint16)(temp & 0xFFFF);
	tss_desc.basem = (uint8)((temp >> 16) & 0xFF);
	tss_desc.baseh = (uint8)((temp >> 24) & 0xFF);
	tss_desc.attr = AT386TSS + DPL0;
	KnlSetDescToGDT(158, (uint8 *)&tss_desc);

	task_gate.offsetl = 0;
	task_gate.offseth = 0;
	task_gate.dcount = 0;
	task_gate.selector = (158 << 3) | RPL0;
	task_gate.attr = ATTASKGATE | DPL0;
	KnlSetGateToIDT(0x11, (uint8 *)&task_gate);

	_KnlFillTSS(tss, (uint32)_KnlAlignCheckExceptionInterrupt, (uint32)stack);
}

/**
	@Function:		_KnlAlignCheckExceptionInterrupt
	@Access:		Private
	@Description:
		处理对齐检查故障的中断处理程序。
	@Parameters:
	@Return:		
*/
static
void
_KnlAlignCheckExceptionInterrupt(void)
{
	while(1)
	{
		if(is_kernel_task)
		{
			struct die_info info;
			fill_info(info, DC_AC, DI_AC);
			die(&info);
		}
		else
		{
			lock();
			if(!use_rtc_for_task_scheduler)
				ApicStopTimer();
			struct Task * task = TskmgrGetTaskInfoPtr(current_tid);

			int8 buffer[1024];
			sprintf_s(	buffer,
						1024,
						"A task causes a exception of alignment check, the id is %d, the name is '%s'\n",
						current_tid,
						task->name);
			Log(LOG_ERROR, buffer);

			ScrPrintStringWithProperty(buffer, CC_RED);
			ScrPrintString("\n");
			_KnlKillTaskAndJumpToKernel(current_tid);
		}
	}
}

/*================================================================================
							处理机器检查故障, 0x12
================================================================================*/

/**
	@Function:		_KnlInitMachineCheckException
	@Access:		Private
	@Description:
		初始化处理机器检查故障的中断程序。
	@Parameters:
	@Return:
*/
static
void
_KnlInitMachineCheckException(void)
{
	struct die_info info;
	struct TSS * tss = &mc_tss;
	uint8 * stack = (uint8 *)MemAlloc(INTERRUPT_PROCEDURE_STACK_SIZE);
	mc_stack = stack;
	if(tss == NULL)
	{
		fill_info(info, DC_INIT_MC, DI_INIT_MC);
		die(&info);
	}
	struct Desc tss_desc;	
	struct Gate task_gate;	

	uint32 temp = (uint32)tss;
	tss_desc.limitl = sizeof(struct TSS) - 1;
	tss_desc.basel = (uint16)(temp & 0xFFFF);
	tss_desc.basem = (uint8)((temp >> 16) & 0xFF);
	tss_desc.baseh = (uint8)((temp >> 24) & 0xFF);
	tss_desc.attr = AT386TSS + DPL0;
	KnlSetDescToGDT(159, (uint8 *)&tss_desc);

	task_gate.offsetl = 0;
	task_gate.offseth = 0;
	task_gate.dcount = 0;
	task_gate.selector = (159 << 3) | RPL0;
	task_gate.attr = ATTASKGATE | DPL0;
	KnlSetGateToIDT(0x12, (uint8 *)&task_gate);

	_KnlFillTSS(tss, (uint32)_KnlMachineCheckExceptionInterrupt, (uint32)stack);
}

/**
	@Function:		_KnlMachineCheckExceptionInterrupt
	@Access:		Private
	@Description:
		处理机器检查故障的中断程序。
	@Parameters:
	@Return:
*/
static
void
_KnlMachineCheckExceptionInterrupt(void)
{
	if(is_kernel_task)
	{
		struct die_info info;
		fill_info(info, DC_MC_KNL, DI_MC_KNL);
		die(&info);
	}
	else
	{
		struct Task * task = TskmgrGetTaskInfoPtr(current_tid);
		int8 buffer[1024];
		sprintf_s(	buffer,
					1024,
					DI_MC_TSK,
					current_tid,
					task->name);
		struct die_info info;
		fill_info(info, DC_MC_TSK, buffer);
		die(&info);
	}
}

/*================================================================================
								SIMD浮点异常故障, 0x13
================================================================================*/

/**
	@Function:		_KnlInitSIMDFloatPointException
	@Access:		Private
	@Description:
		初始化处理SIMD浮点异常故障的中断处理程序。
	@Parameters:
	@Return:	
*/
static
void
_KnlInitSIMDFloatPointException(void)
{
	struct die_info info;
	struct TSS * tss = &xf_tss;
	uint8 * stack = (uint8 *)MemAlloc(INTERRUPT_PROCEDURE_STACK_SIZE);
	xf_stack = stack;
	if(tss == NULL)
	{
		fill_info(info, DC_INIT_XF, DI_INIT_XF);
		die(&info);
	}
	struct Desc tss_desc;	
	struct Gate task_gate;	

	uint32 temp = (uint32)tss;
	tss_desc.limitl = sizeof(struct TSS) - 1;
	tss_desc.basel = (uint16)(temp & 0xFFFF);
	tss_desc.basem = (uint8)((temp >> 16) & 0xFF);
	tss_desc.baseh = (uint8)((temp >> 24) & 0xFF);
	tss_desc.attr = AT386TSS + DPL0;
	KnlSetDescToGDT(160, (uint8 *)&tss_desc);

	task_gate.offsetl = 0;
	task_gate.offseth = 0;
	task_gate.dcount = 0;
	task_gate.selector = (160 << 3) | RPL0;
	task_gate.attr = ATTASKGATE | DPL0;
	KnlSetGateToIDT(0x13, (uint8 *)&task_gate);

	_KnlFillTSS(tss, (uint32)_KnlSIMDFloatPointExceptionInterrupt, (uint32)stack);
}

/**
	@Function:		_KnlSIMDFloatPointExceptionInterrupt
	@Access:		Private
	@Description:
		处理SIMD浮点异常故障的中断处理程序。
	@Parameters:
	@Return:		
*/
static
void
_KnlSIMDFloatPointExceptionInterrupt(void)
{
	while(1)
	{
		if(is_kernel_task)
		{
			struct die_info info;
			fill_info(info, DC_XF, DI_XF);
			die(&info);
		}
		else
		{
			lock();
			if(!use_rtc_for_task_scheduler)
				ApicStopTimer();
			struct Task * task = TskmgrGetTaskInfoPtr(current_tid);

			int8 buffer[1024];
			sprintf_s(	buffer,
						1024,
						"A task causes a exception of SIMD float point, the id is %d, the name is '%s'\n",
						current_tid,
						task->name);
			Log(LOG_ERROR, buffer);

			ScrPrintStringWithProperty(buffer, CC_RED);
			ScrPrintString("\n");
			_KnlKillTaskAndJumpToKernel(current_tid);
		}
	}
}

/*================================================================================
							处理未实现的中断程序
================================================================================*/

/**
	@Function:		_KnlInitUnimplementedInterruptException
	@Access:		Private
	@Description:
		初始化未被使用的中断的 TSS 结构体。
	@Parameters:
	@Return:	
*/
static
void
_KnlInitUnimplementedInterruptException(void)
{
	struct die_info info;
	struct TSS * tss = &noimpl_tss;
	uint8 * stack = (uint8 *)MemAlloc(INTERRUPT_PROCEDURE_STACK_SIZE);
	if(tss == NULL)
	{
		fill_info(info, DC_INIT_NOIMPL, DI_INIT_NOIMPL);
		die(&info);
	}
	struct Desc tss_desc;	
	struct Gate task_gate;	

	uint32 temp = (uint32)tss;
	tss_desc.limitl = sizeof(struct TSS) - 1;
	tss_desc.basel = (uint16)(temp & 0xFFFF);
	tss_desc.basem = (uint8)((temp >> 16) & 0xFF);
	tss_desc.baseh = (uint8)((temp >> 24) & 0xFF);
	tss_desc.attr = AT386TSS + DPL3;
	KnlSetDescToGDT(17, (uint8 *)&tss_desc);

	task_gate.offsetl = 0;
	task_gate.offseth = 0;
	task_gate.dcount = 0;
	task_gate.selector = (17 << 3) | RPL3;
	task_gate.attr = ATTASKGATE | DPL3;
	KnlSetDescToGDT(27, (uint8 *)&task_gate);

	_KnlFillTSS(tss, (uint32)_KnlUnimplementedInterrupt, (uint32)stack);
}

/**
	@Function:		_KnlSetUnimplementedInterrupt
	@Access:		Private
	@Description:
		可以设置相应的中断为未实现。
	@Parameters:
		int_num, uint32, IN
			中断号。
	@Return:
*/
static
void
_KnlSetUnimplementedInterrupt(IN uint32 int_num)
{
	if(int_num >= 256)
		return;
	struct Gate task_gate;
	task_gate.offsetl = 0;
	task_gate.offseth = 0;
	task_gate.dcount = 0;
	task_gate.selector = (17 << 3) | RPL0;
	task_gate.attr = ATTASKGATE | DPL0;
	KnlSetGateToIDT(int_num, (uint8 *)&task_gate);
}

/**
	@Function:		_KnlUnimplementedInterrupt
	@Access:		Private
	@Description:
		未被使用的中断的中断过程。
	@Parameters:
	@Return:
*/
static
void
_KnlUnimplementedInterrupt(void)
{
	while(1)
	{
		if(is_kernel_task)
		{
			struct die_info info;

			int32 intn = KnlGetUnimplementedInterruptNo();
			int8 buffer[1024];
			sprintf_s(	buffer,
						1024,
						DI_NOIMPLEMENT_INT,
						intn,
						intn);

			fill_info(info, DC_NOIMPLEMENT_INT, buffer);
			die(&info);
		}
		else
		{
			lock();
			if(!use_rtc_for_task_scheduler)
				ApicStopTimer();
			struct Task * task = TskmgrGetTaskInfoPtr(current_tid);
			int32 intn = KnlGetUnimplementedInterruptNo();

			int8 buffer[1024];
			sprintf_s(	buffer,
						1024,
						"A task causes a error of calling not implement interrupt(%d, 0x%X) procedure"
						", the id is %d, the name is '%s'\n",
						intn,
						intn,
						current_tid,
						task->name);
			Log(LOG_ERROR, buffer);

			ScrPrintStringWithProperty(buffer, CC_RED);
			ScrPrintString("\n");

			_KnlKillTaskAndJumpToKernel(current_tid);
		}
	}
}

/**
	@Function:		KnlShutdownSystem
	@Access:		Public
	@Description:
		关闭系统。
	@Parameters:
	@Return:	
*/
void
KnlShutdownSystem(void)
{
	_KnlLeaveSystem();
	AcpiPowerOff();
	KnlHaltCpu();
}

/**
	@Function:		KnlRebootSystem
	@Access:		Public
	@Description:
		重启系统。
	@Parameters:
	@Return:	
*/
void
KnlRebootSystem(void)
{
	_KnlLeaveSystem();
	KnlResetCpu();
}

#define	_PRESS_KEY_TO_CONTINUE()	\
{	\
	ScrPrintString("Press any key to continue!\n");	\
	get_char();	\
}

/**
	@Function:		_KnlEnterSystem
	@Access:		Private
	@Description:
		初始化所有资源后被调用。该过程会对系统进行安全检测。
	@Parameters:
	@Return:		
*/
static
void
_KnlEnterSystem(void)
{
	ScrPrintString("Checking system...\n");

	//检查文件系统
	FileObject * fptr = Ifs1OpenFile(SYSTEM_PATH"lock/system.lock", FILE_MODE_ALL);
	if(fptr != NULL)
		if(flen(fptr) != 0)
		{
			Ifs1CloseFile(fptr);
			ScrPrintString("The file system has some error!\n");
			if(DskGetSize("DA") != 0)
				if(Ifs1Repair("DA"))
					ScrPrintString("The file system(DA) has been repaired!\n");
				else				
					ScrPrintString("Failed to repair the file system(DA)!\n");
			if(DskGetSize("DB") != 0)
				if(Ifs1Repair("DB"))
					ScrPrintString("The file system(DB) has been repaired!\n");
				else				
					ScrPrintString("Failed to repair the file system(DB)!\n");
		}
		else
		{
			Ifs1WriteFile(fptr, "1", 1);
			ScrPrintString("The file system is OK!\n");
			Ifs1CloseFile(fptr);
		}
	else
	{
		ScrPrintString("The file system has some error!\n");
		ScrPrintString("Failed to repair the file System!\n");
	}
	//_PRESS_KEY_TO_CONTINUE();
	ScrClearScreen();
	Log(LOG_NORMAL, "Enter system.");
}

/**
	@Function:		_KnlLeaveSystem
	@Access:		Private
	@Description:
		离开系统时被调用。
	@Parameters:
	@Return:	
*/
static
void
_KnlLeaveSystem(void)
{
	FileObject * fptr = Ifs1OpenFile(SYSTEM_PATH"lock/system.lock", FILE_MODE_WRITE);
	if(fptr != NULL)
	{
		Ifs1WriteFile(fptr, "", 0);
		Ifs1CloseFile(fptr);
	}
	Log(LOG_NORMAL, "Leave system.");
	LogFree();
}

/**
	@Function:		KnlGetIdeSignal
	@Access:		Public
	@Description:
		检查IDE是否有信号到达。
	@Parameters:
		primary, BOOL, IN
			TRUE时表示第一IDE，FALSE时表示第二IDE。
	@Return:
		BOOL
			返回TRUE则表示有信号到达。
*/
BOOL
KnlGetIdeSignal(IN BOOL primary)
{
	if(primary)
		return _ide0_signal != 0;
	else
		return _ide1_signal != 0;
}

/**
	@Function:		KnlClearIdeSignal
	@Access:		Public
	@Description:
		清除IDE的信号。
	@Parameters:
		primary, BOOL, IN
			TRUE时表示第一IDE，FALSE时表示第二IDE。
	@Return:
*/
void
KnlClearIdeSignal(IN BOOL primary)
{
	if(primary)
	{
		if(_ide0_signal != 0)
			_ide0_signal--;
	}
	else
	{
		if(_ide1_signal != 0)
			_ide1_signal--;
	}
}

/**
	@Function:		KnlResetIdeSignal
	@Access:		Public
	@Description:
		重置IDE的信号。
	@Parameters:
		primary, BOOL, IN
			TRUE时表示第一IDE，FALSE时表示第二IDE。
	@Return:
*/
void
KnlResetIdeSignal(IN BOOL primary)
{
	if(primary)
		_ide0_signal = 0;
	else
		_ide1_signal = 0;
}
