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
#include "ifs1disk.h"
#include "memory.h"
#include "tasks.h"
#include "vesa.h"
#include "sparams.h"
#include "kernel.h"
#include "paging.h"
#include "ifs1fs.h"
#include "system.h"
#include "lock.h"
#include "log.h"
#include "acpi.h"
#include "mqueue.h"
#include "pci.h"

#include <dslib/dslib.h>

#define	INTERRUPT_PROCEDURE_STACK_SIZE	(KB(64))

static
BOOL
init_dsl(void);

static
void
fpu_error_int(void);

static
void
init_interrupt(void);

static
void
init_noimpl(void);

static
void
set_noimpl(IN uint32 int_num);

static
void
noimpl_int(void);

static
void
init_gp(void);

static
void
gp_int(void);

static
void
init_timer(void);

static
void
timer_int(void);

static
void
init_mouse(void);

static
void
mouse_int(void);

static
void
init_keyboard(void);

static
void
keyboard_int(void);

static
void
init_ide(void);

static
void
ide_int(void);

static
void
init_fpu(void);

static
void
fpu_int(void);

static
void
init_pf(void);

static
void
pf_int(void);

static
void
system_call_screen(	IN uint32 func,
					IN uint32 base,
					IN OUT struct SParams * sparams);

static
void
system_call(void);

static
void
system_call_int(void);

static
void
init_system_call(void);

static
void
enter_system(void);

static
void
leave_system(void);

static
void
init_dividing_by_zero(void);

static
void
dividing_by_zero_int(void);

static
void
init_bound_check(void);

static
void
bound_check_int(void);

static
void
init_invalid_opcode(void);

static
void
invalid_opcode_int(void);

static
void
init_invalid_tss(void);

static
void
invalid_tss_int(void);

static
void
init_invalid_seg(void);

static
void
invalid_seg_int(void);

static
void
init_invalid_stack(void);

static
void
invalid_stack_int(void);

static uint32 gdt_addr = NULL;
static uint32 current_tid;
static BOOL is_kernel_task = TRUE;

//内核各个任务的TSS
static struct TSS noimpl_tss;
static struct TSS gp_tss;
static struct TSS timer_tss;
static struct TSS mouse_tss;
static struct TSS keyboard_tss;
static struct TSS ide_tss;
static struct TSS fpu_tss;
static struct TSS scall_tss[MAX_TASK_COUNT];
static struct TSS pf_tss;

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
	init_memory();
	
	//必须在这里禁用所有锁
	disable_memory_lock();
	disable_paging_lock();
	disable_kernel_lock();
	disable_tasks_lock();
	disable_console_lock();
	disable_ifs1_lock();

	init_vesa();
	init_paging();
	init_noimpl();
	init_gp();
	init_pf();
	init_dividing_by_zero();
	init_bound_check();
	init_invalid_opcode();
	init_invalid_tss();
	init_invalid_seg();
	init_invalid_stack();
	init_interrupt();
	init_disk("VA");
	init_disk("VB");
	init_disk("HD");
	init_ide();
	init_screen();
	init_enfont();
	init_timer();
	init_mouse();
	init_keyboard();
	init_fpu();
	init_tasks();
	init_system_call();
	gdt_addr = get_gdt_addr();
	init_keyboard_driver();
	init_cpu();
	enable_paging();
	init_dsl();
	init_log();
	mqueue_init();
	acpi_init();
	pci_init();
	sti();

	//在这里启用所有锁
	enable_memory_lock();	
	enable_paging_lock();
	enable_kernel_lock();
	enable_tasks_lock();
	enable_console_lock();
	enable_ifs1_lock();

	enter_system();
	console();
}

DEFINE_LOCK_IMPL(kernel)

/**
	@Function:		knl_dsl_malloc
	@Access:		Private
	@Description:
		为 DSL 库准备的释放 malloc 函数。
	@Parameters:
		num_bytes, uint32, IN
			分配大小。
	@Return:
		void *
			分配的内存的指针。返回 NULL 则失败。
*/
static
void *
knl_dsl_malloc(IN uint32 num_bytes)
{
	return alloc_memory(num_bytes);
}

/**
	@Function:		knl_dsl_calloc
	@Access:		Private
	@Description:
		为 DSL 库准备的释放 calloc 函数。
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
knl_dsl_calloc(	IN uint32 n, 
			IN uint32 size)
{
	return alloc_memory(n * size);
}

/**
	@Function:		knl_dsl_free
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
knl_dsl_free(IN void * ptr)
{
	free_memory(ptr);
}

/**
	@Function:		init_dsl
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
init_dsl(void)
{
	DSLEnvironment env;
	env.dsl_malloc = knl_dsl_malloc;
	env.dsl_calloc = knl_dsl_calloc;
	env.dsl_free = knl_dsl_free;
	return dsl_init(&env);
}

/**
	@Function:		init_interrupt
	@Access:		Private
	@Description:
		初始化中断。
	@Parameters:
	@Return:	
*/
static
void
init_interrupt(void)
{
	uint8 n;
	for(n = 0; n <= 255; n++)
	{
		if(	n == 0x00
			|| n == 0x05
			|| n == 0x06
			|| n == 0x0a
			|| n == 0x0b
			|| n == 0x0c
			|| n == 0x0d
			|| n == 0x0e
			|| n == 0x21 
			|| n == 0x22 
			|| n == 0x40 
			|| n == 0x41
			|| n == 0x75
			|| n == 0x76)
			continue;
		else if(n == 7)
		{
			uint32 fpu_error_int_addr = (uint32)&fpu_error_int;
			set_int(n, fpu_error_int_addr);
		}
		else if(n == 0x90)
		{
			uint32 system_call_int_addr = (uint32)&system_call_int;
			set_int(n, system_call_int_addr);
		}
		else
			set_noimpl(n);
		if(n == 255)
			break;
	}
}

