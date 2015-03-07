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

typedef	void *	object;

//typedef	int32	bool;
#define	bool int32

#define	NULL	((void *)0)

#define	BYTE(n)	(n)
#define	KB(n)	((n) * 1024)
#define	MB(n)	((n) * 1024 * 1024)
#define	GB(n)	((n) * 1024 * 1024 * 1024)

#define	BOOL	bool
#define	TRUE	1
#define	FALSE	0

//修饰函数的参数，参数作为输入。
//应用场合：
//	1.	如果参数的类型不为指针，必须标注 IN。
//	2. 	如果参数的类型为指针，但是函数使用该参数来获取一个地址值，
//		而非通过该地址去引用内存进行读操作，必须标注 IN。
//	3.	如果参数的类型为指针，但是函数使用该参数保存的地址去引用
//		内存，而且包含对引用的内存进行读操作的代码，必须标注 IN。
//	4.	如果参数的类型为指针，但是函数使用该参数保存的地址去引用
//		内存，但是不包含对引用的内存进行读操作的代码，但是包含确认
//		指针的合法性的代码，不须标注 IN。
#define	IN	

//修饰函数的参数，参数作为输出
//应用场合：
//	1.	如果参数的类型为指针，并且包含对引用的内存进行写操作的代码，
//		必须标注 OUT。
#define	OUT

#endif
