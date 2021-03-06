







#include "int_lib.h"

#if !defined(_WIN32)
#include <sys/mman.h>
#endif




#define HAVE_SYSCONF 1

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#if !defined(__APPLE__)
#include <unistd.h>
#endif
#endif

#if __LP64__
#define TRAMPOLINE_SIZE 48
#else
#define TRAMPOLINE_SIZE 40
#endif







COMPILER_RT_ABI void __enable_execute_stack(void *addr) {

#if _WIN32
MEMORY_BASIC_INFORMATION mbi;
if (!VirtualQuery(addr, &mbi, sizeof(mbi)))
return;
VirtualProtect(mbi.BaseAddress, mbi.RegionSize, PAGE_EXECUTE_READWRITE,
&mbi.Protect);
#else
#if __APPLE__

const uintptr_t pageSize = 4096;
#elif !defined(HAVE_SYSCONF)
#error "HAVE_SYSCONF not defined! See enable_execute_stack.c"
#else
const uintptr_t pageSize = sysconf(_SC_PAGESIZE);
#endif

const uintptr_t pageAlignMask = ~(pageSize - 1);
uintptr_t p = (uintptr_t)addr;
unsigned char *startPage = (unsigned char *)(p & pageAlignMask);
unsigned char *endPage =
(unsigned char *)((p + TRAMPOLINE_SIZE + pageSize) & pageAlignMask);
size_t length = endPage - startPage;
(void)mprotect((void *)startPage, length, PROT_READ | PROT_WRITE | PROT_EXEC);
#endif
}
