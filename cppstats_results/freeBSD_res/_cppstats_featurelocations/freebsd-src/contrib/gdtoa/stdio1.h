



























#if !defined(STDIO1_H_included)
#define STDIO1_H_included
#if !defined(STDIO_H_included)
#include <stdio.h>
#endif

#if defined(KR_headers)
#if !defined(_SIZE_T)
#define _SIZE_T
typedef unsigned int size_t;
#endif
#define ANSI(x) ()
#include "varargs.h"
#if !defined(Char)
#define Char char
#endif
#else
#define ANSI(x) x
#include "stdarg.h"
#if !defined(Char)
#define Char void
#endif
#endif

#if !defined(NO_STDIO1)

#if defined(__cplusplus)
extern "C" {
#endif

extern int Fprintf ANSI((FILE*, const char*, ...));
extern int Printf ANSI((const char*, ...));
extern int Sprintf ANSI((char*, const char*, ...));
extern int Snprintf ANSI((char*, size_t, const char*, ...));
extern void Perror ANSI((const char*));
extern int Vfprintf ANSI((FILE*, const char*, va_list));
extern int Vsprintf ANSI((char*, const char*, va_list));
extern int Vsnprintf ANSI((char*, size_t, const char*, va_list));

#if defined(PF_BUF)
extern FILE *stderr_ASL;
extern void (*pfbuf_print_ASL) ANSI((char*));
extern char *pfbuf_ASL;
extern void fflush_ASL ANSI((FILE*));
#if defined(fflush)
#define old_fflush_ASL fflush
#undef fflush
#endif
#define fflush fflush_ASL
#endif

#if defined(__cplusplus)
}
#endif

#undef printf
#undef fprintf
#undef sprintf
#undef perror
#undef vfprintf
#undef vsprintf
#define printf Printf
#define fprintf Fprintf
#undef snprintf
#undef vsnprintf
#define snprintf Snprintf
#define sprintf Sprintf
#define perror Perror
#define vfprintf Vfprintf
#define vsnprintf Vsnprintf
#define vsprintf Vsprintf

#endif

#endif
