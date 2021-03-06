




























#if !defined(ARCHIVE_PLATFORM_ACL_H_INCLUDED)
#define ARCHIVE_PLATFORM_ACL_H_INCLUDED

#if !defined(__LIBARCHIVE_BUILD)
#if !defined(__LIBARCHIVE_TEST_COMMON)
#error This header is only to be used internally to libarchive.
#endif
#endif




#if ARCHIVE_ACL_FREEBSD || ARCHIVE_ACL_SUNOS || ARCHIVE_ACL_LIBACL
#define ARCHIVE_ACL_POSIX1E 1
#endif

#if ARCHIVE_ACL_FREEBSD_NFS4 || ARCHIVE_ACL_SUNOS_NFS4 || ARCHIVE_ACL_DARWIN || ARCHIVE_ACL_LIBRICHACL

#define ARCHIVE_ACL_NFS4 1
#endif

#if ARCHIVE_ACL_POSIX1E || ARCHIVE_ACL_NFS4
#define ARCHIVE_ACL_SUPPORT 1
#endif

#endif
