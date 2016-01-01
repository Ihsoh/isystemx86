#Filename:	makefile
#Author:	Ihsoh
#Date:		2014-1-29
#Descriptor:Make ilib

# TargetDir=bin
# Out=libilib.a
Target=$(TargetDir)/$(Out)
CC=gcc
AS=as
CFlags=-O0 -fno-stack-protector -nostdinc -m32 -march=i386 -mtune=i386 $(CFlagsExt)
CFlagsNoExt=-O0 -fno-stack-protector -nostdinc -m32 -march=i386 -mtune=i386
ASFlags=--32 -march=i386 -mtune=i386

Objs=$(TargetDir)/string.obj $(TargetDir)/sparams.obj $(TargetDir)/system.obj $(TargetDir)/screen.obj $(TargetDir)/keyboard.obj \
$(TargetDir)/fs.obj $(TargetDir)/image.obj $(TargetDir)/mouse.obj $(TargetDir)/stdio.obj $(TargetDir)/stdlib.obj $(TargetDir)/assert.obj 	\
$(TargetDir)/setjmp.obj $(TargetDir)/longjmp.obj $(TargetDir)/ctype.obj $(TargetDir)/errno.obj $(TargetDir)/strtod.obj $(TargetDir)/gui.obj

StdlibObjs=$(TargetDir)/strtol.obj $(TargetDir)/strtoul.obj $(TargetDir)/atoi.obj $(TargetDir)/atol.obj $(TargetDir)/mbstowcs.obj 	\
$(TargetDir)/mbtowc.obj $(TargetDir)/wctomb.obj $(TargetDir)/wcstombs.obj $(TargetDir)/strtod.obj $(TargetDir)/atof.obj $(TargetDir)/abs.obj 	\
$(TargetDir)/labs.obj $(TargetDir)/qsort.obj $(TargetDir)/msort.obj $(TargetDir)/mblen.obj

MathObjs=$(TargetDir)/ceil.obj $(TargetDir)/floor.obj $(TargetDir)/fabs.obj $(TargetDir)/__scalb.obj $(TargetDir)/__finite.obj

SysDepsObjs=$(TargetDir)/memmove.obj $(TargetDir)/__infnan.obj $(TargetDir)/__copysign.obj $(TargetDir)/pow.obj $(TargetDir)/atan2.obj 	\
$(TargetDir)/atan.obj $(TargetDir)/exp.obj $(TargetDir)/exp__E.obj $(TargetDir)/sqrt.obj $(TargetDir)/acos.obj $(TargetDir)/acosh.obj $(TargetDir)/__isnan.obj 	\
$(TargetDir)/__isinf.obj $(TargetDir)/__logb.obj $(TargetDir)/ldexp.obj $(TargetDir)/__drem.obj $(TargetDir)/log.obj $(TargetDir)/log10.obj 	\
$(TargetDir)/log1p.obj $(TargetDir)/log__L.obj $(TargetDir)/asin.obj $(TargetDir)/atanh.obj $(TargetDir)/bcopy.obj $(TargetDir)/bzero.obj 	\
$(TargetDir)/hypot.obj $(TargetDir)/cabs.obj $(TargetDir)/cosh.obj $(TargetDir)/fabs.obj $(TargetDir)/ffs.obj $(TargetDir)/fmod.obj $(TargetDir)/fexp.obj 	\
$(TargetDir)/modf.obj $(TargetDir)/sincos.obj $(TargetDir)/sinh.obj $(TargetDir)/__expm1.obj $(TargetDir)/tan.obj $(TargetDir)/tanh.obj 	\
$(TargetDir)/strncat.obj $(TargetDir)/strpbrk.obj $(TargetDir)/strspn.obj $(TargetDir)/wordcopy.obj

