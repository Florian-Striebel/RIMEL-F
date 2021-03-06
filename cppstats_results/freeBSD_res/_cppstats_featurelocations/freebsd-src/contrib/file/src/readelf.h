
































#if !defined(__fake_elf_h__)
#define __fake_elf_h__

#if HAVE_STDINT_H
#include <stdint.h>
#endif

typedef uint32_t Elf32_Addr;
typedef uint32_t Elf32_Off;
typedef uint16_t Elf32_Half;
typedef uint32_t Elf32_Word;
typedef uint8_t Elf32_Char;

typedef uint64_t Elf64_Addr;
typedef uint64_t Elf64_Off;
typedef uint64_t Elf64_Xword;
typedef uint16_t Elf64_Half;
typedef uint32_t Elf64_Word;
typedef uint8_t Elf64_Char;

#define EI_NIDENT 16

typedef struct {
Elf32_Word a_type;
Elf32_Word a_v;
} Aux32Info;

typedef struct {
Elf64_Xword a_type;
Elf64_Xword a_v;
} Aux64Info;

#define AT_NULL 0
#define AT_IGNORE 1
#define AT_EXECFD 2
#define AT_PHDR 3
#define AT_PHENT 4
#define AT_PHNUM 5
#define AT_PAGESZ 6
#define AT_BASE 7
#define AT_FLAGS 8
#define AT_ENTRY 9
#define AT_LINUX_NOTELF 10
#define AT_LINUX_UID 11
#define AT_LINUX_EUID 12
#define AT_LINUX_GID 13
#define AT_LINUX_EGID 14
#define AT_LINUX_PLATFORM 15
#define AT_LINUX_HWCAP 16
#define AT_LINUX_CLKTCK 17

#define AT_LINUX_SECURE 23
#define AT_LINUX_BASE_PLATFORM 24

#define AT_LINUX_RANDOM 25
#define AT_LINUX_HWCAP2 26
#define AT_LINUX_EXECFN 31

typedef struct {
Elf32_Char e_ident[EI_NIDENT];
Elf32_Half e_type;
Elf32_Half e_machine;
Elf32_Word e_version;
Elf32_Addr e_entry;
Elf32_Off e_phoff;
Elf32_Off e_shoff;
Elf32_Word e_flags;
Elf32_Half e_ehsize;
Elf32_Half e_phentsize;
Elf32_Half e_phnum;
Elf32_Half e_shentsize;
Elf32_Half e_shnum;
Elf32_Half e_shstrndx;
} Elf32_Ehdr;

typedef struct {
Elf64_Char e_ident[EI_NIDENT];
Elf64_Half e_type;
Elf64_Half e_machine;
Elf64_Word e_version;
Elf64_Addr e_entry;
Elf64_Off e_phoff;
Elf64_Off e_shoff;
Elf64_Word e_flags;
Elf64_Half e_ehsize;
Elf64_Half e_phentsize;
Elf64_Half e_phnum;
Elf64_Half e_shentsize;
Elf64_Half e_shnum;
Elf64_Half e_shstrndx;
} Elf64_Ehdr;


#define ET_REL 1
#define ET_EXEC 2
#define ET_DYN 3
#define ET_CORE 4


#define EM_SPARC 2
#define EM_386 3
#define EM_SPARC32PLUS 18
#define EM_SPARCV9 43
#define EM_IA_64 50
#define EM_AMD64 62


#define SHT_SYMTAB 2
#define SHT_NOTE 7
#define SHT_DYNSYM 11
#define SHT_SUNW_cap 0x6ffffff5


#define ELFDATANONE 0
#define ELFDATA2LSB 1
#define ELFDATA2MSB 2


#define ELFCLASSNONE 0
#define ELFCLASS32 1
#define ELFCLASS64 2


#define EI_MAG0 0
#define EI_MAG1 1
#define EI_MAG2 2
#define EI_MAG3 3
#define EI_CLASS 4
#define EI_DATA 5
#define EI_VERSION 6
#define EI_PAD 7

#define ELFMAG0 0x7f
#define ELFMAG1 'E'
#define ELFMAG2 'L'
#define ELFMAG3 'F'
#define ELFMAG "\177ELF"

#define OLFMAG1 'O'
#define OLFMAG "\177OLF"

typedef struct {
Elf32_Word p_type;
Elf32_Off p_offset;
Elf32_Addr p_vaddr;
Elf32_Addr p_paddr;
Elf32_Word p_filesz;
Elf32_Word p_memsz;
Elf32_Word p_flags;
Elf32_Word p_align;
} Elf32_Phdr;

typedef struct {
Elf64_Word p_type;
Elf64_Word p_flags;
Elf64_Off p_offset;
Elf64_Addr p_vaddr;
Elf64_Addr p_paddr;
Elf64_Xword p_filesz;
Elf64_Xword p_memsz;
Elf64_Xword p_align;
} Elf64_Phdr;

