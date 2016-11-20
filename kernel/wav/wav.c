/**
	@File:			wav.c
	@Author:		Ihsoh
	@Date:			2016-11-20
	@Description:
		WAV文件解析。
*/

#include "wav.h"
#include "../types.h"
#include "../memory.h"
#include "../fs/ifs1/fs.h"

WavPtr
WavCreateFromFile(
	IN CASCTEXT path)
{
	WavPtr wav = NULL;
	FileObjectPtr file = NULL;
	if (path == NULL)
	{
		goto error;
	}
	file = Ifs1OpenFile(path, FILE_MODE_READ);
	if (file == NULL)
	{
		goto error;
	}
	uint32 len = Ifs1GetFileLength(file);
	if (len < sizeof(Wav))
	{
		goto error;
	}
	wav = (WavPtr)MemAlloc(len);
	if (wav == NULL)
	{
		goto error;
	}
	if (Ifs1ReadFile(file, wav, len) != len)
	{
		goto error;
	}
	Ifs1CloseFile(file);
	file = NULL;
	return wav;
error:
	if (file != NULL)
	{
		Ifs1CloseFile(file);
		file = NULL;
	}
	if (wav != NULL)
	{
		MemFree(wav);
		wav = NULL;
	}
	return NULL;
}

void
WavFree(
	IN WavPtr wav)
{
	if (wav != NULL)
	{
		MemFree(wav);
	}
}
