//Filename:		keyboard.c
//Author:		Ihsoh
//Date:			2014-8-1
//Descriptor:	Keyboard functions

#include <keyboard.h>
#include <sparams.h>
#include <types.h>

uchar get_char(void)
{
	struct SParams sparams;
	system_call(SCALL_KEYBOARD, SCALL_GET_CHAR, &sparams);
	return (uchar)(sparams.param0);
}

uint get_strn(char * input_buffer, uint n)
{
	struct SParams sparams;
	sparams.param0 = SPARAM(input_buffer);
	sparams.param1 = SPARAM(n);
	system_call(SCALL_KEYBOARD, SCALL_GET_STR_N, &sparams);
	return (uint)(sparams.param0);
}

int get_shift(void)
{
	struct SParams sparams;
	system_call(SCALL_KEYBOARD, SCALL_GET_SHIFT, &sparams);
	return (int)(sparams.param0);
}

int get_control(void)
{
	struct SParams sparams;
	system_call(SCALL_KEYBOARD, SCALL_GET_CONTROL, &sparams);
	return (int)(sparams.param0);
}

int get_alt(void)
{
	struct SParams sparams;
	system_call(SCALL_KEYBOARD, SCALL_GET_ALT, &sparams);
	return (int)(sparams.param0);
}

int get_key_status(int key)
{
	struct SParams sparams;
	sparams.param0 = SPARAM(key);
	system_call(SCALL_KEYBOARD, SCALL_GET_KEY_STATUS, &sparams);
	return (int)(sparams.param0);
}

int has_key(void)
{
	struct SParams sparams;
	system_call(SCALL_KEYBOARD, SCALL_HAS_KEY, &sparams);
	return (int)(sparams.param0);
}
