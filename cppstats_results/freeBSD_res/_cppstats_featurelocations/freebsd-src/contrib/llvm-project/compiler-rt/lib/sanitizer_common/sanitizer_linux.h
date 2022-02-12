










#if !defined(SANITIZER_LINUX_H)
#define SANITIZER_LINUX_H

#include "sanitizer_platform.h"
#if SANITIZER_FREEBSD || SANITIZER_LINUX || SANITIZER_NETBSD || SANITIZER_SOLARIS

#include "sanitizer_common.h"
#include "sanitizer_internal_defs.h"
#include "sanitizer_platform_limits_freebsd.h"
#include "sanitizer_platform_limits_netbsd.h"
#include "sanitizer_platform_limits_posix.h"
#include "sanitizer_platform_limits_solaris.h"
#include "sanitizer_posix.h"

struct link_map;
struct utsname;

namespace __sanitizer {


struct linux_dirent;

struct ProcSelfMapsBuff {
char *data;
uptr mmaped_size;
uptr len;
};

struct MemoryMappingLayoutData {
ProcSelfMapsBuff proc_self_maps;
const char *current;
};

void ReadProcMaps(ProcSelfMapsBuff *proc_maps);


uptr internal_getdents(fd_t fd, struct linux_dirent *dirp, unsigned int count);
uptr internal_sigaltstack(const void* ss, void* oss);
uptr internal_sigprocmask(int how, __sanitizer_sigset_t *set,
__sanitizer_sigset_t *oldset);
#if SANITIZER_GLIBC
uptr internal_clock_gettime(__sanitizer_clockid_t clk_id, void *tp);
#endif


#if SANITIZER_LINUX
uptr internal_prctl(int option, uptr arg2, uptr arg3, uptr arg4, uptr arg5);



int internal_sigaction_norestorer(int signum, const void *act, void *oldact);
void internal_sigdelset(__sanitizer_sigset_t *set, int signum);
#if defined(__x86_64__) || defined(__mips__) || defined(__aarch64__) || defined(__powerpc64__) || defined(__s390__) || defined(__i386__) || defined(__arm__) || SANITIZER_RISCV64


uptr internal_clone(int (*fn)(void *), void *child_stack, int flags, void *arg,
int *parent_tidptr, void *newtls, int *child_tidptr);
#endif
int internal_uname(struct utsname *buf);
#elif SANITIZER_FREEBSD
void internal_sigdelset(__sanitizer_sigset_t *set, int signum);
#elif SANITIZER_NETBSD
void internal_sigdelset(__sanitizer_sigset_t *set, int signum);
uptr internal_clone(int (*fn)(void *), void *child_stack, int flags, void *arg);
#endif


class ThreadLister {
public:
explicit ThreadLister(pid_t pid);
~ThreadLister();
enum Result {
Error,
Incomplete,
Ok,
};
Result ListThreads(InternalMmapVector<tid_t> *threads);

private:
bool IsAlive(int tid);

pid_t pid_;
int descriptor_ = -1;
InternalMmapVector<char> buffer_;
};


uptr ThreadDescriptorSize();
uptr ThreadSelf();



bool LibraryNameIs(const char *full_name, const char *base_name);


void ForEachMappedRegion(link_map *map, void (*cb)(const void *, uptr));




inline void ReleaseMemoryPagesToOSAndZeroFill(uptr beg, uptr end) {



CHECK(SANITIZER_LINUX);
ReleaseMemoryPagesToOS(beg, end);
}

#if SANITIZER_ANDROID

#if defined(__aarch64__)
#define __get_tls() ({ void** __v; __asm__("mrs %0, tpidr_el0" : "=r"(__v)); __v; })

#elif defined(__arm__)
#define __get_tls() ({ void** __v; __asm__("mrc p15, 0, %0, c13, c0, 3" : "=r"(__v)); __v; })

#elif defined(__mips__)


#define __get_tls() ({ register void** __v asm("v1"); __asm__(".set push\n" ".set mips32r2\n" "rdhwr %0,$29\n" ".set pop\n" : "=r"(__v)); __v; })






#elif defined(__i386__)
#define __get_tls() ({ void** __v; __asm__("movl %%gs:0, %0" : "=r"(__v)); __v; })

#elif defined(__x86_64__)
#define __get_tls() ({ void** __v; __asm__("mov %%fs:0, %0" : "=r"(__v)); __v; })

#else
#error "Unsupported architecture."
#endif




static const int TLS_SLOT_SANITIZER = 6;

ALWAYS_INLINE uptr *get_android_tls_ptr() {
return reinterpret_cast<uptr *>(&__get_tls()[TLS_SLOT_SANITIZER]);
}

#endif

}

#endif
#endif
