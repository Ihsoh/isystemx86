//Filename:		mouse.c
//Author:		Ihsoh
//Date:			2014-11-14
//Descriptor:	Mouse functions

#include <mouse.h>
#include <sparams.h>
#include <types.h>

void get_mouse_position(int * x, int * y)
{
	struct SParams sparams;
	system_call(SCALL_MOUSE, SCALL_GET_MPOSITION, &sparams);
	*x = (int)(sparams.param0);
	*y = (int)(sparams.param1);
}

int is_mouse_left_button_down(void)
{
	struct SParams sparams;
	system_call(SCALL_MOUSE, SCALL_GET_MLBSTATE, &sparams);
	return (int)(sparams.param0);
}

int is_mouse_right_button_down(void)
{
	struct SParams sparams;
	system_call(SCALL_MOUSE, SCALL_GET_MRBSTATE, &sparams);
	return (int)(sparams.param0);
}
