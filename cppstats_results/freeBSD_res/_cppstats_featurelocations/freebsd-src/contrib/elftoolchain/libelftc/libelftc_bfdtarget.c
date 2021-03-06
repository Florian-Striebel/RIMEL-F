

























#include <sys/param.h>
#include <libelf.h>
#include <libelftc.h>

#include "_libelftc.h"

ELFTC_VCSID("$Id: libelftc_bfdtarget.c 3752 2019-06-28 01:12:53Z emaste $");

struct _Elftc_Bfd_Target _libelftc_targets[] = {

{
.bt_name = "binary",
.bt_type = ETF_BINARY,
},

{
.bt_name = "elf32-avr",
.bt_type = ETF_ELF,
.bt_byteorder = ELFDATA2LSB,
.bt_elfclass = ELFCLASS32,
.bt_machine = EM_AVR,
},

{
.bt_name = "elf32-big",
.bt_type = ETF_ELF,
.bt_byteorder = ELFDATA2MSB,
.bt_elfclass = ELFCLASS32,
},

{
.bt_name = "elf32-bigarm",
.bt_type = ETF_ELF,
.bt_byteorder = ELFDATA2MSB,
.bt_elfclass = ELFCLASS32,
.bt_machine = EM_ARM,
},

{
.bt_name = "elf32-bigmips",
.bt_type = ETF_ELF,
.bt_byteorder = ELFDATA2MSB,
.bt_elfclass = ELFCLASS32,
.bt_machine = EM_MIPS,
},

{
.bt_name = "elf32-i386",
.bt_type = ETF_ELF,
.bt_byteorder = ELFDATA2LSB,
.bt_elfclass = ELFCLASS32,
.bt_machine = EM_386,
},

{
.bt_name = "elf32-i386-freebsd",
.bt_type = ETF_ELF,
.bt_byteorder = ELFDATA2LSB,
.bt_elfclass = ELFCLASS32,
.bt_machine = EM_386,
.bt_osabi = ELFOSABI_FREEBSD,
},

{
.bt_name = "elf32-ia64-big",
.bt_type = ETF_ELF,
.bt_byteorder = ELFDATA2MSB,
.bt_elfclass = ELFCLASS32,
.bt_machine = EM_IA_64,
},

{
.bt_name = "elf32-little",
.bt_type = ETF_ELF,
.bt_byteorder = ELFDATA2LSB,
.bt_elfclass = ELFCLASS32,
},

{
.bt_name = "elf32-littlearm",
.bt_type = ETF_ELF,
.bt_byteorder = ELFDATA2LSB,
.bt_elfclass = ELFCLASS32,
.bt_machine = EM_ARM,
},

{
.bt_name = "elf32-littlemips",
.bt_type = ETF_ELF,
.bt_byteorder = ELFDATA2LSB,
.bt_elfclass = ELFCLASS32,
.bt_machine = EM_MIPS,
},

{
.bt_name = "elf32-powerpc",
.bt_type = ETF_ELF,
.bt_byteorder = ELFDATA2MSB,
.bt_elfclass = ELFCLASS32,
.bt_machine = EM_PPC,
},

{
.bt_name = "elf32-powerpc-freebsd",
.bt_type = ETF_ELF,
.bt_byteorder = ELFDATA2MSB,
.bt_elfclass = ELFCLASS32,
.bt_machine = EM_PPC,
.bt_osabi = ELFOSABI_FREEBSD,
},

{
.bt_name = "elf32-powerpcle",
.bt_type = ETF_ELF,
.bt_byteorder = ELFDATA2LSB,
.bt_elfclass = ELFCLASS32,
.bt_machine = EM_PPC,
},

{
.bt_name = "elf32-sh",
.bt_type = ETF_ELF,
.bt_byteorder = ELFDATA2MSB,
.bt_elfclass = ELFCLASS32,
.bt_machine = EM_SH,
},

{
.bt_name = "elf32-shl",
.bt_type = ETF_ELF,
.bt_byteorder = ELFDATA2LSB,
.bt_elfclass = ELFCLASS32,
.bt_machine = EM_SH,
},

{
.bt_name = "elf32-sh-nbsd",
.bt_type = ETF_ELF,
.bt_byteorder = ELFDATA2MSB,
.bt_elfclass = ELFCLASS32,
.bt_machine = EM_SH,
.bt_osabi = ELFOSABI_NETBSD,
},

{
.bt_name = "elf32-shl-nbsd",
.bt_type = ETF_ELF,
.bt_byteorder = ELFDATA2LSB,
.bt_elfclass = ELFCLASS32,
.bt_machine = EM_SH,
.bt_osabi = ELFOSABI_NETBSD,
},

{
.bt_name = "elf32-shbig-linux",
.bt_type = ETF_ELF,
.bt_byteorder = ELFDATA2MSB,
.bt_elfclass = ELFCLASS32,
.bt_machine = EM_SH,
.bt_osabi = ELFOSABI_LINUX,
},

{
.bt_name = "elf32-sh-linux",
.bt_type = ETF_ELF,
.bt_byteorder = ELFDATA2LSB,
.bt_elfclass = ELFCLASS32,
.bt_machine = EM_SH,
.bt_osabi = ELFOSABI_LINUX,
},

{
.bt_name = "elf32-sparc",
.bt_type = ETF_ELF,
.bt_byteorder = ELFDATA2MSB,
.bt_elfclass = ELFCLASS32,
.bt_machine = EM_SPARC,
},

{
.bt_name = "elf32-tradbigmips",
.bt_type = ETF_ELF,
.bt_byteorder = ELFDATA2MSB,
.bt_elfclass = ELFCLASS32,
.bt_machine = EM_MIPS,
},

{
.bt_name = "elf32-tradlittlemips",
.bt_type = ETF_ELF,
.bt_byteorder = ELFDATA2LSB,
.bt_elfclass = ELFCLASS32,
.bt_machine = EM_MIPS,
},

{
.bt_name = "elf64-alpha",
.bt_type = ETF_ELF,
.bt_byteorder = ELFDATA2LSB,
.bt_elfclass = ELFCLASS64,
.bt_machine = EM_ALPHA,
},

{
.bt_name = "elf64-alpha-freebsd",
.bt_type = ETF_ELF,
.bt_byteorder = ELFDATA2LSB,
.bt_elfclass = ELFCLASS64,
.bt_machine = EM_ALPHA,
.bt_osabi = ELFOSABI_FREEBSD
},

{
.bt_name = "elf64-big",
.bt_type = ETF_ELF,
.bt_byteorder = ELFDATA2MSB,
.bt_elfclass = ELFCLASS64,
},

{
.bt_name = "elf64-bigmips",
.bt_type = ETF_ELF,
.bt_byteorder = ELFDATA2MSB,
.bt_elfclass = ELFCLASS64,
.bt_machine = EM_MIPS,
},

{
.bt_name = "elf64-ia64-big",
.bt_type = ETF_ELF,
.bt_byteorder = ELFDATA2MSB,
.bt_elfclass = ELFCLASS64,
.bt_machine = EM_IA_64,
},

{
.bt_name = "elf64-ia64-little",
.bt_type = ETF_ELF,
.bt_byteorder = ELFDATA2LSB,
.bt_elfclass = ELFCLASS64,
.bt_machine = EM_IA_64,
},

{
.bt_name = "elf64-little",
.bt_type = ETF_ELF,
.bt_byteorder = ELFDATA2LSB,
.bt_elfclass = ELFCLASS64,
},

{
.bt_name = "elf64-littleaarch64",
.bt_type = ETF_ELF,
.bt_byteorder = ELFDATA2LSB,
.bt_elfclass = ELFCLASS64,
.bt_machine = EM_AARCH64,
},

{
.bt_name = "elf64-littlemips",
.bt_type = ETF_ELF,
.bt_byteorder = ELFDATA2LSB,
.bt_elfclass = ELFCLASS64,
.bt_machine = EM_MIPS,
},

{
.bt_name = "elf64-powerpc",
.bt_type = ETF_ELF,
.bt_byteorder = ELFDATA2MSB,
.bt_elfclass = ELFCLASS64,
.bt_machine = EM_PPC64,
},

{
.bt_name = "elf64-powerpc-freebsd",
.bt_type = ETF_ELF,
.bt_byteorder = ELFDATA2MSB,
.bt_elfclass = ELFCLASS64,
.bt_machine = EM_PPC64,
.bt_osabi = ELFOSABI_FREEBSD,
},

{
.bt_name = "elf64-powerpcle",
.bt_type = ETF_ELF,
.bt_byteorder = ELFDATA2LSB,
.bt_elfclass = ELFCLASS64,
.bt_machine = EM_PPC64,
},

{
.bt_name = "elf32-riscv",
.bt_type = ETF_ELF,
.bt_byteorder = ELFDATA2LSB,
.bt_elfclass = ELFCLASS32,
.bt_machine = EM_RISCV,
},

{
.bt_name = "elf64-riscv",
.bt_type = ETF_ELF,
.bt_byteorder = ELFDATA2LSB,
.bt_elfclass = ELFCLASS64,
.bt_machine = EM_RISCV,
},

{
.bt_name = "elf64-riscv-freebsd",
.bt_type = ETF_ELF,
.bt_byteorder = ELFDATA2MSB,
.bt_elfclass = ELFCLASS64,
.bt_machine = EM_RISCV,
.bt_osabi = ELFOSABI_FREEBSD,
},

{
.bt_name = "elf64-sh64",
.bt_type = ETF_ELF,
.bt_byteorder = ELFDATA2MSB,
.bt_elfclass = ELFCLASS64,
.bt_machine = EM_SH,
},

{
.bt_name = "elf64-sh64l",
.bt_type = ETF_ELF,
.bt_byteorder = ELFDATA2LSB,
.bt_elfclass = ELFCLASS64,
.bt_machine = EM_SH,
},

{
.bt_name = "elf64-sh64-nbsd",
.bt_type = ETF_ELF,
.bt_byteorder = ELFDATA2MSB,
.bt_elfclass = ELFCLASS64,
.bt_machine = EM_SH,
.bt_osabi = ELFOSABI_NETBSD,
},

{
.bt_name = "elf64-sh64l-nbsd",
.bt_type = ETF_ELF,
.bt_byteorder = ELFDATA2LSB,
.bt_elfclass = ELFCLASS64,
.bt_machine = EM_SH,
.bt_osabi = ELFOSABI_NETBSD,
},

{
.bt_name = "elf64-sh64big-linux",
.bt_type = ETF_ELF,
.bt_byteorder = ELFDATA2MSB,
.bt_elfclass = ELFCLASS64,
.bt_machine = EM_SH,
.bt_osabi = ELFOSABI_LINUX,
},

{
.bt_name = "elf64-sh64-linux",
.bt_type = ETF_ELF,
.bt_byteorder = ELFDATA2LSB,
.bt_elfclass = ELFCLASS64,
.bt_machine = EM_SH,
.bt_osabi = ELFOSABI_LINUX,
},

{
.bt_name = "elf64-sparc",
.bt_type = ETF_ELF,
.bt_byteorder = ELFDATA2MSB,
.bt_elfclass = ELFCLASS64,
.bt_machine = EM_SPARCV9,
},

{
.bt_name = "elf64-sparc-freebsd",
.bt_type = ETF_ELF,
.bt_byteorder = ELFDATA2MSB,
.bt_elfclass = ELFCLASS64,
.bt_machine = EM_SPARCV9,
.bt_osabi = ELFOSABI_FREEBSD
},

{
.bt_name = "elf64-tradbigmips",
.bt_type = ETF_ELF,
.bt_byteorder = ELFDATA2MSB,
.bt_elfclass = ELFCLASS64,
.bt_machine = EM_MIPS,
},

{
.bt_name = "elf64-tradlittlemips",
.bt_type = ETF_ELF,
.bt_byteorder = ELFDATA2LSB,
.bt_elfclass = ELFCLASS64,
.bt_machine = EM_MIPS,
},

{
.bt_name = "elf64-x86-64",
.bt_type = ETF_ELF,
.bt_byteorder = ELFDATA2LSB,
.bt_elfclass = ELFCLASS64,
.bt_machine = EM_X86_64,
},

{
.bt_name = "elf64-x86-64-freebsd",
.bt_type = ETF_ELF,
.bt_byteorder = ELFDATA2LSB,
.bt_elfclass = ELFCLASS64,
.bt_machine = EM_X86_64,
.bt_osabi = ELFOSABI_FREEBSD
},

{
.bt_name = "ihex",
.bt_type = ETF_IHEX,
},

{
.bt_name = "srec",
.bt_type = ETF_SREC,
},

{
.bt_name = "symbolsrec",
.bt_type = ETF_SREC,
},

{
.bt_name = "efi-app-ia32",
.bt_type = ETF_EFI,
.bt_machine = EM_386,
},

{
.bt_name = "efi-app-x86_64",
.bt_type = ETF_EFI,
.bt_machine = EM_X86_64,
},

{
.bt_name = "pei-i386",
.bt_type = ETF_PE,
.bt_machine = EM_386,
},

{
.bt_name = "pei-x86-64",
.bt_type = ETF_PE,
.bt_machine = EM_X86_64,
},

{
.bt_name = NULL,
.bt_type = ETF_NONE,
},
};
