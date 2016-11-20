/**
	@File:			sb16.c
	@Author:		Ihsoh
	@Date:			2016-11-19
	@Description:
		Sound Blaster 16驱动。
*/

#include "sb16.h"
#include "386.h"

#include "screen.h"

#define	_PORT_DSP_RESET(n)			(0x206 | (n * 0x10))
#define	_PORT_DSP_READ(n)			(0x20A | (n * 0x10))
#define	_PORT_DSP_WRITE(n)			(0x20C | (n * 0x10))
#define	_PORT_DSP_READ_STATUS(n)	(0x20E | (n * 0x10))
#define	_PORT_DSP_16BIT_ACK(n)		(0x20F | (n * 0x10))
#define	_PORT_DSP_SETTING(n)		(0x200 | (n * 0x10))

// DSP 命令。
#define	_CMD_DSP_DIRECT_DAC_8			0x10
#define	_CMD_DSP_DIRECT_ADC_8			0x20
#define	_CMD_DSP_SET_TIME_CONSTANT		0x40
#define	_CMD_DSP_SET_OUTPUT_RATE		0x41
#define	_CMD_DSP_SET_INPUT_RATE			0x42
#define	_CMD_DSP_HIGH_SPEED_SIZE		0x48
#define	_CMD_DSP_HS_DAC_8				0x91
#define	_CMD_DSP_HS_ADC_8				0x99
#define	_CMD_DSP_PAUSE_8BIT				0xD0
#define	_CMD_DSP_SPEAKER_ON				0xD1
#define	_CMD_DSP_SPEAKER_OFF			0xD3
#define	_CMD_DSP_CONTINUE_8BIT			0xD4
#define	_CMD_DSP_PAUSE_16BIT			0xD5
#define	_CMD_DSP_CONTINUE_16BIT			0xD6
#define	_CMD_DSP_READ_SPEAKER			0xD8
#define	_CMD_DSP_EXIT_16BIT				0xD9
#define	_CMD_DSP_EXIT_8BIT				0xDA
#define	_CMD_DSP_GET_VERSION			0xE1

// MIDI DSP 命令。
#define	_CMD_MIDI_DSP_POLL				0x30
#define	_CMD_MIDI_DSP_RED				0x31
#define	_CMD_MIDI_DSP_UART_POLL			0x34
#define	_CMD_MIDI_DSP_UART_RED			0x35
#define	_CMD_MIDI_DSP_WRITE				0x38

// 最大重试数。
#define	_MAX_RETRY	1000

// 输出缓冲区大小。
#define	_OUTPUT_BUFFER_SIZE	(KB(64))

// 双缓冲区。
#define	_OUTPUT_BUFFER0		0x500000
#define	_OUTPUT_BUFFER1		0x600000

static volatile BOOL				_available[6] = {FALSE, FALSE, FALSE, FALSE, FALSE, FALSE};
static volatile uint8 *	volatile 	_output_buffer = NULL;
static volatile BOOL				_output_buffer_ready = FALSE;
static volatile uint32			 	_selected_card = 0;
static volatile uint8 *	volatile 	_data = NULL;
static volatile uint32				_data_len = 0;
static volatile uint32				_data_remainder = 0;
static volatile uint32				_data_frequency = 0;
static volatile BOOL 				_data_bits16 = FALSE;
static volatile BOOL				_data_stereo = FALSE;

static
void
SB16Delay10MS(void)
{
	int i = 0;
	for (i = 0; i < 0; i++) {
		asm volatile ("pause");
	}
}

static
BOOL
SB16Write(
	IN uint32 n,
	IN uint8 data)
{
	int i;
	for (i = 0; i < _MAX_RETRY; i++)
	{
		SB16Delay10MS();	
		if ((KnlInByte(_PORT_DSP_WRITE(n)) & 0x80) == 0x00)
		{
			KnlOutByte(_PORT_DSP_WRITE(n), data);
			return TRUE;
		}
	}
	return FALSE;
}

static
BOOL
SB16Read(
	IN uint32 n,
	OUT uint8 * data)
{
	int i;
	for (i = 0; i < _MAX_RETRY; i++)
	{
		SB16Delay10MS();	
		if (KnlInByte(_PORT_DSP_READ_STATUS(n)) & 0x80)
		{
			*data = KnlInByte(_PORT_DSP_READ(n));
			return TRUE;
		}
	}
	return FALSE;
}

