#ifndef _INTERRUPTINFO_H_
#define _INTERRUPTINFO_H_

#define Interrupt_Inner 0
#define Interrupt_Outer 1

typedef void(*pInterruptCallback)(void *);

typedef struct 
{
	int type;
	int inner_address;
	pInterruptCallback outer_callback;
} InterruptInfo;

#endif
