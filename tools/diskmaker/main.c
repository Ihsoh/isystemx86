//Filename:		main.c
//Author:		Ihsoh
//Date:			2014-7-24
//Descriptor:	Main

#include "types.h"
#include "ifs1vdisk.h"
#include "ifs1disk.h"
#include "ifs1fs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define	BOOT_SIZE	512
#define	KLDR_SIZE	32768
#define	KERNEL_SIZE	524288

#define	BOOT_OFFSET		0
#define	KLDR_OFFSET		(BOOT_SIZE / 512)
#define	KERNEL_OFFSET	((BOOT_SIZE / 512) + (KLDR_SIZE / 512))

#define HD_NO_FEATURES     0x00000000 
#define HD_TEMPORARY       0x00000001 /* disk can be deleted on shutdown */ 
#define HD_RESERVED        0x00000002 /* NOTE: must always be set        */ 

#define HD_TYPE_NONE       0 
#define HD_TYPE_FIXED      2  /* fixed-allocation disk */ 
#define HD_TYPE_DYNAMIC    3  /* dynamic disk */ 
#define HD_TYPE_DIFF       4  /* differencing disk */ 

struct VHDFooter
{
	char cookie[8];
	uint features;
	uint ff_version;
	ullong data_offset;
	uint timestamp;
	char crtr_app[4];
	uint crtr_ver;
	uint crtr_os;
	ullong orig_size;
	ullong curr_size;
	uint geometry;
	uint type;
	uint checksum;
	uchar uuid[16];
	char saved;
	char hidden;
	char reserved[426];
} __attribute__((packed));

static int parse_cmd(char * cmd, char * word, uint n)
{
	static uint len = 0;
	static char * buffer = NULL;
	static uint pos = 0;
	if(cmd != NULL)
	{
		if(buffer != NULL)
			free_memory(buffer);
		len = strlen(cmd) + 1;
		buffer = (char *)alloc_memory(len);
		if(buffer == NULL)
			return 0;
		strcpy(buffer, cmd);
		pos = 0;
	}
	else
	{
		uint ui = 0;
		int state = 0;
		for(; pos < len && ui < n; pos++)
		{
			char chr = buffer[pos];
			//普通
			if(state == 0 && chr != ' ' && chr != '\t' && chr != '"' && chr != '\'')
			{
				word[ui++] = chr;
				state = 1;
			}
			else if(state == 1 && chr != ' ' && chr != '\t' && chr != '"')
				word[ui++] = chr;
			else if(state == 1 && (chr == ' ' || chr == '\t' || chr == '"'))
				break;
			//双引号字符串
			else if(state == 0 && chr == '"')
				state = 2;
			else if(state == 2 && chr == '\\' && pos + 1 < len && (buffer[pos + 1] == '"' || buffer[pos + 1] == '\\'))
				word[ui++] = buffer[++pos];
			else if(state == 2 && chr == '\\' && pos + 1 < len && buffer[pos + 1] == 'r')
			{
				word[ui++] = '\r';
				pos++;			
			}	
			else if(state == 2 && chr == '\\' && pos + 1 < len && buffer[pos + 1] == 'n')
			{
				word[ui++] = '\n';
				pos++;			
			}
			else if(state == 2 && chr == '\\' && pos + 1 < len && buffer[pos + 1] == 't')
			{
				word[ui++] = '\t';
				pos++;			
			}		
			else if(state == 2 && chr == '\\' && pos + 1 < len && buffer[pos + 1] != '"' && buffer[pos + 1] != '\\')
				word[ui++] = '\\';
			else if(state == 2 && chr != '\\' && chr != '"')
				word[ui++] = chr;
			else if(state == 2 && chr == '"')
			{
				pos++;
				break;
			}
			//单引号字符串
			else if(state == 0 && chr == '\'')
				state = 3;
			else if(state == 3 && chr == '\\' && pos + 1 < len && (buffer[pos + 1] == '\'' || buffer[pos + 1] == '\\'))
				word[ui++] = buffer[++pos];
			else if(state == 3 && chr == '\\' && pos + 1 < len && buffer[pos + 1] == 'r')
			{
				word[ui++] = '\r';
				pos++;			
			}	
			else if(state == 3 && chr == '\\' && pos + 1 < len && buffer[pos + 1] == 'n')
			{
				word[ui++] = '\n';
				pos++;			
			}
			else if(state == 3 && chr == '\\' && pos + 1 < len && buffer[pos + 1] == 't')
			{
				word[ui++] = '\t';
				pos++;			
			}
			else if(state == 3 && chr == '\\' && pos + 1 < len && buffer[pos + 1] != '\'' && buffer[pos + 1] != '\\')
				word[ui++] = '\\';
			else if(state == 3 && chr != '\\' && chr != '\'')
				word[ui++] = chr;
			else if(state == 3 && chr == '\'')
			{
				pos++;
				break;
			}
		}
		word[ui] = '\0';
	}
	return len - pos;
}

