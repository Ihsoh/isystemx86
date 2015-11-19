# TargetDir=bin
# Out=libisys.a
Target=$(TargetDir)/$(Out)
CC=gcc
CFlags=-O0 -fno-stack-protector -nostdinc -m32 -march=i386 -mtune=i386 $(CFlagsExt)
Objs=$(TargetDir)/console.obj $(TargetDir)/fs.obj $(TargetDir)/keyboard.obj $(TargetDir)/mouse.obj $(TargetDir)/screen.obj \
$(TargetDir)/system.obj $(TargetDir)/window.obj
AR=ar
Include=-I../

$(Target): $(TargetDir) $(Objs)
	$(AR) -rc $(Target) $(Objs)

$(TargetDir):
	-mkdir $(TargetDir)

$(TargetDir)/console.obj: console.c console.h
	$(CC) $(CFlags) -c console.c -o $(TargetDir)/console.obj $(Include)

$(TargetDir)/fs.obj: fs.c fs.h
	$(CC) $(CFlags) -c fs.c -o $(TargetDir)/fs.obj $(Include)

$(TargetDir)/keyboard.obj: keyboard.c keyboard.h
	$(CC) $(CFlags) -c keyboard.c -o $(TargetDir)/keyboard.obj $(Include)

$(TargetDir)/mouse.obj: mouse.c mouse.h
	$(CC) $(CFlags) -c mouse.c -o $(TargetDir)/mouse.obj $(Include)

$(TargetDir)/screen.obj: screen.c screen.h
	$(CC) $(CFlags) -c screen.c -o $(TargetDir)/screen.obj $(Include)

$(TargetDir)/system.obj: system.c system.h
	$(CC) $(CFlags) -c system.c -o $(TargetDir)/system.obj $(Include)

$(TargetDir)/window.obj: window.c window.h
	$(CC) $(CFlags) -c window.c -o $(TargetDir)/window.obj $(Include)

.PHONY:clean

clean:
	-rm -f $(Target) $(Objs)
