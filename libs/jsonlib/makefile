TargetDir=bin
Out=libjsonlib.a
Target=$(TargetDir)/$(Out)
CC=gcc
CFlags=-O0 -fno-stack-protector -nostdinc -m32 -march=i386 -mtune=i386
Objs=bin/lib.obj bin/jsonlib.obj
AR=ar
Include=-I../

$(Target): bin $(Objs)
	$(AR) -rc $(Target) $(Objs)

bin:
	-mkdir bin

bin/jsonlib.obj: jsonlib.c jsonlib.h
	$(CC) $(CFlags) -c jsonlib.c -o bin/jsonlib.obj $(Include)

bin/lib.obj: lib.c lib.h
	$(CC) $(CFlags) -c lib.c -o bin/lib.obj $(Include)

.PHONY:clean

clean:
	-rm -f $(Target) $(Objs)