/**
	@Function:		fpu_error_int
	@Access:		Private
	@Description:
		FPU 错误的中断处理函数。
	@Parameters:
	@Return:		
*/
static
void
fpu_error_int(void)
{
	//print_str("\n{FPU ERROR}\n");
	asm volatile ("clts");
	INT_EXIT();
}

/**
	@Function:		fill_tss
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
fill_tss(	OUT struct TSS * tss, 
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
	tss->cr3 = get_kernel_cr3();
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

/**
	@Function:		init_noimpl
	@Access:		Private
	@Description:
		初始化未被使用的中断的 TSS 结构体。
	@Parameters:
	@Return:	
*/
static
void
init_noimpl(void)
{
	struct die_info info;
	struct TSS * tss = &noimpl_tss;
	uint8 * stack = (uint8 *)alloc_memory(INTERRUPT_PROCEDURE_STACK_SIZE);
	if(tss == NULL)
	{
		fill_info(info, DC_INIT_NOIMPL, DI_INIT_NOIMPL);
		die(&info);
	}
	struct Desc tss_desc;	

	uint32 temp = (uint32)tss;
	tss_desc.limitl = sizeof(struct TSS) - 1;
	tss_desc.basel = (uint16)(temp & 0xFFFF);
	tss_desc.basem = (uint8)((temp >> 16) & 0xFF);
	tss_desc.baseh = (uint8)((temp >> 24) & 0xFF);
	tss_desc.attr = AT386TSS + DPL0;
	set_desc_to_gdt(17, (uint8 *)&tss_desc);

	fill_tss(tss, (uint32)noimpl_int, (uint32)stack);
}

/**
	@Function:		init_timer
	@Access:		Private
	@Description:
		初始化定时器的中断程序。
	@Parameters:
	@Return:		
*/
static
void
init_timer(void)
{
	struct die_info info;
	struct TSS * tss = &timer_tss;
	uint8 * stack = (uint8 *)alloc_memory(INTERRUPT_PROCEDURE_STACK_SIZE);
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
	set_desc_to_gdt(10, (uint8 *)&tss_desc);

	task_gate.offsetl = 0;
	task_gate.offseth = 0;
	task_gate.dcount = 0;
	task_gate.selector = (10 << 3) | RPL0;
	task_gate.attr = ATTASKGATE | DPL0;
	set_gate_to_idt(0x40, (uint8 *)&task_gate);

	fill_tss(tss, (uint32)timer_int, (uint32)stack);
}

/**
	@Function:		init_mouse
	@Access:		Private
	@Description:
		初始化鼠标的中断程序。
	@Parameters:
	@Return:	
*/
static
void
init_mouse(void)
{
	struct die_info info;
	struct TSS * tss = &mouse_tss;
	uint8 * stack = (uint8 *)alloc_memory(INTERRUPT_PROCEDURE_STACK_SIZE);
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
	set_desc_to_gdt(19, (uint8 *)&tss_desc);

	task_gate.offsetl = 0;
	task_gate.offseth = 0;
	task_gate.dcount = 0;
	task_gate.selector = (19 << 3) | RPL0;
	task_gate.attr = ATTASKGATE | DPL0;
	set_gate_to_idt(0x74, (uint8 *)&task_gate);

	fill_tss(tss, (uint32)mouse_int, (uint32)stack);
	
	//允许鼠标接口
	outb(0x64, 0xa8);
	//通知8042下一个字节的0x60的数据发送给鼠标
	outb(0x64, 0xd4);
	//允许鼠标发送数据
	outb(0x60, 0xf4);
	//通知8042下一个字节发向0x60的数据应方向8042的命令寄存器
	outb(0x64, 0x60);
	//许可键盘及鼠标接口及中断
	outb(0x60, 0x47);
	
	uint8 mask = inb(0xa1);
	mask &= 0xef;
	outb(0xa1, 0xef);
}

/**
	@Function:		init_keyboard
	@Access:		Private
	@Description:
		初始化键盘的中断程序。
	@Parameters:
	@Return:		
*/
static
void
init_keyboard(void)
{
	struct die_info info;
	struct TSS * tss = &keyboard_tss;
	uint8 * stack = (uint8 *)alloc_memory(INTERRUPT_PROCEDURE_STACK_SIZE);
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
	set_desc_to_gdt(12, (uint8 *)&tss_desc);

	task_gate.offsetl = 0;
	task_gate.offseth = 0;
	task_gate.dcount = 0;
	task_gate.selector = (12 << 3) | RPL0;
	task_gate.attr = ATTASKGATE | DPL0;
	set_gate_to_idt(0x41, (uint8 *)&task_gate);

	fill_tss(tss, (uint32)keyboard_int, (uint32)stack);
}

