# TargetDir=bin
# Out=libbaslanglib.a
Target=$(TargetDir)/$(Out)
CC=gcc
CFlags=-O0 -fno-stack-protector -nostdinc -m32 -march=i386 -mtune=i386 $(CFlagsExt)
Objs=$(TargetDir)/baslanglib.obj $(TargetDir)/lexer.obj 	\
$(TargetDir)/stat_expr.obj
AR=ar
Include=-I../

$(Target): $(TargetDir) $(Objs)
	$(AR) -rc $(Target) $(Objs)

$(TargetDir):
	-mkdir $(TargetDir)

$(TargetDir)/baslanglib.obj: baslanglib.c baslanglib.h
	$(CC) $(CFlags) -c baslanglib.c -o $(TargetDir)/baslanglib.obj $(Include)

$(TargetDir)/lexer.obj: lexer.c lexer.h
	$(CC) $(CFlags) -c lexer.c -o $(TargetDir)/lexer.obj $(Include)

$(TargetDir)/stat_expr.obj: stat_expr.c stat_expr.h
	$(CC) $(CFlags) -c stat_expr.c -o $(TargetDir)/stat_expr.obj $(Include)

.PHONY:clean

clean:
	-rm -f $(Target) $(Objs)
