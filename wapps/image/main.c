#include <ilib/ilib.h>
#include <imagelib/imagelib.h>
#include <imagelib/bmp.h>

void
die(CASCTEXT message)
{
	printf("Error: %s\n", message);
	exit(-1);
}

int32
main(	IN int32 argc,
		IN ASCTEXT argv[])
{
	if(!ILCheckVesaStatus())
		die("Image must run in vesa mode.");
	if(argc != 2)
		die("Error: Bad arguments.\n");
	if(!imagelib_init())
		die("Cannot initialize image library.\n");

	printf("File: %s\n", argv[1]);
	IMGLBMPPtr bmpobj = imgl_bmp_create(argv[1]);
	if(bmpobj == NULL)
		die("Cannot open image file.\n");
	int32 width = imgl_bmp_get_width(bmpobj);
	int32 height = imgl_bmp_get_height(bmpobj);
	printf("Width: %d, Height: %d.\n", width, height);

	struct CommonImage image;
	if(!new_empty_image0(&image, width, height))
		die("Cannot create common image.\n");
	
	uint32 x, y;
	for(x = 0; x < (uint32)width; x++)
		for(y = 0; y < (uint32)height; y++)
		{
			uint32 color = imgl_bmp_get_color(bmpobj, x, y);
			set_pixel_common_image(&image, x, y, color);
		}
	imgl_bmp_destroy(bmpobj);
	uint32 wid = ILCreateWindow(width,
								height,
								0xffffffff,
								WINDOW_STYLE_HIDDEN_BUTTON,
								"Image");
	if(wid == -1)
	die("Cannot create window.");
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
