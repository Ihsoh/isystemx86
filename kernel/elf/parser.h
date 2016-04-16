#ifndef	_ELF_PARSER_H_
#define	_ELF_PARSER_H_

#include "elf.h"

typedef struct
{
	int32				valid;
	uint32				file_size;
	uint8 *				file_content;
	Elf32_Ehdr *		header;
	// Section Header Table。
	Elf32_Shdr *		shdr;
	Elf32_Shdr *		itr_shdr;
	uint32				n_shdr;
	Elf32_Shdr *		shstrtab;
	const uint8 *		shstrtab_offset;
	Elf32_Shdr *		strtab;
	Elf32_Shdr *		dynstr;
	const uint8 *		strtab_offset;
	Elf32_Shdr *		dynsym;
	const uint8 *		dynstr_offset;
	Elf32_Sym *			itr_dynsym_sym;
	uint32				n_dynsym_sym;
	// .rel.dyn。
	Elf32_Shdr *		rel_dyn;
	Elf32_Rel *			itr_rel_dyn;
	uint32				n_rel_dyn;
	// .rel.plt。
	Elf32_Shdr *		rel_plt;
	Elf32_Rel *			itr_rel_plt;
	uint32				n_rel_plt;

	// Program Header Table。
	Elf32_Phdr * 		phdr;
	Elf32_Phdr * 		itr_phdr;
	uint32				n_phdr;

	// 以下部分用于ELF SO。
	uint8 *				elf_knl;
	uint8 *				elf_usr;	
} ELFContext, * ELFContextPtr;

extern
int32
ElfParse(	IN CASCTEXT path,
			OUT ELFContextPtr ctx);

extern 
void
ElfFree(IN ELFContextPtr ctx);

extern
void
ElfResetSHDR(IN OUT ELFContextPtr ctx);

extern
Elf32_Shdr *
ElfNextSHDR(IN OUT ELFContextPtr ctx);

extern
void
ElfResetPHDR(IN OUT ELFContextPtr ctx);

extern
Elf32_Phdr *
ElfNextPHDR(IN OUT ELFContextPtr ctx);

extern
void
ElfResetDynsym(IN OUT ELFContextPtr ctx);

extern
Elf32_Sym *
ElfNextDynsymSymbol(IN OUT ELFContextPtr ctx);

extern
const int8 *
ElfParseDynsymSymbolName(	IN ELFContextPtr ctx,
							IN Elf32_Sym * sym);

extern
void
ElfResetRelDyn(IN OUT ELFContextPtr ctx);

extern
Elf32_Rel *
ElfNextRelDyn(IN OUT ELFContextPtr ctx);

extern
void
ElfResetRelPlt(IN OUT ELFContextPtr ctx);

extern
Elf32_Rel *
ElfNextRelPlt(IN OUT ELFContextPtr ctx);

#endif