LocaleObjs=$(TargetDir)/C-collate.obj $(TargetDir)/C-ctype.obj $(TargetDir)/C-numeric.obj $(TargetDir)/C-ctype_ct.obj $(TargetDir)/C-ctype_mb.obj 	\
$(TargetDir)/C-monetary.obj $(TargetDir)/C-response.obj $(TargetDir)/C-time.obj $(TargetDir)/localeconv.obj $(TargetDir)/setlocale.obj

StringObjs=$(TargetDir)/strdup.obj $(TargetDir)/strtok.obj

StdioObjs=$(TargetDir)/__vfscanf.obj

AR=ar
Include=-I../

$(Target): $(TargetDir)  $(TargetDir)/mta32.obj $(TargetDir)/elf32.obj $(TargetDir)/elfso32.obj $(Objs) $(StdlibObjs) $(MathObjs) $(SysDepsObjs) $(LocaleObjs) $(StringObjs) $(StdioObjs)
	$(AR) -rc $(Target) $(Objs) $(StdlibObjs) $(MathObjs) $(SysDepsObjs) $(LocaleObjs) $(StringObjs) $(StdioObjs)

$(TargetDir):
	-mkdir $(TargetDir)

$(TargetDir)/string.obj: string.c string.h
	$(CC) $(CFlags) -c string.c -o $(TargetDir)/string.obj $(Include)
	
$(TargetDir)/sparams.obj: sparams.c sparams.h
	$(CC) $(CFlagsNoExt) -c sparams.c -o $(TargetDir)/sparams.obj $(Include)
	
$(TargetDir)/system.obj: system.c system.h
	$(CC) $(CFlags) -c system.c -o $(TargetDir)/system.obj $(Include)
	
$(TargetDir)/screen.obj: screen.c screen.h
	$(CC) $(CFlags) -c screen.c -o $(TargetDir)/screen.obj $(Include)
	
$(TargetDir)/keyboard.obj: keyboard.c keyboard.h
	$(CC) $(CFlags) -c keyboard.c -o $(TargetDir)/keyboard.obj $(Include)
	
$(TargetDir)/fs.obj: fs.c fs.h
	$(CC) $(CFlags) -c fs.c -o $(TargetDir)/fs.obj $(Include)

$(TargetDir)/image.obj: image.c image.h
	$(CC) $(CFlags) -c image.c -o $(TargetDir)/image.obj $(Include)

$(TargetDir)/mouse.obj: mouse.c mouse.h
	$(CC) $(CFlags) -c mouse.c -o $(TargetDir)/mouse.obj $(Include)

$(TargetDir)/stdio.obj: stdio.c stdio.h
	$(CC) $(CFlags) -c stdio.c -o $(TargetDir)/stdio.obj $(Include)

$(TargetDir)/stdlib.obj: stdlib.c stdlib.h
	$(CC) $(CFlags) -c stdlib.c -o $(TargetDir)/stdlib.obj $(Include)

$(TargetDir)/assert.obj: assert.c assert.h
	$(CC) $(CFlags) -c assert.c -o $(TargetDir)/assert.obj $(Include)

$(TargetDir)/setjmp.obj: setjmp.s
	$(AS) $(ASFlags) -c setjmp.s -o $(TargetDir)/setjmp.obj

$(TargetDir)/longjmp.obj: longjmp.s
	$(AS) $(ASFlags) -c longjmp.s -o $(TargetDir)/longjmp.obj

$(TargetDir)/ctype.obj: ctype.c ctype.h
	$(CC) $(CFlags) -c ctype.c -o $(TargetDir)/ctype.obj $(Include)

$(TargetDir)/errno.obj: errno.c errno.h
	$(CC) $(CFlags) -c errno.c -o $(TargetDir)/errno.obj $(Include)

$(TargetDir)/strtod.obj: strtod.c
	$(CC) $(CFlags) -c strtod.c -o $(TargetDir)/strtod.obj $(Include)

$(TargetDir)/gui.obj: gui.c
	$(CC) $(CFlags) -c gui.c -o $(TargetDir)/gui.obj $(Include)


#==============================Stdlib==============================

