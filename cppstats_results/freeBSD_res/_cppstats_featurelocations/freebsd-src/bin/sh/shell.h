




































#if !defined(SHELL_H_)
#define SHELL_H_

#include <inttypes.h>












#define JOBS 1





typedef intmax_t arith_t;
#define ARITH_FORMAT_STR "%" PRIdMAX
#define ARITH_MIN INTMAX_MIN
#define ARITH_MAX INTMAX_MAX

typedef void *pointer;

#include <sys/cdefs.h>

extern char nullstr[1];

#if defined(DEBUG)
#define TRACE(param) sh_trace param
#else
#define TRACE(param)
#endif

#endif
