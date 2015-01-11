//Filename:		floppymaker.c
//Author:		Ihsoh
//Date:			2014-1-25
//Descriptor:	Floppy maker

#include "floppymaker.h"

void die(const char * message)
{
	printf("\nFloppy maker error: %s!\n", message);
	exit(-1);
}

//floppymaker [boot] [kernel loader] [kernel] [image]
int main(int argc, char * argv[])
{
	char * boot, * kldr, * kernel, * image;
	uchar * bootbuf, * kldrbuf, * kernelbuf;
	uchar * imgbuf;
	FILE * fptr;
	int i;	

	if(argc != 5)
		die("Parameter error");
	boot = argv[1];
	kldr = argv[2];
	kernel = argv[3];
	image = argv[4];
	imgbuf = (uchar *)malloc(FLOPPY_SIZE);
	bootbuf = (uchar *)malloc(BOOT_SIZE);
	kldrbuf = (uchar *)malloc(KLDR_SIZE);
	kernelbuf = (uchar *)malloc(KERNEL_SIZE);
	if(	imgbuf == NULL		||
		bootbuf == NULL		||
		kldrbuf == NULL		||
		kernelbuf == NULL)
		die("Cannot allow memory");
	for(i = 0; i < FLOPPY_SIZE; i++)
		*(imgbuf + i) = 0;

	//Read boot
	fptr = fopen(boot, "rb");
	if(fptr == NULL)
		die("Cannot open boot file");
	fread(bootbuf, sizeof(uchar), BOOT_SIZE, fptr);
	fclose(fptr);

	//Read kernel loader
	fptr = fopen(kldr, "rb");
	if(fptr == NULL)
		die("Cannot open kernel loader file");
	fread(kldrbuf, sizeof(uchar), KLDR_SIZE, fptr);
	fclose(fptr);

	//Read kernel
	fptr = fopen(kernel, "rb");
	if(fptr == NULL)
		die("Cannot open kernel file");
	fread(kernelbuf, sizeof(uchar), KERNEL_SIZE, fptr);
	fclose(fptr);

	//Store boot to image
	for(i = 0; i < BOOT_SIZE; i++)
		*(imgbuf + BOOT_OFFSET + i) = *(bootbuf + i);

	//Store kernel loader to image
	for(i = 0; i < KLDR_SIZE; i++)
		*(imgbuf + KLDR_OFFSET + i) = *(kldrbuf + i);

	//Store kernel to image
	for(i = 0; i < KERNEL_SIZE; i++)
		*(imgbuf + KERNEL_OFFSET + i) = *(kernelbuf + i);

	//Store image to file
	fptr = fopen(image, "wb");
	if(fptr == NULL)
		die("Cannot open image file");
	fwrite(imgbuf, sizeof(uchar), FLOPPY_SIZE, fptr);
	fclose(fptr);

	free(bootbuf);
	free(kldrbuf);
	free(kernelbuf);
	free(imgbuf);

	return 0;
}

