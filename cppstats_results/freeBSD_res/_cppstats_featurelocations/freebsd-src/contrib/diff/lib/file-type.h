



















#if !defined(FILE_TYPE_H)
#define FILE_TYPE_H 1

#if ! defined S_ISREG && ! defined S_IFREG
you must include <sys/stat.h> before including this file
#endif

char const *file_type (struct stat const *);

#if !defined(S_IFMT)
#define S_IFMT 0170000
#endif

#if STAT_MACROS_BROKEN
#undef S_ISBLK
#undef S_ISCHR
#undef S_ISDIR
#undef S_ISDOOR
#undef S_ISFIFO
#undef S_ISLNK
#undef S_ISNAM
#undef S_ISMPB
#undef S_ISMPC
#undef S_ISNWK
#undef S_ISREG
#undef S_ISSOCK
#endif


#if !defined(S_ISBLK)
#if defined(S_IFBLK)
#define S_ISBLK(m) (((m) & S_IFMT) == S_IFBLK)
#else
#define S_ISBLK(m) 0
#endif
#endif

#if !defined(S_ISCHR)
#if defined(S_IFCHR)
#define S_ISCHR(m) (((m) & S_IFMT) == S_IFCHR)
#else
#define S_ISCHR(m) 0
#endif
#endif

#if !defined(S_ISDIR)
#if defined(S_IFDIR)
#define S_ISDIR(m) (((m) & S_IFMT) == S_IFDIR)
#else
#define S_ISDIR(m) 0
#endif
#endif

#if !defined(S_ISDOOR)
#if defined(S_IFDOOR)
#define S_ISDOOR(m) (((m) & S_IFMT) == S_IFDOOR)
#else
#define S_ISDOOR(m) 0
#endif
#endif

#if !defined(S_ISFIFO)
#if defined(S_IFIFO)
#define S_ISFIFO(m) (((m) & S_IFMT) == S_IFIFO)
#else
#define S_ISFIFO(m) 0
#endif
#endif

#if !defined(S_ISLNK)
#if defined(S_IFLNK)
#define S_ISLNK(m) (((m) & S_IFMT) == S_IFLNK)
#else
#define S_ISLNK(m) 0
#endif
#endif

#if !defined(S_ISMPB)
#if defined(S_IFMPB)
#define S_ISMPB(m) (((m) & S_IFMT) == S_IFMPB)
#define S_ISMPC(m) (((m) & S_IFMT) == S_IFMPC)
#else
#define S_ISMPB(m) 0
#define S_ISMPC(m) 0
#endif
#endif

#if !defined(S_ISNAM)
#if defined(S_IFNAM)
#define S_ISNAM(m) (((m) & S_IFMT) == S_IFNAM)
#else
#define S_ISNAM(m) 0
#endif
#endif

#if !defined(S_ISNWK)
#if defined(S_IFNWK)
#define S_ISNWK(m) (((m) & S_IFMT) == S_IFNWK)
#else
#define S_ISNWK(m) 0
#endif
#endif

#if !defined(S_ISREG)
#if defined(S_IFREG)
#define S_ISREG(m) (((m) & S_IFMT) == S_IFREG)
#else
#define S_ISREG(m) 0
#endif
#endif

#if !defined(S_ISSOCK)
#if defined(S_IFSOCK)
#define S_ISSOCK(m) (((m) & S_IFMT) == S_IFSOCK)
#else
#define S_ISSOCK(m) 0
#endif
#endif


#if !defined(S_TYPEISMQ)
#define S_TYPEISMQ(p) 0
#endif

#if !defined(S_TYPEISTMO)
#define S_TYPEISTMO(p) 0
#endif


#if !defined(S_TYPEISSEM)
#if defined(S_INSEM)
#define S_TYPEISSEM(p) (S_ISNAM ((p)->st_mode) && (p)->st_rdev == S_INSEM)
#else
#define S_TYPEISSEM(p) 0
#endif
#endif

#if !defined(S_TYPEISSHM)
#if defined(S_INSHD)
#define S_TYPEISSHM(p) (S_ISNAM ((p)->st_mode) && (p)->st_rdev == S_INSHD)
#else
#define S_TYPEISSHM(p) 0
#endif
#endif

#endif