$(TargetDir)/strtol.obj: stdlib/strtol.c
	$(CC) $(CFlags) -c stdlib/strtol.c -o $(TargetDir)/strtol.obj $(Include)

$(TargetDir)/strtoul.obj: stdlib/strtoul.c
	$(CC) $(CFlags) -c stdlib/strtoul.c -o $(TargetDir)/strtoul.obj $(Include)

$(TargetDir)/atoi.obj: stdlib/atoi.c
	$(CC) $(CFlags) -c stdlib/atoi.c -o $(TargetDir)/atoi.obj $(Include)

$(TargetDir)/atol.obj: stdlib/atol.c
	$(CC) $(CFlags) -c stdlib/atol.c -o $(TargetDir)/atol.obj $(Include)

$(TargetDir)/mbstowcs.obj: stdlib/mbstowcs.c
	$(CC) $(CFlags) -c stdlib/mbstowcs.c -o $(TargetDir)/mbstowcs.obj $(Include)

$(TargetDir)/mbtowc.obj: stdlib/mbtowc.c
	$(CC) $(CFlags) -c stdlib/mbtowc.c -o $(TargetDir)/mbtowc.obj $(Include)

$(TargetDir)/wctomb.obj: stdlib/wctomb.c
	$(CC) $(CFlags) -c stdlib/wctomb.c -o $(TargetDir)/wctomb.obj $(Include)

$(TargetDir)/wcstombs.obj: stdlib/wcstombs.c
	$(CC) $(CFlags) -c stdlib/wcstombs.c -o $(TargetDir)/wcstombs.obj $(Include)

$(TargetDir)/strtod.obj: stdlib/strtod.c
	$(CC) $(CFlags) -c stdlib/strtod.c -o $(TargetDir)/strtod.obj $(Include)

$(TargetDir)/atof.obj: stdlib/atof.c
	$(CC) $(CFlags) -c stdlib/atof.c -o $(TargetDir)/atof.obj $(Include)

$(TargetDir)/abs.obj: stdlib/abs.c
	$(CC) $(CFlags) -c stdlib/abs.c -o $(TargetDir)/abs.obj $(Include)

$(TargetDir)/labs.obj: stdlib/labs.c
	$(CC) $(CFlags) -c stdlib/labs.c -o $(TargetDir)/labs.obj $(Include)

$(TargetDir)/msort.obj: stdlib/msort.c
	$(CC) $(CFlags) -c stdlib/msort.c -o $(TargetDir)/msort.obj $(Include)

$(TargetDir)/qsort.obj: stdlib/qsort.c
	$(CC) $(CFlags) -c stdlib/qsort.c -o $(TargetDir)/qsort.obj $(Include)

$(TargetDir)/mblen.obj: stdlib/mblen.c
	$(CC) $(CFlags) -c stdlib/mblen.c -o $(TargetDir)/mblen.obj $(Include)




#==============================Math==============================

$(TargetDir)/ceil.obj: math/ceil.c
	$(CC) $(CFlags) -c math/ceil.c -o $(TargetDir)/ceil.obj $(Include)

$(TargetDir)/floor.obj: math/floor.c
	$(CC) $(CFlags) -c math/floor.c -o $(TargetDir)/floor.obj $(Include)

$(TargetDir)/fabs.obj: math/fabs.c
	$(CC) $(CFlags) -c math/fabs.c -o $(TargetDir)/fabs.obj $(Include)

$(TargetDir)/__scalb.obj: math/__scalb.c
	$(CC) $(CFlags) -c math/__scalb.c -o $(TargetDir)/__scalb.obj $(Include)

$(TargetDir)/__finite.obj: math/__finite.c
	$(CC) $(CFlags) -c math/__finite.c -o $(TargetDir)/__finite.obj $(Include)


#==============================SysDeps==============================

$(TargetDir)/memmove.obj: sysdeps/generic/memmove.c
	$(CC) $(CFlags) -c sysdeps/generic/memmove.c -o $(TargetDir)/memmove.obj $(Include)