static
BOOL
SB16GetVersion(
	IN uint32 n,
	OUT uint8 * major,
	OUT uint8 * minor)
{
	if (SB16Write(n, _CMD_DSP_GET_VERSION))
	{
		SB16Read(n, major);
		SB16Read(n, minor);
		return TRUE;
	}
	return FALSE;
}

static
BOOL
SB16SpeakerOn(
	IN uint8 n)
{
	return SB16Write(n, _CMD_DSP_SPEAKER_ON);
}

static
BOOL
SB16SpeakerOff(
	IN uint8 n)
{
	return SB16Write(n, _CMD_DSP_SPEAKER_OFF);
}

static
BOOL
SB16SetFrequency(
	IN uint8 n,
	IN uint16 frequency)
{
	uint8 time_constant = 256 - 1000000 / frequency;
	return SB16Write(n, _CMD_DSP_SET_TIME_CONSTANT) && SB16Write(n, time_constant);
}

static
BOOL
SB16SetOutputRate(
	IN uint8 n,
	IN uint16 rate)
{
	if (SB16Write(n, _CMD_DSP_SET_OUTPUT_RATE))
	{
		SB16Write(n, (uint8)(rate >> 8));
		SB16Write(n, (uint8)rate);
	}
	return TRUE;
}

static
BOOL
SB16SetInputRate(
	IN uint8 n,
	IN uint16 rate)
{
	if (SB16Write(n, _CMD_DSP_SET_INPUT_RATE))
	{
		SB16Write(n, (uint8)(rate >> 8));
		SB16Write(n, (uint8)rate);
	}
	return TRUE;
}

static
BOOL
SB16DetermineDevice(
	IN uint32 n)
{
	KnlOutByte(_PORT_DSP_RESET(n), 1);
	SB16Delay10MS();
	KnlOutByte(_PORT_DSP_RESET(n), 0);
	int i;
	for (i = 0; i < _MAX_RETRY; i++)
	{
		SB16Delay10MS();
		if ((KnlInByte(_PORT_DSP_READ_STATUS(n)) & 0x80) && KnlInByte(_PORT_DSP_READ(n)) == 0xAA)
		{
			return TRUE;
		}
	}
	return FALSE;
}

static
void
SB16DetermineDevices(void)
{
	uint32 i;
	for (i = 0; i < 6; i++)
	{
		uint32 n = i + 1;
		BOOL available = SB16DetermineDevice(n);
		_available[i] = available;
		if (available)
		{
			SB16SpeakerOn(n);
		}
	}
}

