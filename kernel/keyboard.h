/**
	@File:			keyboard.h
	@Author:		Ihsoh
	@Date:			2014-1-30
	@Description:
*/

#ifndef	_KEYBOARD_H_
#define	_KEYBOARD_H_

#include "types.h"

extern
void tran_key(IN uint8 scan_code);

extern
uint8 get_char(void);

extern
uint32 get_strn(OUT int8 * input_buffer,
				IN uint32 n);

extern
BOOL get_shift(void);

extern
BOOL get_control(void);

extern
BOOL get_alt(void);

extern
void init_keyboard_driver(void);

extern
int32 get_key_status(IN int32 key);

extern
BOOL has_key(void);

#define	get_str(input_buffer) get_strn(input_buffer, 0x7FFFFFFF);

#define KEY_L1 			0x4F  
#define KEY_L2 			0x50
#define KEY_L3 			0x51
#define KEY_L4 			0x4B
#define KEY_L6 			0x4D
#define KEY_L7 			0x47
#define KEY_L8 			0x48
#define KEY_L9 			0x49
#define KEY_ADD 		0x2B
#define KEY_SUB 		0x2D   
#define KEY_F1 			59
#define KEY_F2 			60
#define KEY_F3 			61
#define KEY_F4 			62
#define KEY_F5 			63
#define KEY_F6 			64
#define KEY_F7 			65
#define KEY_F8 			66
#define KEY_F9 			67
#define KEY_F10 		68
#define KEY_INSERT 		82
#define KEY_HOME 		71
#define KEY_PAGEUP 		73
#define KEY_PAGEDOWN 	81
#define KEY_DEL 		83
#define KEY_END 		79
#define KEY_DASH 		12
#define KEY_EQUAL 		13
#define KEY_LBRACKET 	26
#define KEY_RBRACKET 	27
#define KEY_SEMICOLON 	39
#define KEY_RQUOTE 		40
#define KEY_LQUOTE 		41
#define KEY_PERIOD 		52
#define KEY_COMMA 		51
#define KEY_SLASH 		53
#define KEY_BACKSLASH 	43
#define KEY_ENTER 		28        
#define KEY_BACKSPACE 	14    
#define KEY_SPACE 		57        
#define KEY_TAB 		15
#define KEY_ESC 		1
#define KEY_Q 			16
#define KEY_W 			17
#define KEY_E 			18
#define KEY_R 			19
#define KEY_T 			20
#define KEY_Y 			21
#define KEY_U 			22
#define KEY_I 			23
#define KEY_O 			24
#define KEY_P 			25
#define KEY_A 			30
#define KEY_S 			31
#define KEY_D 			32
#define KEY_F 			33
#define KEY_G 			34
#define KEY_H 			35
#define KEY_J 			36
#define KEY_K 			37
#define KEY_L 			38
#define KEY_Z 			44
#define KEY_X 			45
#define KEY_C 			46
#define KEY_V 			47
#define KEY_B 			48
#define KEY_N 			49
#define KEY_M 			50
#define KEY_1 			2
#define KEY_2 			3
#define KEY_3 			4
#define KEY_4 			5
#define KEY_5 			6
#define KEY_6 			7
#define KEY_7 			8
#define KEY_8 			9
#define KEY_9 			10
#define KEY_0 			11
#define	KEY_LEFTCTR		29
#define	KEY_LEFTSHIFT	42
#define	KEY_LEFTALT		56
#define	KEY_CAPSLOCK	58

#define	KEY_EXT			224
	
#define KEY_LEFT 		75      
#define KEY_RIGHT 		77    
#define KEY_UP 			72        
#define KEY_DOWN 		80  

#define	KEY_KEYUP		0x80
#define	KEY_KEYDOWN		0x00

#endif
