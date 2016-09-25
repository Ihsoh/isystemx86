#include "parser.h"

#include <ilib/ilib.h>

int main(int argc, char * argv[])
{
	int SourceFileFlag = 0;
	char SourceFile[1024];
	int OutputFileFlag = 0;
	char OutputFile[1024];
	int ListingFileFlag = 0;
	char ListingFile[1024];
	int i;
	for (i = 1; i < argc; i++)
	{
		char * arg = argv[i];
		if (strcmp(arg, "-s") == 0)
		{
			if (SourceFileFlag)
			{
				Error("Duplicate option '-s' for source file path.");
			}
			else
			{
				i++;
				if (i == argc)
				{
					Error("The option '-s' expects source file path.");
				}
				ILFixPath(argv[i], SourceFile);
				SourceFileFlag = 1;
			}
		}
		else if (strcmp(arg, "-o") == 0)
		{
			if (OutputFileFlag)
			{
				Error("Duplicate option '-o' for output file path.");
			}
			else
			{
				i++;
				if (i == argc)
				{
					Error("The option '-o' expects output file path.");
				}
				ILFixPath(argv[i], OutputFile);
				OutputFileFlag = 1;
			}
		}
		else if (strcmp(arg, "-l") == 0)
		{
			if (ListingFileFlag)
			{
				Error("Duplicate option '-l' for listing file path.");
			}
			else
			{
				i++;
				if (i == argc)
				{
					Error("The option '-l' expects listing file path.");
				}
				ILFixPath(argv[i], ListingFile);
				ListingFileFlag = 1;
			}
		}
		else
		{
			char buffer[KB(1)];
			sprintf_s(buffer, sizeof(buffer), "Invalid option '%s'.", argv[i]);
			Error(buffer);
		}
	}
	if (!SourceFileFlag)
	{
		Error("No source file specified.");
	}
	if (!OutputFileFlag)
	{
		Error("No output file specified.");
	}

	FILE * SourceFilePtr = fopen(SourceFile, "r");
	if(SourceFilePtr == NULL)
	{
		char buffer[KB(1)];
		sprintf_s(buffer, sizeof(buffer), "Cannot open source file '%s'.", SourceFile);
		Error(buffer);
	}
	uint SourceFileLength = ILGetFileLength(SourceFilePtr->ilfptr);
	char * SourceFileContent = (char *)malloc(SourceFileLength + 1);
	fread(SourceFileContent, sizeof(char), SourceFileLength, SourceFilePtr);
	SourceFileContent[SourceFileLength] = '\0';
	fclose(SourceFilePtr);
	InitParser(SourceFileContent);
	Scan();
	if (ListingFileFlag)
	{
		EnableListing();
	}
	Parse();
	SaveToFile(OutputFile);
	if (ListingFileFlag)
	{
		SaveListingToFile(ListingFile);
	}
	DestroyParser();
	free(SourceFileContent);

	return 0;
}
