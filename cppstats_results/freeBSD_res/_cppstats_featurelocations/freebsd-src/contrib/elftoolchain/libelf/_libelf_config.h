



























#if defined(__APPLE__) || defined(__DragonFly__) || defined(__FreeBSD__)






#if defined(__amd64__)

#define LIBELF_ARCH EM_X86_64
#define LIBELF_BYTEORDER ELFDATA2LSB
#define LIBELF_CLASS ELFCLASS64

#elif defined(__aarch64__)

#define LIBELF_ARCH EM_AARCH64
#define LIBELF_BYTEORDER ELFDATA2LSB
#define LIBELF_CLASS ELFCLASS64

#elif defined(__arm__)

#define LIBELF_ARCH EM_ARM
#if defined(__ARMEB__)
#define LIBELF_BYTEORDER ELFDATA2MSB
#else
#define LIBELF_BYTEORDER ELFDATA2LSB
#endif
#define LIBELF_CLASS ELFCLASS32

#elif defined(__i386__)

#define LIBELF_ARCH EM_386
#define LIBELF_BYTEORDER ELFDATA2LSB
#define LIBELF_CLASS ELFCLASS32

#elif defined(__ia64__)

#define LIBELF_ARCH EM_IA_64
#define LIBELF_BYTEORDER ELFDATA2LSB
#define LIBELF_CLASS ELFCLASS64

#elif defined(__mips__)

#define LIBELF_ARCH EM_MIPS
#if defined(__MIPSEB__)
#define LIBELF_BYTEORDER ELFDATA2MSB
#else
#define LIBELF_BYTEORDER ELFDATA2LSB
#endif
#define LIBELF_CLASS ELFCLASS32

#elif defined(__powerpc64__)

#define LIBELF_ARCH EM_PPC64
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#define LIBELF_BYTEORDER ELFDATA2LSB
#else
#define LIBELF_BYTEORDER ELFDATA2MSB
#endif
#define LIBELF_CLASS ELFCLASS64

#elif defined(__powerpc__)

#define LIBELF_ARCH EM_PPC
#define LIBELF_BYTEORDER ELFDATA2MSB
#define LIBELF_CLASS ELFCLASS32

#elif defined(__riscv) && (__riscv_xlen == 64)

#define LIBELF_ARCH EM_RISCV
#define LIBELF_BYTEORDER ELFDATA2LSB
#define LIBELF_CLASS ELFCLASS64

#elif defined(__riscv64)

#define LIBELF_ARCH EM_RISCV
#define LIBELF_BYTEORDER ELFDATA2LSB
#define LIBELF_CLASS ELFCLASS64

#elif defined(__sparc__)

#define LIBELF_ARCH EM_SPARCV9
#define LIBELF_BYTEORDER ELFDATA2MSB
#define LIBELF_CLASS ELFCLASS64

#else
#error Unknown architecture.
#endif
#endif




#if defined(__minix)

#define LIBELF_ARCH EM_386
#define LIBELF_BYTEORDER ELFDATA2LSB
#define LIBELF_CLASS ELFCLASS32

#endif

#if defined(__NetBSD__)

#include <machine/elf_machdep.h>

#if !defined(ARCH_ELFSIZE)
#error ARCH_ELFSIZE is not defined.
#endif

#if ARCH_ELFSIZE == 32
#define LIBELF_ARCH ELF32_MACHDEP_ID
#define LIBELF_BYTEORDER ELF32_MACHDEP_ENDIANNESS
#define LIBELF_CLASS ELFCLASS32
#define Elf_Note Elf32_Nhdr
#else
#define LIBELF_ARCH ELF64_MACHDEP_ID
#define LIBELF_BYTEORDER ELF64_MACHDEP_ENDIANNESS
#define LIBELF_CLASS ELFCLASS64
#define Elf_Note Elf64_Nhdr
#endif

#endif

#if defined(__OpenBSD__)

#include <machine/exec.h>

#define LIBELF_ARCH ELF_TARG_MACH
#define LIBELF_BYTEORDER ELF_TARG_DATA
#define LIBELF_CLASS ELF_TARG_CLASS

#endif










#if defined(__linux__) || defined(__GNU__) || defined(__GLIBC__)

#if defined(__linux__)

#include "native-elf-format.h"

#define LIBELF_CLASS ELFTC_CLASS
#define LIBELF_ARCH ELFTC_ARCH
#define LIBELF_BYTEORDER ELFTC_BYTEORDER

#endif

#if LIBELF_CLASS == ELFCLASS32
#define Elf_Note Elf32_Nhdr
#elif LIBELF_CLASS == ELFCLASS64
#define Elf_Note Elf64_Nhdr
#else
#error LIBELF_CLASS needs to be one of ELFCLASS32 or ELFCLASS64
#endif

#endif
