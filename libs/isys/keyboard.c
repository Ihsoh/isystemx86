#include "keyboard.h"
#include "types.h"

#include "ilib/ilib.h"

UINT32
ISYSGetChar(VOID)
{
	return il_get_char();
}

UINT32
ISYSGetString(	OUT BYTEPTR str,
				IN UINT32 n)
{
	return il_get_strn(str, n);
}

BOOL
ISYSHasChar(VOID)
{
	return il_has_key();
}

UINT32
ISYSGetControlKeyStatus(VOID)
{
	UINT32 status = ISYS_CKEY_STATUS_NONE;
	if(il_get_control())
		status |= ISYS_CKEY_STATUS_CTRL;
	if(il_get_shift())
		status |= ISYS_CKEY_STATUS_SHIFT;
	if(il_get_alt())
		status |= ISYS_CKEY_STATUS_ALT;
	return status;
}

UINT32
ISYSGetKeyStatus(UINT32 key)
{
	return il_get_key_status(key);
}