static
void
SB16Next(void)
{
	if (_data_remainder != 0)
	{
		uint32 n = _selected_card;
		uint32 len = 0;
		if (_data_remainder >= _OUTPUT_BUFFER_SIZE)
		{
			len = _OUTPUT_BUFFER_SIZE;
		}
		else
		{
			len = _data_remainder;
		}
		uint32 limit = len - 1;
		
		// 第一次装载输出缓冲区。
		if (_output_buffer == NULL)
		{
			_output_buffer = _OUTPUT_BUFFER0;
			UtlCopyMemory(_output_buffer, _OUTPUT_BUFFER_SIZE, _data, len);
		}
		uint32 addr = (uint32)_output_buffer;

		if (_data_bits16)
		{
			KnlOutByte(0xD4, 0x05);
			KnlOutByte(0xD8, 0x00);
			KnlOutByte(0xD6, 0x49);
			KnlOutByte(0xC4, (uint8)addr);
			KnlOutByte(0xC4, (uint8)(addr >> 8));
			KnlOutByte(0x8B, (uint8)(addr >> 16));
			KnlOutByte(0xD8, 0x00);
			KnlOutByte(0xC6, (uint8)((limit / 2)));
			KnlOutByte(0xC6, (uint8)((limit / 2) >> 8));
			KnlOutByte(0xD4, 1);
			SB16Write(n, 0xB0);
			if (_data_stereo)
			{
				SB16Write(n, 0x30);
				SB16Write(n, (uint8)(limit / 4));
				SB16Write(n, (uint8)((limit / 4) >> 8));
			}
			else
			{
				SB16Write(n, 0x10);
				SB16Write(n, (uint8)(limit / 2));
				SB16Write(n, (uint8)((limit / 2) >> 8));
			}
		}
		else
		{
			KnlOutByte(0x0A, 0x05);
			KnlOutByte(0x0C, 0x00);
			KnlOutByte(0x0B, 0x49);
			KnlOutByte(0x02, (uint8)addr);
			KnlOutByte(0x02, (uint8)(addr >> 8));
			KnlOutByte(0x83, (uint8)(addr >> 16));
			KnlOutByte(0x0C, 0x00);
			KnlOutByte(0x03, (uint8)(limit));
			KnlOutByte(0x03, (uint8)(limit >> 8));
			KnlOutByte(0x0A, 1);
			SB16Write(n, 0xC0);
			if (_data_stereo)
			{
				SB16Write(n, 0x20);
				SB16Write(n, (uint8)(limit / 2));
				SB16Write(n, (uint8)((limit / 2) >> 8));
			}
			else
			{
				SB16Write(n, 0x00);
				SB16Write(n, (uint8)limit);
				SB16Write(n, (uint8)(limit >> 8));
			}
		}
		_data_remainder -= len;
		_data += len;

		// 双缓冲。
		if (_output_buffer != NULL)
		{
			if (_output_buffer == _OUTPUT_BUFFER0)
			{
				_output_buffer = _OUTPUT_BUFFER1;
			}
			else
			{
				_output_buffer = _OUTPUT_BUFFER0;
			}
			UtlCopyMemory(_output_buffer, _OUTPUT_BUFFER_SIZE, _data, _data_remainder >= _OUTPUT_BUFFER_SIZE ? _OUTPUT_BUFFER_SIZE : _data_remainder);
			_output_buffer_ready = TRUE;
		}
	}
	else
	{
		_selected_card = 0;
		_data = NULL;
		_data_len = 0;
		_data_remainder = 0;
		_data_frequency = 0;
		_data_bits16 = FALSE;
		_data_stereo = FALSE;
	}
}

void
SB16Interrupt(
	IN uint32 peripheral,
	IN uint32 irq)
{
	if (peripheral == 0)
	{
		while (!_output_buffer_ready)
		{
			asm volatile ("pause");
		}
		_output_buffer_ready = FALSE;
		SB16Next();
		if (_data_bits16)
		{
			KnlInByte(0x22F);
		}
		else
		{
			KnlInByte(0x22E);
		}
	}
}

BOOL
SB16Playback(
	IN uint32 n,
	IN uint8 * data,
	IN uint32 len,
	IN uint32 frequency,
	IN BOOL bits16,
	IN BOOL stereo)
{
	if (n < 1
		|| n > 6
		|| !_available[n - 1]
		|| data == NULL
		|| len == 0
		|| frequency == 0
		|| _data != NULL)
	{
		return FALSE;
	}
	_selected_card = n;
	_data = data;
	_data_len = len;
	_data_remainder = len;
	_data_frequency = frequency;
	_data_bits16 = bits16;
	_data_stereo = stereo;
	SB16SetOutputRate(n, _data_frequency);
	SB16Next();
	return TRUE;
}

void
SB16Init(void)
{
	SB16DetermineDevices();
	#include "wav/wav.h"
	WavPtr wav = WavCreateFromFile("VS:/isystem/demo.wav");
	printuihex(wav);
	ScrPrintString("\n");
	BOOL r = SB16Playback(
		2,
		&(wav->data_first_byte),
		wav->data_subchunk_size,
		wav->sample_rate,
		wav->bits_per_sample == 16,
		wav->num_channels == 2);

	ScrPrintString("len: ");
	printn(wav->data_subchunk_size);
	ScrPrintString("\n");
	
	ScrPrintString("frequency: ");
	printuihex(wav->sample_rate);
	ScrPrintString("\n");

	ScrPrintString("bits16: ");
	printuihex(wav->bits_per_sample == 16);
	ScrPrintString("\n");

	ScrPrintString("stereo: ");
	printuihex(wav->num_channels == 2);
	ScrPrintString("\n");

	printuihex(r);
	ScrPrintString("\n");
}
