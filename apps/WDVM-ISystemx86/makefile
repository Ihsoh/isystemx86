TargetDir=bin
Out=wdvm.bin
Target=$(TargetDir)/$(Out)
CC=gcc
CFlags=-O0 -fno-stack-protector -nostdinc -m32 -march=i386 -mtune=i386
Objs=bin/main.obj bin/BinaryHelper.obj bin/ParameterInfo.obj bin/CPU.obj bin/CPU_Registers.obj bin/CPU_Instructions.obj 	\
bin/Machine.obj bin/InterruptInfo.obj bin/Memory.obj bin/InstructionInfo.obj
Linker=ld -m elf_i386
Include=-I../../libs
AppMaker=../../tools/appmaker/bin/appmaker
MTA32Obj=../../libs/ilib/bin/mta32.obj

$(Target): bin $(Objs)
	$(Linker) -Tsegs.lds -e mta32main --oformat binary -o $(Target) $(MTA32Obj) $(Objs)	\
-L../../libs/ilib/bin -lilib \
-L../../libs/dslib/bin -ldslib	\
-L../../libs/mempoollib/bin -lmempoollib
	$(AppMaker) $(Target)

bin:
	-mkdir bin

bin/BinaryHelper.obj: BinaryHelper.c
	$(CC) $(CFlags) -c BinaryHelper.c -o bin/BinaryHelper.obj $(Include)

bin/ParameterInfo.obj: ParameterInfo.c
	$(CC) $(CFlags) -c ParameterInfo.c -o bin/ParameterInfo.obj $(Include)

bin/InstructionInfo.obj: InstructionInfo.c
	$(CC) $(CFlags) -c InstructionInfo.c -o bin/InstructionInfo.obj $(Include)

bin/Memory.obj: Memory.c
	$(CC) $(CFlags) -c Memory.c -o bin/Memory.obj $(Include)

bin/InterruptInfo.obj: InterruptInfo.c
	$(CC) $(CFlags) -c InterruptInfo.c -o bin/InterruptInfo.obj $(Include)

bin/CPU.obj: CPU.c
	$(CC) $(CFlags) -c CPU.c -o bin/CPU.obj $(Include)

bin/CPU_Registers.obj: CPU_Registers.c
	$(CC) $(CFlags) -c CPU_Registers.c -o bin/CPU_Registers.obj $(Include)

bin/CPU_Instructions.obj: CPU_Instructions.c
	$(CC) $(CFlags) -c CPU_Instructions.c -o bin/CPU_Instructions.obj $(Include)

bin/Machine.obj: Machine.c
	$(CC) $(CFlags) -c Machine.c -o bin/Machine.obj $(Include)

bin/main.obj: main.c
	$(CC) $(CFlags) -c main.c -o bin/main.obj $(Include)

.PHONY:clean

clean:
	-rm -f $(Objs) $(Target)
