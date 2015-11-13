#include "parser.h"
#include "types.h"
#include "elf.h"

#include "../memory.h"
#include "../fs/ifs1/fs.h"

static int _parse_header(ELFContextPtr ctx)
{
	if(ctx == NULL)
		goto err;
	if(ctx->file_size < sizeof(Elf32_Ehdr))
		goto err;
	ctx->header = (Elf32_Ehdr *)ctx->file_content;
	// 检查文件MAGIC。
	if(	ctx->header->e_ident[EI_MAG0] != ELFMAG0
		|| ctx->header->e_ident[EI_MAG1] != ELFMAG1
		|| ctx->header->e_ident[EI_MAG2] != ELFMAG2
		|| ctx->header->e_ident[EI_MAG3] != ELFMAG3)
		goto err;

	// 检查ELF是否为32位。
	if(ctx->header->e_ident[EI_CLASS] != ELFCLASS32)
		goto err;

	ctx->phdr = (Elf32_Phdr *)(ctx->file_content + ctx->header->e_phoff);
	ctx->itr_phdr = ctx->phdr;
	ctx->n_phdr = ctx->header->e_phnum;

	return 1;
err:
	return 0;
}


static int _parse(ELFContextPtr ctx)
{
	if(ctx == NULL)
		goto err;
	if(!_parse_header(ctx))
		goto err;
	return 1;
err:
	return 0;
}

int elf_parse(const char * path, ELFContextPtr ctx)
{
	// 初始化上下文。
	ctx->valid = 0;
	ctx->file_size = 0;
	ctx->file_content = NULL;
	ctx->header = NULL;

	if(path == NULL || ctx == NULL)
		goto err;
	FileObjectPtr foptr = open_file((char *)path, FILE_MODE_READ);
	if(foptr == NULL)
		goto err;
	ctx->file_size = flen(foptr);
	ctx->file_content = (unsigned char *)alloc_memory(ctx->file_size);
	if(read_file(foptr, ctx->file_content, ctx->file_size) != ctx->file_size)
		goto err;
	close_file(foptr);
	if(!_parse(ctx))
		goto err;
	ctx->valid = 1;
	return 1;
err:
	if(foptr != NULL)
		close_file(foptr);
	if(ctx->file_content != NULL)
		free_memory(ctx->file_content);
	return 0;
}

void elf_free(ELFContextPtr ctx)
{
	if(ctx == NULL)
		return;
	if(ctx->file_content != NULL)
		free_memory(ctx->file_content);
}

void elf_reset_phdr(ELFContextPtr ctx)
{
	if(ctx == NULL)
		return;
	ctx->itr_phdr = ctx->phdr;
	ctx->n_phdr = ctx->header->e_phnum;
}

Elf32_Phdr * elf_next_phdr(ELFContextPtr ctx)
{
	if(ctx == NULL || ctx->n_phdr == 0)
		return NULL;
	ctx->n_phdr--;
	return ctx->itr_phdr++;
}
