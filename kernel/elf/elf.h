#ifndef	_ELF_ELF_H_
#define	_ELF_ELF_H_

#include "types.h"

//
//	ELF Header
//

#define	EI_MAG0			0
#define	EI_MAG1			1
#define	EI_MAG2			2
#define	EI_MAG3			3
#define	EI_CLASS		4
#define	EI_DATA			5
#define	EI_VERSION		6
#define	EI_PAD			7
#define	EI_NIDENT		16

typedef struct
{
	unsigned char	e_ident[EI_NIDENT];	// 0
	Elf32_Half		e_type;				// 16
	Elf32_Half		e_machine;			// 18
	Elf32_Word		e_version;			// 20
	Elf32_Addr		e_entry;			// 24
	Elf32_Off		e_phoff;			// 28
	Elf32_Off		e_shoff;			// 32
	Elf32_Word		e_flags;			// 36
	Elf32_Half		e_ehsize;			// 40
	Elf32_Half		e_phentsize;		// 42
	Elf32_Half		e_phnum;			// 44
	Elf32_Half		e_shentsize;		// 46
	Elf32_Half		e_shnum;			// 48
	Elf32_Half		e_shstrndx;			// 50
} Elf32_Ehdr;

#define	ELFMAG0		0x7f
#define	ELFMAG1		'E'
#define	ELFMAG2		'L'
#define	ELFMAG3		'F'

#define	ELFCLASSNONE	0 	// Invalid class
#define	ELFCLASS32		1 	// 32-bit objects
#define	ELFCLASS64		2 	// 64-bit objects

#define	ELFDATANONE		0
#define	ELFDATA2LSB		1
#define	ELFDATA2MSB		2

#define	ET_NONE		0
#define	ET_REL		1
#define	ET_EXEC		2
#define	ET_DYN		3
#define	ET_CORE		4
#define	ET_LOPROC	0xff00
#define	ET_HIPROC	0xffff

#define	EM_NONE		0
#define	EM_M32		1
#define	EM_SPARC	2
#define	EM_386		3
#define	EM_68K		4
#define	EM_88K		5
#define	EM_860		7
#define	EM_MIPS		8
#define	EM_X86_64	0x3e	// from wiki.osdev.org
#define	EM_ARM 		0x28	// from wiki.osdev.org

#define	EM_NONE_MEANING		"No machine"
#define	EM_M32_MEANING		"AT&T WE 32100"
#define	EM_SPARC_MEANING	"SPARC"
#define	EM_386_MEANING		"Intel 80386"
#define	EM_68K_MEANING		"Motorola 68000"
#define	EM_88K_MEANING		"Motorola 88000"
#define	EM_860_MEANING		"Intel 80860"
#define	EM_MIPS_MEANING		"MIPS RS3000"
#define	EM_X86_64_MEANING	"x86_64"	// from wiki.osdev.org
#define	EM_ARM_MEANING		"ARM"		// from wiki.osdev.org

#define	EV_NONE		0
#define	EV_CURRENT	1

//
//	Section Header Table
//

typedef struct
{
	Elf32_Word		sh_name;
	Elf32_Word		sh_type;
	Elf32_Word		sh_flags;
	Elf32_Addr		sh_addr;
	Elf32_Off		sh_offset;
	Elf32_Word		sh_size;
	Elf32_Word		sh_link;
	Elf32_Word		sh_info;
	Elf32_Word		sh_addralign;
	Elf32_Word		sh_entsize;
} Elf32_Shdr;

#define	SHT_NULL		0
#define	SHT_PROGBITS	1
#define	SHT_SYMTAB		2
#define	SHT_STRTAB		3
#define	SHT_RELA		4
#define	SHT_HASH		5
#define	SHT_DYNAMIC		6
#define	SHT_NOTE		7
#define	SHT_NOBITS		8
#define	SHT_REL 		9
#define	SHT_SHLIB		10
#define	SHT_DYNSYM		11
#define	SHT_LOPROC		0x70000000
#define	SHT_HIPROC		0x7fffffff
#define	SHT_LOUSER		0x80000000
#define	SHT_HIUSER		0xffffffff

#define	SHF_WRITE		0x1
#define	SHF_ALLOC		0x2
#define	SHF_EXECINSTR	0x4
#define	SHF_MASKPROC	0xf0000000

typedef struct
{
	Elf32_Word		st_name;
	Elf32_Addr		st_value;
	Elf32_Word		st_size;
	unsigned char	st_info;
	unsigned char	st_other;
	Elf32_Half		st_shndx;
} Elf32_Sym;

#define	STN_UNDEF		0

#define	ELF32_ST_BIND(i)	((i) >> 4)
#define	ELF32_ST_TYPE(i)	((i) & 0x0f)
#define	ELF32_ST_INFO(b, t)	(((b) << 4) + ((t) & 0x0f))

#define	STB_LOCAL		0
#define	STB_GLOBAL		1
#define	STB_WEAK		2
#define	STB_LOPROC		13
#define	STB_HIPROC		15

#define	STT_NOTYPE		0
#define	STT_OBJECT		1
#define	STT_FUNC		2
#define	STT_SECTION		3
#define	STT_FILE		4
#define	STT_LOPROC		13
#define	STT_HIPROC		15

typedef struct
{
	Elf32_Addr		r_offset;
	Elf32_Word		r_info;
} Elf32_Rel;

typedef struct
{
	Elf32_Addr		r_offset;
	Elf32_Word		r_info;
	Elf32_Sword		r_addend;
} Elf32_Rela;

#define	ELF32_R_SYM(i)		((i) >> 8)
#define	ELF32_R_TYPE(i)		((unsigned char)(i))
#define	ELF32_R_INFO(s, t)	(((s) << 8) + (unsigned char)(t))

#define	R_386_NONE			0
#define	R_386_32			1
#define	R_386_PC32			2
#define	R_386_GOT32			3
#define	R_386_PLT32			4
#define	R_386_COPY			5
#define	R_386_GLOB_DAT		6
#define	R_386_JMP_SLOT		7
#define	R_386_RELATIVE		8
#define	R_386_GOTOFF		9
#define	R_386_GOTPC			10

//
//	Program Header Table
//

typedef struct
{
	Elf32_Word		p_type;
	Elf32_Off		p_offset;
	Elf32_Addr		p_vaddr;
	Elf32_Addr		p_paddr;
	Elf32_Word		p_filesz;
	Elf32_Word		p_memsz;
	Elf32_Word		p_flags;
	Elf32_Word		p_align;
}  Elf32_Phdr;

#define	PT_NULL		0
#define	PT_LOAD		1
#define	PT_DYNAMIC	2
#define	PT_INTERP	3
#define	PT_NOTE		4
#define	PT_SHLIB	5
#define	PT_PHDR		6
#define	PT_LOPROC	0x70000000
#define	PT_HIPROC	0x7fffffff

#define	PF_X		0x1
#define	PF_W		0x2
#define	PF_R		0x4
#define	PF_MASKPROC	0xf0000000

#endif
