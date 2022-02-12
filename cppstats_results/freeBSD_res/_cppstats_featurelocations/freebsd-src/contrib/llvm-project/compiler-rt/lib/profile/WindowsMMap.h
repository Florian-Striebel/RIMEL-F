







#if !defined(PROFILE_INSTRPROFILING_WINDOWS_MMAP_H)
#define PROFILE_INSTRPROFILING_WINDOWS_MMAP_H

#if defined(_WIN32)

#include <basetsd.h>
#include <io.h>
#include <sys/types.h>




#define PROT_READ 0x1
#define PROT_WRITE 0x2
#define PROT_EXEC 0x0

#define MAP_FILE 0x00
#define MAP_SHARED 0x01
#define MAP_PRIVATE 0x02
#define MAP_ANONYMOUS 0x20
#define MAP_ANON MAP_ANONYMOUS
#define MAP_FAILED ((void *) -1)




#define MS_ASYNC 0x0001
#define MS_INVALIDATE 0x0002
#define MS_SYNC 0x0010





#define MADV_NORMAL 0
#define MADV_WILLNEED 3
#define MADV_DONTNEED 4




#define LOCK_SH 1
#define LOCK_EX 2
#define LOCK_NB 4
#define LOCK_UN 8

#if defined(__USE_FILE_OFFSET64)
#define DWORD_HI(x) (x >> 32)
#define DWORD_LO(x) ((x) & 0xffffffff)
#else
#define DWORD_HI(x) (0)
#define DWORD_LO(x) (x)
#endif

void *mmap(void *start, size_t length, int prot, int flags, int fd,
off_t offset);

void munmap(void *addr, size_t length);

int msync(void *addr, size_t length, int flags);

int madvise(void *addr, size_t length, int advice);

int flock(int fd, int operation);

#endif

#endif
