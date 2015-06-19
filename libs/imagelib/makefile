TargetDir=bin
Out=libimagelib.a
Target=$(TargetDir)/$(Out)
CC=gcc
CFlags=-O0 -fno-stack-protector -nostdinc -m32 -march=i386 -mtune=i386 -std=c99
Objs=bin/imagelib.obj bin/bmp.obj
AR=ar
Include=-I../

$(Target): bin $(Objs)
	$(AR) -rc $(Target) $(Objs)

bin:
	-mkdir bin

bin/imagelib.obj: imagelib.c imagelib.h
	$(CC) $(CFlags) -c imagelib.c -o bin/imagelib.obj $(Include)

bin/bmp.obj: bmp.c bmp.h
	$(CC) $(CFlags) -c bmp.c -o bin/bmp.obj $(Include)

.PHONY:clean

clean:
	-rm -f $(Target) $(Objs)