#define PT_NULL 0
#define PT_LOAD 1
#define PT_DYNAMIC 2
#define PT_INTERP 3
#define PT_NOTE 4
#define PT_SHLIB 5
#define PT_PHDR 6
#define PT_NUM 7

typedef struct {
Elf32_Word sh_name;
Elf32_Word sh_type;
Elf32_Word sh_flags;
Elf32_Addr sh_addr;
Elf32_Off sh_offset;
Elf32_Word sh_size;
Elf32_Word sh_link;
Elf32_Word sh_info;
Elf32_Word sh_addralign;
Elf32_Word sh_entsize;
} Elf32_Shdr;

typedef struct {
Elf64_Word sh_name;
Elf64_Word sh_type;
Elf64_Off sh_flags;
Elf64_Addr sh_addr;
Elf64_Off sh_offset;
Elf64_Off sh_size;
Elf64_Word sh_link;
Elf64_Word sh_info;
Elf64_Off sh_addralign;
Elf64_Off sh_entsize;
} Elf64_Shdr;

#define NT_NETBSD_CORE_PROCINFO 1
#define NT_NETBSD_CORE_AUXV 2

struct NetBSD_elfcore_procinfo {

uint32_t cpi_version;
uint32_t cpi_cpisize;
uint32_t cpi_signo;
uint32_t cpi_sigcode;
uint32_t cpi_sigpend[4];
uint32_t cpi_sigmask[4];
uint32_t cpi_sigignore[4];
uint32_t cpi_sigcatch[4];
int32_t cpi_pid;
int32_t cpi_ppid;
int32_t cpi_pgrp;
int32_t cpi_sid;
uint32_t cpi_ruid;
uint32_t cpi_euid;
uint32_t cpi_svuid;
uint32_t cpi_rgid;
uint32_t cpi_egid;
uint32_t cpi_svgid;
uint32_t cpi_nlwps;
int8_t cpi_name[32];

int32_t cpi_siglwp;
};


typedef struct elf_note {
Elf32_Word n_namesz;
Elf32_Word n_descsz;
Elf32_Word n_type;
} Elf32_Nhdr;

typedef struct {
Elf64_Word n_namesz;
Elf64_Word n_descsz;
Elf64_Word n_type;
} Elf64_Nhdr;


#define NT_PRSTATUS 1
#define NT_PRFPREG 2
#define NT_PRPSINFO 3
#define NT_PRXREG 4
#define NT_TASKSTRUCT 4
#define NT_PLATFORM 5
#define NT_AUXV 6



#define NT_NETBSD_VERSION 1
#define NT_NETBSD_EMULATION 2
#define NT_FREEBSD_VERSION 1
#define NT_OPENBSD_VERSION 1
#define NT_DRAGONFLY_VERSION 1







#define NT_GNU_VERSION 1


#define GNU_OS_LINUX 0
#define GNU_OS_HURD 1
#define GNU_OS_SOLARIS 2
#define GNU_OS_KFREEBSD 3
#define GNU_OS_KNETBSD 4







#define NT_GNU_HWCAP 2






#define NT_GNU_BUILD_ID 3










#define NT_NETBSD_PAX 3
#define NT_NETBSD_PAX_MPROTECT 0x01
#define NT_NETBSD_PAX_NOMPROTECT 0x02
#define NT_NETBSD_PAX_GUARD 0x04
#define NT_NETBSD_PAX_NOGUARD 0x08
#define NT_NETBSD_PAX_ASLR 0x10
#define NT_NETBSD_PAX_NOASLR 0x20









#define NT_NETBSD_MARCH 5









#define NT_NETBSD_CMODEL 6








#define NT_GO_BUILD_ID 4




#define NT_FREEBSD_PROCSTAT_AUXV 16

#if !defined(ELFSIZE) && defined(ARCH_ELFSIZE)
#define ELFSIZE ARCH_ELFSIZE
#endif

typedef struct {
Elf32_Word c_tag;
union {
Elf32_Word c_val;
Elf32_Addr c_ptr;
} c_un;
} Elf32_Cap;

typedef struct {
Elf64_Xword c_tag;
union {
Elf64_Xword c_val;
Elf64_Addr c_ptr;
} c_un;
} Elf64_Cap;


#define CA_SUNW_NULL 0
#define CA_SUNW_HW_1 1
#define CA_SUNW_SF_1 2


#define SF1_SUNW_FPKNWN 0x01
#define SF1_SUNW_FPUSED 0x02
#define SF1_SUNW_MASK 0x03


#define AV_SPARC_MUL32 0x0001
#define AV_SPARC_DIV32 0x0002
#define AV_SPARC_FSMULD 0x0004
#define AV_SPARC_V8PLUS 0x0008
#define AV_SPARC_POPC 0x0010
#define AV_SPARC_VIS 0x0020
#define AV_SPARC_VIS2 0x0040
#define AV_SPARC_ASI_BLK_INIT 0x0080
#define AV_SPARC_FMAF 0x0100
#define AV_SPARC_FJFMAU 0x4000
#define AV_SPARC_IMA 0x8000


