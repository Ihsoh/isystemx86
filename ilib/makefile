#Filename:	makefile
#Author:	Ihsoh
#Date:		2014-1-29
#Descriptor:Make ilib

TargetDir=bin
Out=libilib.a
Target=$(TargetDir)/$(Out)
CC=gcc
CFlags=-O0 -fno-stack-protector -nostdinc -I. -m32 -march=i386 -mtune=i386
Objs=bin/string.obj bin/sparams.obj bin/system.obj bin/screen.obj bin/keyboard.obj \
bin/fs.obj bin/image.obj bin/mouse.obj bin/stdio.obj bin/stdlib.obj
AR=ar

$(Target): bin $(Objs)
	$(AR) -rc $(Target) $(Objs)

bin:
	-mkdir bin

bin/string.obj: string.c string.h
	$(CC) $(CFlags) -c string.c -o bin/string.obj
	
bin/sparams.obj: sparams.c sparams.h
	$(CC) $(CFlags) -c sparams.c -o bin/sparams.obj
	
bin/system.obj: system.c system.h
	$(CC) $(CFlags) -c system.c -o bin/system.obj
	
bin/screen.obj: screen.c screen.h
	$(CC) $(CFlags) -c screen.c -o bin/screen.obj
	
bin/keyboard.obj: keyboard.c keyboard.h
	$(CC) $(CFlags) -c keyboard.c -o bin/keyboard.obj
	
bin/fs.obj: fs.c fs.h
	$(CC) $(CFlags) -c fs.c -o bin/fs.obj

bin/image.obj: image.c image.h
	$(CC) $(CFlags) -c image.c -o bin/image.obj

bin/mouse.obj: mouse.c mouse.h
	$(CC) $(CFlags) -c mouse.c -o bin/mouse.obj

bin/stdio.obj: stdio.c stdio.h
	$(CC) $(CFlags) -c stdio.c -o bin/stdio.obj

bin/stdlib.obj: stdlib.c stdlib.h
	$(CC) $(CFlags) -c stdlib.c -o bin/stdlib.obj

.PHONY:Clear

Clear:
	-rm -f $(Target) $(Objs)