$(TargetDir)/__infnan.obj: sysdeps/generic/__infnan.c
	$(CC) $(CFlags) -c sysdeps/generic/__infnan.c -o $(TargetDir)/__infnan.obj $(Include)

$(TargetDir)/__copysign.obj: sysdeps/generic/__copysign.c
	$(CC) $(CFlags) -c sysdeps/generic/__copysign.c -o $(TargetDir)/__copysign.obj $(Include)

$(TargetDir)/pow.obj: sysdeps/generic/pow.c
	$(CC) $(CFlags) -c sysdeps/generic/pow.c -o $(TargetDir)/pow.obj $(Include)

$(TargetDir)/atan2.obj: sysdeps/generic/atan2.c
	$(CC) $(CFlags) -c sysdeps/generic/atan2.c -o $(TargetDir)/atan2.obj $(Include)

$(TargetDir)/atan.obj: sysdeps/generic/atan.c
	$(CC) $(CFlags) -c sysdeps/generic/atan.c -o $(TargetDir)/atan.obj $(Include)

$(TargetDir)/exp.obj: sysdeps/generic/exp.c
	$(CC) $(CFlags) -c sysdeps/generic/exp.c -o $(TargetDir)/exp.obj $(Include)

$(TargetDir)/exp__E.obj: sysdeps/generic/exp__E.c
	$(CC) $(CFlags) -c sysdeps/generic/exp__E.c -o $(TargetDir)/exp__E.obj $(Include)

$(TargetDir)/sqrt.obj: sysdeps/ieee754/sqrt.c
	$(CC) $(CFlags) -c sysdeps/ieee754/sqrt.c -o $(TargetDir)/sqrt.obj $(Include)

$(TargetDir)/acos.obj: sysdeps/generic/acos.c
	$(CC) $(CFlags) -c sysdeps/generic/acos.c -o $(TargetDir)/acos.obj $(Include)

$(TargetDir)/acosh.obj: sysdeps/generic/acosh.c
	$(CC) $(CFlags) -c sysdeps/generic/acosh.c -o $(TargetDir)/acosh.obj $(Include)

$(TargetDir)/__isnan.obj: sysdeps/ieee754/__isnan.c
	$(CC) $(CFlags) -c sysdeps/ieee754/__isnan.c -o $(TargetDir)/__isnan.obj $(Include)

$(TargetDir)/__isinf.obj: sysdeps/ieee754/__isinf.c
	$(CC) $(CFlags) -c sysdeps/ieee754/__isinf.c -o $(TargetDir)/__isinf.obj $(Include)

$(TargetDir)/__logb.obj: sysdeps/ieee754/__logb.c
	$(CC) $(CFlags) -c sysdeps/ieee754/__logb.c -o $(TargetDir)/__logb.obj $(Include)

$(TargetDir)/ldexp.obj: sysdeps/ieee754/ldexp.c
	$(CC) $(CFlags) -c sysdeps/ieee754/ldexp.c -o $(TargetDir)/ldexp.obj $(Include)

$(TargetDir)/__drem.obj: sysdeps/ieee754/__drem.c
	$(CC) $(CFlags) -c sysdeps/ieee754/__drem.c -o $(TargetDir)/__drem.obj $(Include)

$(TargetDir)/log.obj: sysdeps/generic/log.c
	$(CC) $(CFlags) -c sysdeps/generic/log.c -o $(TargetDir)/log.obj $(Include)

$(TargetDir)/log10.obj: sysdeps/generic/log10.c
	$(CC) $(CFlags) -c sysdeps/generic/log10.c -o $(TargetDir)/log10.obj $(Include)

$(TargetDir)/log1p.obj: sysdeps/generic/log1p.c
	$(CC) $(CFlags) -c sysdeps/generic/log1p.c -o $(TargetDir)/log1p.obj $(Include)

$(TargetDir)/log__L.obj: sysdeps/generic/log__L.c
	$(CC) $(CFlags) -c sysdeps/generic/log__L.c -o $(TargetDir)/log__L.obj $(Include)

