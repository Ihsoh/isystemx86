#Filename:		makefile
#Author:		Ihsoh
#Date:			2014-12-8
#Descriptor:	Application maker

TargetDir=bin
Out=sysmaker
Target=$(TargetDir)/$(Out)
CC=gcc
CFlags=-o2
Objs=bin/sysmaker.obj

$(Target): bin $(Objs)
	$(CC) $(CFlags) $(Objs) -o $(Target)

bin:
	-mkdir bin

bin/sysmaker.obj: sysmaker.c
	$(CC) $(CFlags) -c sysmaker.c -o bin/sysmaker.obj

.PHONY:Clear

Clear:
	-rm $(Objs) $(Target)