void die(const char * message)
{
	printf("\nDisk maker error: %s!\n", message);
	exit(-1);
}

uint get_file_size(char * filename)
{
	FILE * fileptr = fopen(filename, "rb");
	int len = 0;
	if(fileptr == NULL)
		die("Cannot open file");
	fgetc(fileptr);
	while(!feof(fileptr))
	{
		fgetc(fileptr);
		len++;
	}
	fclose(fileptr);
	return len;
}

void to_big_endian(uchar * data, uint size)
{
	uchar * buffer = (uchar *)malloc(size);
	if(buffer == NULL)
		die("Cannot allow memory");
	memcpy(buffer, data, size);
	int i;
	for(i = size - 1; i >= 0; i--)
		*(data++) = buffer[i];
	free(buffer);
}

void make_vhd(char * boot, char * kldr, char * kernel, char * image, char * script, char * disk_type)
{
	//Init VA
	init_disk("VA");

	//Format VA
	if(format_disk("VA"))
		printf("Format VA is OK\n");
	else
		die("Failed to format VA\n");

	uchar * bootbuf, * kldrbuf, * kernelbuf;
	uchar * scriptbuf;
	FILE * fptr;
	int i;
	uint script_len = get_file_size(script) + 1;
	bootbuf = (uchar *)malloc(BOOT_SIZE);
	kldrbuf = (uchar *)malloc(KLDR_SIZE);
	kernelbuf = (uchar *)malloc(KERNEL_SIZE);
	scriptbuf = (uchar *)malloc(script_len);
	scriptbuf[script_len - 1] = '\0';
	if(	bootbuf == NULL		||
		kldrbuf == NULL		||
		kernelbuf == NULL	||
		scriptbuf == NULL)
		die("Cannot allow memory");

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

	//Read script
	fptr = fopen(script, "rb");
	if(fptr == NULL)
		die("Cannot open script file");
	fread(scriptbuf, sizeof(uchar), script_len - 1, fptr);
	fclose(fptr);

	//Store boot to image
	if(!write_sector("VA", BOOT_OFFSET, bootbuf))
		die("Cannot store boot to image");

	//Store kernel loader to image
	if(!write_sectors("VA", KLDR_OFFSET, KLDR_SIZE / 512, kldrbuf))
		die("Cannot store kernel loader to image");

	//Store kernel to image
	if(	!write_sectors("VA", KERNEL_OFFSET + 256 * 0, 0, kernelbuf + 256 * 512 * 0)
		|| !write_sectors("VA", KERNEL_OFFSET + 256 * 1, 0, kernelbuf + 256 * 512 * 1)
		|| !write_sectors("VA", KERNEL_OFFSET + 256 * 2, 0, kernelbuf + 256 * 512 * 2)
		|| !write_sectors("VA", KERNEL_OFFSET + 256 * 3, 0, kernelbuf + 256 * 512 * 3))
		die("Cannot store kernel to image");

	//Parse script
	char chr;
	char cmd[1024];
	char * s = cmd;
	cmd[0] = '\0';
	while((chr = *(scriptbuf++)) != '\0')
	{
		if(chr == '\n')
		{
			if(cmd[0] == '#')
			{
				s = cmd;
				cmd[0] = '\0';
				continue;
			}
			char word[1024];
			parse_cmd(cmd, word, 1023);
			parse_cmd(NULL, word, 1023);
			if(strcmp(word, "MAKE_DIR") == 0)
			{
				char path[1024];
				char name[256];
				parse_cmd(NULL, path, 1023);
				parse_cmd(NULL, name, 255);
				printf("Make directory '%s' in '%s'\n", name, path);
				if(!create_dir(path, name))
					die("Cannot create directory");
			}
			else if(strcmp(word, "MAKE_FILE") == 0)
			{
				char path[1024];
				char name[256];
				parse_cmd(NULL, path, 1023);
				parse_cmd(NULL, name, 255);
				printf("Make file '%s' in '%s'\n", name, path);
				if(!create_file(path, name))
					die("Cannot create file");
			}
			else if(strcmp(word, "WRITE_FILE") == 0)
			{
				char dst[1024];
				char src[1024];
				parse_cmd(NULL, dst, 1023);
				parse_cmd(NULL, src, 1023);
				printf("Write file '%s' to '%s'\n", src, dst);
				uint data_len = get_file_size(src);
				uchar * data = (uchar *)malloc(data_len);
				if(data == NULL)
					die("Cannot allow memory");
				fptr = fopen(src, "rb");
				if(fptr == NULL)
					die("Cannot open file");
				fread(data, sizeof(uchar), data_len, fptr);
				fclose(fptr);
				IFS1_FILE * ifs1_fptr = IFS1_fopen(dst, FILE_MODE_WRITE);
				if(ifs1_fptr == NULL)
					die("Cannot write file");
				IFS1_fwrite(ifs1_fptr, data, data_len);
				IFS1_fclose(ifs1_fptr);
				free(data);
			}
			else if(strcmp(word, "") == 0)
				;
			else
				die("Invalid command!\n");
			s = cmd;
			cmd[0] = '\0';
		}
		else if(chr == '\r')
			;
		else
		{
			*(s++) = chr;
			*s = '\0';
		}
	}

	//Store image to file
	fptr = fopen(image, "wb");
	if(fptr == NULL)
		die("Cannot open image file");
	fwrite(get_vdiska(), sizeof(uchar), VDISK_BUFFER_SIZE, fptr);
	fclose(fptr);

	if(strcmp(disk_type, "vhd") == 0)
	{
		struct VHDFooter vhd_footer;
		memcpy(vhd_footer.cookie, "conectix", sizeof(vhd_footer.cookie));
		vhd_footer.features = HD_RESERVED;
		to_big_endian((uchar *)&(vhd_footer.features), sizeof(vhd_footer.features));
		vhd_footer.ff_version = 0x00000100;
		vhd_footer.data_offset = 0xffffffff;
		vhd_footer.timestamp = 0;
		memcpy(vhd_footer.crtr_app, "vbox", sizeof(vhd_footer.crtr_app));
		vhd_footer.crtr_ver = 0x01000400;
		vhd_footer.crtr_os = 0x6b326957;
		vhd_footer.orig_size = VDISK_BUFFER_SIZE;
		to_big_endian((uchar *)&(vhd_footer.orig_size), sizeof(vhd_footer.orig_size));
		vhd_footer.curr_size = VDISK_BUFFER_SIZE;
		to_big_endian((uchar *)&(vhd_footer.curr_size), sizeof(vhd_footer.curr_size));
		vhd_footer.geometry = (VDISK_CLY << 16) | (VDISK_HEAD << 8) | VDISK_SPT;
		to_big_endian((uchar *)&(vhd_footer.geometry), sizeof(vhd_footer.geometry));
		vhd_footer.type = HD_TYPE_FIXED;
		to_big_endian((uchar *)&(vhd_footer.type), sizeof(vhd_footer.type));
	
		uchar uuid[16] = {0xee, 0xc3, 0xee, 0xea, 0x76, 0xb5, 0x4f, 0x5a, 0xbf, 0x41, 0x22, 0x6c, 0x53, 0x44, 0xbb, 0xd0};
					   //{0x9f, 0x27, 0xfd, 0x73, 0x1e, 0xdb, 0xc4, 0x4b, 0xb9, 0x15, 0x90, 0x98, 0x69, 0xc6, 0x5a, 0x30};
		memcpy(vhd_footer.uuid, uuid, sizeof(vhd_footer.uuid));
	
		vhd_footer.saved = 0;
		vhd_footer.hidden = 0;
		memset(vhd_footer.reserved, 0, sizeof(vhd_footer.reserved));
		uchar * vhd_footer_ptr = (uchar *)&vhd_footer;
		uint checksum = 0;
		for(i = 0; i < sizeof(vhd_footer); i++)
			checksum += vhd_footer_ptr[i];
		vhd_footer.checksum = ~checksum;
		to_big_endian((uchar *)&(vhd_footer.checksum), sizeof(vhd_footer.checksum));
	
		fptr = fopen(image, "ab");
		fwrite(vhd_footer_ptr, sizeof(uchar), sizeof(struct VHDFooter), fptr);
		fclose(fptr);
	}

	free(bootbuf);
	free(kldrbuf);
	free(kernelbuf);
}

