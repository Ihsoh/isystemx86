/**
	@File:			knlstatic.h
	@Author:		Ihsoh
	@Date:			2015-05-12
	@Description:
		kernel.c中的static函数声明。
		该文件只会被kernel.c包含。
*/

#ifndef	_KNLSTATIC_H_
#define	_KNLSTATIC_H_

#include "types.h"

static
void
reset_all_exceptions(void);

static
void
disk_va_init(void);

static
void *
knl_lib_malloc(IN uint32 num_bytes);

static
void *
knl_lib_calloc(	IN uint32 n, 
				IN uint32 size);

static
void
knl_lib_free(IN void * ptr);

static
BOOL
init_dsl(void);

static
BOOL
init_jsonl(void);

static
BOOL
init_pathl(void);

static
BOOL
init_mempooll(void);

static
void
irq_ack(IN uint32 no);

static
void
init_interrupt(void);

static
void
fpu_error_int(void);

static
void
fill_tss(	OUT struct TSS * tss, 
			IN uint32 eip,
			IN uint32 esp);

static
void
init_timer(void);

static
void
init_mouse(void);

static
void
init_keyboard(void);

static
void
init_ide(void);

static
void
init_ide1(void);

static
void
init_fpu(void);

static
void
free_system_call_tss(void);

static
void
free_task_tss(void);

static
void
timer_int(void);

static
void
mouse_int(void);

static
void
keyboard_int(void);

static
void
ide_int(void);

static
void
ide1_int(void);

static
void
fpu_int(void);

static
void
init_system_call(void);

static
void
system_call(void);

static
void
system_call_int(void);

static
void
kill_task_and_jump_to_kernel(IN uint32 tid);

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
init_double_fault(void);

static
void
double_fault_int(void);

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

static
void
init_gp(void);

static
void
gp_int(void);

static
void
init_pf(void);

static
void
pf_int(void);

static
void
init_mf(void);

static
void
mf_int(void);

static
void
init_ac(void);

static
void
ac_int(void);

static
void
init_mc(void);

static
void
mc_int(void);

static
void
init_xf(void);

static
void
xf_int(void);

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
enter_system(void);

static
void
leave_system(void);

#endif
