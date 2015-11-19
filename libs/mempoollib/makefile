# TargetDir=bin
# Out=libmempoollib.a
Target=$(TargetDir)/$(Out)
CC=gcc
CFlags=-O0 -fno-stack-protector -nostdinc -m32 -march=i386 -mtune=i386 $(CFlagsExt)
Objs=$(TargetDir)/mempoollib.obj
AR=ar
Include=-I../

$(Target): $(TargetDir) $(Objs)
	$(AR) -rc $(Target) $(Objs)

$(TargetDir):
	-mkdir $(TargetDir)

$(TargetDir)/mempoollib.obj: mempoollib.c mempoollib.h
	$(CC) $(CFlags) -c mempoollib.c -o $(TargetDir)/mempoollib.obj $(Include)

.PHONY:clean

clean:
	-rm -f $(Target) $(Objs)
