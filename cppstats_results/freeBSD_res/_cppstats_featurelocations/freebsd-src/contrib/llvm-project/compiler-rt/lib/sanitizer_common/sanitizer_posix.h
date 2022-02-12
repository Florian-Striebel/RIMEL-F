










#if !defined(SANITIZER_POSIX_H)
#define SANITIZER_POSIX_H



#include "sanitizer_internal_defs.h"
#include "sanitizer_platform_limits_freebsd.h"
#include "sanitizer_platform_limits_netbsd.h"
#include "sanitizer_platform_limits_posix.h"
#include "sanitizer_platform_limits_solaris.h"

#if !SANITIZER_POSIX

#error This file should only be included on POSIX
#endif

namespace __sanitizer {



uptr internal_open(const char *filename, int flags);
uptr internal_open(const char *filename, int flags, u32 mode);
uptr internal_close(fd_t fd);

uptr internal_read(fd_t fd, void *buf, uptr count);
uptr internal_write(fd_t fd, const void *buf, uptr count);


uptr internal_mmap(void *addr, uptr length, int prot, int flags,
int fd, u64 offset);
uptr internal_munmap(void *addr, uptr length);
#if SANITIZER_LINUX
uptr internal_mremap(void *old_address, uptr old_size, uptr new_size, int flags,
void *new_address);
#endif
int internal_mprotect(void *addr, uptr length, int prot);
int internal_madvise(uptr addr, uptr length, int advice);


uptr internal_filesize(fd_t fd);
uptr internal_stat(const char *path, void *buf);
uptr internal_lstat(const char *path, void *buf);
uptr internal_fstat(fd_t fd, void *buf);
uptr internal_dup(int oldfd);
uptr internal_dup2(int oldfd, int newfd);
uptr internal_readlink(const char *path, char *buf, uptr bufsize);
uptr internal_unlink(const char *path);
uptr internal_rename(const char *oldpath, const char *newpath);
uptr internal_lseek(fd_t fd, OFF_T offset, int whence);

#if SANITIZER_NETBSD
uptr internal_ptrace(int request, int pid, void *addr, int data);
#else
uptr internal_ptrace(int request, int pid, void *addr, void *data);
#endif
uptr internal_waitpid(int pid, int *status, int options);

int internal_fork();
fd_t internal_spawn(const char *argv[], const char *envp[], pid_t *pid);

int internal_sysctl(const int *name, unsigned int namelen, void *oldp,
uptr *oldlenp, const void *newp, uptr newlen);
int internal_sysctlbyname(const char *sname, void *oldp, uptr *oldlenp,
const void *newp, uptr newlen);



SANITIZER_WEAK_ATTRIBUTE
int real_pthread_create(void *th, void *attr, void *(*callback)(void *),
void *param);
SANITIZER_WEAK_ATTRIBUTE
int real_pthread_join(void *th, void **ret);

#define DEFINE_REAL_PTHREAD_FUNCTIONS namespace __sanitizer { int real_pthread_create(void *th, void *attr, void *(*callback)(void *), void *param) { return REAL(pthread_create)(th, attr, callback, param); } int real_pthread_join(void *th, void **ret) { return REAL(pthread_join(th, ret)); } }










int my_pthread_attr_getstack(void *attr, void **addr, uptr *size);



int internal_sigaction(int signum, const void *act, void *oldact);
void internal_sigfillset(__sanitizer_sigset_t *set);
void internal_sigemptyset(__sanitizer_sigset_t *set);
bool internal_sigismember(__sanitizer_sigset_t *set, int signum);

uptr internal_execve(const char *filename, char *const argv[],
char *const envp[]);

bool IsStateDetached(int state);


fd_t ReserveStandardFds(fd_t fd);

bool ShouldMockFailureToOpen(const char *path);


uptr MmapNamed(void *addr, uptr length, int prot, int flags, const char *name);




int GetNamedMappingFd(const char *name, uptr size, int *flags);


void DecorateMapping(uptr addr, uptr size, const char *name);


}

#endif
