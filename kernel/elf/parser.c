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

	ctx->shdr = (Elf32_Shdr *)(ctx->file_content + ctx->header->e_shoff);
	ctx->itr_shdr = ctx->shdr;
	ctx->n_shdr = ctx->header->e_shnum;
	ctx->shstrtab = ctx->shdr + ctx->header->e_shstrndx;
	ctx->shstrtab_offset = ctx->file_content + ctx->shstrtab->sh_offset;
	ctx->strtab = NULL;
	ctx->dynstr = NULL;
	ctx->dynsym = NULL;

	ctx->phdr = (Elf32_Phdr *)(ctx->file_content + ctx->header->e_phoff);
	ctx->itr_phdr = ctx->phdr;
	ctx->n_phdr = ctx->header->e_phnum;

	return 1;
err:
	return 0;
}

static int _parse_sht(ELFContextPtr ctx)
{
	if(ctx == NULL)
		goto err;
	Elf32_Shdr * shdr = NULL;
	while((shdr = elf_next_shdr(ctx)) != NULL)
	{
		const char * name = "";
		if((ctx->shstrtab_offset + shdr->sh_name)[0] != '\0')
			name = ctx->shstrtab_offset + shdr->sh_name;
		if(strcmp(name, ".strtab") == 0)
		{
			ctx->strtab = shdr;
			ctx->strtab_offset = ctx->file_content + ctx->strtab->sh_offset;
		}
		else if(strcmp(name, ".dynstr") == 0)
		{
			ctx->dynstr = shdr;
			ctx->dynstr_offset = ctx->file_content + ctx->dynstr->sh_offset;
		}
		else if(strcmp(name, ".dynsym") == 0)
		{
			ctx->dynsym = shdr;
			elf_reset_dynsym(ctx);
		}
		else if(strcmp(name, ".rel.dyn") == 0)
		{
			ctx->rel_dyn = shdr;
			elf_reset_rel_dyn(ctx);
		}
		else if(strcmp(name, ".rel.plt") == 0)
		{
			ctx->rel_plt = shdr;
			elf_reset_rel_plt(ctx);
		}
	}
	elf_reset_shdr(ctx);
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
	if(!_parse_sht(ctx))
		goto err;
	return 1;
err:
	return 0;
}

int elf_parse(const char * path, ELFContextPtr ctx)
{
	// 初始化上下文。
	memset(ctx, 0, sizeof(ELFContext));

	if(path == NULL || ctx == NULL)
		goto err;
	FileObjectPtr foptr = Ifs1OpenFile((char *)path, FILE_MODE_READ);
	if(foptr == NULL)
		goto err;
	ctx->file_size = flen(foptr);
	ctx->file_content = (unsigned char *)alloc_memory(ctx->file_size);
	if(Ifs1ReadFile(foptr, ctx->file_content, ctx->file_size) != ctx->file_size)
		goto err;
	Ifs1CloseFile(foptr);
	if(!_parse(ctx))
		goto err;
	ctx->valid = 1;
	return 1;
err:
	if(foptr != NULL)
		Ifs1CloseFile(foptr);
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

void elf_reset_shdr(ELFContextPtr ctx)
{
	if(ctx == NULL)
		return;
	ctx->itr_shdr = ctx->shdr;
	ctx->n_shdr = ctx->header->e_shnum;
}

Elf32_Shdr * elf_next_shdr(ELFContextPtr ctx)
{
	if(ctx == NULL || ctx->n_shdr == 0)
		return NULL;
	ctx->n_shdr--;
	return ctx->itr_shdr++;
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

void elf_reset_dynsym(ELFContextPtr ctx)
{
	if(ctx == NULL)
		return;
	ctx->itr_dynsym_sym = (Elf32_Sym *)(ctx->file_content + ctx->dynsym->sh_offset);
	ctx->n_dynsym_sym = ctx->dynsym->sh_size / ctx->dynsym->sh_entsize;
}

Elf32_Sym * elf_next_dynsym_sym(ELFContextPtr ctx)
{
	if(ctx == NULL || ctx->n_dynsym_sym == 0)
		return NULL;
	ctx->n_dynsym_sym--;
	return ctx->itr_dynsym_sym++;
}

const char * elf_parse_dynsym_sym_name(	ELFContextPtr ctx,
										Elf32_Sym * sym)
{
	if(	ctx == NULL
		|| sym == NULL
		|| ctx->dynstr_offset == NULL)
		return NULL;
	return ctx->dynstr_offset + sym->st_name;
}

void elf_reset_rel_dyn(ELFContextPtr ctx)
{
	if(ctx == NULL)
		return;
	ctx->itr_rel_dyn = (Elf32_Rel *)(ctx->file_content + ctx->rel_dyn->sh_offset);
	ctx->n_rel_dyn = ctx->rel_dyn->sh_size / ctx->rel_dyn->sh_entsize;
}

Elf32_Rel * elf_next_rel_dyn(ELFContextPtr ctx)
{
	if(ctx == NULL || ctx->n_rel_dyn == 0)
		return NULL;
	ctx->n_rel_dyn--;
	return ctx->itr_rel_dyn++;
}

void elf_reset_rel_plt(ELFContextPtr ctx)
{
	if(ctx == NULL)
		return;
	ctx->itr_rel_plt = (Elf32_Rel *)(ctx->file_content + ctx->rel_plt->sh_offset);
	ctx->n_rel_plt = ctx->rel_plt->sh_size / ctx->rel_plt->sh_entsize;
}

Elf32_Rel * elf_next_rel_plt(ELFContextPtr ctx)
{
	if(ctx == NULL || ctx->n_rel_plt == 0)
		return NULL;
	ctx->n_rel_plt--;
	return ctx->itr_rel_plt++;
}
