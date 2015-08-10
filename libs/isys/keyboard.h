#ifndef	_ISYS_KEYBOARD_H_
#define	_ISYS_KEYBOARD_H_

#include "types.h"

#define	ISYS_CKEY_STATUS_NONE	0x00000000
#define	ISYS_CKEY_STATUS_CTRL	0x00000001
#define	ISYS_CKEY_STATUS_SHIFT	0x00000002
#define	ISYS_CKEY_STATUS_ALT	0x00000004

#define	ISYS_KEY_STATUS_DOWN	0x00000000
#define	ISYS_KEY_STATUS_UP		0x00000001

#define ISYS_KEY_L1 		0x4F  
#define ISYS_KEY_L2 		0x50
#define ISYS_KEY_L3 		0x51
#define ISYS_KEY_L4 		0x4B
#define ISYS_KEY_L6 		0x4D
#define ISYS_KEY_L7 		0x47
#define ISYS_KEY_L8 		0x48
#define ISYS_KEY_L9 		0x49
#define ISYS_KEY_ADD 		0x2B
#define ISYS_KEY_SUB 		0x2D   
#define ISYS_KEY_F1 		59
#define ISYS_KEY_F2 		60
#define ISYS_KEY_F3 		61
#define ISYS_KEY_F4 		62
#define ISYS_KEY_F5 		63
#define ISYS_KEY_F6 		64
#define ISYS_KEY_F7 		65
#define ISYS_KEY_F8 		66
#define ISYS_KEY_F9 		67
#define ISYS_KEY_F10 		68
#define ISYS_KEY_INSERT		82
#define ISYS_KEY_HOME 		71
#define ISYS_KEY_PAGEUP		73
#define ISYS_KEY_PAGEDOWN 	81
#define ISYS_KEY_DEL 		83
#define ISYS_KEY_END 		79
#define ISYS_KEY_DASH 		12
#define ISYS_KEY_EQUAL 		13
#define ISYS_KEY_LBRACKET 	26
#define ISYS_KEY_RBRACKET 	27
#define ISYS_KEY_SEMICOLON 	39
#define ISYS_KEY_RQUOTE 	40
#define ISYS_KEY_LQUOTE 	41
#define ISYS_KEY_PERIOD 	52
#define ISYS_KEY_COMMA 		51
#define ISYS_KEY_SLASH 		53
#define ISYS_KEY_BACKSLASH 	43
#define ISYS_KEY_ENTER 		28        
#define ISYS_KEY_BACKSPACE 	14    
#define ISYS_KEY_SPACE 		57        
#define ISYS_KEY_TAB 		15
#define ISYS_KEY_ESC 		1
#define ISYS_KEY_Q 			16
#define ISYS_KEY_W 			17
#define ISYS_KEY_E 			18
#define ISYS_KEY_R 			19
#define ISYS_KEY_T 			20
#define ISYS_KEY_Y 			21
#define ISYS_KEY_U 			22
#define ISYS_KEY_I 			23
#define ISYS_KEY_O 			24
#define ISYS_KEY_P 			25
#define ISYS_KEY_A 			30
#define ISYS_KEY_S 			31
#define ISYS_KEY_D 			32
#define ISYS_KEY_F 			33
#define ISYS_KEY_G 			34
#define ISYS_KEY_H 			35
#define ISYS_KEY_J 			36
#define ISYS_KEY_K 			37
#define ISYS_KEY_L 			38
#define ISYS_KEY_Z 			44
#define ISYS_KEY_X 			45
#define ISYS_KEY_C 			46
#define ISYS_KEY_V 			47
#define ISYS_KEY_B 			48
#define ISYS_KEY_N 			49
#define ISYS_KEY_M 			50
#define ISYS_KEY_1 			2
#define ISYS_KEY_2 			3
#define ISYS_KEY_3 			4
#define ISYS_KEY_4 			5
#define ISYS_KEY_5 			6
#define ISYS_KEY_6 			7
#define ISYS_KEY_7 			8
#define ISYS_KEY_8 			9
#define ISYS_KEY_9 			10
#define ISYS_KEY_0 			11
#define	ISYS_KEY_LEFTCTR	29
#define	ISYS_KEY_LEFTSHIFT	42
#define	ISYS_KEY_LEFTALT	56
#define	ISYS_KEY_CAPSLOCK	58
#define	ISYS_KEY_EXT		224
#define ISYS_KEY_LEFT 		75      
#define ISYS_KEY_RIGHT 		77    
#define ISYS_KEY_UP 		72        
#define ISYS_KEY_DOWN 		80  
#define	ISYS_KEY_KEYUP		0x80 

extern
UINT32
ISYSGetChar(VOID);

extern
UINT32
ISYSGetString(	OUT BYTEPTR str,
				IN UINT32 n);

extern
BOOL
ISYSHasChar(VOID);

extern
UINT32
ISYSGetControlKeyStatus(VOID);

extern
UINT32
ISYSGetKeyStatus(UINT32 key);

#endif
