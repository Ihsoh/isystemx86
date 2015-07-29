#ifndef	_ISYS_TYPES_H_
#define	_ISYS_TYPES_H_

typedef	char 					SBYTE;
typedef	char 					INT8;
typedef	short					INT16;
typedef	int 					INT32;
typedef	long long				INT64;
typedef float					FLOAT;
typedef	double					DOUBLE;
typedef	unsigned char 			BYTE;
typedef	unsigned char 			UINT8;
typedef	unsigned short			UINT16;
typedef	unsigned int 			UINT32;
typedef unsigned long long		UINT64;

typedef	char * 					SBYTEPTR;
typedef	char * 					INT8PTR;
typedef	short *					INT16PTR;
typedef	int *					INT32PTR;
typedef	long long *				INT64PTR;
typedef	float *					FLOATPTR;
typedef	double *				DOUBLEPTR;
typedef	unsigned char * 		BYTEPTR;
typedef	unsigned char * 		UINT8PTR;
typedef	unsigned short *		UINT16PTR;
typedef	unsigned int *			UINT32PTR;
typedef unsigned long long *	UINT64PTR;

#define	VOID 	void
typedef	VOID *	VOIDPTR;

typedef	INT32	BOOL;

#define	IN
#define	OUT

typedef struct
{
	UINT16	Year;
	UINT8	Month;
	UINT8	Day;
	UINT8	DayOfWeek;
	UINT8	Hour;
	UINT8	Minute;
	UINT8	Second;
} ISYSDateTime, * ISYSDateTimePtr;

#endif
