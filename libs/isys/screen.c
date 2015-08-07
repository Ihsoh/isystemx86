#include "screen.h"
#include "types.h"

#include "ilib/ilib.h"

BOOL
ISYSIsGUIValid(VOID)
{
	return vesa_is_valid();
}

BOOL
ISYSGetScreenSize(	OUT UINT32PTR width,
					OUT UINT32PTR height)
{
	il_get_screen_size(width, height);
	return TRUE;
}

BOOL
ISYSSetPixelToScreen(	IN UINT32 x,
						IN UINT32 y,
						IN UINT32 pixel)
{
	il_set_screen_pixel(x, y, pixel);
	return TRUE;
}

BOOL
ISYSGetPixelFromScreen(	IN UINT32 x,
						IN UINT32 y,
						OUT UINT32PTR pixel)
{
	*pixel = il_get_screen_pixel(x, y);
	return TRUE;
}

BOOL
ISYSDrawRawBitmapToScreen(	IN UINT32 x,
							IN UINT32 y,
							IN UINT32 width,
							IN UINT32 height,
							IN BYTEPTR bitmap)
{
	il_draw_screen_image(x, y, width, height, bitmap);
	return TRUE;
}
