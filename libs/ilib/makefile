#Filename:	makefile
#Author:	Ihsoh
#Date:		2014-1-29
#Descriptor:Make ilib

TargetDir=bin
Out=libilib.a
Target=$(TargetDir)/$(Out)
CC=gcc
CFlags=-O0 -fno-stack-protector -nostdinc -m32 -march=i386 -mtune=i386
Objs=bin/string.obj bin/sparams.obj bin/system.obj bin/screen.obj bin/keyboard.obj \
bin/fs.obj bin/image.obj bin/mouse.obj bin/stdio.obj bin/stdlib.obj bin/assert.obj
AR=ar
Include=-I../

$(Target): bin  bin/mta32.obj $(Objs)
	$(AR) -rc $(Target) $(Objs)

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



bin/mta32.obj: mta32.c
	$(CC) $(CFlags) -c mta32.c -o bin/mta32.obj $(Include)

.PHONY:Clear

Clear:
	-rm -f $(Target) $(Objs)
