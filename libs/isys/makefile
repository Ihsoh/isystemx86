TargetDir=bin
Out=libisys.a
Target=$(TargetDir)/$(Out)
CC=gcc
CFlags=-O0 -fno-stack-protector -nostdinc -m32 -march=i386 -mtune=i386
Objs=bin/console.obj bin/fs.obj bin/keyboard.obj bin/mouse.obj bin/screen.obj \
bin/system.obj bin/window.obj
AR=ar

$(Target): bin $(Objs)
	$(AR) -rc $(Target) $(Objs)

bin:
	-mkdir bin

bin/console.obj: console.c console.h
	$(CC) $(CFlags) -c console.c -o bin/console.obj

bin/fs.obj: fs.c fs.h
	$(CC) $(CFlags) -c fs.c -o bin/fs.obj

bin/keyboard.obj: keyboard.c keyboard.h
	$(CC) $(CFlags) -c keyboard.c -o bin/keyboard.obj

bin/mouse.obj: mouse.c mouse.h
	$(CC) $(CFlags) -c mouse.c -o bin/mouse.obj

bin/screen.obj: screen.c screen.h
	$(CC) $(CFlags) -c screen.c -o bin/screen.obj

bin/system.obj: system.c system.h
	$(CC) $(CFlags) -c system.c -o bin/system.obj

bin/window.obj: window.c window.h
	$(CC) $(CFlags) -c window.c -o bin/window.obj

.PHONY:clean

clean:
	-rm -f $(Target) $(Objs)
