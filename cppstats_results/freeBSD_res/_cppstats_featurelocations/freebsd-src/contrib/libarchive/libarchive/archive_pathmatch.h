



























#if !defined(ARCHIVE_PATHMATCH_H)
#define ARCHIVE_PATHMATCH_H

#if !defined(__LIBARCHIVE_BUILD)
#if !defined(__LIBARCHIVE_TEST)
#error This header is only to be used internally to libarchive.
#endif
#endif


#define PATHMATCH_NO_ANCHOR_START 1

#define PATHMATCH_NO_ANCHOR_END 2




int __archive_pathmatch(const char *p, const char *s, int flags);
int __archive_pathmatch_w(const wchar_t *p, const wchar_t *s, int flags);

#define archive_pathmatch(p, s, f) __archive_pathmatch(p, s, f)
#define archive_pathmatch_w(p, s, f) __archive_pathmatch_w(p, s, f)

#endif
