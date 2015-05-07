TargetDir=bin
Out=libbaslanglib.a
Target=$(TargetDir)/$(Out)
CC=gcc
CFlags=-O0 -fno-stack-protector -nostdinc -m32 -march=i386 -mtune=i386
Objs=bin/baslanglib.obj bin/lexer.obj
AR=ar
Include=-I../

$(Target): bin $(Objs)
	$(AR) -rc $(Target) $(Objs)

bin:
	-mkdir bin

bin/baslanglib.obj: baslanglib.c baslanglib.h
	$(CC) $(CFlags) -c baslanglib.c -o bin/baslanglib.obj $(Include)

bin/lexer.obj: lexer.c lexer.h
	$(CC) $(CFlags) -c lexer.c -o bin/lexer.obj $(Include)

.PHONY:Clear

Clear:
	-rm -f $(Target) $(Objs)
