#Filename:	makefile
#Author:	Ihsoh
#Date:		2014-1-29
#Descriptor:Make ilib

TargetDir=bin
Out=libilib.a
Target=$(TargetDir)/$(Out)
CC=gcc
AS=as
CFlags=-O0 -fno-stack-protector -nostdinc -m32 -march=i386 -mtune=i386
ASFlags=--32 -march=i386 -mtune=i386

Objs=bin/string.obj bin/sparams.obj bin/system.obj bin/screen.obj bin/keyboard.obj \
bin/fs.obj bin/image.obj bin/mouse.obj bin/stdio.obj bin/stdlib.obj bin/assert.obj 	\
bin/setjmp.obj bin/longjmp.obj bin/ctype.obj bin/errno.obj bin/strtod.obj

StdlibObjs=bin/strtol.obj bin/strtoul.obj bin/atoi.obj bin/atol.obj bin/mbstowcs.obj 	\
bin/mbtowc.obj bin/wctomb.obj bin/wcstombs.obj bin/strtod.obj bin/atof.obj bin/abs.obj 	\
bin/labs.obj bin/qsort.obj bin/msort.obj

MathObjs=bin/ceil.obj bin/floor.obj bin/fabs.obj bin/__scalb.obj bin/__finite.obj

SysDepsObjs=bin/memmove.obj bin/__infnan.obj bin/__copysign.obj bin/pow.obj bin/atan2.obj 	\
bin/atan.obj bin/exp.obj bin/exp__E.obj bin/sqrt.obj bin/acos.obj bin/acosh.obj bin/__isnan.obj 	\
bin/__isinf.obj bin/__logb.obj bin/ldexp.obj bin/__drem.obj bin/log.obj bin/log10.obj 	\
bin/log1p.obj bin/log__L.obj bin/asin.obj bin/atanh.obj bin/bcopy.obj bin/bzero.obj 	\
bin/hypot.obj bin/cabs.obj bin/cosh.obj bin/fabs.obj bin/ffs.obj bin/fmod.obj bin/fexp.obj 	\
bin/modf.obj bin/sincos.obj bin/sinh.obj bin/__expm1.obj bin/tan.obj bin/tanh.obj

LocaleObjs=bin/C-collate.obj bin/C-ctype.obj bin/C-numeric.obj bin/C-ctype_ct.obj bin/C-ctype_mb.obj 	\
bin/C-monetary.obj bin/C-response.obj bin/C-time.obj bin/localeconv.obj bin/setlocale.obj

StringObjs=bin/strdup.obj


AR=ar
Include=-I../

$(Target): bin  bin/mta32.obj $(Objs) $(StdlibObjs) $(MathObjs) $(SysDepsObjs) $(LocaleObjs) $(StringObjs)
	$(AR) -rc $(Target) $(Objs) $(StdlibObjs) $(MathObjs) $(SysDepsObjs) $(LocaleObjs) $(StringObjs)

bin:
	-mkdir bin

bin/string.obj: string.c string.h
	$(CC) $(CFlags) -c string.c -o bin/string.obj $(Include)
	
bin/sparams.obj: sparams.c sparams.h
	$(CC) $(CFlags) -c sparams.c -o bin/sparams.obj $(Include)
	
bin/system.obj: system.c system.h
	$(CC) $(CFlags) -c system.c -o bin/system.obj $(Include)
	
bin/screen.obj: screen.c screen.h
	$(CC) $(CFlags) -c screen.c -o bin/screen.obj $(Include)
	
bin/keyboard.obj: keyboard.c keyboard.h
	$(CC) $(CFlags) -c keyboard.c -o bin/keyboard.obj $(Include)
	
bin/fs.obj: fs.c fs.h
	$(CC) $(CFlags) -c fs.c -o bin/fs.obj $(Include)

bin/image.obj: image.c image.h
	$(CC) $(CFlags) -c image.c -o bin/image.obj $(Include)

bin/mouse.obj: mouse.c mouse.h
	$(CC) $(CFlags) -c mouse.c -o bin/mouse.obj $(Include)

bin/stdio.obj: stdio.c stdio.h
	$(CC) $(CFlags) -c stdio.c -o bin/stdio.obj $(Include)

