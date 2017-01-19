/**
	@File:			sb16.c
	@Author:		Ihsoh
	@Date:			2016-11-19
	@Description:
		Sound Blaster 16驱动。
*/

#include "sb16.h"
#include "../386.h"

#include "../screen.h"
#include "../kernel.h"

#define	_PORT_SB16_IO_BASE(n)		(0x220 + (n * 0x20))

// FM Music Status Port
#define	_PORT_FM0_STATUS(n)			(_PORT_SB16_IO_BASE(n) + 0x00)
// FM Music Register Address Port
#define	_PORT_FM0_REG_ADDR(n)		(_PORT_SB16_IO_BASE(n) + 0x00)
// FM Music Data Port
#define	_PORT_FM0_DATA(n)			(_PORT_SB16_IO_BASE(n) + 0x01)
// Advanced FM Music Status Port
#define	_PORT_AFM_STATUS(n)			(_PORT_SB16_IO_BASE(n) + 0x02)
// Advanced FM Music Register Address Port
#define	_PORT_AFM_REG_ADDR(n)		(_PORT_SB16_IO_BASE(n) + 0x02)
// Advanced FM Music Data Port
#define	_PORT_AFM_DATA				(_PORT_SB16_IO_BASE(n) + 0x03)
// Mixer chip Register Address Port
#define	_PORT_MIXER_REG_ADDR(n)		(_PORT_SB16_IO_BASE(n) + 0x04)
// Mixer chip Data Port
#define	_PORT_MIXER_DATA(n)			(_PORT_SB16_IO_BASE(n) + 0x05)
// DSP Reset
#define	_PORT_DSP_RESET(n)			(_PORT_SB16_IO_BASE(n) + 0x06)
// FM Music Status Port
#define	_PORT_FM1_STATUS(n)			(_PORT_SB16_IO_BASE(n) + 0x08)
// FM Music Register Address Port
#define	_PORT_FM1_REG_ADDR(n)		(_PORT_SB16_IO_BASE(n) + 0x08)
// FM Music Data Port
#define	_PORT_FM1_DATA(n)			(_PORT_SB16_IO_BASE(n) + 0x09)
// DSP Read Data Port
#define	_PORT_DSP_READ(n)			(_PORT_SB16_IO_BASE(n) + 0x0A)
// DSP Write Command/Data
#define	_PORT_DSP_WRITE(n)			(_PORT_SB16_IO_BASE(n) + 0x0C)
// DSP Write-Buffer Status(Bit 7)
#define	_PORT_DSP_WRITE_STATUS(n)	(_PORT_SB16_IO_BASE(n) + 0x0C)
// DSP Read-Buffer Status(Bit 7)
#define	_PORT_DSP_READ_STATUS(n)	(_PORT_SB16_IO_BASE(n) + 0x0E)
// CD-ROM Command or Data Register
#define	_PORT_CDROM_CMD_DATA		(_PORT_SB16_IO_BASE(n) + 0x10)
// CD-ROM Status Register
#define	_PORT_CDROM_STATUS			(_PORT_SB16_IO_BASE(n) + 0x11)
// CD-ROM Reset Register
#define	_PORT_CDROM_RESET			(_PORT_SB16_IO_BASE(n) + 0x12)
// CD-ROM Enable Register
#define	_PORT_CDROM_ENABLE			(_PORT_SB16_IO_BASE(n) + 0x13)

#define	_PORT_MPU401_IO_BASE(n)		(0x300 + (n * 0x30))

// MPU-401 Data Port
#define	_PORT_MPU401_DATA			(_PORT_MPU401_IO_BASE(n) + 0x00)
// MPU-401 Status Port
#define	_PORT_MPU401_STATUS			(_PORT_MPU401_IO_BASE(n) + 0x01)
// MPU-401 Command Port
#define	_PORT_MPU401_CMD			(_PORT_MPU401_IO_BASE(n) + 0x01)

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

// 最大基地址数量。
#define	_MAX_BASE	4

// 输出缓冲区大小。
#define	_OUTPUT_BUFFER_SIZE	(KB(128))

// 双缓冲区。
#define	_OUTPUT_BUFFER0		0x500000
#define	_OUTPUT_BUFFER1		0x520000

#define	_IS_VALID_BASE_N(n)	((n) == _base_n)
#define	_BASE_N				(_base_n)

