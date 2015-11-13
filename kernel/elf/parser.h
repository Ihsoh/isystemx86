#ifndef	_ELF_PARSER_H_
#define	_ELF_PARSER_H_

#include "elf.h"

typedef struct
{
	int					valid;
	unsigned int		file_size;
	unsigned char *		file_content;
	Elf32_Ehdr *		header;
	Elf32_Phdr * 		phdr;
	Elf32_Phdr * 		itr_phdr;
	unsigned int		n_phdr;
} ELFContext, * ELFContextPtr;

extern int elf_parse(const char * path, ELFContextPtr ctx);
extern void elf_free(ELFContextPtr ctx);

extern void elf_reset_phdr(ELFContextPtr ctx);
extern Elf32_Phdr * elf_next_phdr(ELFContextPtr ctx);

#endif