$(TargetDir)/asin.obj: sysdeps/generic/asin.c
	$(CC) $(CFlags) -c sysdeps/generic/asin.c -o $(TargetDir)/asin.obj $(Include)

$(TargetDir)/atanh.obj: sysdeps/generic/atanh.c
	$(CC) $(CFlags) -c sysdeps/generic/atanh.c -o $(TargetDir)/atanh.obj $(Include)

$(TargetDir)/bcopy.obj: sysdeps/generic/bcopy.c
	$(CC) $(CFlags) -c sysdeps/generic/bcopy.c -o $(TargetDir)/bcopy.obj $(Include)

$(TargetDir)/bzero.obj: sysdeps/generic/bzero.c
	$(CC) $(CFlags) -c sysdeps/generic/bzero.c -o $(TargetDir)/bzero.obj $(Include)

$(TargetDir)/hypot.obj: sysdeps/generic/hypot.c
	$(CC) $(CFlags) -c sysdeps/generic/hypot.c -o $(TargetDir)/hypot.obj $(Include)

$(TargetDir)/cabs.obj: sysdeps/generic/cabs.c
	$(CC) $(CFlags) -c sysdeps/generic/cabs.c -o $(TargetDir)/cabs.obj $(Include)

$(TargetDir)/cosh.obj: sysdeps/generic/cosh.c
	$(CC) $(CFlags) -c sysdeps/generic/cosh.c -o $(TargetDir)/cosh.obj $(Include)

$(TargetDir)/fabs.obj: sysdeps/generic/fabs.c
	$(CC) $(CFlags) -c sysdeps/generic/fabs.c -o $(TargetDir)/fabs.obj $(Include)

$(TargetDir)/ffs.obj: sysdeps/generic/ffs.c
	$(CC) $(CFlags) -c sysdeps/generic/ffs.c -o $(TargetDir)/ffs.obj $(Include)

$(TargetDir)/fmod.obj: sysdeps/generic/fmod.c
	$(CC) $(CFlags) -c sysdeps/generic/fmod.c -o $(TargetDir)/fmod.obj $(Include)

$(TargetDir)/fexp.obj: sysdeps/generic/fexp.c
	$(CC) $(CFlags) -c sysdeps/generic/fexp.c -o $(TargetDir)/fexp.obj $(Include)

$(TargetDir)/modf.obj: sysdeps/generic/modf.c
	$(CC) $(CFlags) -c sysdeps/generic/modf.c -o $(TargetDir)/modf.obj $(Include)

$(TargetDir)/sincos.obj: sysdeps/generic/sincos.c
	$(CC) $(CFlags) -c sysdeps/generic/sincos.c -o $(TargetDir)/sincos.obj $(Include)

$(TargetDir)/sinh.obj: sysdeps/generic/sinh.c
	$(CC) $(CFlags) -c sysdeps/generic/sinh.c -o $(TargetDir)/sinh.obj $(Include)

$(TargetDir)/__expm1.obj: sysdeps/generic/__expm1.c
	$(CC) $(CFlags) -c sysdeps/generic/__expm1.c -o $(TargetDir)/__expm1.obj $(Include)

$(TargetDir)/tan.obj: sysdeps/generic/tan.c
	$(CC) $(CFlags) -c sysdeps/generic/tan.c -o $(TargetDir)/tan.obj $(Include)

$(TargetDir)/tanh.obj: sysdeps/generic/tanh.c
	$(CC) $(CFlags) -c sysdeps/generic/tanh.c -o $(TargetDir)/tanh.obj $(Include)

$(TargetDir)/strncat.obj: sysdeps/generic/strncat.c
	$(CC) $(CFlags) -c sysdeps/generic/strncat.c -o $(TargetDir)/strncat.obj $(Include)

$(TargetDir)/strpbrk.obj: sysdeps/generic/strpbrk.c
	$(CC) $(CFlags) -c sysdeps/generic/strpbrk.c -o $(TargetDir)/strpbrk.obj $(Include)