static volatile uint32				_base_n = 0xffffffff;
static volatile uint8 *	volatile 	_output_buffer = NULL;
static volatile BOOL				_output_buffer_ready = FALSE;
static volatile uint8 *	volatile 	_data = NULL;
static volatile uint32				_data_len = 0;
static volatile uint32				_data_remainder = 0;
static volatile uint32				_data_frequency = 0;
static volatile BOOL 				_data_bits16 = FALSE;
static volatile BOOL				_data_stereo = FALSE;
static volatile BOOL				_dma_auto_init = FALSE;
static volatile BOOL				_stop = FALSE;
static volatile BOOL				_pause = FALSE;
static volatile BOOL				_dma_auto_init_last_intr = FALSE;

static
void
SB16Delay10MS(void)
{
	int32 i = 0;
	for (i = 0; i < 5; i++) {
		asm volatile ("pause");
	}
}

static
BOOL
SB16WriteDSP(
	IN uint8 data)
{
	uint8 n = _BASE_N;
	if (_IS_VALID_BASE_N(n))
	{
		int32 i;
		for (i = 0; i < _MAX_RETRY; i++)
		{
			SB16Delay10MS();	
			if ((KnlInByte(_PORT_DSP_WRITE_STATUS(n)) & 0x80) == 0x00)
			{
				KnlOutByte(_PORT_DSP_WRITE(n), data);
				return TRUE;
			}
		}
	}
	return FALSE;
}

static
BOOL
SB16ReadDSP(
	OUT uint8 * data)
{
	if (data == NULL)
	{
		return FALSE;
	}
	uint8 n = _BASE_N;
	if (_IS_VALID_BASE_N(n))
	{
		int32 i;
		for (i = 0; i < _MAX_RETRY; i++)
		{
			SB16Delay10MS();	
			if (KnlInByte(_PORT_DSP_READ_STATUS(n)) & 0x80)
			{
				*data = KnlInByte(_PORT_DSP_READ(n));
				return TRUE;
			}
		}
	}
	return FALSE;
}

static
BOOL
SB16SetMixerRegisterAddress(
	IN uint8 addr)
{
	uint8 n = _BASE_N;
	if (_IS_VALID_BASE_N(n))
	{
		KnlOutByte(_PORT_MIXER_REG_ADDR(n), addr);
		return TRUE;
	}
	return FALSE;
}

static
BOOL
SB16WriteMixer(
	IN uint8 data)
{
	uint8 n = _BASE_N;
	if (_IS_VALID_BASE_N(n))
	{
		KnlOutByte(_PORT_MIXER_DATA(n), data);
		return TRUE;
	}
	return FALSE;
}

static
BOOL
SB16ReadMixer(
	OUT uint8 * data)
{
	if (data == NULL)
	{
		return FALSE;
	}
	uint8 n = _BASE_N;
	if (_IS_VALID_BASE_N(n))
	{
		*data = KnlInByte(_PORT_MIXER_DATA(n));
		return TRUE;
	}
	return FALSE;
}

static
BOOL
SB16GetVersion(
	OUT uint8 * major,
	OUT uint8 * minor)
{
	if (major == NULL || minor == NULL)
	{
		return FALSE;
	}
	if (SB16WriteDSP(_CMD_DSP_GET_VERSION))
	{
		SB16ReadDSP(major);
		SB16ReadDSP(minor);
		return TRUE;
	}
	return FALSE;
}

static
BOOL
SB16SpeakerOn()
{
	return SB16WriteDSP(_CMD_DSP_SPEAKER_ON);
}

static
BOOL
SB16SpeakerOff()
{
	return SB16WriteDSP(_CMD_DSP_SPEAKER_OFF);
}

static
BOOL
SB16SetFrequency(
	IN uint16 frequency)
{
	uint8 time_constant = 256 - 1000000 / frequency;
	return SB16WriteDSP(_CMD_DSP_SET_TIME_CONSTANT) && SB16WriteDSP(time_constant);
}

static
BOOL
SB16SetOutputRate(
	IN uint16 rate)
{
	if (SB16WriteDSP(_CMD_DSP_SET_OUTPUT_RATE))
	{
		SB16WriteDSP((uint8)(rate >> 8));
		SB16WriteDSP((uint8)rate);
		return TRUE;
	}
	return FALSE;
}

static
BOOL
SB16SetInputRate(
	IN uint16 rate)
{
	if (SB16WriteDSP(_CMD_DSP_SET_INPUT_RATE))
	{
		SB16WriteDSP((uint8)(rate >> 8));
		SB16WriteDSP((uint8)rate);
		return TRUE;
	}
	return FALSE;
}

