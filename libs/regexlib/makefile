# TargetDir=bin
# Out=libregexlib.a
Target=$(TargetDir)/$(Out)
CC=gcc
CFlags=-O0 -fno-stack-protector -nostdinc -m32 -march=i386 -mtune=i386 -std=c99 $(CFlagsExt)
Objs=$(TargetDir)/reg_error.obj $(TargetDir)/reg_list.obj $(TargetDir)/reg_malloc.obj $(TargetDir)/reg_parse.obj 	\
$(TargetDir)/reg_state.obj $(TargetDir)/reg_stream.obj $(TargetDir)/regex.obj $(TargetDir)/state_gen.obj $(TargetDir)/state_match.obj

AR=ar
Include=-I../

$(Target): $(TargetDir) $(Objs)
	$(AR) -rc $(Target) $(Objs)

$(TargetDir):
	-mkdir $(TargetDir)

$(TargetDir)/reg_error.obj: reg_error.c reg_error.h
	$(CC) $(CFlags) -c reg_error.c -o $(TargetDir)/reg_error.obj $(Include)

$(TargetDir)/reg_list.obj: reg_list.c reg_list.h
	$(CC) $(CFlags) -c reg_list.c -o $(TargetDir)/reg_list.obj $(Include)

$(TargetDir)/reg_malloc.obj: reg_malloc.c reg_malloc.h
	$(CC) $(CFlags) -c reg_malloc.c -o $(TargetDir)/reg_malloc.obj $(Include)

$(TargetDir)/reg_parse.obj: reg_parse.c reg_parse.h
	$(CC) $(CFlags) -c reg_parse.c -o $(TargetDir)/reg_parse.obj $(Include)

$(TargetDir)/reg_state.obj: reg_state.c reg_state.h
	$(CC) $(CFlags) -c reg_state.c -o $(TargetDir)/reg_state.obj $(Include)

$(TargetDir)/reg_stream.obj: reg_stream.c reg_stream.h
	$(CC) $(CFlags) -c reg_stream.c -o $(TargetDir)/reg_stream.obj $(Include)

$(TargetDir)/regex.obj: regex.c regex.h
	$(CC) $(CFlags) -c regex.c -o $(TargetDir)/regex.obj $(Include)

$(TargetDir)/state_gen.obj: state_gen.c
	$(CC) $(CFlags) -c state_gen.c -o $(TargetDir)/state_gen.obj $(Include)

$(TargetDir)/state_match.obj: state_match.c
	$(CC) $(CFlags) -c state_match.c -o $(TargetDir)/state_match.obj $(Include)

.PHONY:clean

clean:
	-rm -f $(Target) $(Objs)
