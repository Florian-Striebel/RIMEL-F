






























#if !defined(GDTOA_H_INCLUDED)
#define GDTOA_H_INCLUDED

#include "arith.h"
#include <stddef.h>

#if !defined(Long)
#define Long int
#endif
#if !defined(ULong)
typedef unsigned Long ULong;
#endif
#if !defined(UShort)
typedef unsigned short UShort;
#endif

#if !defined(ANSI)
#if defined(KR_headers)
#define ANSI(x) ()
#define Void
#else
#define ANSI(x) x
#define Void void
#endif
#endif

#if !defined(CONST)
#if defined(KR_headers)
#define CONST
#else
#define CONST const
#endif
#endif

enum {
STRTOG_Zero = 0,
STRTOG_Normal = 1,
STRTOG_Denormal = 2,
STRTOG_Infinite = 3,
STRTOG_NaN = 4,
STRTOG_NaNbits = 5,
STRTOG_NoNumber = 6,
STRTOG_Retmask = 7,



STRTOG_Neg = 0x08,
STRTOG_Inexlo = 0x10,
STRTOG_Inexhi = 0x20,
STRTOG_Inexact = 0x30,
STRTOG_Underflow= 0x40,
STRTOG_Overflow = 0x80
};

typedef struct
FPI {
int nbits;
int emin;
int emax;
int rounding;
int sudden_underflow;
} FPI;

enum {
FPI_Round_zero = 0,
FPI_Round_near = 1,
FPI_Round_up = 2,
FPI_Round_down = 3
};

#if defined(__cplusplus)
extern "C" {
#endif

extern char* dtoa ANSI((double d, int mode, int ndigits, int *decpt,
int *sign, char **rve));
extern char* gdtoa ANSI((FPI *fpi, int be, ULong *bits, int *kindp,
int mode, int ndigits, int *decpt, char **rve));
extern void freedtoa ANSI((char*));
extern float strtof ANSI((CONST char *, char **));
extern double strtod ANSI((CONST char *, char **));
extern int strtodg ANSI((CONST char*, char**, FPI*, Long*, ULong*));

extern char* g_ddfmt ANSI((char*, double*, int, size_t));
extern char* g_dfmt ANSI((char*, double*, int, size_t));
extern char* g_ffmt ANSI((char*, float*, int, size_t));
extern char* g_Qfmt ANSI((char*, void*, int, size_t));
extern char* g_xfmt ANSI((char*, void*, int, size_t));
extern char* g_xLfmt ANSI((char*, void*, int, size_t));

extern int strtoId ANSI((CONST char*, char**, double*, double*));
extern int strtoIdd ANSI((CONST char*, char**, double*, double*));
extern int strtoIf ANSI((CONST char*, char**, float*, float*));
extern int strtoIQ ANSI((CONST char*, char**, void*, void*));
extern int strtoIx ANSI((CONST char*, char**, void*, void*));
extern int strtoIxL ANSI((CONST char*, char**, void*, void*));
extern int strtord ANSI((CONST char*, char**, int, double*));
extern int strtordd ANSI((CONST char*, char**, int, double*));
extern int strtorf ANSI((CONST char*, char**, int, float*));
extern int strtorQ ANSI((CONST char*, char**, int, void*));
extern int strtorx ANSI((CONST char*, char**, int, void*));
extern int strtorxL ANSI((CONST char*, char**, int, void*));
#if 1
extern int strtodI ANSI((CONST char*, char**, double*));
extern int strtopd ANSI((CONST char*, char**, double*));
extern int strtopdd ANSI((CONST char*, char**, double*));
extern int strtopf ANSI((CONST char*, char**, float*));
extern int strtopQ ANSI((CONST char*, char**, void*));
extern int strtopx ANSI((CONST char*, char**, void*));
extern int strtopxL ANSI((CONST char*, char**, void*));
#else
#define strtopd(s,se,x) strtord(s,se,1,x)
#define strtopdd(s,se,x) strtordd(s,se,1,x)
#define strtopf(s,se,x) strtorf(s,se,1,x)
#define strtopQ(s,se,x) strtorQ(s,se,1,x)
#define strtopx(s,se,x) strtorx(s,se,1,x)
#define strtopxL(s,se,x) strtorxL(s,se,1,x)
#endif

#if defined(__cplusplus)
}
#endif
#endif
