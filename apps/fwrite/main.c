#include <ilib.h>

int main(int argc, char * argv[])
{
	if(argc != 3)
	{
		printf("%s\n", "fwrite Error: Invalid arguments!");
		return -1;
	}
	char * text = argv[1];
	char path[1024];
	if(strlen(argv[2]) >= 1024)
	{
		printf("%s\n", "fwrite Error: Path too long!");
		return -1;
	}
	strcpy(path, argv[2]);
	fix_path(path, path);
	FILE * fptr = fopen(path, FILE_MODE_WRITE);
	if(fptr == NULL)
	{
		printf("fwrite Error: Failed to open '%s'!\n", path);
		return -1;
	}
	if(!fwrite(fptr, text, strlen(text)))
	{
		printf("fwrite Error: Failed to write text to '%s'!\n", path);
		fclose(fptr);
		return -1;
	}
	fclose(fptr);
	return 0;
}