/**
	@Function:		init_ide
	@Access:		Private
	@Description:
		初始化 IDE 的中断程序。
	@Parameters:
	@Return:	
*/
static
void
init_ide(void)
{
	struct die_info info;
	struct TSS * tss = &ide_tss;
	uint8 * stack = (uint8 *)alloc_memory(INTERRUPT_PROCEDURE_STACK_SIZE);
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
	set_desc_to_gdt(13, (uint8 *)&tss_desc);

	task_gate.offsetl = 0;
	task_gate.offseth = 0;
	task_gate.dcount = 0;
	task_gate.selector = (13 << 3) | RPL0;
	task_gate.attr = ATTASKGATE | DPL0;
	set_gate_to_idt(0x76, (uint8 *)&task_gate);

	fill_tss(tss, (uint32)ide_int, (uint32)stack);
}

/**
	@Function:		init_fpu
	@Access:		Private
	@Description:
		初始化 FPU 的中断程序。
	@Parameters:
	@Return:		
*/
static
void
init_fpu(void)
{
	struct die_info info;
	struct TSS * tss = &fpu_tss;
	uint8 * stack = (uint8 *)alloc_memory(INTERRUPT_PROCEDURE_STACK_SIZE);
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
	set_desc_to_gdt(16, (uint8 *)&tss_desc);

	task_gate.offsetl = 0;
	task_gate.offseth = 0;
	task_gate.dcount = 0;
	task_gate.selector = (16 << 3) | RPL0;
	task_gate.attr = ATTASKGATE | DPL0;
	set_gate_to_idt(0x75, (uint8 *)&task_gate);
	
	fill_tss(tss, (uint32)fpu_int, (uint32)stack);
}

/**
	@Function:		free_system_call_tss
	@Access:		Private
	@Description:
		释放系统调用的 TSS。清除系统调用的 TSS 的 BUSY 状态。
	@Parameters:
	@Return:	
*/
static
void
free_system_call_tss(void)
{
	struct Desc system_call_tss_desc;
	get_desc_from_gdt(14, (uint8 *)&system_call_tss_desc);
	system_call_tss_desc.attr = AT386TSS + DPL0; 
	set_desc_to_gdt(14, (uint8 *)&system_call_tss_desc);
}

static int32 counter = 1;
static int32 clock_counter = 100;
static int32 is_system_call = 0;
static int32 is_enable_flush_screen = 1;
static int32 flush_counter = 0;


/**
	@Function:		timer_int
	@Access:		Private
	@Description:
		定时器的中断程序。
	@Parameters:
	@Return:	
*/
static
void
timer_int(void)
{	
	while(1)
	{
		if(is_enable_flush_screen && vesa_is_valid())
			if(++flush_counter == 2)
			{
				flush_screen();
				flush_counter = 0;
			}
		if(!is_system_call)
		{
			if(--clock_counter == 0)
			{
				console_clock();
				clock_counter = 100;
			}
			uint32 running_tid = get_running_tid();
			uint32 tid = get_next_task_id();
			current_tid = tid;
			if(counter == 1 || tid == -1)
			{
				counter = 0;
				is_kernel_task = TRUE;
				struct Task task;
				if(running_tid != -1)
				{
					get_task_info(running_tid, &task);
					asm volatile ("fnsave %0"::"m"(task.i387_state));
					set_task_info(running_tid, &task);
				}
				struct Desc kernel_tss_desc;
				get_desc_from_gdt(6, (uint8 *)&kernel_tss_desc);
				kernel_tss_desc.attr = AT386TSS + DPL0; 
				set_desc_to_gdt(6, (uint8 *)&kernel_tss_desc);
				free_system_call_tss();
				outb(0x20, 0x20);
				outb(0xa0, 0x20);
				asm volatile ("ljmp	$56, $0;");
			}
			else
			{
				counter++;
				is_kernel_task = FALSE;
				struct Task task;
				if(running_tid != -1)
				{
					get_task_info(running_tid, &task);
					asm volatile ("fnsave %0"::"m"(task.i387_state));
					set_task_info(running_tid, &task);
				}
				get_task_info(tid, &task);
				if(task.init_i387)
					asm volatile ("frstor %0"::"m"(task.i387_state));
				else
				{
					asm volatile ("fninit");
					task.init_i387 = 1;
					set_task_info(tid, &task);
				}
				set_task_ran_state(tid);
				struct Desc tss_desc;
				struct Gate task_gate;
				get_desc_from_gdt(400 + tid * 5 + 0, (uint8 *)&tss_desc);
				get_desc_from_gdt(400 + tid * 5 + 1, (uint8 *)&task_gate);
				tss_desc.attr = AT386TSS + DPL3;
				set_desc_to_gdt(400 + tid * 5 + 0, (uint8 *)&tss_desc);
				set_desc_to_gdt(11, (uint8 *)&task_gate);
				free_system_call_tss();
				outb(0x20, 0x20);
				outb(0xa0, 0x20);
				asm volatile ("ljmp	$88, $0;");	
			}
		}
		else
		{
			outb(0x20, 0x20);
			outb(0xa0, 0x20);
			asm volatile ("iret;");
		}
	}
}

/**
	@Function:		enable_flush_screen
	@Access:		Public
	@Description:
		允许刷新屏幕。
	@Parameters:
	@Return:	
*/
void
enable_flush_screen(void)
{
	is_enable_flush_screen = 1;
}

/**
	@Function:		disable_flush_screen
	@Access:		Public
	@Description:
		禁用刷新屏幕。
	@Parameters:
	@Return:	
*/
void
disable_flush_screen(void)
{
	is_enable_flush_screen = 0;
}

