TargetDir=bin
Out=libjsonlib.a
Target=$(TargetDir)/$(Out)
CC=gcc
CFlags=-O0 -fno-stack-protector -nostdinc -I. -m32 -march=i386 -mtune=i386
Objs=bin/jsonlib.obj
AR=ar

$(Target): bin $(Objs)
	$(AR) -rc $(Target) $(Objs)

bin:
	-mkdir bin

bin/jsonlib.obj: jsonlib.c jsonlib.h
	$(CC) $(CFlags) -c jsonlib.c -o bin/jsonlib.obj

.PHONY:Clear

Clear:
	-rm -f $(Target) $(Objs)
