/* Copyright (C) 1991, 1992, 1993, 1994 Free Software Foundation, Inc.
This file is part of the GNU C Library.

The GNU C Library is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public License as
published by the Free Software Foundation; either version 2 of the
License, or (at your option) any later version.

The GNU C Library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Library General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with the GNU C Library; see the file COPYING.LIB.  If
not, write to the, 1992 Free Software Foundation, Inc., 675 Mass Ave,
Cambridge, MA 02139, USA.  */

/*
 *  ANSI Standard: 4.10 GENERAL UTILITIES <stdlib.h>
 */

#ifndef _STDLIB_H
#define _STDLIB_H

#include "../features.h"
#include "../stddef.h"
#include "../errno.h"

#include "alloca.h"

/* Returned by `div'.  */
typedef struct
  {
    int quot;			/* Quotient.  */
    int rem;			/* Remainder.  */
  } div_t;

/* Returned by `ldiv'.  */
typedef struct
  {
    long int quot;		/* Quotient.  */
    long int rem;		/* Remainder.  */
  } ldiv_t;

/* The largest number rand will return (same as INT_MAX).  */
#define	RAND_MAX	2147483647


/* We define these the same for all machines.
   Changes from this to the outside world should be done in `_exit'.  */
#define	EXIT_FAILURE	1	/* Failing exit status.  */
#define	EXIT_SUCCESS	0	/* Successful exit status.  */

/* Maximum length of a multibyte character in the current locale.
   This is just one until the fancy locale support is finished.  */
#define MB_CUR_MAX  1

/* Shorthand for type of comparison functions.  */
typedef int (*__compar_fn_t) __P ((__const __ptr_t, __const __ptr_t));

/* Convert a string to a floating-point number.  */
extern double atof __P ((__const char *__nptr));
/* Convert a string to an integer.  */
extern int atoi __P ((__const char *__nptr));
/* Convert a string to a long integer.  */
extern long int atol __P ((__const char *__nptr));

/* Convert a string to a floating-point number.  */
extern double strtod __P ((__const char *__nptr, char **__endptr));
/* Convert a string to a long integer.  */
extern long int strtol __P ((__const char *__nptr, char **__endptr,
			     int __base));
/* Convert a string to an unsigned long integer.  */
extern unsigned long int strtoul __P ((__const char *__nptr,
				       char **__endptr, int __base));

#if defined (__OPTIMIZE__) && __GNUC__ >= 2
extern __inline double atof (__const char *__nptr)
{ return strtod(__nptr, (char **) NULL); }
extern __inline int atoi (__const char *__nptr)
{ return (int) strtol (__nptr, (char **) NULL, 10); }
extern __inline long int atol (__const char *__nptr)
{ return strtol (__nptr, (char **) NULL, 10); }
#endif /* Optimizing GCC >=2.  */

/* Return the absolute value of X.  */
extern __CONSTVALUE int abs __P ((int __x));
extern __CONSTVALUE long int labs __P ((long int __x));

/* Return the length of the multibyte character
   in S, which is no longer than N.  */
extern int mblen __P ((__const char *__s, size_t __n));
/* Return the length of the given multibyte character,
   putting its `wchar_t' representation in *PWC.  */
extern int mbtowc __P ((wchar_t * __pwc, __const char *__s, size_t __n));
/* Put the multibyte character represented
   by WCHAR in S, returning its length.  */
extern int wctomb __P ((char *__s, wchar_t __wchar));

#if defined (__OPTIMIZE__) && __GNUC__ >= 2
extern __inline int mblen (__const char *__s, size_t __n)
{ return mbtowc ((wchar_t *) NULL, __s, __n); }
#endif /* Optimizing GCC >=2.  */

/* Convert a multibyte string to a wide char string.  */
extern size_t mbstowcs __P ((wchar_t * __pwcs, __const char *__s, size_t __n));
/* Convert a wide char string to multibyte string.  */
extern size_t wcstombs __P ((char *__s, __const wchar_t * __pwcs, size_t __n));

#endif /* stdlib.h  */
