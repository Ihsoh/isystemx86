#Filename:		makefile
#Author:		Ihsoh
#Date:			2014-1-25
#Descriptor:	Make kernel loader

all: bin kernelldr.asm
	nasm -f bin kernelldr.asm -o bin/kernelldrtm.bin
	nasm -f bin kernelldr.asm -o bin/kernelldrgm_320_200.bin -DVESA -DSIZE_320_200
	nasm -f bin kernelldr.asm -o bin/kernelldrgm_640_400.bin -DVESA -DSIZE_640_400
	nasm -f bin kernelldr.asm -o bin/kernelldrgm_640_480.bin -DVESA -DSIZE_640_480
	nasm -f bin kernelldr.asm -o bin/kernelldrgm_800_500.bin -DVESA -DSIZE_800_500
	nasm -f bin kernelldr.asm -o bin/kernelldrgm_800_600.bin -DVESA -DSIZE_800_600
	nasm -f bin kernelldr.asm -o bin/kernelldrgm_896_672.bin -DVESA -DSIZE_896_672
	nasm -f bin kernelldr.asm -o bin/kernelldrgm_1024_640.bin -DVESA -DSIZE_1024_640
	nasm -f bin kernelldr.asm -o bin/kernelldrgm_1024_768.bin -DVESA -DSIZE_1024_768
	nasm -f bin kernelldr.asm -o bin/kernelldrgm_1152_720.bin -DVESA -DSIZE_1152_720
	nasm -f bin kernelldr.asm -o bin/kernelldrgm_1280_1024.bin -DVESA -DSIZE_1280_1024
	nasm -f bin kernelldr.asm -o bin/kernelldrgm_1440_900.bin -DVESA -DSIZE_1440_900
	nasm -f bin kernelldr.asm -o bin/kernelldrgm_1600_1200.bin -DVESA -DSIZE_1600_1200

bin:
	-mkdir bin

.PHONY:clean

clean:
	-rm bin/*
