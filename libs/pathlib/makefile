# TargetDir=bin
# Out=libpathlib.a
Target=$(TargetDir)/$(Out)
CC=gcc
CFlags=-O0 -fno-stack-protector -nostdinc -m32 -march=i386 -mtune=i386 $(CFlagsExt)
Objs=$(TargetDir)/pathlib.obj $(TargetDir)/lib.obj $(TargetDir)/test.obj
AR=ar
Include=-I../

$(Target): $(TargetDir) $(Objs)
	$(AR) -rc $(Target) $(Objs)

$(TargetDir):
	-mkdir $(TargetDir)

$(TargetDir)/pathlib.obj: pathlib.c pathlib.h
	$(CC) $(CFlags) -c pathlib.c -o $(TargetDir)/pathlib.obj $(Include)

$(TargetDir)/lib.obj: lib.c lib.h
	$(CC) $(CFlags) -c lib.c -o $(TargetDir)/lib.obj $(Include)

$(TargetDir)/test.obj: test.c test.h
	$(CC) $(CFlags) -c test.c -o $(TargetDir)/test.obj $(Include)

.PHONY:clean

clean:
	-rm -f $(Target) $(Objs)
