#include <ilib.h>

int main(int argc, char * argv[])
{
	if(argc < 2)
	{
		il_print_str_p("printf Error: Invalid arguments!\n", CC_RED);
		return -1;
	}
	char * format = argv[1];
	uint len = strlen(format);
	uint index = 0;
	uint ui;
	for(ui = 0; ui < len; ui++)
	{
		char chr = format[ui];
		if(chr != '%')
			il_print_char(chr);
		else
			if(ui + 1 < len)
			{
				chr = format[++ui];
				switch(chr)
				{
					case '%':
						il_print_char('%');
						break;
					case 's':
						if(2 + index < argc)
							il_print_str(argv[2 + index++]);
						break;
					case 'F':
					{
						if(2 + index < argc)
						{
							char buffer[64 * 1024];
							uint ui;
							for(ui = 0; ui < sizeof(buffer); ui++)
								buffer[ui] = '\0';
							char path[1024];
							if(strlen(argv[2 + index]) < sizeof(path))
							{
								strcpy(path, argv[2 + index++]);
								fix_path(path, path);
								FILE * fptr = fopen(path, FILE_MODE_READ);
								if(fptr != NULL)
								{
									fread(	fptr, 
											buffer, 
											flen(fptr) < sizeof(buffer)
												? flen(fptr)
												: sizeof(buffer) - 1);
									print_str(buffer);
									fclose(fptr);
								}
							}
						}
						break;
					}
					default:
						il_print_char(chr);
						break;
				}
			}
			else
				il_print_char('%');
	}
	return 0;
}
