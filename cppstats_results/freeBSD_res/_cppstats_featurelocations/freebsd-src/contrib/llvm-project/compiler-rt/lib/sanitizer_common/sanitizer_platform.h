










#if !defined(SANITIZER_PLATFORM_H)
#define SANITIZER_PLATFORM_H

#if !defined(__linux__) && !defined(__FreeBSD__) && !defined(__NetBSD__) && !defined(__APPLE__) && !defined(_WIN32) && !defined(__Fuchsia__) && !(defined(__sun__) && defined(__svr4__))


#error "This operating system is not supported"
#endif




#if __has_include(<features.h>) && !defined(__ANDROID__)
#include <features.h>
#endif

#if defined(__linux__)
#define SANITIZER_LINUX 1
#else
#define SANITIZER_LINUX 0
#endif

#if defined(__GLIBC__)
#define SANITIZER_GLIBC 1
#else
#define SANITIZER_GLIBC 0
#endif

#if defined(__FreeBSD__)
#define SANITIZER_FREEBSD 1
#else
#define SANITIZER_FREEBSD 0
#endif

#if defined(__NetBSD__)
#define SANITIZER_NETBSD 1
#else
#define SANITIZER_NETBSD 0
#endif

#if defined(__sun__) && defined(__svr4__)
#define SANITIZER_SOLARIS 1
#else
#define SANITIZER_SOLARIS 0
#endif

#if defined(__APPLE__)
#define SANITIZER_MAC 1
#include <TargetConditionals.h>
#if TARGET_OS_OSX
#define SANITIZER_OSX 1
#else
#define SANITIZER_OSX 0
#endif
#if TARGET_OS_IPHONE
#define SANITIZER_IOS 1
#else
#define SANITIZER_IOS 0
#endif
#if TARGET_OS_SIMULATOR
#define SANITIZER_IOSSIM 1
#else
#define SANITIZER_IOSSIM 0
#endif
#else
#define SANITIZER_MAC 0
#define SANITIZER_IOS 0
#define SANITIZER_IOSSIM 0
#define SANITIZER_OSX 0
#endif

#if defined(__APPLE__) && TARGET_OS_IPHONE && TARGET_OS_WATCH
#define SANITIZER_WATCHOS 1
#else
#define SANITIZER_WATCHOS 0
#endif

#if defined(__APPLE__) && TARGET_OS_IPHONE && TARGET_OS_TV
#define SANITIZER_TVOS 1
#else
#define SANITIZER_TVOS 0
#endif

#if defined(_WIN32)
#define SANITIZER_WINDOWS 1
#else
#define SANITIZER_WINDOWS 0
#endif

#if defined(_WIN64)
#define SANITIZER_WINDOWS64 1
#else
#define SANITIZER_WINDOWS64 0
#endif

#if defined(__ANDROID__)
#define SANITIZER_ANDROID 1
#else
#define SANITIZER_ANDROID 0
#endif

#if defined(__Fuchsia__)
#define SANITIZER_FUCHSIA 1
#else
#define SANITIZER_FUCHSIA 0
#endif

#define SANITIZER_POSIX (SANITIZER_FREEBSD || SANITIZER_LINUX || SANITIZER_MAC || SANITIZER_NETBSD || SANITIZER_SOLARIS)



#if __LP64__ || defined(_WIN64)
#define SANITIZER_WORDSIZE 64
#else
#define SANITIZER_WORDSIZE 32
#endif

#if SANITIZER_WORDSIZE == 64
#define FIRST_32_SECOND_64(a, b) (b)
#else
#define FIRST_32_SECOND_64(a, b) (a)
#endif

#if defined(__x86_64__) && !defined(_LP64)
#define SANITIZER_X32 1
#else
#define SANITIZER_X32 0
#endif

#if defined(__i386__) || defined(_M_IX86)
#define SANITIZER_I386 1
#else
#define SANITIZER_I386 0
#endif

#if defined(__mips__)
#define SANITIZER_MIPS 1
#if defined(__mips64)
#define SANITIZER_MIPS32 0
#define SANITIZER_MIPS64 1
#else
#define SANITIZER_MIPS32 1
#define SANITIZER_MIPS64 0
#endif
#else
#define SANITIZER_MIPS 0
#define SANITIZER_MIPS32 0
#define SANITIZER_MIPS64 0
#endif

#if defined(__s390__)
#define SANITIZER_S390 1
#if defined(__s390x__)
#define SANITIZER_S390_31 0
#define SANITIZER_S390_64 1
#else
#define SANITIZER_S390_31 1
#define SANITIZER_S390_64 0
#endif
#else
#define SANITIZER_S390 0
#define SANITIZER_S390_31 0
#define SANITIZER_S390_64 0
#endif

#if defined(__powerpc__)
#define SANITIZER_PPC 1
#if defined(__powerpc64__)
#define SANITIZER_PPC32 0
#define SANITIZER_PPC64 1







#if _CALL_ELF == 2
#define SANITIZER_PPC64V1 0
#define SANITIZER_PPC64V2 1
#else
#define SANITIZER_PPC64V1 1
#define SANITIZER_PPC64V2 0
#endif
#else
#define SANITIZER_PPC32 1
#define SANITIZER_PPC64 0
#define SANITIZER_PPC64V1 0
#define SANITIZER_PPC64V2 0
#endif
#else
#define SANITIZER_PPC 0
#define SANITIZER_PPC32 0
#define SANITIZER_PPC64 0
#define SANITIZER_PPC64V1 0
#define SANITIZER_PPC64V2 0
#endif