bin/stdlib.obj: stdlib.c stdlib.h
	$(CC) $(CFlags) -c stdlib.c -o bin/stdlib.obj $(Include)

bin/assert.obj: assert.c assert.h
	$(CC) $(CFlags) -c assert.c -o bin/assert.obj $(Include)

bin/setjmp.obj: setjmp.s
	$(AS) $(ASFlags) -c setjmp.s -o bin/setjmp.obj

bin/longjmp.obj: longjmp.s
	$(AS) $(ASFlags) -c longjmp.s -o bin/longjmp.obj

bin/ctype.obj: ctype.c ctype.h
	$(CC) $(CFlags) -c ctype.c -o bin/ctype.obj $(Include)

bin/errno.obj: errno.c errno.h
	$(CC) $(CFlags) -c errno.c -o bin/errno.obj $(Include)

bin/strtod.obj: strtod.c
	$(CC) $(CFlags) -c strtod.c -o bin/strtod.obj $(Include)


#==============================Stdlib==============================

bin/strtol.obj: stdlib/strtol.c
	$(CC) $(CFlags) -c stdlib/strtol.c -o bin/strtol.obj $(Include)

bin/strtoul.obj: stdlib/strtoul.c
	$(CC) $(CFlags) -c stdlib/strtoul.c -o bin/strtoul.obj $(Include)

bin/atoi.obj: stdlib/atoi.c
	$(CC) $(CFlags) -c stdlib/atoi.c -o bin/atoi.obj $(Include)

bin/atol.obj: stdlib/atol.c
	$(CC) $(CFlags) -c stdlib/atol.c -o bin/atol.obj $(Include)

bin/mbstowcs.obj: stdlib/mbstowcs.c
	$(CC) $(CFlags) -c stdlib/mbstowcs.c -o bin/mbstowcs.obj $(Include)

bin/mbtowc.obj: stdlib/mbtowc.c
	$(CC) $(CFlags) -c stdlib/mbtowc.c -o bin/mbtowc.obj $(Include)

bin/wctomb.obj: stdlib/wctomb.c
	$(CC) $(CFlags) -c stdlib/wctomb.c -o bin/wctomb.obj $(Include)

bin/wcstombs.obj: stdlib/wcstombs.c
	$(CC) $(CFlags) -c stdlib/wcstombs.c -o bin/wcstombs.obj $(Include)

bin/strtod.obj: stdlib/strtod.c
	$(CC) $(CFlags) -c stdlib/strtod.c -o bin/strtod.obj $(Include)

bin/atof.obj: stdlib/atof.c
	$(CC) $(CFlags) -c stdlib/atof.c -o bin/atof.obj $(Include)

bin/abs.obj: stdlib/abs.c
	$(CC) $(CFlags) -c stdlib/abs.c -o bin/abs.obj $(Include)

bin/labs.obj: stdlib/labs.c
	$(CC) $(CFlags) -c stdlib/labs.c -o bin/labs.obj $(Include)

bin/msort.obj: stdlib/msort.c
	$(CC) $(CFlags) -c stdlib/msort.c -o bin/msort.obj $(Include)

bin/qsort.obj: stdlib/qsort.c
	$(CC) $(CFlags) -c stdlib/qsort.c -o bin/qsort.obj $(Include)




#==============================Math==============================

bin/ceil.obj: math/ceil.c
	$(CC) $(CFlags) -c math/ceil.c -o bin/ceil.obj $(Include)

bin/floor.obj: math/floor.c
	$(CC) $(CFlags) -c math/floor.c -o bin/floor.obj $(Include)

bin/fabs.obj: math/fabs.c
	$(CC) $(CFlags) -c math/fabs.c -o bin/fabs.obj $(Include)

bin/__scalb.obj: math/__scalb.c
	$(CC) $(CFlags) -c math/__scalb.c -o bin/__scalb.obj $(Include)

bin/__finite.obj: math/__finite.c
	$(CC) $(CFlags) -c math/__finite.c -o bin/__finite.obj $(Include)


#==============================SysDeps==============================

bin/memmove.obj: sysdeps/generic/memmove.c
	$(CC) $(CFlags) -c sysdeps/generic/memmove.c -o bin/memmove.obj $(Include)

bin/__infnan.obj: sysdeps/generic/__infnan.c
	$(CC) $(CFlags) -c sysdeps/generic/__infnan.c -o bin/__infnan.obj $(Include)

