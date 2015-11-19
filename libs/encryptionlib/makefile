# TargetDir=bin
# Out=libencryptionlib.a
Target=$(TargetDir)/$(Out)
CC=gcc
CFlags=-O0 -fno-stack-protector -nostdinc -m32 -march=i386 -mtune=i386 $(CFlagsExt)
Objs=$(TargetDir)/lib.obj $(TargetDir)/md5.obj $(TargetDir)/sha1.obj $(TargetDir)/sha256.obj $(TargetDir)/sha512.obj
AR=ar
Include=-I../

$(Target): $(TargetDir) $(Objs)
	$(AR) -rc $(Target) $(Objs)

$(TargetDir):
	-mkdir $(TargetDir)

$(TargetDir)/lib.obj: lib.c lib.h
	$(CC) $(CFlags) -c lib.c -o $(TargetDir)/lib.obj $(Include)

$(TargetDir)/md5.obj: md5.c md5.h
	$(CC) $(CFlags) -c md5.c -o $(TargetDir)/md5.obj $(Include)

$(TargetDir)/sha1.obj: sha1.c sha1.h
	$(CC) $(CFlags) -c sha1.c -o $(TargetDir)/sha1.obj $(Include)

$(TargetDir)/sha256.obj: sha256.c sha256.h
	$(CC) $(CFlags) -c sha256.c -o $(TargetDir)/sha256.obj $(Include)

$(TargetDir)/sha512.obj: sha512.c sha512.h
	$(CC) $(CFlags) -c sha512.c -o $(TargetDir)/sha512.obj $(Include)

.PHONY:clean

clean:
	-rm -f $(Target) $(Objs)