void add_file_to_vhd(char * hdfile, char * type, char * path, char * name, char * file)
{
	//Init VA
	init_disk("VA");

	//Format VA
	if(format_disk("VA"))
		printf("Format VA is OK\n");
	else
		die("Failed to format VA");
	
	uchar * diska = get_vdiska();
	struct VHDFooter vhd_footer;
	FILE * fptr;
	fptr = fopen(vhd, "rb");
	if(fptr == NULL)
		die("Cannot open hard disk file");
	fread(diska, sizeof(uchar), VDISK_BUFFER_SIZE, fptr);
	if(strcmp(type, "vhd") == 0)
		fread(&vhd_footer, sizeof(struct VHDFooter), 1, fptr);
	fclose(fptr);

	if(!create_file(path, name))
		die("Cannot create file");

	

	fptr = fopen(vhd, "wb");
	if(fptr == NULL)
		die("Cannot open hard disk file");
	fwrite(diska, sizeof(uchar), VDISK_BUFFER_SIZE, fptr);
	fclose(fptr);

	if(strcmp(type, "vhd") == 0)
	{
		fptr = fopen(vhd, "ab");
		if(fptr == NULL)
			die("Cannot open hard disk file");
		fwrite(&vhd_footer, sizeof(struct VHDFooter), 1, fptr);
		fclose(fptr);
	}
}

int main(int argc, char * argv[])
{
	if(argc < 2)
		die("Parameter error!");
	char * option = argv[1];

	if(strcmp(option, "-m") == 0)
	{
		if(argc != 8)
			die("Parameter error");
		char * boot, * kldr, * kernel, * image, * script, * disk_type;
		boot = argv[2];
		kldr = argv[3];
		kernel = argv[4];
		image = argv[5];
		script = argv[6];
		disk_type = argv[7];
		if(strcmp(disk_type, "vhd") != 0 && strcmp(disk_type, "flat") == 0)
			die("Invalid disk type");
		make_vhd(boot, kldr, kernel, image, script, disk_type);
	}
	else if(strcmp(option, "-i") == 0)
	{
		if(argc != 7)
			die("Parameter error!");
		char * hdfile;
		char * type;
		char * path;
		char * name;
		char * file;
		hdfile = argv[2];
		type = argv[3];
		path = argv[4];
		name = argv[5];
		file = argv[6];
		if(strcmp(type, "vhd") != 0 && strcmp(type, "flat") == 0)
			die("Invalid disk type");
		add_file_to_vhd(hdfile, path, name, file);
	}

	return 0;
}