bin/__copysign.obj: sysdeps/generic/__copysign.c
	$(CC) $(CFlags) -c sysdeps/generic/__copysign.c -o bin/__copysign.obj $(Include)

bin/pow.obj: sysdeps/generic/pow.c
	$(CC) $(CFlags) -c sysdeps/generic/pow.c -o bin/pow.obj $(Include)

bin/atan2.obj: sysdeps/generic/atan2.c
	$(CC) $(CFlags) -c sysdeps/generic/atan2.c -o bin/atan2.obj $(Include)

bin/atan.obj: sysdeps/generic/atan.c
	$(CC) $(CFlags) -c sysdeps/generic/atan.c -o bin/atan.obj $(Include)

bin/exp.obj: sysdeps/generic/exp.c
	$(CC) $(CFlags) -c sysdeps/generic/exp.c -o bin/exp.obj $(Include)

bin/exp__E.obj: sysdeps/generic/exp__E.c
	$(CC) $(CFlags) -c sysdeps/generic/exp__E.c -o bin/exp__E.obj $(Include)

bin/sqrt.obj: sysdeps/ieee754/sqrt.c
	$(CC) $(CFlags) -c sysdeps/ieee754/sqrt.c -o bin/sqrt.obj $(Include)

bin/acos.obj: sysdeps/generic/acos.c
	$(CC) $(CFlags) -c sysdeps/generic/acos.c -o bin/acos.obj $(Include)

bin/acosh.obj: sysdeps/generic/acosh.c
	$(CC) $(CFlags) -c sysdeps/generic/acosh.c -o bin/acosh.obj $(Include)

bin/__isnan.obj: sysdeps/ieee754/__isnan.c
	$(CC) $(CFlags) -c sysdeps/ieee754/__isnan.c -o bin/__isnan.obj $(Include)

bin/__isinf.obj: sysdeps/ieee754/__isinf.c
	$(CC) $(CFlags) -c sysdeps/ieee754/__isinf.c -o bin/__isinf.obj $(Include)

bin/__logb.obj: sysdeps/ieee754/__logb.c
	$(CC) $(CFlags) -c sysdeps/ieee754/__logb.c -o bin/__logb.obj $(Include)

bin/ldexp.obj: sysdeps/ieee754/ldexp.c
	$(CC) $(CFlags) -c sysdeps/ieee754/ldexp.c -o bin/ldexp.obj $(Include)

bin/__drem.obj: sysdeps/ieee754/__drem.c
	$(CC) $(CFlags) -c sysdeps/ieee754/__drem.c -o bin/__drem.obj $(Include)

bin/log.obj: sysdeps/generic/log.c
	$(CC) $(CFlags) -c sysdeps/generic/log.c -o bin/log.obj $(Include)

bin/log10.obj: sysdeps/generic/log10.c
	$(CC) $(CFlags) -c sysdeps/generic/log10.c -o bin/log10.obj $(Include)

bin/log1p.obj: sysdeps/generic/log1p.c
	$(CC) $(CFlags) -c sysdeps/generic/log1p.c -o bin/log1p.obj $(Include)

bin/log__L.obj: sysdeps/generic/log__L.c
	$(CC) $(CFlags) -c sysdeps/generic/log__L.c -o bin/log__L.obj $(Include)

bin/asin.obj: sysdeps/generic/asin.c
	$(CC) $(CFlags) -c sysdeps/generic/asin.c -o bin/asin.obj $(Include)

bin/atanh.obj: sysdeps/generic/atanh.c
	$(CC) $(CFlags) -c sysdeps/generic/atanh.c -o bin/atanh.obj $(Include)

bin/bcopy.obj: sysdeps/generic/bcopy.c
	$(CC) $(CFlags) -c sysdeps/generic/bcopy.c -o bin/bcopy.obj $(Include)

bin/bzero.obj: sysdeps/generic/bzero.c
	$(CC) $(CFlags) -c sysdeps/generic/bzero.c -o bin/bzero.obj $(Include)

bin/hypot.obj: sysdeps/generic/hypot.c
	$(CC) $(CFlags) -c sysdeps/generic/hypot.c -o bin/hypot.obj $(Include)