#if defined(__arm__)
#define SANITIZER_ARM 1
#else
#define SANITIZER_ARM 0
#endif

#if SANITIZER_SOLARIS && SANITIZER_WORDSIZE == 32
#define SANITIZER_SOLARIS32 1
#else
#define SANITIZER_SOLARIS32 0
#endif

#if defined(__riscv) && (__riscv_xlen == 64)
#define SANITIZER_RISCV64 1
#else
#define SANITIZER_RISCV64 0
#endif






#if !defined(SANITIZER_CAN_USE_ALLOCATOR64)
#if (SANITIZER_ANDROID && defined(__aarch64__)) || SANITIZER_FUCHSIA
#define SANITIZER_CAN_USE_ALLOCATOR64 1
#elif defined(__mips64) || defined(__aarch64__)
#define SANITIZER_CAN_USE_ALLOCATOR64 0
#else
#define SANITIZER_CAN_USE_ALLOCATOR64 (SANITIZER_WORDSIZE == 64)
#endif
#endif




#if defined(__mips__)
#if SANITIZER_GO && defined(__mips64)
#define SANITIZER_MMAP_RANGE_SIZE FIRST_32_SECOND_64(1ULL << 32, 1ULL << 47)
#else
#define SANITIZER_MMAP_RANGE_SIZE FIRST_32_SECOND_64(1ULL << 32, 1ULL << 40)
#endif
#elif SANITIZER_RISCV64
#define SANITIZER_MMAP_RANGE_SIZE FIRST_32_SECOND_64(1ULL << 32, 1ULL << 38)
#elif defined(__aarch64__)
#if SANITIZER_MAC
#if SANITIZER_OSX || SANITIZER_IOSSIM
#define SANITIZER_MMAP_RANGE_SIZE FIRST_32_SECOND_64(1ULL << 32, 1ULL << 47)
#else

#define SANITIZER_MMAP_RANGE_SIZE FIRST_32_SECOND_64(1ULL << 32, 1ULL << 36)
#endif
#else
#define SANITIZER_MMAP_RANGE_SIZE FIRST_32_SECOND_64(1ULL << 32, 1ULL << 48)
#endif
#elif defined(__sparc__)
#define SANITIZER_MMAP_RANGE_SIZE FIRST_32_SECOND_64(1ULL << 32, 1ULL << 52)
#else
#define SANITIZER_MMAP_RANGE_SIZE FIRST_32_SECOND_64(1ULL << 32, 1ULL << 47)
#endif




#if defined(__sparc__) && SANITIZER_WORDSIZE == 64
#define SANITIZER_SIGN_EXTENDED_ADDRESSES 1
#else
#define SANITIZER_SIGN_EXTENDED_ADDRESSES 0
#endif




#if !defined(SANITIZER_USES_CANONICAL_LINUX_SYSCALLS)
#if (defined(__aarch64__) || defined(__riscv)) && SANITIZER_LINUX
#define SANITIZER_USES_CANONICAL_LINUX_SYSCALLS 1
#else
#define SANITIZER_USES_CANONICAL_LINUX_SYSCALLS 0
#endif
#endif








#if defined(__arm__) || SANITIZER_X32 || defined(__sparc__)
#define SANITIZER_USES_UID16_SYSCALLS 1
#else
#define SANITIZER_USES_UID16_SYSCALLS 0
#endif

#if defined(__mips__)
#define SANITIZER_POINTER_FORMAT_LENGTH FIRST_32_SECOND_64(8, 10)
#else
#define SANITIZER_POINTER_FORMAT_LENGTH FIRST_32_SECOND_64(8, 12)
#endif






#if defined(_MSC_VER)
#define MSC_PREREQ(version) (_MSC_VER >= (version))
#else
#define MSC_PREREQ(version) 0
#endif

#if SANITIZER_MAC && !(defined(__arm64__) && SANITIZER_IOS)
#define SANITIZER_NON_UNIQUE_TYPEINFO 0
#else
#define SANITIZER_NON_UNIQUE_TYPEINFO 1
#endif





#if SANITIZER_LINUX && (SANITIZER_S390 || SANITIZER_PPC32 || SANITIZER_PPC64V1)
#define SANITIZER_NLDBL_VERSION "GLIBC_2.4"
#endif

#if SANITIZER_GO == 0
#define SANITIZER_GO 0
#endif





#if SANITIZER_PPC || defined(__thumb__)
#define SANITIZER_SUPPRESS_LEAK_ON_PTHREAD_EXIT 1
#else
#define SANITIZER_SUPPRESS_LEAK_ON_PTHREAD_EXIT 0
#endif

#if SANITIZER_FREEBSD || SANITIZER_MAC || SANITIZER_NETBSD || SANITIZER_SOLARIS

#define SANITIZER_MADVISE_DONTNEED MADV_FREE
#else
#define SANITIZER_MADVISE_DONTNEED MADV_DONTNEED
#endif



#if defined(__powerpc__) || defined(__powerpc64__)
#define SANITIZER_CACHE_LINE_SIZE 128
#else
#define SANITIZER_CACHE_LINE_SIZE 64
#endif


#if SANITIZER_FUCHSIA
#define SANITIZER_SYMBOLIZER_MARKUP 1
#else
#define SANITIZER_SYMBOLIZER_MARKUP 0
#endif




#if SANITIZER_MAC
#define SANITIZER_SUPPORTS_INIT_FOR_DLOPEN 1
#else
#define SANITIZER_SUPPORTS_INIT_FOR_DLOPEN 0
#endif

#endif
