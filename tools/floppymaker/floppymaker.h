//Filename:		floppymaker.h
//Author:		Ihsoh
//Date:			2014-1-25
//Descriptor:	Floppy maker

#ifndef	_FLOPPYMAKER_H_
#define	_FLOPPYMAKER_H_

#include <stdio.h>
#include <stdlib.h>

#define	FLOPPY_SIZE 1474560
#define	BOOT_SIZE	512
#define	KLDR_SIZE	32768
#define	KERNEL_SIZE	524288

#define	BOOT_OFFSET		0
#define	KLDR_OFFSET		512
#define	KERNEL_OFFSET	33280

typedef unsigned char uchar;

#endif

