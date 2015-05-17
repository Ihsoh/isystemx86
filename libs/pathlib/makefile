TargetDir=bin
Out=libpathlib.a
Target=$(TargetDir)/$(Out)
CC=gcc
CFlags=-O0 -fno-stack-protector -nostdinc -m32 -march=i386 -mtune=i386
Objs=bin/pathlib.obj bin/lib.obj bin/test.obj
AR=ar
Include=-I../

$(Target): bin $(Objs)
	$(AR) -rc $(Target) $(Objs)

bin:
	-mkdir bin

bin/pathlib.obj: pathlib.c pathlib.h
	$(CC) $(CFlags) -c pathlib.c -o bin/pathlib.obj $(Include)

bin/lib.obj: lib.c lib.h
	$(CC) $(CFlags) -c lib.c -o bin/lib.obj $(Include)

bin/test.obj: test.c test.h
	$(CC) $(CFlags) -c test.c -o bin/test.obj $(Include)

.PHONY:clean

clean:
	-rm -f $(Target) $(Objs)
