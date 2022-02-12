




























#if !defined(ARCHIVE_PLATFORM_XATTR_H_INCLUDED)
#define ARCHIVE_PLATFORM_XATTR_H_INCLUDED

#if !defined(__LIBARCHIVE_BUILD)
#if !defined(__LIBARCHIVE_TEST_COMMON)
#error This header is only to be used internally to libarchive.
#endif
#endif




#if ARCHIVE_XATTR_LINUX || ARCHIVE_XATTR_DARWIN || ARCHIVE_XATTR_FREEBSD || ARCHIVE_XATTR_AIX

#define ARCHIVE_XATTR_SUPPORT 1
#endif

#endif
