#Filename:		makefile
#Author:		Ihsoh
#Date:			2014-12-8
#Descriptor:	Application maker

TargetDir=bin
Out=appmaker
Target=$(TargetDir)/$(Out)
CC=gcc
CFlags=-o2
Objs=bin/appmaker.obj

$(Target): bin $(Objs)
	$(CC) $(CFlags) $(Objs) -o $(Target)

bin:
	-mkdir bin

bin/appmaker.obj: appmaker.c
	$(CC) $(CFlags) -c appmaker.c -o bin/appmaker.obj

.PHONY:Clear

Clear:
	-rm $(Objs) $(Target)
