
























#if !defined(ARCHIVE_RANDOM_PRIVATE_H_INCLUDED)
#define ARCHIVE_RANDOM_PRIVATE_H_INCLUDED

#if !defined(__LIBARCHIVE_BUILD)
#error This header is only to be used internally to libarchive.
#endif


int archive_random(void *buf, size_t nbytes);

#endif