static int32 old_mouse_x = 0;
static int32 old_mouse_y = 0;
static int32 mouse_x = 0;
static int32 mouse_y = 0;
static BOOL mouse_left_button_down = 0;
static BOOL mouse_right_button_down = 0;
static int32 mouse_count = 0;
static int32 x_sign = 0;
static int32 y_sign = 0;

/**
	@Function:		mouse_int
	@Access:		Private
	@Description:
		鼠标的中断程序。
	@Parameters:
	@Return:		
*/
static
void
mouse_int(void)
{
	while(1)
	{
		int32 max_screen_width = vesa_get_width();
		int32 max_screen_height = vesa_get_height();
		uint8 data = inb(0x60);
	
		switch(++mouse_count)
		{
			case 1:
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
				break;
			case 2:
				mouse_x += (x_sign | data);
				if(mouse_x < 0)
					mouse_x = 0;
				else if(mouse_x >= max_screen_width)
					mouse_x = max_screen_width - 1;
				break;
			case 3:
				mouse_y += -(y_sign | data);
				if(mouse_y < 0)
					mouse_y = 0;
				else if(mouse_y >= max_screen_height)
					mouse_y = max_screen_height - 1;
				mouse_count = 0;
				break;
		}
		outb(0x20, 0x20);
		outb(0xa0, 0x20);
		asm volatile ("iret");
	}
}

/**
	@Function:		get_mouse_position
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
get_mouse_position(	OUT int32 * x,
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
	@Function:		is_mouse_left_button_down
	@Access:		Public
	@Description:
		获取鼠标的左键是否被按下。
	@Parameters:
	@Return:
		BOOL
			返回TRUE则被按下，否则没被按下。
*/
BOOL
is_mouse_left_button_down(void)
{
	if(mouse_count == 0)
		return mouse_left_button_down;
	else
		return FALSE;
}

/**
	@Function:		is_mouse_right_button_down
	@Access:		Public
	@Description:
		获取鼠标的右键是否被按下。
	@Parameters:
	@Return:
		BOOL
			返回TRUE则被按下，否则没被按下。
*/
BOOL
is_mouse_right_button_down(void)
{
	if(mouse_count == 0)
		return mouse_right_button_down;
	else
		return FALSE;
}

/**
	@Function:		keyboard_int
	@Access:		Private
	@Description:
		键盘的中断程序。
	@Parameters:
	@Return:	
*/
static
void
keyboard_int(void)
{
	while(1)
	{
		uint8 scan_code = 0;
		scan_code = inb(0x60);
		tran_key(scan_code);
		outb(0x20, 0x20);
		outb(0xa0, 0x20);
		asm volatile ("iret;");
	}
}

/**
	@Function:		ide_int
	@Access:		Private
	@Description:
		IDE 的中断程序。
	@Parameters:
	@Return:	
*/
static
void
ide_int(void)
{
	while(1)
	{
		outb(0x20, 0x20);
		outb(0xa0, 0x20);
		asm volatile ("iret;");
	}
}

/**
	@Function:		fpu_int
	@Access:		Private
	@Description:
		FPU 的中断程序。
	@Parameters:
	@Return:	
*/
static
void
fpu_int(void)
{
	while(1)
	{
		outb(0xf0, 0x00);
		outb(0x20, 0x20);
		outb(0xa0, 0x20);
		asm volatile ("iret;");
	}
}

static struct TSS * scall_tsses[MAX_TASK_COUNT];
static uint8 * scall_stacks[MAX_TASK_COUNT];

/**
	@Function:		init_system_call
	@Access:		Private
	@Description:
		初始化系统调用的中断程序。
	@Parameters:
	@Return:
*/
static
void
init_system_call(void)
{
	uint32 ui;
	for(ui = 0; ui < MAX_TASK_COUNT; ui++)
	{
		struct die_info info;
		struct TSS * tss = &(scall_tss[ui]);
		scall_tsses[ui] = tss;
		uint8 * stack = (uint8 *)alloc_memory(INTERRUPT_PROCEDURE_STACK_SIZE);
		if(tss == NULL)
		{
			fill_info(info, DC_INIT_SCALL, DI_INIT_SCALL);
			die(&info);
		}
		scall_stacks[ui] = stack;
		struct Desc tss_desc;
		struct Gate task_gate;

		uint32 temp = (uint32)tss;
		tss_desc.limitl = sizeof(struct TSS) - 1;
		tss_desc.basel = (uint16)(temp & 0xFFFF);
		tss_desc.basem = (uint8)((temp >> 16) & 0xFF);
		tss_desc.baseh = (uint8)((temp >> 24) & 0xFF);
		tss_desc.attr = AT386TSS + DPL3;
		set_desc_to_gdt(30 + ui * 2 + 0, (uint8 *)&tss_desc);

		task_gate.offsetl = 0;
		task_gate.offseth = 0;
		task_gate.dcount = 0;
		task_gate.selector = ((30 + ui * 2 + 0) << 3) | RPL3;
		task_gate.attr = ATTASKGATE | DPL3;
		set_desc_to_gdt(30 + ui * 2 + 1, (uint8 *)&task_gate);

		fill_tss(tss, (uint32)system_call, (uint32)stack);
		//System Call的任务开中断的原因是如果关闭中断且用户程序调用SCALL_GET_CHAR, SCALL_GET_STR_N等
		//功能时会导致等待按键队列有数据, 由于关闭了中断, 键盘驱动程序永远不会被启动, 所以导致无限等待.
		tss->flags = 0x200;
	}
}

