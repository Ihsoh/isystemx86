#Filename:		makefile
#Author:		Ihsoh
#Date:			2014-1-25
#Descriptor:	Make kernel loader

TargetDir=bin
TMOut=kernelldrtm.bin
TMTarget=$(TargetDir)/$(TMOut)
GMOut_640_480=kernelldrgm_640_480.bin
GMTarget_640_480=$(TargetDir)/$(GMOut_640_480)
GMOut_800_600=kernelldrgm_800_600.bin
GMTarget_800_600=$(TargetDir)/$(GMOut_800_600)
GMOut_1024_768=kernelldrgm_1024_768.bin
GMTarget_1024_768=$(TargetDir)/$(GMOut_1024_768)
GMOut_1280_1024=kernelldrgm_1280_1024.bin
GMTarget_1280_1024=$(TargetDir)/$(GMOut_1280_1024)
ASM=nasm

$(TMTarget): kernelldr.asm
	nasm -f bin kernelldr.asm -o $(TMTarget)
	nasm -f bin kernelldr.asm -o $(GMTarget_640_480) -DVESA -DSIZE_640_480
	nasm -f bin kernelldr.asm -o $(GMTarget_800_600) -DVESA -DSIZE_800_600
	nasm -f bin kernelldr.asm -o $(GMTarget_1024_768) -DVESA -DSIZE_1024_768
	nasm -f bin kernelldr.asm -o $(GMTarget_1280_1024) -DVESA -DSIZE_1280_1024

.PHONY:Clear

Clear:
	-rm -f $(TargetDir)/$(TMOut) $(TargetDir)/$(GMOut)

