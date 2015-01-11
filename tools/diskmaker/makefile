#Filename:	makefile
#Author:	Ihsoh
#Date:		2014-7-24
#Descriptor:Make diskmaker

TargetDir=bin
Out=diskmaker
Target=$(TargetDir)/$(Out)
CC=gcc
CFlags=-o2
Request= main.c memory.c memory.h types.h ifs1blocks.c ifs1blocks.h ifs1disk.c ifs1disk.h \
ifs1vdisk.c cmos.c cmos.h ifs1fs.c ifs1fs.h

$(Target): bin $(Request)
	$(CC) $(CFlags) main.c memory.c ifs1blocks.c ifs1disk.c ifs1vdisk.c cmos.c ifs1fs.c -o $(Target)

bin:
	-mkdir bin

.PHONY:Clear

Clear:
	-rm $(TargetDir)/*
