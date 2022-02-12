#include "fmacros.h"
#include "config.h"
#include "solarisfixes.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <assert.h>
#if defined(__linux__)
#include <sys/mman.h>
#endif
void zlibc_free(void *ptr) {
free(ptr);
}
#include <string.h>
#include <pthread.h>
#include "zmalloc.h"
#include "atomicvar.h"
#if defined(HAVE_MALLOC_SIZE)
#define PREFIX_SIZE (0)
#define ASSERT_NO_SIZE_OVERFLOW(sz)
#else
#if defined(__sun) || defined(__sparc) || defined(__sparc__)
#define PREFIX_SIZE (sizeof(long long))
#else
#define PREFIX_SIZE (sizeof(size_t))
#endif
#define ASSERT_NO_SIZE_OVERFLOW(sz) assert((sz) + PREFIX_SIZE > (sz))
#endif
#define MALLOC_MIN_SIZE(x) ((x) > 0 ? (x) : sizeof(long))
#if defined(USE_TCMALLOC)
#define malloc(size) tc_malloc(size)
#define calloc(count,size) tc_calloc(count,size)
#define realloc(ptr,size) tc_realloc(ptr,size)
#define free(ptr) tc_free(ptr)
#elif defined(USE_JEMALLOC)
#define malloc(size) je_malloc(size)
#define calloc(count,size) je_calloc(count,size)
#define realloc(ptr,size) je_realloc(ptr,size)
#define free(ptr) je_free(ptr)
#define mallocx(size,flags) je_mallocx(size,flags)
#define dallocx(ptr,flags) je_dallocx(ptr,flags)
#endif
#define update_zmalloc_stat_alloc(__n) atomicIncr(used_memory,(__n))
#define update_zmalloc_stat_free(__n) atomicDecr(used_memory,(__n))
static redisAtomic size_t used_memory = 0;
static void zmalloc_default_oom(size_t size) {
fprintf(stderr, "zmalloc: Out of memory trying to allocate %zu bytes\n",
size);
fflush(stderr);
abort();
}
static void (*zmalloc_oom_handler)(size_t) = zmalloc_default_oom;
void *ztrymalloc_usable(size_t size, size_t *usable) {
ASSERT_NO_SIZE_OVERFLOW(size);
void *ptr = malloc(MALLOC_MIN_SIZE(size)+PREFIX_SIZE);
if (!ptr) return NULL;
#if defined(HAVE_MALLOC_SIZE)
size = zmalloc_size(ptr);
update_zmalloc_stat_alloc(size);
if (usable) *usable = size;
return ptr;
#else
*((size_t*)ptr) = size;
update_zmalloc_stat_alloc(size+PREFIX_SIZE);
if (usable) *usable = size;
return (char*)ptr+PREFIX_SIZE;
#endif
}
void *zmalloc(size_t size) {
void *ptr = ztrymalloc_usable(size, NULL);
if (!ptr) zmalloc_oom_handler(size);
return ptr;
}
void *ztrymalloc(size_t size) {
void *ptr = ztrymalloc_usable(size, NULL);
return ptr;
}
void *zmalloc_usable(size_t size, size_t *usable) {
void *ptr = ztrymalloc_usable(size, usable);
if (!ptr) zmalloc_oom_handler(size);
return ptr;
}
#if defined(HAVE_DEFRAG)
void *zmalloc_no_tcache(size_t size) {
ASSERT_NO_SIZE_OVERFLOW(size);
void *ptr = mallocx(size+PREFIX_SIZE, MALLOCX_TCACHE_NONE);
if (!ptr) zmalloc_oom_handler(size);
update_zmalloc_stat_alloc(zmalloc_size(ptr));
return ptr;
}
void zfree_no_tcache(void *ptr) {
if (ptr == NULL) return;
update_zmalloc_stat_free(zmalloc_size(ptr));
dallocx(ptr, MALLOCX_TCACHE_NONE);
}
#endif
void *ztrycalloc_usable(size_t size, size_t *usable) {
ASSERT_NO_SIZE_OVERFLOW(size);
void *ptr = calloc(1, MALLOC_MIN_SIZE(size)+PREFIX_SIZE);
if (ptr == NULL) return NULL;
#if defined(HAVE_MALLOC_SIZE)
size = zmalloc_size(ptr);
update_zmalloc_stat_alloc(size);
if (usable) *usable = size;
return ptr;
#else
*((size_t*)ptr) = size;
update_zmalloc_stat_alloc(size+PREFIX_SIZE);
if (usable) *usable = size;
return (char*)ptr+PREFIX_SIZE;
#endif
}
void *zcalloc_num(size_t num, size_t size) {
if ((size == 0) || (num > SIZE_MAX/size)) {
zmalloc_oom_handler(SIZE_MAX);
return NULL;
}
void *ptr = ztrycalloc_usable(num*size, NULL);
if (!ptr) zmalloc_oom_handler(num*size);
return ptr;
}
void *zcalloc(size_t size) {
void *ptr = ztrycalloc_usable(size, NULL);
if (!ptr) zmalloc_oom_handler(size);
return ptr;
}
void *ztrycalloc(size_t size) {
void *ptr = ztrycalloc_usable(size, NULL);
return ptr;
}
void *zcalloc_usable(size_t size, size_t *usable) {
void *ptr = ztrycalloc_usable(size, usable);
if (!ptr) zmalloc_oom_handler(size);
return ptr;
}
void *ztryrealloc_usable(void *ptr, size_t size, size_t *usable) {
ASSERT_NO_SIZE_OVERFLOW(size);
#if !defined(HAVE_MALLOC_SIZE)
void *realptr;
#endif
size_t oldsize;
void *newptr;
if (size == 0 && ptr != NULL) {
zfree(ptr);
if (usable) *usable = 0;
return NULL;
}
if (ptr == NULL)
return ztrymalloc_usable(size, usable);
#if defined(HAVE_MALLOC_SIZE)
oldsize = zmalloc_size(ptr);
newptr = realloc(ptr,size);
if (newptr == NULL) {
if (usable) *usable = 0;
return NULL;
}
update_zmalloc_stat_free(oldsize);
size = zmalloc_size(newptr);
update_zmalloc_stat_alloc(size);
if (usable) *usable = size;
return newptr;
#else
realptr = (char*)ptr-PREFIX_SIZE;
oldsize = *((size_t*)realptr);
newptr = realloc(realptr,size+PREFIX_SIZE);
if (newptr == NULL) {
if (usable) *usable = 0;
return NULL;
}
*((size_t*)newptr) = size;
update_zmalloc_stat_free(oldsize);
update_zmalloc_stat_alloc(size);
if (usable) *usable = size;
return (char*)newptr+PREFIX_SIZE;
#endif
}
void *zrealloc(void *ptr, size_t size) {
ptr = ztryrealloc_usable(ptr, size, NULL);
if (!ptr && size != 0) zmalloc_oom_handler(size);
return ptr;
}
void *ztryrealloc(void *ptr, size_t size) {
ptr = ztryrealloc_usable(ptr, size, NULL);
return ptr;
}
void *zrealloc_usable(void *ptr, size_t size, size_t *usable) {
ptr = ztryrealloc_usable(ptr, size, usable);
if (!ptr && size != 0) zmalloc_oom_handler(size);
return ptr;
}
#if !defined(HAVE_MALLOC_SIZE)
size_t zmalloc_size(void *ptr) {
void *realptr = (char*)ptr-PREFIX_SIZE;
size_t size = *((size_t*)realptr);
return size+PREFIX_SIZE;
}
size_t zmalloc_usable_size(void *ptr) {
return zmalloc_size(ptr)-PREFIX_SIZE;
}
#endif
void zfree(void *ptr) {
#if !defined(HAVE_MALLOC_SIZE)
void *realptr;
size_t oldsize;
#endif
if (ptr == NULL) return;
#if defined(HAVE_MALLOC_SIZE)
update_zmalloc_stat_free(zmalloc_size(ptr));
free(ptr);
#else
realptr = (char*)ptr-PREFIX_SIZE;
oldsize = *((size_t*)realptr);
update_zmalloc_stat_free(oldsize+PREFIX_SIZE);
free(realptr);
#endif
}
void zfree_usable(void *ptr, size_t *usable) {
#if !defined(HAVE_MALLOC_SIZE)
void *realptr;
size_t oldsize;
#endif
if (ptr == NULL) return;
#if defined(HAVE_MALLOC_SIZE)
update_zmalloc_stat_free(*usable = zmalloc_size(ptr));
free(ptr);
#else
realptr = (char*)ptr-PREFIX_SIZE;
*usable = oldsize = *((size_t*)realptr);
update_zmalloc_stat_free(oldsize+PREFIX_SIZE);
free(realptr);
#endif
}
char *zstrdup(const char *s) {
size_t l = strlen(s)+1;
char *p = zmalloc(l);
memcpy(p,s,l);
return p;
}
size_t zmalloc_used_memory(void) {
size_t um;
atomicGet(used_memory,um);
return um;
}
void zmalloc_set_oom_handler(void (*oom_handler)(size_t)) {
zmalloc_oom_handler = oom_handler;
}
void zmadvise_dontneed(void *ptr) {
#if defined(USE_JEMALLOC) && defined(__linux__)
static size_t page_size = 0;
if (page_size == 0) page_size = sysconf(_SC_PAGESIZE);
size_t page_size_mask = page_size - 1;
size_t real_size = zmalloc_size(ptr);
if (real_size < page_size) return;
char *aligned_ptr = (char *)(((size_t)ptr+page_size_mask) & ~page_size_mask);
real_size -= (aligned_ptr-(char*)ptr);
if (real_size >= page_size) {
madvise((void *)aligned_ptr, real_size&~page_size_mask, MADV_DONTNEED);
}
#else
(void)(ptr);
#endif
}
#if defined(HAVE_PROC_STAT)
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
size_t zmalloc_get_rss(void) {
int page = sysconf(_SC_PAGESIZE);
size_t rss;
char buf[4096];
char filename[256];
int fd, count;
char *p, *x;
snprintf(filename,256,"/proc/%ld/stat",(long) getpid());
if ((fd = open(filename,O_RDONLY)) == -1) return 0;
if (read(fd,buf,4096) <= 0) {
close(fd);
return 0;
}
close(fd);
p = buf;
count = 23;
while(p && count--) {
p = strchr(p,' ');
if (p) p++;
}
if (!p) return 0;
x = strchr(p,' ');
if (!x) return 0;
*x = '\0';
rss = strtoll(p,NULL,10);
rss *= page;
return rss;
}
#elif defined(HAVE_TASKINFO)
#include <sys/types.h>
#include <sys/sysctl.h>
#include <mach/task.h>
#include <mach/mach_init.h>
size_t zmalloc_get_rss(void) {
task_t task = MACH_PORT_NULL;
struct task_basic_info t_info;
mach_msg_type_number_t t_info_count = TASK_BASIC_INFO_COUNT;
if (task_for_pid(current_task(), getpid(), &task) != KERN_SUCCESS)
return 0;
task_info(task, TASK_BASIC_INFO, (task_info_t)&t_info, &t_info_count);
return t_info.resident_size;
}
#elif defined(__FreeBSD__) || defined(__DragonFly__)
#include <sys/types.h>
#include <sys/sysctl.h>
#include <sys/user.h>
size_t zmalloc_get_rss(void) {
struct kinfo_proc info;
size_t infolen = sizeof(info);
int mib[4];
mib[0] = CTL_KERN;
mib[1] = KERN_PROC;
mib[2] = KERN_PROC_PID;
mib[3] = getpid();
if (sysctl(mib, 4, &info, &infolen, NULL, 0) == 0)
#if defined(__FreeBSD__)
return (size_t)info.ki_rssize * getpagesize();
#else
return (size_t)info.kp_vm_rssize * getpagesize();
#endif
return 0L;
}
#elif defined(__NetBSD__) || defined(__OpenBSD__)
#include <sys/types.h>
#include <sys/sysctl.h>
#if defined(__OpenBSD__)
#define kinfo_proc2 kinfo_proc
#define KERN_PROC2 KERN_PROC
#define __arraycount(a) (sizeof(a) / sizeof(a[0]))
#endif
size_t zmalloc_get_rss(void) {
struct kinfo_proc2 info;
size_t infolen = sizeof(info);
int mib[6];
mib[0] = CTL_KERN;
mib[1] = KERN_PROC2;
mib[2] = KERN_PROC_PID;
mib[3] = getpid();
mib[4] = sizeof(info);
mib[5] = 1;
if (sysctl(mib, __arraycount(mib), &info, &infolen, NULL, 0) == 0)
return (size_t)info.p_vm_rssize * getpagesize();
return 0L;
}
#elif defined(HAVE_PSINFO)
#include <unistd.h>
#include <sys/procfs.h>
#include <fcntl.h>
size_t zmalloc_get_rss(void) {
struct prpsinfo info;
char filename[256];
int fd;
snprintf(filename,256,"/proc/%ld/psinfo",(long) getpid());
if ((fd = open(filename,O_RDONLY)) == -1) return 0;
if (ioctl(fd, PIOCPSINFO, &info) == -1) {
close(fd);
return 0;
}
close(fd);
return info.pr_rssize;
}
#else
size_t zmalloc_get_rss(void) {
return zmalloc_used_memory();
}
#endif
#if defined(USE_JEMALLOC)
int zmalloc_get_allocator_info(size_t *allocated,
size_t *active,
size_t *resident) {
uint64_t epoch = 1;
size_t sz;
*allocated = *resident = *active = 0;
sz = sizeof(epoch);
je_mallctl("epoch", &epoch, &sz, &epoch, sz);
sz = sizeof(size_t);
je_mallctl("stats.resident", resident, &sz, NULL, 0);
je_mallctl("stats.active", active, &sz, NULL, 0);
je_mallctl("stats.allocated", allocated, &sz, NULL, 0);
return 1;
}
void set_jemalloc_bg_thread(int enable) {
char val = !!enable;
je_mallctl("background_thread", NULL, 0, &val, 1);
}
int jemalloc_purge() {
char tmp[32];
unsigned narenas = 0;
size_t sz = sizeof(unsigned);
if (!je_mallctl("arenas.narenas", &narenas, &sz, NULL, 0)) {
sprintf(tmp, "arena.%d.purge", narenas);
if (!je_mallctl(tmp, NULL, 0, NULL, 0))
return 0;
}
return -1;
}
#else
int zmalloc_get_allocator_info(size_t *allocated,
size_t *active,
size_t *resident) {
*allocated = *resident = *active = 0;
return 1;
}
void set_jemalloc_bg_thread(int enable) {
((void)(enable));
}
int jemalloc_purge() {
return 0;
}
#endif
#if defined(__APPLE__)
#include <libproc.h>
#endif
#if defined(HAVE_PROC_SMAPS)
size_t zmalloc_get_smap_bytes_by_field(char *field, long pid) {
char line[1024];
size_t bytes = 0;
int flen = strlen(field);
FILE *fp;
if (pid == -1) {
fp = fopen("/proc/self/smaps","r");
} else {
char filename[128];
snprintf(filename,sizeof(filename),"/proc/%ld/smaps",pid);
fp = fopen(filename,"r");
}
if (!fp) return 0;
while(fgets(line,sizeof(line),fp) != NULL) {
if (strncmp(line,field,flen) == 0) {
char *p = strchr(line,'k');
if (p) {
*p = '\0';
bytes += strtol(line+flen,NULL,10) * 1024;
}
}
}
fclose(fp);
return bytes;
}
#else
size_t zmalloc_get_smap_bytes_by_field(char *field, long pid) {
#if defined(__APPLE__)
struct proc_regioninfo pri;
if (pid == -1) pid = getpid();
if (proc_pidinfo(pid, PROC_PIDREGIONINFO, 0, &pri,
PROC_PIDREGIONINFO_SIZE) == PROC_PIDREGIONINFO_SIZE)
{
int pagesize = getpagesize();
if (!strcmp(field, "Private_Dirty:")) {
return (size_t)pri.pri_pages_dirtied * pagesize;
} else if (!strcmp(field, "Rss:")) {
return (size_t)pri.pri_pages_resident * pagesize;
} else if (!strcmp(field, "AnonHugePages:")) {
return 0;
}
}
return 0;
#endif
((void) field);
((void) pid);
return 0;
}
#endif
size_t zmalloc_get_private_dirty(long pid) {
return zmalloc_get_smap_bytes_by_field("Private_Dirty:",pid);
}
size_t zmalloc_get_memory_size(void) {
#if defined(__unix__) || defined(__unix) || defined(unix) || (defined(__APPLE__) && defined(__MACH__))
#if defined(CTL_HW) && (defined(HW_MEMSIZE) || defined(HW_PHYSMEM64))
int mib[2];
mib[0] = CTL_HW;
#if defined(HW_MEMSIZE)
mib[1] = HW_MEMSIZE;
#elif defined(HW_PHYSMEM64)
mib[1] = HW_PHYSMEM64;
#endif
int64_t size = 0;
size_t len = sizeof(size);
if (sysctl( mib, 2, &size, &len, NULL, 0) == 0)
return (size_t)size;
return 0L;
#elif defined(_SC_PHYS_PAGES) && defined(_SC_PAGESIZE)
return (size_t)sysconf(_SC_PHYS_PAGES) * (size_t)sysconf(_SC_PAGESIZE);
#elif defined(CTL_HW) && (defined(HW_PHYSMEM) || defined(HW_REALMEM))
int mib[2];
mib[0] = CTL_HW;
#if defined(HW_REALMEM)
mib[1] = HW_REALMEM;
#elif defined(HW_PHYSMEM)
mib[1] = HW_PHYSMEM;
#endif
unsigned int size = 0;
size_t len = sizeof(size);
if (sysctl(mib, 2, &size, &len, NULL, 0) == 0)
return (size_t)size;
return 0L;
#else
return 0L;
#endif
#else
return 0L;
#endif
}
#if defined(REDIS_TEST)
#define UNUSED(x) ((void)(x))
int zmalloc_test(int argc, char **argv, int flags) {
void *ptr;
UNUSED(argc);
UNUSED(argv);
UNUSED(flags);
printf("Malloc prefix size: %d\n", (int) PREFIX_SIZE);
printf("Initial used memory: %zu\n", zmalloc_used_memory());
ptr = zmalloc(123);
printf("Allocated 123 bytes; used: %zu\n", zmalloc_used_memory());
ptr = zrealloc(ptr, 456);
printf("Reallocated to 456 bytes; used: %zu\n", zmalloc_used_memory());
zfree(ptr);
printf("Freed pointer; used: %zu\n", zmalloc_used_memory());
return 0;
}
#endif
