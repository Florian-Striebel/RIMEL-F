



#if !defined(__SYSCALLS_H__)
#define __SYSCALLS_H__




#if defined(__FreeBSD__)


#define umount2(T, F) unmount(T, F)


#define sighandler_t sig_t


#define profil_arg1_t char


#include <sys/types.h>
#include <dirent.h>
inline int getdents_(unsigned int fd, void *dirp, unsigned int count) {
return getdents(fd, (char*)dirp, count);
}
#include <sys/mman.h>
inline int mincore_(void *addr, size_t length, unsigned char *vec) {
return mincore(addr, length, (char*)vec);
}
#define getpid_ getpid


#include <sys/socket.h>
#include <sys/uio.h>
inline ssize_t sendfile_(int out_fd, int in_fd, off_t *offset, size_t count) {
return sendfile(in_fd, out_fd, *offset, count, NULL, offset, 0);
}


#include <sys/param.h>
#include <sys/mount.h>
inline int bogus_mount_() {
return mount("procfs", "/not_mounted", 0, NULL);
}


#include <sys/extattr.h>
inline ssize_t flistxattr_(int fd, char *list, size_t size) {
return extattr_list_fd(fd, EXTATTR_NAMESPACE_USER, list, size);
}
inline ssize_t fgetxattr_(int fd, const char *name, void *value, size_t size) {
return extattr_get_fd(fd, EXTATTR_NAMESPACE_USER, name, value, size);
}
inline int fsetxattr_(int fd, const char *name, const void *value, size_t size, int) {
return extattr_set_fd(fd, EXTATTR_NAMESPACE_USER, name, value, size);
}
inline int fremovexattr_(int fd, const char *name) {
return extattr_delete_fd(fd, EXTATTR_NAMESPACE_USER, name);
}


#include <sys/syscall.h>
extern "C" {
extern int __sys_kmq_notify(int, const struct sigevent *);
extern int __sys_kmq_open(const char *, int, mode_t, const struct mq_attr *);
extern int __sys_kmq_setattr(int, const struct mq_attr *__restrict, struct mq_attr *__restrict);
extern ssize_t __sys_kmq_timedreceive(int, char *__restrict, size_t,
unsigned *__restrict, const struct timespec *__restrict);
extern int __sys_kmq_timedsend(int, const char *, size_t, unsigned,
const struct timespec *);
extern int __sys_kmq_unlink(const char *);
}
#define mq_notify_ __sys_kmq_notify
#define mq_open_ __sys_kmq_open
#define mq_setattr_ __sys_kmq_setattr
#define mq_getattr_(A, B) __sys_kmq_setattr(A, NULL, B)
#define mq_timedreceive_ __sys_kmq_timedreceive
#define mq_timedsend_ __sys_kmq_timedsend
#define mq_unlink_ __sys_kmq_unlink
#define mq_close_ close
#include <sys/ptrace.h>
inline long ptrace_(int request, pid_t pid, void *addr, void *data) {
return ptrace(request, pid, (caddr_t)addr, static_cast<int>((long)data));
}
#define PTRACE_PEEKDATA_ PT_READ_D
#define getegid_ getegid
#define getgid_ getgid
#define geteuid_ geteuid
#define getuid_ getuid
#define getgroups_ getgroups
#define getrlimit_ getrlimit
#define bind_ bind
#define connect_ connect


#if __FreeBSD_version >= 1000000
#define HAVE_CHFLAGSAT
#define HAVE_BINDAT
#define HAVE_CONNECTAT
#endif
#define HAVE_CHFLAGS
#define HAVE_GETFSSTAT
#define HAVE_REVOKE
#define HAVE_GETLOGIN
#define HAVE_MKFIFOAT
#define HAVE_SYSARCH
#include <machine/sysarch.h>
#define HAVE_STAT_BIRTHTIME
#define HAVE_SYSCTL
#define HAVE_FPATHCONF
#define HAVE_F_DUP2FD
#define HAVE_PSELECT
#define HAVE_SCTP