/**
	@Function:		system_call
	@Access:		Private
	@Description:
		系统调用。由系统调用中断程序调用。
	@Parameters:
	@Return:	
*/
static
void
system_call(void)
{
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

	is_system_call++;

	uint32 base = (uint32)get_physical_address(ecx, 0x01300000); 
	struct SParams * sparams = get_physical_address(ecx, edx); 
	switch(eax >> 16)
	{
		case SCALL_SCREEN:
			system_call_screen(eax & 0xffff, base, sparams);
			break;
		case SCALL_KEYBOARD:
			system_call_keyboard(eax & 0xffff, base, sparams);
			break;
		case SCALL_FS:
			system_call_fs(eax & 0xffff, base, sparams);
			break;
		case SCALL_SYSTEM:
			system_call_system(eax & 0xffff, base, sparams);
			break;
		case SCALL_MOUSE:
			system_call_mouse(eax & 0xffff, base, sparams);
			break;
	}

	is_system_call--;

	asm volatile ("iret");
}

/**
	@Function:		system_call_int
	@Access:		Private
	@Description:
		系统调用中断程序。
	@Parameters:
	@Return:	
*/
static
void
system_call_int(void)
{
	uint32 edx, ecx, eax;
	asm volatile (
		"pushw	%%si\n\t"
		"pushw	%%ds\n\t"
		"pushw	%%es\n\t"
		"pushw	%%fs\n\t"
		"pushw	%%gs\n\t"
		"movw	$0x4b, %%si\n\t"
		"movw	%%si, %%ds\n\t"
		"movw	%%si, %%es\n\t"
		"movw	%%si, %%fs\n\t"
		"movw	%%si, %%gs\n\t"
		"pushl	%%eax\n\t"
		"pushl	%%ecx\n\t"
		"pushl	%%edx\n\t"
		"popl	%0\n\t"
		"popl	%1\n\t"
		"popl	%2\n\t"
		:
		:"m"(edx), "m"(ecx), "m"(eax));
	
	//获取指定的TSS和Stack。
	struct TSS * system_call_tss = scall_tsses[ecx];
	uint8 * system_call_stack = scall_stacks[ecx];

	//重置System Call的TSS。
	system_call_tss->eip = (uint32)system_call;
	system_call_tss->esp0 = (uint32)(system_call_stack + 0xffffe);
	system_call_tss->esp = (uint32)(system_call_stack + 0xffffe);
	system_call_tss->flags = 0x200;
	system_call_tss->edx = edx;
	system_call_tss->ecx = ecx;
	system_call_tss->eax = eax;

	asm volatile (
		"popw	%gs\n\t"
		"popw	%fs\n\t"
		"popw	%es\n\t"
		"popw	%ds\n\t"
		"popw	%si\n\t");
	
	/*
	????BUG????

	uint16 address[3];
	address[0] = 0;
	address[1] = 0;
	address[2] = (uint16)(((30 + ecx * 2 + 1) << 3) | RPL3);
	asm volatile ("lcall	%0\n\t"::"m"(address));
	*/

	#define SYSTEM_CALL(tid_s)	\
		asm volatile (	".set TARGET"tid_s", (((30 + "tid_s" * 2 + 1) << 3) | 3)\n\t"	\
						"lcall	$TARGET"tid_s", $0\n\t");

	#define SYSTEM_CALL_CASE(tid, tid_s)	\
		case tid:	\
			SYSTEM_CALL(tid_s);	\
			break;

	switch(ecx)
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

	INT_EXIT();
}

/**
	@Function:		kill_task_and_jump_to_kernel
	@Access:		Private
	@Description:
		杀死任务后跳转到内核任务。
	@Parameters:
		tid, uint32, IN
			要杀死的任务任务ID。
	@Return:
*/
static
void
kill_task_and_jump_to_kernel(IN uint32 tid)
{
	kill_task(tid);

	enable_flush_screen();

	int32 wait_app_tid = get_wait_app_tid();
	if(tid == wait_app_tid)
		set_wait_app_tid(-1);

	struct Desc kernel_tss_desc;
	get_desc_from_gdt(6, (uint8 *)&kernel_tss_desc);
	kernel_tss_desc.attr = AT386TSS + DPL0; 
	set_desc_to_gdt(6, (uint8 *)&kernel_tss_desc);
	free_system_call_tss();
	asm volatile ("ljmp	$56, $0;");
}

/*================================================================================
							处理除数为0的故障, 0x00
================================================================================*/

/**
	@Function:		init_dividing_by_zero
	@Access:		Private
	@Description:
		设置除以0时引发的异常处理程序。
	@Parameters:
	@Return:
*/
static
void
init_dividing_by_zero(void)
{
	struct die_info info;
	struct TSS * tss = &pf_tss;
	uint8 * stack = (uint8 *)alloc_memory(INTERRUPT_PROCEDURE_STACK_SIZE);
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
	set_desc_to_gdt(21, (uint8 *)&tss_desc);

	task_gate.offsetl = 0;
	task_gate.offseth = 0;
	task_gate.dcount = 0;
	task_gate.selector = (21 << 3) | RPL0;
	task_gate.attr = ATTASKGATE | DPL0;
	set_gate_to_idt(0x00, (uint8 *)&task_gate);

	fill_tss(tss, (uint32)dividing_by_zero_int, (uint32)stack);
}

