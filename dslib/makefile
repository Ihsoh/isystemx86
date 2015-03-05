TargetDir=bin
Out=libdslib.a
Target=$(TargetDir)/$(Out)
CC=gcc
CFlags=-O0 -fno-stack-protector -nostdinc -I. -m32 -march=i386 -mtune=i386
Objs=bin/dslib.obj bin/value.obj bin/linked_list.obj bin/list.obj bin/lib.obj
AR=ar

$(Target): bin $(Objs)
	$(AR) -rc $(Target) $(Objs)

bin:
	-mkdir bin

bin/dslib.obj: dslib.c dslib.h
	$(CC) $(CFlags) -c dslib.c -o bin/dslib.obj

bin/value.obj: value.c value.h
	$(CC) $(CFlags) -c value.c -o bin/value.obj

bin/linked_list.obj: linked_list.c linked_list.h
	$(CC) $(CFlags) -c linked_list.c -o bin/linked_list.obj

bin/list.obj: list.c list.h
	$(CC) $(CFlags) -c list.c -o bin/list.obj

bin/lib.obj: lib.c lib.h
	$(CC) $(CFlags) -c lib.c -o bin/lib.obj

.PHONY:Clear

Clear:
	-rm -f $(Target) $(Objs)
