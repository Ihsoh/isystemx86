TargetDir=bin
Out=lua2.bin
Out1=lua2c.bin
Target=$(TargetDir)/$(Out)
Target1=$(TargetDir)/$(Out1)
CC=gcc
CFlags=-O0 -fno-stack-protector -nostdinc -m32 -march=i386 -mtune=i386
Objs0=bin/fallback.obj bin/func.obj bin/hash.obj bin/inout.obj bin/lex.obj bin/mem.obj bin/opcode.obj 	\
bin/parser.obj bin/table.obj bin/tree.obj bin/undump.obj
Objs= $(Objs0) bin/lua.obj bin/iolib.obj bin/mathlib.obj bin/strlib.obj
Objs2= $(Objs0) bin/luac.obj bin/dump.obj bin/print.obj


Linker=ld -m elf_i386
Include=-I../../libs/
AppMaker=../../tools/appmaker/bin/appmaker
MTA32Obj=../../libs/ilib/bin/mta32.obj

all: bin $(Target) $(Target1)

$(Target): $(Objs) 
	$(Linker) -Tsegs.lds -e mta32main --oformat binary -o $(Target) $(MTA32Obj) $(Objs)	\
-L../../libs/ilib/bin -lilib	\
-L../../libs/dslib/bin -ldslib	\
-L../../libs/mempoollib/bin -lmempoollib
	$(AppMaker) $(Target)

$(Target1): $(Objs2) 
	$(Linker) -Tsegs.lds -e mta32main --oformat binary -o $(Target1) $(MTA32Obj) $(Objs2)	\
-L../../libs/ilib/bin -lilib	\
-L../../libs/dslib/bin -ldslib	\
-L../../libs/mempoollib/bin -lmempoollib
	$(AppMaker) $(Target1)

bin:
	-mkdir bin

bin/fallback.obj: fallback.c
	$(CC) $(CFlags) -c fallback.c -o bin/fallback.obj $(Include)

bin/func.obj: func.c
	$(CC) $(CFlags) -c func.c -o bin/func.obj $(Include)

bin/hash.obj: hash.c
	$(CC) $(CFlags) -c hash.c -o bin/hash.obj $(Include)

bin/inout.obj: inout.c
	$(CC) $(CFlags) -c inout.c -o bin/inout.obj $(Include)

bin/lex.obj: lex.c
	$(CC) $(CFlags) -c lex.c -o bin/lex.obj $(Include)

bin/mem.obj: mem.c
	$(CC) $(CFlags) -c mem.c -o bin/mem.obj $(Include)

bin/opcode.obj: opcode.c
	$(CC) $(CFlags) -c opcode.c -o bin/opcode.obj $(Include)

bin/parser.obj: parser.c
	$(CC) $(CFlags) -c parser.c -o bin/parser.obj $(Include)

bin/table.obj: table.c
	$(CC) $(CFlags) -c table.c -o bin/table.obj $(Include)

bin/tree.obj: tree.c
	$(CC) $(CFlags) -c tree.c -o bin/tree.obj $(Include)

bin/undump.obj: undump.c
	$(CC) $(CFlags) -c undump.c -o bin/undump.obj $(Include)

bin/lua.obj: lua.c
	$(CC) $(CFlags) -c lua.c -o bin/lua.obj $(Include)

bin/iolib.obj: iolib.c
	$(CC) $(CFlags) -c iolib.c -o bin/iolib.obj $(Include)

bin/mathlib.obj: mathlib.c
	$(CC) $(CFlags) -c mathlib.c -o bin/mathlib.obj $(Include)

bin/strlib.obj: strlib.c
	$(CC) $(CFlags) -c strlib.c -o bin/strlib.obj $(Include)

bin/luac.obj: luac.c
	$(CC) $(CFlags) -c luac.c -o bin/luac.obj $(Include)

bin/dump.obj: dump.c
	$(CC) $(CFlags) -c dump.c -o bin/dump.obj $(Include)

bin/print.obj: print.c
	$(CC) $(CFlags) -c print.c -o bin/print.obj $(Include)	




.PHONY:clean

clean:
	-rm -f $(Objs) $(Target) $(Target1)
