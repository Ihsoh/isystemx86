//Filename:		types.h
//Author:		Ihsoh
//Date:			2014-1-28
//Descriptor:	Types

#ifndef	_TYPES_H_
#define	_TYPES_H_

typedef	char		int8;
typedef	short		int16;
typedef	int			int32;
typedef	long long	int64;

typedef	unsigned char		uint8;
typedef	unsigned short		uint16;
typedef	unsigned int		uint32;
typedef	unsigned long long	uint64;

typedef uint8	uchar;
typedef uint16	ushort;
typedef uint32	uint;
typedef uint32	ulong;
typedef uint64	ullong;

#define	NULL	((void *)0)

#define	BYTE(n)	(n)
#define	KB(n)	((n) * 1024)
#define	MB(n)	((n) * 1024 * 1024)
#define	GB(n)	((n) * 1024 * 1024 * 1024)

#define	BOOL	int32
#define	TRUE	1
#define	FALSE	0

#define	IN	//修饰函数的参数，参数作为输入
#define	OUT	//修饰函数的参数，参数作为输出

#endif