$(TargetDir)/strspn.obj: sysdeps/generic/strspn.c
	$(CC) $(CFlags) -c sysdeps/generic/strspn.c -o $(TargetDir)/strspn.obj $(Include)

$(TargetDir)/wordcopy.obj: sysdeps/generic/wordcopy.c
	$(CC) $(CFlags) -c sysdeps/generic/wordcopy.c -o $(TargetDir)/wordcopy.obj $(Include)


#==============================Locale==============================

$(TargetDir)/C-collate.obj: locale/C-collate.c
	$(CC) $(CFlags) -c locale/C-collate.c -o $(TargetDir)/C-collate.obj $(Include)

$(TargetDir)/C-ctype.obj: locale/C-ctype.c
	$(CC) $(CFlags) -c locale/C-ctype.c -o $(TargetDir)/C-ctype.obj $(Include)

$(TargetDir)/C-numeric.obj: locale/C-numeric.c
	$(CC) $(CFlags) -c locale/C-numeric.c -o $(TargetDir)/C-numeric.obj $(Include)

$(TargetDir)/C-ctype_ct.obj: locale/C-ctype_ct.c
	$(CC) $(CFlags) -c locale/C-ctype_ct.c -o $(TargetDir)/C-ctype_ct.obj $(Include)

$(TargetDir)/C-ctype_mb.obj: locale/C-ctype_mb.c
	$(CC) $(CFlags) -c locale/C-ctype_mb.c -o $(TargetDir)/C-ctype_mb.obj $(Include)

$(TargetDir)/C-monetary.obj: locale/C-monetary.c
	$(CC) $(CFlags) -c locale/C-monetary.c -o $(TargetDir)/C-monetary.obj $(Include)

$(TargetDir)/C-response.obj: locale/C-response.c
	$(CC) $(CFlags) -c locale/C-response.c -o $(TargetDir)/C-response.obj $(Include)

$(TargetDir)/C-time.obj: locale/C-time.c
	$(CC) $(CFlags) -c locale/C-time.c -o $(TargetDir)/C-time.obj $(Include)

$(TargetDir)/localeconv.obj: locale/localeconv.c
	$(CC) $(CFlags) -c locale/localeconv.c -o $(TargetDir)/localeconv.obj $(Include)

$(TargetDir)/setlocale.obj: locale/setlocale.c
	$(CC) $(CFlags) -c locale/setlocale.c -o $(TargetDir)/setlocale.obj $(Include)



#==============================String==============================

$(TargetDir)/strdup.obj: string/strdup.c
	$(CC) $(CFlags) -c string/strdup.c -o $(TargetDir)/strdup.obj $(Include)

$(TargetDir)/strtok.obj: string/strtok.c
	$(CC) $(CFlags) -c string/strtok.c -o $(TargetDir)/strtok.obj $(Include)

#==============================Stdio==============================

$(TargetDir)/__vfscanf.obj: stdio/__vfscanf.c
	$(CC) $(CFlags) -c stdio/__vfscanf.c -o $(TargetDir)/__vfscanf.obj $(Include)

$(TargetDir)/mta32.obj: mta32.c
	$(CC) $(CFlags) -c mta32.c -o $(TargetDir)/mta32.obj $(Include)

$(TargetDir)/elf32.obj: elf32.c
	$(CC) $(CFlags) -c elf32.c -o $(TargetDir)/elf32.obj $(Include)

$(TargetDir)/elfso32.obj: elfso32.c
	$(CC) $(CFlags) -c elfso32.c -o $(TargetDir)/elfso32.obj $(Include)

.PHONY:clean

clean:
	-rm -f $(Target) $(TargetDir)/mta32.obj $(TargetDir)/elf32.obj $(TargetDir)/elfso32.obj $(Objs) $(StdlibObjs) $(MathObjs) $(SysDepsObjs) $(LocaleObjs) $(StringObjs)