static
BOOL
SB16DetermineDevice(
	IN uint32 n)
{
	KnlOutByte(_PORT_DSP_RESET(n), 1);
	SB16Delay10MS();
	KnlOutByte(_PORT_DSP_RESET(n), 0);
	int32 i;
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
	uint32 n;
	for (n = 0; n < _MAX_BASE; n++)
	{
		if (SB16DetermineDevice(n))
		{
			_base_n = n;
			break;
		}
	}
}

static
void
SB16Reset(void)
{
	_output_buffer = NULL;
	_output_buffer_ready = FALSE;
	_data = NULL;
	_data_len = 0;
	_data_remainder = 0;
	_data_frequency = 0;
	_data_bits16 = FALSE;
	_data_stereo = FALSE;
	_dma_auto_init = FALSE;
	_stop = FALSE;
	_pause = FALSE;
}

static
void
SB16StopDMAAutoInit(void)
{
	if (_data_bits16)
	{
		SB16WriteDSP(0xD9);
	}
	else
	{
		SB16WriteDSP(0xDA);
	}
}

static
void
SB16NextAI(
	IN BOOL first)
{
	if (_data_remainder != 0)
	{
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

		_output_buffer = _OUTPUT_BUFFER0;
		UtlCopyMemory(_output_buffer, _OUTPUT_BUFFER_SIZE, _data, len);
		uint32 addr = (uint32)_output_buffer;

		if (first)
		{
			if (_data_bits16)
			{
				KnlOutByte(0xD4, 0x05);
				KnlOutByte(0xD8, 0x00);
				KnlOutByte(0xD6, 0x99);
				KnlOutByte(0xC4, (uint8)addr);
				KnlOutByte(0xC4, (uint8)(addr >> 8));
				KnlOutByte(0x8B, (uint8)(addr >> 16));
				KnlOutByte(0xD8, 0x00);
				KnlOutByte(0xC6, (uint8)limit);
				KnlOutByte(0xC6, (uint8)(limit >> 8));
				KnlOutByte(0xD4, 1);
				SB16WriteDSP(0xB6);
				if (_data_stereo)
				{
					uint32 temp = limit / 4 * 2;
					SB16WriteDSP(0x30);
					SB16WriteDSP((uint8)temp);
					SB16WriteDSP((uint8)(temp >> 8));
				}
				else
				{
					uint32 temp = limit / 2 * 2;
					SB16WriteDSP(0x10);
					SB16WriteDSP((uint8)temp);
					SB16WriteDSP((uint8)(temp >> 8));
				}
			}
			else
			{
				KnlOutByte(0x0A, 0x05);
				KnlOutByte(0x0C, 0x00);
				KnlOutByte(0x0B, 0x99);
				KnlOutByte(0x02, (uint8)addr);
				KnlOutByte(0x02, (uint8)(addr >> 8));
				KnlOutByte(0x83, (uint8)(addr >> 16));
				KnlOutByte(0x0C, 0x00);
				KnlOutByte(0x03, (uint8)(limit));
				KnlOutByte(0x03, (uint8)(limit >> 8));
				KnlOutByte(0x0A, 1);
				SB16WriteDSP(0xC6);
				if (_data_stereo)
				{
					uint32 temp = limit / 2 * 2;
					SB16WriteDSP(0x20);
					SB16WriteDSP((uint8)temp);
					SB16WriteDSP((uint8)(temp >> 8));
				}
				else
				{
					uint32 temp = limit * 2;
					SB16WriteDSP(0x00);
					SB16WriteDSP((uint8)temp);
					SB16WriteDSP((uint8)(temp >> 8));
				}
			}
		}

		_data_remainder -= len;
		_data += len;
	}
	else
	{
		SB16Reset();
	}

}

