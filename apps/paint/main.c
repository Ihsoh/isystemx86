#include <ilib.h>

extern void init(void);
extern void draw_cursor(int x, int y);
extern void draw(int x, int y);
extern void paint(void);
extern void destroy(void);

struct CommonImage buffer;
struct CommonImage image;
uint width, height;
char filename[1024];

void main(void)
{
	init();
	paint();
	destroy();
	app_exit();
}

void die(char * message)
{
	print_str_p(message, CC_RED);
	app_exit();
}

void init(void)
{
	get_param_w(NULL);
	get_param_w(filename);
	get_param_w(filename);
	if(strcmp(filename, "") == 0)
		die("Format:\n\tpaint {filename}");
	get_screen_size(&width, &height);
	FILE * fptr = fopen(filename, FILE_MODE_ALL);
	if(fptr == NULL)
		die("Fail to open file!");
	uchar * image_data = allocm(flen(fptr));
	if(image_data == NULL)
	{
		fclose(fptr);
		die("Fail to allocate memory!");
	}
	fread(fptr, image_data, flen(fptr));
	fclose(fptr);
	if(!new_common_image(&image, image_data))		
		new_empty_image0(&image, width, height);
	freem(image_data);
	new_empty_image0(&buffer, width, height);
	fill_image_by_byte(&buffer, 0xff);
	fill_image_by_byte(&image, 0xff);
	disable_flush_screen();
}

void draw_cursor(int x, int y)
{
	int i;
	for(i = 0; i < 10; i++)
	{
		set_pixel_common_image(&buffer, x + i, y, 0xff00ff00);
		set_pixel_common_image(&buffer, x + i, y + i, 0xff888888);
		set_pixel_common_image(&buffer, x, y + i, 0xff0000ff);
	}
}

void draw(int x, int y)
{
	set_pixel_common_image(&image, x, y, 0xffff0000);
}

void paint(void)
{
	while(1)
	{
		if(get_key_status(KEY_LEFTCTR) && get_key_status(KEY_S))
			save_common_image(&image, filename);
		if(get_key_status(KEY_LEFTCTR) && get_key_status(KEY_X))
			return;
		int x, y;
		get_mouse_position(&x, &y);
		if(is_mouse_left_button_down())
			draw(x, y);
		draw_common_image(&buffer, &image, 0, 0, width, height);
		draw_cursor(x, y);
		draw_screen_image(0, 0, width, height, buffer.data);
	}
}

void destroy(void)
{
	destroy_common_image(&image);
	destroy_common_image(&buffer);
	enable_flush_screen();
}
