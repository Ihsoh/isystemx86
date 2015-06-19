#include <ilib/ilib.h>

static
void
delay(void);

int
main(	IN int argc,
		IN char * argv[])
{
	if(vesa_is_valid())
		for(;;)
		{
			flush_screen();
			delay();
		}
	return 0;
}

static
void
delay(void)
{
	uint32 ui;
	for(ui = 0; ui < 0x00030000; ui++)
		asm volatile("pause");
}