static
void
SB16Next(void)
{
	if (!_IS_VALID_BASE_N(_BASE_N))
	{
		return;
	}
	if (_data_remainder != 0)
	{
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
			KnlOutByte(0xC6, (uint8)limit);
			KnlOutByte(0xC6, (uint8)(limit >> 8));
			KnlOutByte(0xD4, 1);
			SB16WriteDSP(0xB0);
			if (_data_stereo)
			{
				uint32 temp = limit / 4;
				SB16WriteDSP(0x30);
				SB16WriteDSP((uint8)temp);
				SB16WriteDSP((uint8)(temp >> 8));
			}
			else
			{
				uint32 temp = limit / 2;
				SB16WriteDSP(0x10);
				SB16WriteDSP((uint8)temp);
				SB16WriteDSP((uint8)(temp >> 8));
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
			SB16WriteDSP(0xC0);
			if (_data_stereo)
			{
				uint32 temp = limit / 2;
				SB16WriteDSP(0x20);
				SB16WriteDSP((uint8)temp);
				SB16WriteDSP((uint8)(temp >> 8));
			}
			else
			{
				uint32 temp = limit;
				SB16WriteDSP(0x00);
				SB16WriteDSP((uint8)temp);
				SB16WriteDSP((uint8)(temp >> 8));
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
		SB16Reset();
	}
}

static
BOOL
SB16GetInterruptStatus(
	OUT uint8 * status)
{
	if (status == NULL)
	{
		return FALSE;
	}
	if (SB16SetMixerRegisterAddress(0x82))
	{
		return SB16ReadMixer(status);
	}
	return FALSE;
}

static
BOOL
SB16GetInterruptSetup(
	OUT uint8 * setup)
{
	if (setup == NULL)
	{
		return FALSE;
	}
	if (SB16SetMixerRegisterAddress(0x80))
	{
		return SB16ReadMixer(setup);
	}
	return FALSE;
}

static
BOOL
SB16SetInterruptSetup(
	uint8 setup)
{
	if (SB16SetMixerRegisterAddress(0x80))
	{
		return SB16WriteMixer(setup);
	}
	return FALSE;
}

static
BOOL
SB16GetDMASetup(
	OUT uint8 * setup)
{
	if (setup == NULL)
	{
		return FALSE;
	}
	if (SB16SetMixerRegisterAddress(0x81))
	{
		return SB16ReadMixer(setup);
	}
	return FALSE;
}

void
SB16Interrupt(
	IN uint32 peripheral,
	IN uint32 irq)
{
	if (_dma_auto_init)
	{
		if (_stop)
		{
			SB16StopDMAAutoInit();
			SB16NextAI(FALSE);
			_dma_auto_init_last_intr = TRUE;
			_stop = FALSE;
		}
		else
		{
			if (_dma_auto_init_last_intr)
			{
				_dma_auto_init_last_intr = FALSE;
				SB16Reset();
			}
			else
			{
				SB16NextAI(FALSE);
			}
		}
	}
	else
	{
		while (!_output_buffer_ready)
		{
			asm volatile ("pause");
		}
		_output_buffer_ready = FALSE;
		if (_stop) 
		{
			SB16Reset();
		}
		else
		{
			SB16Next();
		}
	}
	uint8 status;
	if (SB16GetInterruptStatus(&status))
	{
		if (status & 0x01)
		{
			KnlInByte(_PORT_SB16_IO_BASE(_BASE_N) + 0x0E);
		}
		else if (status & 0x02)
		{
			KnlInByte(_PORT_SB16_IO_BASE(_BASE_N) + 0x0F);
		}
		else if (status & 0x04)
		{
			KnlInByte(_PORT_MPU401_IO_BASE(_BASE_N) + 0x00);
		}
	}
}

BOOL
SB16Playback(
	IN uint8 * data,
	IN uint32 len,
	IN uint32 frequency,
	IN BOOL bits16,
	IN BOOL stereo,
	IN BOOL auto_init)
{
	if (!_IS_VALID_BASE_N(_BASE_N)
		|| data == NULL
		|| len == 0
		|| frequency == 0
		|| _data != NULL)
	{
		return FALSE;
	}
	_data = data;
	_data_len = len;
	_data_remainder = len;
	_data_frequency = frequency;
	_data_bits16 = bits16;
	_data_stereo = stereo;
	SB16SpeakerOn();
	SB16SetOutputRate(_data_frequency);
	if (auto_init)
	{
		_dma_auto_init = TRUE;
		SB16NextAI(TRUE);
	}
	else
	{
		SB16Next();
	}
	return TRUE;
}

BOOL
SB16Stop(void)
{
	if (_data_remainder != 0)
	{
		_stop = TRUE;
		if (_pause)
		{
			SB16Reset();
		}
		return TRUE;
	}
}

BOOL
SB16Pause(void)
{
	if (_data_remainder != 0 && !_stop)
	{
		SB16WriteDSP(0xD5);
		_pause = TRUE;
		return TRUE;
	}
	return FALSE;
}

BOOL
SB16Resume(void)
{
	if (_data_remainder != 0 && _pause)
	{
		SB16WriteDSP(0xD6);
		_pause = FALSE;
		return TRUE;
	}
	return FALSE;
}

void
SB16Init(void)
{
	KnlSetPeripheralInterrupt(0, SB16Interrupt);
	SB16DetermineDevices();
}
