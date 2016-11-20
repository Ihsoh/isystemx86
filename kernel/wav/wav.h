/**
	@File:			wav.h
	@Author:		Ihsoh
	@Date:			2016-11-20
	@Description:
		WAV文件解析。
*/

#ifndef	_WAV_WAV_H_
#define	_WAV_WAV_H_

#include "../types.h"

typedef struct
{
	// Chunk Descriptor	
	int8		sign_riff[4];		// “RIFF”，资源交换文件标志。
	uint32		chunk_size;			// 从下一个地址开始到文件文的总字节数。
	int8		sign_wave[4];		// “WAVE”，代表wav文件格式的标志。

	// FMT Subchunk
	int8		sign_fmt[4];		// “fmt ”，波形格式标志。
	uint32		fmt_subchunk_size;	// FMT Subchunk的大小，值为16。
	uint16		audio_format;		// 1（PCM）。
	uint16		num_channels;		// 声道数。
	uint32		sample_rate;		// 采样率。
	uint32		byte_rate;			// 字节率。
	uint16		block_align;		// 块对齐。
	uint16		bits_per_sample;	// 每个样本的位数。

	// Data Subchunk
	int8		sign_data[4];		// “data”，数据块的标志。
	uint32		data_subchunk_size;	// 数据块大小。
	uint8		data_first_byte;
} Wav, * WavPtr;

extern
WavPtr
WavCreateFromFile(
	IN CASCTEXT path);

extern
void
WavFree(
	IN WavPtr wav);

#endif