/**
	@Function:		dividing_by_zero_int
	@Access:		Private
	@Description:
		处理除以0时引发的异常的中断过程。
	@Parameters:
	@Return:
*/
static
void
dividing_by_zero_int(void)
{
	while(1)
	{
		if(is_kernel_task)
		{
			struct die_info info;
			fill_info(info, DC_DIV_BY_0, DI_DIV_BY_0);
			die(&info);
		}
		else
		{
			struct Task * task = get_task_info_ptr(current_tid);

			int8 buffer[1024];
			sprintf_s(	buffer,
						1024,
						"A task causes a exception of dividing by zero"
						", the id is %d, the name is '%s'\n",
						current_tid,
						task->name);
			log(LOG_ERROR, buffer);

			print_str_p(buffer, CC_RED);
			print_str("\n");

			kill_task_and_jump_to_kernel(current_tid);
		}
	}
}

/*================================================================================
							处理边界检测的故障, 0x05
================================================================================*/

/**
	@Function:		init_bound_check
	@Access:		Private
	@Description:
		设置边界检查时引发的异常处理程序。
	@Parameters:
	@Return:
*/
static
void
init_bound_check(void)
{
	struct die_info info;
	struct TSS * tss = &pf_tss;
	uint8 * stack = (uint8 *)alloc_memory(INTERRUPT_PROCEDURE_STACK_SIZE);
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
	set_desc_to_gdt(22, (uint8 *)&tss_desc);

	task_gate.offsetl = 0;
	task_gate.offseth = 0;
	task_gate.dcount = 0;
	task_gate.selector = (22 << 3) | RPL0;
	task_gate.attr = ATTASKGATE | DPL0;
	set_gate_to_idt(0x05, (uint8 *)&task_gate);

	fill_tss(tss, (uint32)bound_check_int, (uint32)stack);
}

/**
	@Function:		bound_check_int
	@Access:		Private
	@Description:
		处理边界检查时引发的异常的中断过程。
	@Parameters:
	@Return:
*/
static
void
bound_check_int(void)
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
			struct Task * task = get_task_info_ptr(current_tid);

			int8 buffer[1024];
			sprintf_s(	buffer,
						1024,
						"A task causes a exception of bound check"
						", the id is %d, the name is '%s'\n",
						current_tid,
						task->name);
			log(LOG_ERROR, buffer);

			print_str_p(buffer, CC_RED);
			print_str("\n");

			kill_task_and_jump_to_kernel(current_tid);
		}
	}
}

/*================================================================================
							处理非法的操作码的故障, 0x06
================================================================================*/

/**
	@Function:		init_invalid_opcode
	@Access:		Private
	@Description:
		设置检测到错误的操作码时引发的异常处理程序。
	@Parameters:
	@Return:
*/
static
void
init_invalid_opcode(void)
{
	struct die_info info;
	struct TSS * tss = &pf_tss;
	uint8 * stack = (uint8 *)alloc_memory(INTERRUPT_PROCEDURE_STACK_SIZE);
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
	set_desc_to_gdt(23, (uint8 *)&tss_desc);

	task_gate.offsetl = 0;
	task_gate.offseth = 0;
	task_gate.dcount = 0;
	task_gate.selector = (23 << 3) | RPL0;
	task_gate.attr = ATTASKGATE | DPL0;
	set_gate_to_idt(0x06, (uint8 *)&task_gate);

	fill_tss(tss, (uint32)invalid_opcode_int, (uint32)stack);
}

/**
	@Function:		invalid_opcode_int
	@Access:		Private
	@Description:
		处理检测到错误的操作码引发的异常的中断过程。
	@Parameters:
	@Return:
*/
static
void
invalid_opcode_int(void)
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
			struct Task * task = get_task_info_ptr(current_tid);

			int8 buffer[1024];
			sprintf_s(	buffer,
						1024,
						"A task causes a exception of invalid opcode"
						", the id is %d, the name is '%s'\n",
						current_tid,
						task->name);
			log(LOG_ERROR, buffer);

			print_str_p(buffer, CC_RED);
			print_str("\n");

			kill_task_and_jump_to_kernel(current_tid);
		}
	}
}

/*================================================================================
							处理非法的TSS的中断程序, 0x0a
================================================================================*/

/**
	@Function:		init_invalid_tss
	@Access:		Private
	@Description:
		初始化检测到非法的TSS的中断程序。
	@Parameters:
	@Return:	
*/
static
void
init_invalid_tss(void)
{
	struct die_info info;
	struct TSS * tss = &pf_tss;
	uint8 * stack = (uint8 *)alloc_memory(INTERRUPT_PROCEDURE_STACK_SIZE);
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
	set_desc_to_gdt(24, (uint8 *)&tss_desc);

	task_gate.offsetl = 0;
	task_gate.offseth = 0;
	task_gate.dcount = 0;
	task_gate.selector = (24 << 3) | RPL0;
	task_gate.attr = ATTASKGATE | DPL0;
	set_gate_to_idt(0x0a, (uint8 *)&task_gate);

	fill_tss(tss, (uint32)invalid_tss_int, (uint32)stack);
}

