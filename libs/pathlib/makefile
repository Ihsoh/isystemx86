TargetDir=bin
Out=libpathlib.a
Target=$(TargetDir)/$(Out)
CC=gcc
CFlags=-O0 -fno-stack-protector -nostdinc -m32 -march=i386 -mtune=i386
Objs=bin/pathlib.obj
AR=ar
Include=-I../

$(Target): bin $(Objs)
	$(AR) -rc $(Target) $(Objs)

bin:
	-mkdir bin

bin/pathlib.obj: pathlib.c pathlib.h
	$(CC) $(CFlags) -c pathlib.c -o bin/pathlib.obj $(Include)

.PHONY:Clear

Clear:
	-rm -f $(Target) $(Objs)
