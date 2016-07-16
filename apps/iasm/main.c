#include "parser.h"

#include <ilib/ilib.h>

int main(int argc, char * argv[])
{
	if(argc != 3)
	{
		Error("Invalid arguments.");
	}
	char SourceFile[1024];
	char DestFile[1024];
	ILFixPath(argv[1], SourceFile);
	ILFixPath(argv[2], DestFile);
	FILE * SourceFilePtr = fopen(SourceFile, "r");
	if(SourceFilePtr == NULL)
	{
		Error("Cannot open source file.");
	}
	uint SourceFileLength = ILGetFileLength(SourceFilePtr->ilfptr);
	char * SourceFileContent = (char *)malloc(SourceFileLength + 1);
	fread(SourceFileContent, sizeof(char), SourceFileLength, SourceFilePtr);
	SourceFileContent[SourceFileLength] = '\0';
	fclose(SourceFilePtr);
	InitParser(SourceFileContent);
	Scan();
	Parse();
	SaveToFile(DestFile);
	DestroyParser();
	free(SourceFileContent);
	return 0;
}
