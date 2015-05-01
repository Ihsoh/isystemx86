TargetDir=bin
Out=libencryptionlib.a
Target=$(TargetDir)/$(Out)
CC=gcc
CFlags=-O0 -fno-stack-protector -nostdinc -m32 -march=i386 -mtune=i386
Objs=bin/lib.obj bin/md5.obj bin/sha1.obj bin/sha256.obj bin/sha512.obj
AR=ar
Include=-I../

$(Target): bin $(Objs)
	$(AR) -rc $(Target) $(Objs)

bin:
	-mkdir bin

bin/lib.obj: lib.c lib.h
	$(CC) $(CFlags) -c lib.c -o bin/lib.obj $(Include)

bin/md5.obj: md5.c md5.h
	$(CC) $(CFlags) -c md5.c -o bin/md5.obj $(Include)

bin/sha1.obj: sha1.c sha1.h
	$(CC) $(CFlags) -c sha1.c -o bin/sha1.obj $(Include)

bin/sha256.obj: sha256.c sha256.h
	$(CC) $(CFlags) -c sha256.c -o bin/sha256.obj $(Include)

bin/sha512.obj: sha512.c sha512.h
	$(CC) $(CFlags) -c sha512.c -o bin/sha512.obj $(Include)

.PHONY:Clear

Clear:
	-rm -f $(Target) $(Objs)
