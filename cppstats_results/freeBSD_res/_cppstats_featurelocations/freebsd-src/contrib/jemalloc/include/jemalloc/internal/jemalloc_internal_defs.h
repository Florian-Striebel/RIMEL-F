
#if !defined(JEMALLOC_INTERNAL_DEFS_H_)
#define JEMALLOC_INTERNAL_DEFS_H_


















#define JEMALLOC_OVERRIDE___POSIX_MEMALIGN







#define JEMALLOC_PRIVATE_NAMESPACE __je_





#define CPU_SPINWAIT __asm__ volatile("pause")

#define HAVE_CPU_SPINWAIT 1






#define LG_VADDR 48


#define JEMALLOC_C11_ATOMICS 1


#define JEMALLOC_GCC_ATOMIC_ATOMICS 1

#define JEMALLOC_GCC_U8_ATOMIC_ATOMICS 1


#define JEMALLOC_GCC_SYNC_ATOMICS 1

#define JEMALLOC_GCC_U8_SYNC_ATOMICS 1




#define JEMALLOC_HAVE_BUILTIN_CLZ







#define JEMALLOC_USE_SYSCALL









#define JEMALLOC_HAVE_ISSETUGID


#define JEMALLOC_HAVE_PTHREAD_ATFORK












#define JEMALLOC_HAVE_CLOCK_MONOTONIC 1













#define JEMALLOC_MALLOC_THREAD_CLEANUP













#define JEMALLOC_MUTEX_INIT_CB 1


#define JEMALLOC_TLS_MODEL __attribute__((tls_model("initial-exec")))








#define JEMALLOC_STATS




















#define JEMALLOC_DSS


#define JEMALLOC_FILL


#define JEMALLOC_UTRACE


#define JEMALLOC_XMALLOC


#define JEMALLOC_LAZY_LOCK








#define LG_PAGE 12






#define LG_HUGEPAGE 21








#define JEMALLOC_MAPS_COALESCE










#define JEMALLOC_TLS





#define JEMALLOC_INTERNAL_UNREACHABLE __builtin_unreachable





#define JEMALLOC_INTERNAL_FFSLL __builtin_ffsll
#define JEMALLOC_INTERNAL_FFSL __builtin_ffsl
#define JEMALLOC_INTERNAL_FFS __builtin_ffs




#define JEMALLOC_INTERNAL_POPCOUNTL __builtin_popcountl
#define JEMALLOC_INTERNAL_POPCOUNT __builtin_popcount





#define JEMALLOC_CACHE_OBLIVIOUS
























#define JEMALLOC_SYSCTL_VM_OVERCOMMIT



#define JEMALLOC_HAVE_MADVISE




















#define JEMALLOC_PURGE_MADVISE_FREE
#define JEMALLOC_PURGE_MADVISE_DONTNEED




















#define JEMALLOC_HAS_RESTRICT 1





#define LG_SIZEOF_INT 2


#define LG_SIZEOF_LONG 3


#define LG_SIZEOF_LONG_LONG 3


#define LG_SIZEOF_INTMAX_T 3








#define JEMALLOC_HAVE_PTHREAD


#define JEMALLOC_HAVE_DLSYM


#define JEMALLOC_HAVE_PTHREAD_MUTEX_ADAPTIVE_NP










#define JEMALLOC_BACKGROUND_THREAD 1








#define JEMALLOC_CONFIG_MALLOC_CONF "abort_conf:false"


#define JEMALLOC_IS_MALLOC 1









#endif
