


























#if !defined(LAFE_ERR_H)
#define LAFE_ERR_H

#if defined(__GNUC__) && (__GNUC__ > 2 || (__GNUC__ == 2 && __GNUC_MINOR__ >= 5))

#define __LA_DEAD __attribute__((__noreturn__))
#else
#define __LA_DEAD
#endif

#if defined(__GNUC__) && (__GNUC__ > 2 || (__GNUC__ == 2 && __GNUC_MINOR__ >= 7))

#if defined(__MINGW_PRINTF_FORMAT)
#define __LA_PRINTF_FORMAT __MINGW_PRINTF_FORMAT
#else
#define __LA_PRINTF_FORMAT __printf__
#endif
#define __LA_PRINTFLIKE(f,a) __attribute__((__format__(__LA_PRINTF_FORMAT, f, a)))
#else
#define __LA_PRINTFLIKE(f,a)
#endif

void lafe_warnc(int code, const char *fmt, ...) __LA_PRINTFLIKE(2, 3);
void lafe_errc(int eval, int code, const char *fmt, ...) __LA_DEAD
__LA_PRINTFLIKE(3, 4);

const char * lafe_getprogname(void);
void lafe_setprogname(const char *name, const char *defaultname);

#endif
