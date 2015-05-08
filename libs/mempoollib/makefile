TargetDir=bin
Out=libmempoollib.a
Target=$(TargetDir)/$(Out)
CC=gcc
CFlags=-O0 -fno-stack-protector -nostdinc -m32 -march=i386 -mtune=i386
Objs=bin/mempoollib.obj
AR=ar
Include=-I../

$(Target): bin $(Objs)
	$(AR) -rc $(Target) $(Objs)

bin:
	-mkdir bin

bin/mempoollib.obj: mempoollib.c mempoollib.h
	$(CC) $(CFlags) -c mempoollib.c -o bin/mempoollib.obj $(Include)

.PHONY:Clear

Clear:
	-rm -f $(Target) $(Objs)
