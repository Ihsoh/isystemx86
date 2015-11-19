# TargetDir=bin
# Out=libimagelib.a
Target=$(TargetDir)/$(Out)
CC=gcc
CFlags=-O0 -fno-stack-protector -nostdinc -m32 -march=i386 -mtune=i386 -std=c99 $(CFlagsExt)
Objs=$(TargetDir)/imagelib.obj $(TargetDir)/bmp.obj
AR=ar
Include=-I../

$(Target): $(TargetDir) $(Objs)
	$(AR) -rc $(Target) $(Objs)

$(TargetDir):
	-mkdir $(TargetDir)

$(TargetDir)/imagelib.obj: imagelib.c imagelib.h
	$(CC) $(CFlags) -c imagelib.c -o $(TargetDir)/imagelib.obj $(Include)

$(TargetDir)/bmp.obj: bmp.c bmp.h
	$(CC) $(CFlags) -c bmp.c -o $(TargetDir)/bmp.obj $(Include)

.PHONY:clean

clean:
	-rm -f $(Target) $(Objs)
