#include <ilib/ilib.h>
#include "die.h"

#include <imagelib/imagelib.h>
#include <imagelib/bmp.h>

int main(int argc, char * argv[])
{
	if(argc != 2)
	{
		printf("Bad arguments!\n");
		exit(-1);
	}
	imagelib_init();
	printf("Ready to open image file!\n"); 
	IMGLBMPPtr bmpobj = imgl_bmp_create(argv[1]);
	if(bmpobj == NULL)
	{
		printf("Cannot open image file!\n");
		exit(-1);
	}
	printf("Opened image file!\n");
	int width = imgl_bmp_get_width(bmpobj);
	int height = imgl_bmp_get_height(bmpobj);

	printf("0 Width: %d, Height: %d\n", width, height);

	struct CommonImage image;
	if(!new_empty_image0(&image, width, height))
	{
		printf("Cannot create common image!\n");
		exit(-1);
	}

	printf("1 Width: %d, Height: %d\n", width, height);
	
	uint x, y;
	for(x = 0; x < (int)width; x++)
		for(y = 0; y < (int)height; y++)
		{
			uint color = imgl_bmp_get_color(bmpobj, x, y);
			set_pixel_common_image(&image, x, y, color);
		}
	imgl_bmp_destroy(bmpobj);
	uint32 wid = ILCreateWindow(width, 
								height, 
								0xffffffff,
								WINDOW_STYLE_HIDDEN_BUTTON,
								"BMP Test");
	printf("Created window!\n");


	ILShowWindow(wid);
	ILDrawWindow(wid, &image);
	for(;;)
	{
		if(ILGetKeyFromWindow(wid) == 'x')
			break;
		uint32 ui;
		for(ui = 0; ui < 100000; ui++)
			asm volatile ("pause");
	}
	ILDestroyWindow(wid);

	return 0;
}
