
















#define DEFAULT_EDITOR_PROGRAM "/usr/bin/vi"















#define ENABLE_NLS 0




#define FILESYSTEM_ACCEPTS_DRIVE_LETTER_PREFIX 0



#define FILESYSTEM_BACKSLASH_IS_FILE_NAME_SEPARATOR 0

#if FILESYSTEM_ACCEPTS_DRIVE_LETTER_PREFIX
#define FILESYSTEM_PREFIX_LEN(Filename) ((Filename)[0] && (Filename)[1] == ':' ? 2 : 0)

#else
#define FILESYSTEM_PREFIX_LEN(Filename) 0
#endif





#define HAVE_ALLOCA 1






#define HAVE_BTOWC 1


#define HAVE_CLOCK_GETTIME 1


#define HAVE_C_VARARRAYS 1







#define HAVE_DECL_CLEARERR_UNLOCKED 1



#define HAVE_DECL_FEOF_UNLOCKED 1



#define HAVE_DECL_FERROR_UNLOCKED 1



#define HAVE_DECL_FFLUSH_UNLOCKED 1



#define HAVE_DECL_FGETS_UNLOCKED 0



#define HAVE_DECL_FPUTC_UNLOCKED 1



#define HAVE_DECL_FPUTS_UNLOCKED 1



#define HAVE_DECL_FREAD_UNLOCKED 1



#define HAVE_DECL_FWRITE_UNLOCKED 1



#define HAVE_DECL_GETCHAR_UNLOCKED 1



#define HAVE_DECL_GETCONTEXT 1



#define HAVE_DECL_GETC_UNLOCKED 1



#define HAVE_DECL_GETENV 1



#define HAVE_DECL_PUTCHAR_UNLOCKED 1



#define HAVE_DECL_PUTC_UNLOCKED 1



#define HAVE_DECL_SIGALTSTACK 1



#define HAVE_DECL_STRERROR_R 1



#define HAVE_DECL_STRTOIMAX 1



#define HAVE_DECL_STRTOLL 1



#define HAVE_DECL_STRTOULL 1



#define HAVE_DECL_STRTOUMAX 1



#define HAVE_DIRENT_H 1


#define HAVE_DUP2 1


#define HAVE_FCNTL_H 1


#define HAVE_FORK 1


#define HAVE_GETCONTEXT 1





#define HAVE_GETTIMEOFDAY 1





#define HAVE_INTMAX_T 1


#define HAVE_INTTYPES_H 1



#define HAVE_INTTYPES_H_WITH_UINTMAX 1


#define HAVE_ISASCII 1


#define HAVE_LOCALE_H 1


#define HAVE_LONG_LONG 1



#define HAVE_MALLOC 1


#define HAVE_MBLEN 1


#define HAVE_MBRLEN 1


#define HAVE_MBSRTOWCS 1


#define HAVE_MBSTATE_T 1


#define HAVE_MEMORY_H 1





#define HAVE_MKSTEMP 1






#define HAVE_REALLOC 1


#define HAVE_RUN_TZSET_TEST 1


#define HAVE_SETLOCALE 1





#define HAVE_SETRLIMIT 1


#define HAVE_SIGACTION 1


#define HAVE_SIGALTSTACK 1


#define HAVE_SIGPROCMASK 1


#define HAVE_STACK_T 1


#define HAVE_STDBOOL_H 1


#define HAVE_STDINT_H 1



#define HAVE_STDINT_H_WITH_UINTMAX 1


#define HAVE_STDLIB_H 1


#define HAVE_STRCASECMP 1





#define HAVE_STRERROR_R 1


#define HAVE_STRFTIME 1





#define HAVE_STRINGS_H 1


#define HAVE_STRING_H 1


#define HAVE_STRNCASECMP 1


#define HAVE_STRTOIMAX 1


#define HAVE_STRTOL 1


#define HAVE_STRTOLL 1


#define HAVE_STRTOUL 1


#define HAVE_STRTOULL 1


#define HAVE_STRTOUMAX 1


#define HAVE_STRUCT_SIGACTION_SA_SIGACTION 1


#define HAVE_STRUCT_STAT_ST_BLKSIZE 1


#define HAVE_STRUCT_STAT_ST_RDEV 1


#define HAVE_STRUCT_TM_TM_ZONE 1






#define HAVE_SYS_FILE_H 1






#define HAVE_SYS_RESOURCE_H 1


#define HAVE_SYS_STAT_H 1


#define HAVE_SYS_TIME_H 1


#define HAVE_SYS_TYPES_H 1


#define HAVE_SYS_WAIT_H 1



#define HAVE_TIME_R_POSIX 1


#define HAVE_TM_GMTOFF 1



#define HAVE_TM_ZONE 1






#define HAVE_TZSET 1


#define HAVE_UCONTEXT_H 1


#define HAVE_UINTMAX_T 1


#define HAVE_UNISTD_H 1


#define HAVE_UNSIGNED_LONG_LONG 1


#define HAVE_VFORK 1





#define HAVE_WAITPID 1


#define HAVE_WCHAR_H 1


#define HAVE_WCTYPE_H 1





#define HAVE_WORKING_FORK 1


#define HAVE_WORKING_VFORK 1










#define HAVE__BOOL 1




#if FILESYSTEM_BACKSLASH_IS_FILE_NAME_SEPARATOR
#define ISSLASH(C) ((C) == '/' || (C) == '\\')
#else
#define ISSLASH(C) ((C) == '/')
#endif


#define PACKAGE "diffutils"


#define PACKAGE_BUGREPORT "bug-gnu-utils@gnu.org"


#define PACKAGE_NAME "GNU diffutils"


#define PACKAGE_STRING "GNU diffutils 2.8.7"


#define PACKAGE_TARNAME "diffutils"


#define PACKAGE_VERSION "2.8.7"


#define PR_PROGRAM "/usr/bin/pr"


#define REGEX_MALLOC 1













#define STDC_HEADERS 1






#define ST_MTIM_NSEC tv_nsec


#define TIME_WITH_SYS_TIME 1








#define VERSION "2.8.7"




#if !defined(_ALL_SOURCE)

#endif





#if !defined(_GNU_SOURCE)
#define _GNU_SOURCE 1
#endif















#if !defined(__EXTENSIONS__)
#define __EXTENSIONS__ 1
#endif















#if !defined(__cplusplus)

#endif

















#define my_strftime nstrftime












#define restrict __restrict