#define MLOCK_REQUIRES_ROOT 1

#define SCHED_SETSCHEDULER_REQUIRES_ROOT 1

#endif




#if defined(__linux__)
#include <fcntl.h>
#include <unistd.h>
#include <sys/prctl.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <sys/sendfile.h>
#include <sys/statfs.h>
#include <sys/xattr.h>
#include <sys/mount.h>
#include <linux/net.h>


#define profil_arg1_t unsigned short

static inline int getdents_(unsigned int fd, void *dirp, unsigned int count) {
return syscall(__NR_getdents, fd, dirp, count);
}

static inline int bogus_mount_() {
return mount("/dev/bogus", "/bogus", "debugfs", MS_RDONLY, "");
}



static inline pid_t getpid_() {
return syscall(__NR_getpid);
}
static inline int execveat(int fd, const char *path,
char *const argv[], char *const envp[], int flags) {
return syscall(__NR_execveat, fd, path, argv, envp, flags);
}





static inline int fexecve_(int fd, char *const argv[], char *const envp[]) {
return execveat(fd, "", argv, envp, AT_EMPTY_PATH);
}




static inline gid_t getegid_(void) { return syscall(__NR_getegid); }
static inline gid_t getgid_(void) { return syscall(__NR_getgid); }
static inline uid_t geteuid_(void) { return syscall(__NR_geteuid); }
static inline uid_t getuid_(void) { return syscall(__NR_getuid); }
static inline int getgroups_(int size, gid_t list[]) { return syscall(__NR_getgroups, size, list); }
static inline int getrlimit_(int resource, struct rlimit *rlim) {
return syscall(__NR_getrlimit, resource, rlim);
}





#if defined(__NR_bind)
#define bind_ bind
#else
static inline int bind_(int sockfd, const struct sockaddr *addr, socklen_t addrlen) {
unsigned long args[3] = {(unsigned long)sockfd, (unsigned long)(intptr_t)addr, (unsigned long)addrlen};
return syscall(__NR_socketcall, SYS_BIND, args);
}
#endif
#if defined(__NR_connect)
#define connect_ connect
#else
static inline int connect_(int sockfd, const struct sockaddr *addr, socklen_t addrlen) {
unsigned long args[3] = {(unsigned long)sockfd, (unsigned long)(intptr_t)addr, (unsigned long)addrlen};
return syscall(__NR_socketcall, SYS_CONNECT, args);
}
#endif

#define mincore_ mincore
#define sendfile_ sendfile
#define flistxattr_ flistxattr
#define fgetxattr_ fgetxattr
#define fsetxattr_ fsetxattr
#define fremovexattr_ fremovexattr
#define mq_notify_ mq_notify
#define mq_open_ mq_open
#define mq_setattr_ mq_setattr
#define mq_getattr_ mq_getattr
#define mq_timedreceive_ mq_timedreceive
#define mq_timedsend_ mq_timedsend
#define mq_unlink_ mq_unlink
#define mq_close_ mq_close
#define ptrace_ ptrace
#define PTRACE_PEEKDATA_ PTRACE_PEEKDATA


#define HAVE_DUP3
#define HAVE_PIPE2
#include <sys/fsuid.h>
#define HAVE_SETFSUID
#define HAVE_SETFSGID
#define HAVE_READAHEAD
#define HAVE_SEND_RECV_MMSG
#define HAVE_SYNCFS
#define HAVE_SYNC_FILE_RANGE
#include <sys/uio.h>
#define HAVE_TEE
#define HAVE_SPLICE
#define HAVE_VMSPLICE
#define HAVE_PSELECT
#define HAVE_PPOLL
#define HAVE_EXECVEAT
#define HAVE_SYSCALL
#define HAVE_MKNOD_REG
#define HAVE_MKNOD_SOCKET






#if !defined(O_BENEATH)
#define O_BENEATH 040000000
#endif



#define MLOCK_REQUIRES_ROOT 0

#define SCHED_SETSCHEDULER_REQUIRES_ROOT 1

#endif

#endif
