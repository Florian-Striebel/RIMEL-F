





























#if !defined(_SUPPORT_H)
#define _SUPPORT_H

__BEGIN_DECLS
const char *fmttime(char *, size_t, time_t);
const char *fmtydhms(char *, size_t, time_t);
void vdlog(int, const char *, va_list)
__attribute__((__format__(__printf__, 2, 0)));
void dlog(int, const char *, ...)
__attribute__((__format__(__printf__, 2, 3)));
ssize_t blhexdump(char *, size_t, const char *, const void *, size_t);
__END_DECLS

#endif
