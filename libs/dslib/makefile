# TargetDir=bin
# Out=libdslib.a
Target=$(TargetDir)/$(Out)
CC=gcc
CFlags=-O0 -fno-stack-protector -nostdinc -m32 -march=i386 -mtune=i386 $(CFlagsExt)
Objs=$(TargetDir)/dslib.obj $(TargetDir)/value.obj $(TargetDir)/linked_list.obj $(TargetDir)/list.obj $(TargetDir)/lib.obj 	\
$(TargetDir)/hashtable.obj
AR=ar

$(Target): $(TargetDir) $(Objs)
	$(AR) -rc $(Target) $(Objs)

$(TargetDir):
	-mkdir $(TargetDir)

$(TargetDir)/dslib.obj: dslib.c dslib.h
	$(CC) $(CFlags) -c dslib.c -o $(TargetDir)/dslib.obj

$(TargetDir)/value.obj: value.c value.h
	$(CC) $(CFlags) -c value.c -o $(TargetDir)/value.obj

$(TargetDir)/linked_list.obj: linked_list.c linked_list.h
	$(CC) $(CFlags) -c linked_list.c -o $(TargetDir)/linked_list.obj

$(TargetDir)/list.obj: list.c list.h
	$(CC) $(CFlags) -c list.c -o $(TargetDir)/list.obj

$(TargetDir)/lib.obj: lib.c lib.h
	$(CC) $(CFlags) -c lib.c -o $(TargetDir)/lib.obj

$(TargetDir)/hashtable.obj: hashtable.c hashtable.h
	$(CC) $(CFlags) -c hashtable.c -o $(TargetDir)/hashtable.obj

.PHONY:clean

clean:
	-rm -f $(Target) $(Objs)
