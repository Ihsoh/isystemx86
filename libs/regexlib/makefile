TargetDir=bin
Out=libregexlib.a
Target=$(TargetDir)/$(Out)
CC=gcc
CFlags=-O0 -fno-stack-protector -nostdinc -m32 -march=i386 -mtune=i386 -std=c99
Objs=bin/reg_error.obj bin/reg_list.obj bin/reg_malloc.obj bin/reg_parse.obj 	\
bin/reg_state.obj bin/reg_stream.obj bin/regex.obj bin/state_gen.obj bin/state_match.obj

AR=ar
Include=-I../

$(Target): bin $(Objs)
	$(AR) -rc $(Target) $(Objs)

bin:
	-mkdir bin

bin/reg_error.obj: reg_error.c reg_error.h
	$(CC) $(CFlags) -c reg_error.c -o bin/reg_error.obj $(Include)

bin/reg_list.obj: reg_list.c reg_list.h
	$(CC) $(CFlags) -c reg_list.c -o bin/reg_list.obj $(Include)

bin/reg_malloc.obj: reg_malloc.c reg_malloc.h
	$(CC) $(CFlags) -c reg_malloc.c -o bin/reg_malloc.obj $(Include)

bin/reg_parse.obj: reg_parse.c reg_parse.h
	$(CC) $(CFlags) -c reg_parse.c -o bin/reg_parse.obj $(Include)

bin/reg_state.obj: reg_state.c reg_state.h
	$(CC) $(CFlags) -c reg_state.c -o bin/reg_state.obj $(Include)

bin/reg_stream.obj: reg_stream.c reg_stream.h
	$(CC) $(CFlags) -c reg_stream.c -o bin/reg_stream.obj $(Include)

bin/regex.obj: regex.c regex.h
	$(CC) $(CFlags) -c regex.c -o bin/regex.obj $(Include)

bin/state_gen.obj: state_gen.c
	$(CC) $(CFlags) -c state_gen.c -o bin/state_gen.obj $(Include)

bin/state_match.obj: state_match.c
	$(CC) $(CFlags) -c state_match.c -o bin/state_match.obj $(Include)

.PHONY:clean

clean:
	-rm -f $(Target) $(Objs)
