#Filename:		makefile
#Author:		Ihsoh
#Date:			2014-1-25
#Descriptor:	Make floppy maker

TargetDir=bin
Out=floppymaker
Target=$(TargetDir)/$(Out)
CC=gcc
CFlags=-o2
Objs=bin/floppymaker.obj

$(Target): bin $(Objs)
	$(CC) $(CFlags) $(Objs) -o $(Target)

bin:
	-mkdir bin

bin/floppymaker.obj: floppymaker.c floppymaker.h
	$(CC) $(CFlags) -c floppymaker.c -o bin/floppymaker.obj

.PHONY:clean

clean:
	-rm $(Objs) $(Target)

