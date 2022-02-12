


















#if !defined(_GETOPT_H)

#if !defined(__need_getopt)
#define _GETOPT_H 1
#endif








#if !defined __GNU_LIBRARY__
#include <ctype.h>
#endif

#if !defined(__THROW)
#if !defined(__GNUC_PREREQ)
#define __GNUC_PREREQ(maj, min) (0)
#endif
#if defined __cplusplus && __GNUC_PREREQ (2,8)
#define __THROW throw ()
#else
#define __THROW
#endif
#endif

#if defined(__cplusplus)
extern "C" {
#endif







extern char *optarg;













extern int optind;




extern int opterr;



extern int optopt;

#if !defined(__need_getopt)





















struct option
{
const char *name;


int has_arg;
int *flag;
int val;
};



#define no_argument 0
#define required_argument 1
#define optional_argument 2
#endif


























#if defined(__GNU_LIBRARY__)



extern int getopt (int ___argc, char *const *___argv, const char *__shortopts)
__THROW;
#else
extern int getopt ();
#endif

#if !defined(__need_getopt)
extern int getopt_long (int ___argc, char *const *___argv,
const char *__shortopts,
const struct option *__longopts, int *__longind)
__THROW;
extern int getopt_long_only (int ___argc, char *const *___argv,
const char *__shortopts,
const struct option *__longopts, int *__longind)
__THROW;

#endif

#if defined(__cplusplus)
}
#endif


#undef __need_getopt

#endif