#define AV_386_FPU 0x00000001
#define AV_386_TSC 0x00000002
#define AV_386_CX8 0x00000004
#define AV_386_SEP 0x00000008
#define AV_386_AMD_SYSC 0x00000010
#define AV_386_CMOV 0x00000020
#define AV_386_MMX 0x00000040
#define AV_386_AMD_MMX 0x00000080
#define AV_386_AMD_3DNow 0x00000100
#define AV_386_AMD_3DNowx 0x00000200
#define AV_386_FXSR 0x00000400
#define AV_386_SSE 0x00000800
#define AV_386_SSE2 0x00001000
#define AV_386_PAUSE 0x00002000
#define AV_386_SSE3 0x00004000
#define AV_386_MON 0x00008000
#define AV_386_CX16 0x00010000
#define AV_386_AHF 0x00020000
#define AV_386_TSCP 0x00040000
#define AV_386_AMD_SSE4A 0x00080000
#define AV_386_POPCNT 0x00100000
#define AV_386_AMD_LZCNT 0x00200000
#define AV_386_SSSE3 0x00400000
#define AV_386_SSE4_1 0x00800000
#define AV_386_SSE4_2 0x01000000




typedef struct {
Elf32_Word d_tag;
union {
Elf32_Addr d_ptr;
Elf32_Word d_val;
} d_un;
} Elf32_Dyn;

typedef struct {
Elf64_Xword d_tag;
union {
Elf64_Addr d_ptr;
Elf64_Xword d_val;
} d_un;
} Elf64_Dyn;


#define DT_NULL 0
#define DT_NEEDED 1
#define DT_PLTRELSZ 2
#define DT_PLTGOT 3
#define DT_HASH 4
#define DT_STRTAB 5
#define DT_SYMTAB 6
#define DT_RELA 7
#define DT_RELASZ 8
#define DT_RELAENT 9
#define DT_STRSZ 10
#define DT_SYMENT 11
#define DT_INIT 12
#define DT_FINI 13
#define DT_SONAME 14
#define DT_RPATH 15
#define DT_SYMBOLIC 16
#define DT_REL 17
#define DT_RELSZ 18
#define DT_RELENT 19
#define DT_PLTREL 20
#define DT_DEBUG 21
#define DT_TEXTREL 22
#define DT_JMPREL 23
#define DT_BIND_NOW 24
#define DT_INIT_ARRAY 25
#define DT_FINI_ARRAY 26
#define DT_INIT_ARRAYSZ 27
#define DT_FINI_ARRAYSZ 28
#define DT_RUNPATH 29
#define DT_FLAGS 30
#define DT_ENCODING 31
#define DT_PREINIT_ARRAY 32
#define DT_PREINIT_ARRAYSZ 33
#define DT_NUM 34

#define DT_LOOS 0x60000000
#define DT_VERSYM 0x6ffffff0
#define DT_FLAGS_1 0x6ffffffb
#define DT_VERDEF 0x6ffffffc
#define DT_VERDEFNUM 0x6ffffffd
#define DT_VERNEED 0x6ffffffe
#define DT_VERNEEDNUM 0x6fffffff
#define DT_HIOS 0x6fffffff
#define DT_LOPROC 0x70000000
#define DT_HIPROC 0x7fffffff


#define DF_ORIGIN 0x00000001
#define DF_SYMBOLIC 0x00000002
#define DF_TEXTREL 0x00000004
#define DF_BIND_NOW 0x00000008
#define DF_STATIC_TLS 0x00000010


#define DF_1_NOW 0x00000001
#define DF_1_GLOBAL 0x00000002
#define DF_1_GROUP 0x00000004
#define DF_1_NODELETE 0x00000008
#define DF_1_LOADFLTR 0x00000010
#define DF_1_INITFIRST 0x00000020
#define DF_1_NOOPEN 0x00000040
#define DF_1_ORIGIN 0x00000080
#define DF_1_DIRECT 0x00000100
#define DF_1_INTERPOSE 0x00000400
#define DF_1_NODEFLIB 0x00000800
#define DF_1_NODUMP 0x00001000
#define DF_1_CONFALT 0x00002000
#define DF_1_ENDFILTEE 0x00004000
#define DF_1_DISPRELDNE 0x00008000
#define DF_1_DISPRELPND 0x00010000
#define DF_1_NODIRECT 0x00020000
#define DF_1_IGNMULDEF 0x00040000
#define DF_1_NOKSYMS 0x00080000
#define DF_1_NOHDR 0x00100000
#define DF_1_EDITED 0x00200000
#define DF_1_NORELOC 0x00400000
#define DF_1_SYMINTPOSE 0x00800000
#define DF_1_GLOBAUDIT 0x01000000
#define DF_1_SINGLETON 0x02000000
#define DF_1_STUB 0x04000000
#define DF_1_PIE 0x08000000

#endif
