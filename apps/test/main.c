#include <ilib/ilib.h>

#define	MAX_BUFFER_SIZE		1024

int main(int argc, char * argv[])
{
	if(argc != 3)
	{
		printf("%s\n", "Format:\n    test {file0} {file1}");
		return -1;
	}
	char file0[1024], file1[1024];
	if(strlen(argv[1]) >= sizeof(file0))
	{
		printf("Error:\n    The file0's name too long\n");
		return -1;
	}
	if(strlen(argv[2]) >= sizeof(file1))
	{
		printf("Error:\n    The file1's name too long\n");
		return -1;
	}
	strcpy(file0, argv[1]);
	strcpy(file1, argv[2]);
	ILFixPath(file0, file0);
	ILFixPath(file1, file1);
	ILFILE * fptr0, * fptr1;
	fptr0 = ILOpenFile(file0, FILE_MODE_READ);
	if(fptr0 == NULL)
	{
		printf("Error:\n    Cannot open '%s'\n", file0);
		return -1;
	}
	fptr1 = ILOpenFile(file1, FILE_MODE_READ);
	if(fptr1 == NULL)
	{
		printf("Error:\n    Cannot open '%s'\n", file1);
		return -1;
	}
	printf(	"The file0 length is %u byte(s), the file1 length is %u byte(s).\n",
			ILGetFileLength(fptr0),
			ILGetFileLength(fptr1));
	if(ILGetFileLength(fptr0) != ILGetFileLength(fptr1))
		printf("'%s' != '%s'\n", file0, file1);
	else
	{
		char buffer0[MAX_BUFFER_SIZE], buffer1[MAX_BUFFER_SIZE];
		unsigned int len = ILGetFileLength(fptr0);
		while(len != 0)
		{
			unsigned int l = len % (MAX_BUFFER_SIZE + 1);
			if(l == 0)
				l = MAX_BUFFER_SIZE;
			unsigned int l0 = 0, l1 = 0;
			while(l0 != l)
				l0 += ILReadFile(fptr0, buffer0 + l0, l - l0);
			while(l1 != l)
				l1 += ILReadFile(fptr1, buffer1 + l1, l - l1);
			unsigned int ui;
			for(ui = 0; ui < l; ui++)
				if(buffer0[ui] != buffer1[ui])
				{
					printf("'%s' != '%s'\n", file0, file1);
					ILCloseFile(fptr0);
					ILCloseFile(fptr1);
					return 0;
				}
			len -= l;
		}
		printf("'%s' == '%s'\n", file0, file1);
	}
	ILCloseFile(fptr0);
	ILCloseFile(fptr1);
	return 0;
}
