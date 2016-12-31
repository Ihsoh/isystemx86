/**
	@File:			so.c
	@Author:		Ihsoh
	@Date:			2017-01-01
	@Description:
		内核加载Elf Shared Object的功能。
*/

#include "so.h"
#include "memory.h"

#include "elf/parser.h"

#include "utils/sfstr.h"

#define	_MAX_COUNT	32

static ELFContextPtr _ctx[_MAX_COUNT];

/**
	@Function:		KnlInitElfSharedObject
	@Access:		Public
	@Description:
		初始化内核加载Elf Shared Object的功能。
	@Parameters:
	@Return:
*/
void
KnlInitElfSharedObject(void)
{
	int32 i;
	for (i = 0; i < _MAX_COUNT; i++)
	{
		_ctx[i] = NULL;
	}
}

/**
	@Function:		KnlLoadElfSharedObjectFile
	@Access:		Public
	@Description:
		把ELF Shared Object文件加载进内核空间。
	@Parameters:
		path, CASCTEXT, IN
			ELF Shared Object文件路径。
	@Return:
		uint32
			ELF Shared Object上下文索引。
			通过该索引可以获取ELF Shared Object的符号在内核空间的地址。
			当加载失败时返回0xffffffff。
*/
uint32
KnlLoadElfSharedObjectFile(
	IN CASCTEXT path)
{
	uint8 * elf_knl = NULL;
	uint32 ctx_idx = 0xffffffff;
	ELFContextPtr ctx = NULL;
	if(path == NULL)
	{
		goto err;	
	}

	// 获取可用的上下文槽。
	for(ctx_idx = 0; ctx_idx < _MAX_COUNT; ctx_idx++)
	{
		if(_ctx[ctx_idx] == NULL)
		{
			break;
		}
	}
	if(ctx_idx >= _MAX_COUNT)
	{
		goto err;
	}

	// 新建上下文，并且添加到可用的上下文槽中。
	ctx = NEW(ELFContext);
	if(ctx == NULL)
	{
		goto err;
	}
	_ctx[ctx_idx] = ctx;
	if(!ElfParse(path, ctx))
	{
		goto err;
	}

	// 把Program Header Table内的LOAD类型Program Header加载进内存。
	uint32 vbase = 0xffffffff;
	uint32 max_vaddr = 0x00000000;	// 储存PHDR表中最大的虚拟地址。
	uint32 max_msize = 0;			// 拥有最大的虚拟地址的PHDR的内存大小。
	Elf32_Phdr * phdr = NULL;
	ElfResetPHDR(ctx);
	while((phdr = ElfNextPHDR(ctx)) != NULL)
	{
		if(	phdr->p_type == PT_LOAD
			&& phdr->p_vaddr < vbase)
		{
			vbase = phdr->p_vaddr;
		}
		if(	phdr->p_type == PT_LOAD
			&& phdr->p_vaddr > max_vaddr)
		{
			max_vaddr = phdr->p_vaddr;
			max_msize = phdr->p_memsz;
		}
	}
	uint32 msize = max_vaddr - vbase + max_msize;
	elf_knl = MemAlloc(msize);
	if(elf_knl == NULL)
	{
		goto err;
	}
	memset(elf_knl, 0, msize);
	ElfResetPHDR(ctx);
	while((phdr = ElfNextPHDR(ctx)) != NULL)
	{
		if(phdr->p_type == PT_LOAD)
		{
			memcpy(	elf_knl + (phdr->p_vaddr - vbase),
					ctx->file_content + phdr->p_offset,
					phdr->p_filesz);
		}
	}
	ctx->elf_knl = elf_knl;
	ctx->elf_usr = NULL;

	// 修正动态重定向符号的地址（.rel.dyn）。
	if(ctx->rel_dyn != NULL)
	{
		ElfResetRelDyn(ctx);
		Elf32_Rel * rel = NULL;
		while((rel = ElfNextRelDyn(ctx)) != NULL)
		{
			uint32 sym_idx = ELF32_R_SYM(rel->r_info);
			Elf32_Sym * sym = (Elf32_Sym *)(ctx->file_content + ctx->dynsym->sh_offset) + sym_idx;
			uint32 * off = (uint32 *)(ctx->elf_knl + rel->r_offset);
			*off = (uint32)ctx->elf_knl + sym->st_value;
		}
	}

	// 修正动态重定向符号的地址（.rel.plt）。
	if(ctx->rel_plt != NULL)
	{
		ElfResetRelPlt(ctx);
		Elf32_Rel * rel = NULL;
		while((rel = ElfNextRelPlt(ctx)) != NULL)
		{
			uint32 sym_idx = ELF32_R_SYM(rel->r_info);
			Elf32_Sym * sym = (Elf32_Sym *)(ctx->file_content + ctx->dynsym->sh_offset) + sym_idx;
			uint32 * off = (uint32 *)(ctx->elf_knl + rel->r_offset);
			*off = (uint32)ctx->elf_knl + sym->st_value;
		}
	}

	return ctx_idx;
err:
	if(elf_knl != NULL)
	{
		DELETE(elf_knl);
	}
	if(ctx != NULL)
	{
		if(ctx_idx < _MAX_COUNT)
		{
			_ctx[ctx_idx] = NULL;
		}
		ElfFree(ctx);
		DELETE(ctx);
	}
	return 0xffffffff;
}

void *
KnlGetElfSharedObjectSymbol(
	IN uint32 ctx_idx,
	IN CASCTEXT name)
{
	ELFContextPtr ctx = NULL;
	if(name == NULL || ctx_idx >= _MAX_COUNT)
	{
		goto err;
	}

	ctx = _ctx[ctx_idx];
	if(ctx == NULL)
	{
		goto err;
	}

	uint32 symaddr = 0;
	ElfResetDynsym(ctx);
	Elf32_Sym * sym = NULL;
	while((sym = ElfNextDynsymSymbol(ctx)) != NULL)
	{
		if(UtlCompareString(ElfParseDynsymSymbolName(ctx, sym), name) == SFSTR_R_EQUAL)
		{
			symaddr = sym->st_value;
		}
	}
	ElfResetDynsym(ctx);
	if(symaddr == 0)
	{
		goto err;
	}
	return (void *)(ctx->elf_knl + symaddr);
err:
	return NULL;
}

/**
	@Function:		KnlUnloadElfSharedObjectFile
	@Access:		Public
	@Description:
		释放内核加载的ELF Shared Object上下文。
	@Parameters:
		ctx_idx, uint32, IN
			ELF Shared Object上下文索引。
		name, CASCTEXT, IN
			符号名。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。
*/
BOOL
KnlUnloadElfSharedObjectFile(
	IN uint32 ctx_idx)
{
	ELFContextPtr ctx = NULL;
	if(ctx_idx >= _MAX_COUNT)
	{
		goto err;
	}

	ctx = _ctx[ctx_idx];
	if(ctx == NULL)
	{
		goto err;
	}

	// 释放ELF SO上下文。
	_ctx[ctx_idx] = NULL;
	DELETE(ctx->elf_knl);
	ctx->elf_usr = NULL;
	ctx->elf_knl = NULL;
	ElfFree(ctx);
	DELETE(ctx);
	
	return TRUE;
err:
	return FALSE;
}
