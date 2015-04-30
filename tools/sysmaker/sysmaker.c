#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static
void
die(const char * message)
{
	printf("\nSystem application maker error: %s!\n", message);
	exit(-1);
}

static
unsigned int
get_file_size(char * filename)
{
	FILE * fileptr = fopen(filename, "rb");
	unsigned int len = 0;
	if(fileptr == NULL)
		die("Cannot open file");
	fgetc(fileptr);
	while(!feof(fileptr))
	{
		fgetc(fileptr);
		len++;
	}
	fclose(fileptr);
	return len;
}

int
main(int argc, char * argv[])
{
	if(argc != 2)
		die("Invalid arguments");
	char * filename = argv[1];
	unsigned int file_size = get_file_size(filename);
	unsigned char * app = malloc(256 + file_size);
	FILE * fptr = fopen(filename, "rb");
	if(fptr == NULL)
		die("Cannot open file");
	fread(app + 256, sizeof(unsigned char), file_size, fptr);
	fclose(fptr);
	memcpy(app + 0, "SYS32", 5);
	memset(app + 5, 0, 256 - 5);
	fptr = fopen(filename, "wb");
	if(fptr == NULL)
		die("Cannot open file");
	fwrite(app, sizeof(unsigned char), 256 + file_size, fptr);
	fclose(fptr);
	free(app);
	return 0;
}
