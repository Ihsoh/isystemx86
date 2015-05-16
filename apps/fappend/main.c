#include <ilib/ilib.h>

int main(int argc, char * argv[])
{
	if(argc != 3)
	{
		printf("%s\n", "fappend Error: Invalid arguments!");
		return -1;
	}
	char * text = argv[1];
	char path[1024];
	if(strlen(argv[2]) >= 1024)
	{
		printf("%s\n", "fappend Error: Path too long!");
		return -1;
	}
	strcpy(path, argv[2]);
	ILFixPath(path, path);
	ILFILE * fptr = ILOpenFile(path, FILE_MODE_APPEND);
	if(fptr == NULL)
	{
		printf("fappend Error: Failed to open '%s'!\n", path);
		return -1;
	}
	if(!ILAppendFile(fptr, text, strlen(text)))
	{
		printf("fappend Error: Failed to append text to '%s'!\n", path);
		ILCloseFile(fptr);
		return -1;
	}
	ILCloseFile(fptr);
	return 0;
}