bin/cabs.obj: sysdeps/generic/cabs.c
	$(CC) $(CFlags) -c sysdeps/generic/cabs.c -o bin/cabs.obj $(Include)

bin/cosh.obj: sysdeps/generic/cosh.c
	$(CC) $(CFlags) -c sysdeps/generic/cosh.c -o bin/cosh.obj $(Include)

bin/fabs.obj: sysdeps/generic/fabs.c
	$(CC) $(CFlags) -c sysdeps/generic/fabs.c -o bin/fabs.obj $(Include)

bin/ffs.obj: sysdeps/generic/ffs.c
	$(CC) $(CFlags) -c sysdeps/generic/ffs.c -o bin/ffs.obj $(Include)

bin/fmod.obj: sysdeps/generic/fmod.c
	$(CC) $(CFlags) -c sysdeps/generic/fmod.c -o bin/fmod.obj $(Include)

bin/fexp.obj: sysdeps/generic/fexp.c
	$(CC) $(CFlags) -c sysdeps/generic/fexp.c -o bin/fexp.obj $(Include)

bin/modf.obj: sysdeps/generic/modf.c
	$(CC) $(CFlags) -c sysdeps/generic/modf.c -o bin/modf.obj $(Include)

bin/sincos.obj: sysdeps/generic/sincos.c
	$(CC) $(CFlags) -c sysdeps/generic/sincos.c -o bin/sincos.obj $(Include)

bin/sinh.obj: sysdeps/generic/sinh.c
	$(CC) $(CFlags) -c sysdeps/generic/sinh.c -o bin/sinh.obj $(Include)

bin/__expm1.obj: sysdeps/generic/__expm1.c
	$(CC) $(CFlags) -c sysdeps/generic/__expm1.c -o bin/__expm1.obj $(Include)

bin/tan.obj: sysdeps/generic/tan.c
	$(CC) $(CFlags) -c sysdeps/generic/tan.c -o bin/tan.obj $(Include)

bin/tanh.obj: sysdeps/generic/tanh.c
	$(CC) $(CFlags) -c sysdeps/generic/tanh.c -o bin/tanh.obj $(Include)


#==============================Locale==============================

bin/C-collate.obj: locale/C-collate.c
	$(CC) $(CFlags) -c locale/C-collate.c -o bin/C-collate.obj $(Include)

bin/C-ctype.obj: locale/C-ctype.c
	$(CC) $(CFlags) -c locale/C-ctype.c -o bin/C-ctype.obj $(Include)

bin/C-numeric.obj: locale/C-numeric.c
	$(CC) $(CFlags) -c locale/C-numeric.c -o bin/C-numeric.obj $(Include)

bin/C-ctype_ct.obj: locale/C-ctype_ct.c
	$(CC) $(CFlags) -c locale/C-ctype_ct.c -o bin/C-ctype_ct.obj $(Include)

bin/C-ctype_mb.obj: locale/C-ctype_mb.c
	$(CC) $(CFlags) -c locale/C-ctype_mb.c -o bin/C-ctype_mb.obj $(Include)

bin/C-monetary.obj: locale/C-monetary.c
	$(CC) $(CFlags) -c locale/C-monetary.c -o bin/C-monetary.obj $(Include)

bin/C-response.obj: locale/C-response.c
	$(CC) $(CFlags) -c locale/C-response.c -o bin/C-response.obj $(Include)

bin/C-time.obj: locale/C-time.c
	$(CC) $(CFlags) -c locale/C-time.c -o bin/C-time.obj $(Include)

bin/localeconv.obj: locale/localeconv.c
	$(CC) $(CFlags) -c locale/localeconv.c -o bin/localeconv.obj $(Include)

bin/setlocale.obj: locale/setlocale.c
	$(CC) $(CFlags) -c locale/setlocale.c -o bin/setlocale.obj $(Include)



#==============================String==============================

bin/strdup.obj: string/strdup.c
	$(CC) $(CFlags) -c string/strdup.c -o bin/strdup.obj $(Include)




bin/mta32.obj: mta32.c
	$(CC) $(CFlags) -c mta32.c -o bin/mta32.obj $(Include)

.PHONY:clean

clean:
	-rm -f $(Target) $(Objs) $(StdlibObjs) $(MathObjs) $(SysDepsObjs) $(LocaleObjs) $(StringObjs)
