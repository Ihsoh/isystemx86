TargetDir=bin
Out=lua.bin
Target=$(TargetDir)/$(Out)
CC=gcc
CFlags=-O0 -fno-stack-protector -nostdinc -m32 -march=i386 -mtune=i386
Objs=bin/hash.obj bin/inout.obj bin/iolib.obj bin/lex_yy.obj bin/lua.obj bin/mathlib.obj 	\
bin/opcode.obj bin/strlib.obj bin/table.obj bin/y_tab.obj bin/lib.obj
Linker=ld -m elf_i386
Include=-I../../libs/
AppMaker=../../tools/appmaker/bin/appmaker
MTA32Obj=../../libs/ilib/bin/mta32.obj

$(Target): bin $(Objs)
	$(Linker) -Tsegs.lds -e mta32main --oformat binary -o $(Target) $(MTA32Obj) $(Objs)	\
-L../../libs/ilib/bin -lilib	\
-L../../libs/dslib/bin -ldslib	\
-L../../libs/mempoollib/bin -lmempoollib
	$(AppMaker) $(Target)

bin:
	-mkdir bin

bin/hash.obj: hash.c
	$(CC) $(CFlags) -c hash.c -o bin/hash.obj $(Include)

bin/inout.obj: inout.c
	$(CC) $(CFlags) -c inout.c -o bin/inout.obj $(Include)

bin/iolib.obj: iolib.c
	$(CC) $(CFlags) -c iolib.c -o bin/iolib.obj $(Include)

bin/lex_yy.obj: lex_yy.c
	$(CC) $(CFlags) -c lex_yy.c -o bin/lex_yy.obj $(Include)

bin/lua.obj: lua.c
	$(CC) $(CFlags) -c lua.c -o bin/lua.obj $(Include)

bin/mathlib.obj: mathlib.c
	$(CC) $(CFlags) -c mathlib.c -o bin/mathlib.obj $(Include)

bin/opcode.obj: opcode.c
	$(CC) $(CFlags) -c opcode.c -o bin/opcode.obj $(Include)

bin/strlib.obj: strlib.c
	$(CC) $(CFlags) -c strlib.c -o bin/strlib.obj $(Include)

bin/table.obj: table.c
	$(CC) $(CFlags) -c table.c -o bin/table.obj $(Include)

bin/y_tab.obj: y_tab.c
	$(CC) $(CFlags) -c y_tab.c -o bin/y_tab.obj $(Include)

bin/lib.obj: lib.c
	$(CC) $(CFlags) -c lib.c -o bin/lib.obj $(Include)

.PHONY:Clear

Clear:
	-rm -f $(Objs) $(Target)
