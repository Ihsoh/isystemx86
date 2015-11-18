#ifndef	_ELF_PARSER_H_
#define	_ELF_PARSER_H_

#include "elf.h"

typedef struct
{
	int					valid;
	unsigned int		file_size;
	unsigned char *		file_content;
	Elf32_Ehdr *		header;
	// Section Header Table。
	Elf32_Shdr *		shdr;
	Elf32_Shdr *		itr_shdr;
	unsigned int		n_shdr;
	Elf32_Shdr *		shstrtab;
	const char *		shstrtab_offset;
	Elf32_Shdr *		strtab;
	Elf32_Shdr *		dynstr;
	const char *		strtab_offset;
	Elf32_Shdr *		dynsym;
	const char *		dynstr_offset;
	Elf32_Sym *			itr_dynsym_sym;
	unsigned int		n_dynsym_sym;
	// .rel.dyn。
	Elf32_Shdr *		rel_dyn;
	Elf32_Rel *			itr_rel_dyn;
	unsigned int		n_rel_dyn;
	// .rel.plt。
	Elf32_Shdr *		rel_plt;
	Elf32_Rel *			itr_rel_plt;
	unsigned int		n_rel_plt;

	// Program Header Table。
	Elf32_Phdr * 		phdr;
	Elf32_Phdr * 		itr_phdr;
	unsigned int		n_phdr;

	// 以下部分用于ELF SO。
	unsigned char *		elf_knl;
	unsigned char *		elf_usr;	
} ELFContext, * ELFContextPtr;

extern int elf_parse(const char * path, ELFContextPtr ctx);
extern void elf_free(ELFContextPtr ctx);

extern void elf_reset_shdr(ELFContextPtr ctx);
extern Elf32_Shdr * elf_next_shdr(ELFContextPtr ctx);

extern void elf_reset_phdr(ELFContextPtr ctx);
extern Elf32_Phdr * elf_next_phdr(ELFContextPtr ctx);

extern void elf_reset_dynsym(ELFContextPtr ctx);
extern Elf32_Sym * elf_next_dynsym_sym(ELFContextPtr ctx);
extern const char * elf_parse_dynsym_sym_name(	ELFContextPtr ctx,
												Elf32_Sym * sym);

extern void elf_reset_rel_dyn(ELFContextPtr ctx);
extern Elf32_Rel * elf_next_rel_dyn(ELFContextPtr ctx);

extern void elf_reset_rel_plt(ELFContextPtr ctx);
extern Elf32_Rel * elf_next_rel_plt(ELFContextPtr ctx);

#endif
