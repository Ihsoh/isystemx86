TargetDir=bin
Out=lua3.bin
Out1=lua3c.bin
Target=$(TargetDir)/$(Out)
Target1=$(TargetDir)/$(Out1)
CC=gcc
CFlags=-O0 -fno-stack-protector -nostdinc -m32 -march=i386 -mtune=i386

Objs0=bin/lapi.obj bin/lbuffer.obj bin/lbuiltin.obj bin/ldo.obj bin/lfunc.obj	\
bin/lgc.obj bin/llex.obj bin/lmem.obj bin/lobject.obj bin/lparser.obj bin/lstate.obj	\
bin/lstring.obj bin/ltable.obj bin/ltm.obj bin/lundump.obj bin/lvm.obj bin/lzio.obj

Objs=$(Objs0) bin/lua.obj bin/lauxlib.obj bin/ldblib.obj bin/liolib.obj bin/lmathlib.obj	\
bin/lstrlib.obj bin/linit.obj

Objs2=$(Objs0) bin/luac.obj bin/dump.obj bin/opcode.obj bin/opt.obj bin/print.obj	\
bin/test.obj bin/lauxlib.obj


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

#
#	Objs0
#
bin/lapi.obj: lapi.c
	$(CC) $(CFlags) -c lapi.c -o bin/lapi.obj $(Include)

bin/lbuffer.obj: lbuffer.c
	$(CC) $(CFlags) -c lbuffer.c -o bin/lbuffer.obj $(Include)

bin/lbuiltin.obj: lbuiltin.c
	$(CC) $(CFlags) -c lbuiltin.c -o bin/lbuiltin.obj $(Include)

bin/ldo.obj: ldo.c
	$(CC) $(CFlags) -c ldo.c -o bin/ldo.obj $(Include)

bin/lfunc.obj: lfunc.c
	$(CC) $(CFlags) -c lfunc.c -o bin/lfunc.obj $(Include)

bin/lgc.obj: lgc.c
	$(CC) $(CFlags) -c lgc.c -o bin/lgc.obj $(Include)

bin/llex.obj: llex.c
	$(CC) $(CFlags) -c llex.c -o bin/llex.obj $(Include)

bin/lmem.obj: lmem.c
	$(CC) $(CFlags) -c lmem.c -o bin/lmem.obj $(Include)

bin/lobject.obj: lobject.c
	$(CC) $(CFlags) -c lobject.c -o bin/lobject.obj $(Include)

bin/lparser.obj: lparser.c
	$(CC) $(CFlags) -c lparser.c -o bin/lparser.obj $(Include)

bin/lstate.obj: lstate.c
	$(CC) $(CFlags) -c lstate.c -o bin/lstate.obj $(Include)

bin/lstring.obj: lstring.c
	$(CC) $(CFlags) -c lstring.c -o bin/lstring.obj $(Include)

bin/ltable.obj: ltable.c
	$(CC) $(CFlags) -c ltable.c -o bin/ltable.obj $(Include)

bin/ltm.obj: ltm.c
	$(CC) $(CFlags) -c ltm.c -o bin/ltm.obj $(Include)

bin/lundump.obj: lundump.c
	$(CC) $(CFlags) -c lundump.c -o bin/lundump.obj $(Include)

bin/lvm.obj: lvm.c
	$(CC) $(CFlags) -c lvm.c -o bin/lvm.obj $(Include)

bin/lzio.obj: lzio.c
	$(CC) $(CFlags) -c lzio.c -o bin/lzio.obj $(Include)

#
#	Objs
#
bin/lua.obj: lua.c
	$(CC) $(CFlags) -c lua.c -o bin/lua.obj $(Include)

bin/lauxlib.obj: lauxlib.c
	$(CC) $(CFlags) -c lauxlib.c -o bin/lauxlib.obj $(Include)

bin/ldblib.obj: ldblib.c
	$(CC) $(CFlags) -c ldblib.c -o bin/ldblib.obj $(Include)

bin/liolib.obj: liolib.c
	$(CC) $(CFlags) -c liolib.c -o bin/liolib.obj $(Include)

bin/lmathlib.obj: lmathlib.c
	$(CC) $(CFlags) -c lmathlib.c -o bin/lmathlib.obj $(Include)

bin/lstrlib.obj: lstrlib.c
	$(CC) $(CFlags) -c lstrlib.c -o bin/lstrlib.obj $(Include)

bin/linit.obj: linit.c
	$(CC) $(CFlags) -c linit.c -o bin/linit.obj $(Include)

#
#	Objs2
#
bin/luac.obj: luac.c
	$(CC) $(CFlags) -c luac.c -o bin/luac.obj $(Include)

bin/dump.obj: dump.c
	$(CC) $(CFlags) -c dump.c -o bin/dump.obj $(Include)

bin/opcode.obj: opcode.c
	$(CC) $(CFlags) -c opcode.c -o bin/opcode.obj $(Include)

bin/opt.obj: opt.c
	$(CC) $(CFlags) -c opt.c -o bin/opt.obj $(Include)

bin/print.obj: print.c
	$(CC) $(CFlags) -c print.c -o bin/print.obj $(Include)

bin/test.obj: test.c
	$(CC) $(CFlags) -c test.c -o bin/test.obj $(Include)

bin/lauxlib.obj: lauxlib.c
	$(CC) $(CFlags) -c lauxlib.c -o bin/lauxlib.obj $(Include)

.PHONY:clean

clean:
	-rm -f $(TargetDir)/*