/**
	@Function:		invalid_tss_int
	@Access:		Private
	@Description:
		检测到非法的TSS的中断程序
	@Parameters:
	@Return:		
*/
static
void
invalid_tss_int(void)
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
			struct Task * task = get_task_info_ptr(current_tid);

			int8 buffer[1024];
			sprintf_s(	buffer,
						1024,
						"A task causes a exception of invalid tss, the id is %d, the name is '%s'\n",
						current_tid,
						task->name);
			log(LOG_ERROR, buffer);

			print_str_p(buffer, CC_RED);
			print_str("\n");

			kill_task_and_jump_to_kernel(current_tid);
		}
	}
}

/*================================================================================
							段不存在故障, 0x0b
================================================================================*/

/**
	@Function:		init_invalid_seg
	@Access:		Private
	@Description:
		初始化处理段不存在故障的中断程序。
	@Parameters:
	@Return:	
*/
static
void
init_invalid_seg(void)
{
	struct die_info info;
	struct TSS * tss = &pf_tss;
	uint8 * stack = (uint8 *)alloc_memory(INTERRUPT_PROCEDURE_STACK_SIZE);
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
	set_desc_to_gdt(25, (uint8 *)&tss_desc);

	task_gate.offsetl = 0;
	task_gate.offseth = 0;
	task_gate.dcount = 0;
	task_gate.selector = (25 << 3) | RPL0;
	task_gate.attr = ATTASKGATE | DPL0;
	set_gate_to_idt(0x0b, (uint8 *)&task_gate);

	fill_tss(tss, (uint32)invalid_seg_int, (uint32)stack);
}

/**
	@Function:		invalid_seg_int
	@Access:		Private
	@Description:
		处理段不存在故障的中断程序。
	@Parameters:
	@Return:		
*/
static
void
invalid_seg_int(void)
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
			struct Task * task = get_task_info_ptr(current_tid);

			int8 buffer[1024];
			sprintf_s(	buffer,
						1024,
						"A task causes a exception of non-existent segment, the id is %d, the name is '%s'\n",
						current_tid,
						task->name);
			log(LOG_ERROR, buffer);

			print_str_p(buffer, CC_RED);
			print_str("\n");

			kill_task_and_jump_to_kernel(current_tid);
		}
	}
}

/*================================================================================
							堆栈段故障, 0x0c
================================================================================*/

/**
	@Function:		init_invalid_stack
	@Access:		Private
	@Description:
		初始化处理堆栈段故障的中断程序。
	@Parameters:
	@Return:
*/
static
void
init_invalid_stack(void)
{
	struct die_info info;
	struct TSS * tss = &pf_tss;
	uint8 * stack = (uint8 *)alloc_memory(INTERRUPT_PROCEDURE_STACK_SIZE);
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
	set_desc_to_gdt(26, (uint8 *)&tss_desc);

	task_gate.offsetl = 0;
	task_gate.offseth = 0;
	task_gate.dcount = 0;
	task_gate.selector = (26 << 3) | RPL0;
	task_gate.attr = ATTASKGATE | DPL0;
	set_gate_to_idt(0x0c, (uint8 *)&task_gate);

	fill_tss(tss, (uint32)invalid_stack_int, (uint32)stack);
}

/**
	@Function:		invalid_stack_int
	@Access:		Private
	@Description:
		处理堆栈段故障的中断程序。
	@Parameters:
	@Return:		
*/
static
void
invalid_stack_int(void)
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
			struct Task * task = get_task_info_ptr(current_tid);

			int8 buffer[1024];
			sprintf_s(	buffer,
						1024,
						"A task causes a exception of invalid stack, the id is %d, the name is '%s'\n",
						current_tid,
						task->name);
			log(LOG_ERROR, buffer);

			print_str_p(buffer, CC_RED);
			print_str("\n");

			kill_task_and_jump_to_kernel(current_tid);
		}
	}
}

/*================================================================================
							通用保护异常, 0x0d
================================================================================*/

/**
	@Function:		init_gp
	@Access:		Private
	@Description:
		初始化通用保护的中断程序。
	@Parameters:
	@Return:		
*/
static
void
init_gp(void)
{
	struct die_info info;
	struct TSS * tss = &gp_tss;
	uint8 * stack = (uint8 *)alloc_memory(INTERRUPT_PROCEDURE_STACK_SIZE);
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
	set_desc_to_gdt(18, (uint8 *)&tss_desc);

	task_gate.offsetl = 0;
	task_gate.offseth = 0;
	task_gate.dcount = 0;
	task_gate.selector = (18 << 3) | RPL0;
	task_gate.attr = ATTASKGATE | DPL0;
	set_gate_to_idt(0x0d, (uint8 *)&task_gate);

	fill_tss(tss, (uint32)gp_int, (uint32)stack);
}

/**
	@Function:		gp_int
	@Access:		Private
	@Description:
		通用保护的中断程序。
	@Parameters:
	@Return:	
*/
static
void
gp_int(void)
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
			struct Task * task = get_task_info_ptr(current_tid);

			int8 buffer[1024];
			sprintf_s(	buffer,
						1024,
						"A task causes a global protected exception, the id is %d, the name is '%s'\n",
						current_tid,
						task->name);
			log(LOG_ERROR, buffer);

			print_str_p(buffer, CC_RED);
			print_str("\n");

			kill_task_and_jump_to_kernel(current_tid);
		}
	}
}

