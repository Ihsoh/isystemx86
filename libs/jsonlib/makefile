# TargetDir=bin
# Out=libjsonlib.a
Target=$(TargetDir)/$(Out)
CC=gcc
CFlags=-O0 -fno-stack-protector -nostdinc -m32 -march=i386 -mtune=i386 $(CFlagsExt)
Objs=$(TargetDir)/lib.obj $(TargetDir)/jsonlib.obj
AR=ar
Include=-I../

$(Target): $(TargetDir) $(Objs)
	$(AR) -rc $(Target) $(Objs)

$(TargetDir):
	-mkdir $(TargetDir)

$(TargetDir)/jsonlib.obj: jsonlib.c jsonlib.h
	$(CC) $(CFlags) -c jsonlib.c -o $(TargetDir)/jsonlib.obj $(Include)

$(TargetDir)/lib.obj: lib.c lib.h
	$(CC) $(CFlags) -c lib.c -o $(TargetDir)/lib.obj $(Include)

.PHONY:clean

clean:
	-rm -f $(Target) $(Objs)