/*================================================================================
							处理页故障, 0x0e
================================================================================*/

/**
	@Function:		init_pf
	@Access:		Private
	@Description:
		初始化页失败的中断程序。
	@Parameters:
	@Return:	
*/
static
void
init_pf(void)
{
	struct die_info info;
	struct TSS * tss = &pf_tss;
	uint8 * stack = (uint8 *)alloc_memory(INTERRUPT_PROCEDURE_STACK_SIZE);
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
	set_desc_to_gdt(20, (uint8 *)&tss_desc);

	task_gate.offsetl = 0;
	task_gate.offseth = 0;
	task_gate.dcount = 0;
	task_gate.selector = (20 << 3) | RPL0;
	task_gate.attr = ATTASKGATE | DPL0;
	set_gate_to_idt(0x0e, (uint8 *)&task_gate);

	fill_tss(tss, (uint32)pf_int, (uint32)stack);
}

/**
	@Function:		pf_int
	@Access:		Private
	@Description:
		页失败的中断程序。
	@Parameters:
	@Return:		
*/
static
void
pf_int(void)
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
			struct Task * task = get_task_info_ptr(current_tid);

			int8 buffer[1024];
			sprintf_s(	buffer,
						1024,
						"A task causes a error of page fault, the id is %d, the name is '%s'\n",
						current_tid,
						task->name);
			log(LOG_ERROR, buffer);

			print_str_p(buffer, CC_RED);
			print_str("\n");

			kill_task_and_jump_to_kernel(current_tid);
		}
	}
}

/*================================================================================
							处理未实现的中断程序
================================================================================*/

/**
	@Function:		set_noimpl
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
set_noimpl(IN uint32 int_num)
{
	if(int_num >= 256)
		return;
	struct Gate task_gate;
	task_gate.offsetl = 0;
	task_gate.offseth = 0;
	task_gate.dcount = 0;
	task_gate.selector = (17 << 3) | RPL0;
	task_gate.attr = ATTASKGATE | DPL0;
	set_gate_to_idt(int_num, (uint8 *)&task_gate);
}

/**
	@Function:		noimpl_int
	@Access:		Private
	@Description:
		未被使用的中断的中断过程。
	@Parameters:
	@Return:
*/
static
void
noimpl_int(void)
{
	while(1)
	{
		if(is_kernel_task)
		{
			struct die_info info;
			fill_info(info, DC_NOIMPLEMENT_INT, DI_NOIMPLEMENT_INT);
			die(&info);
		}
		else
		{
			struct Task * task = get_task_info_ptr(current_tid);

			int8 buffer[1024];
			sprintf_s(	buffer,
						1024,
						"A task causes a error of calling not implement interrupt procedure"
						", the id is %d, the name is '%s'\n",
						current_tid,
						task->name);
			log(LOG_ERROR, buffer);

			print_str_p(buffer, CC_RED);
			print_str("\n");

			kill_task_and_jump_to_kernel(current_tid);
		}
	}
}

/**
	@Function:		shutdown_system
	@Access:		Public
	@Description:
		关闭系统。
	@Parameters:
	@Return:	
*/
void
shutdown_system(void)
{
	leave_system();
	acpi_power_off();
	halt_cpu();
}

/**
	@Function:		reboot_system
	@Access:		Public
	@Description:
		重启系统。
	@Parameters:
	@Return:	
*/
void
reboot_system(void)
{
	leave_system();
	reset_cpu();
}

#define	press_key_to_continue()	\
{	\
	print_str("Press any key to continue!\n");	\
	get_char();	\
}

/**
	@Function:		enter_system
	@Access:		Private
	@Description:
		初始化所有资源后被调用。该过程会对系统进行安全检测。
	@Parameters:
	@Return:		
*/
static
void
enter_system(void)
{
	print_str("Checking system...\n");

	//检查文件系统
	FILE * fptr = fopen(SYSTEM_FLAGS_CHECK_FS, FILE_MODE_ALL);
	if(fptr != NULL)
		if(flen(fptr) != 0)
		{
			fclose(fptr);
			print_str("The file system has some error!\n");
			if(get_disk_size("DA") != 0)
				if(repair_ifs1("DA"))
					print_str("The file system(DA) has been repaired!\n");
				else				
					print_str("Failed to repair the file system(DA)!\n");
			if(get_disk_size("DB") != 0)
				if(repair_ifs1("DB"))
					print_str("The file system(DB) has been repaired!\n");
				else				
					print_str("Failed to repair the file system(DB)!\n");
		}
		else
		{
			fwrite(fptr, "1", 1);
			print_str("The file system is OK!\n");
			fclose(fptr);
		}
	else
	{
		print_str("The file system has some error!\n");
		print_str("Failed to repair the file System!\n");
	}
	//press_key_to_continue();
	clear_screen();
	log(LOG_NORMAL, "Enter system.");
}

/**
	@Function:		leave_system
	@Access:		Private
	@Description:
		离开系统时被调用。
	@Parameters:
	@Return:	
*/
static
void
leave_system(void)
{
	FILE * fptr = fopen(SYSTEM_FLAGS_CHECK_FS, FILE_MODE_WRITE);
	if(fptr != NULL)
	{
		fwrite(fptr, "", 0);
		fclose(fptr);
	}
	log(LOG_NORMAL, "Leave system.");
	free_log();
}
