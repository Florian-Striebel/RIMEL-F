
































#if !defined(ftmacros_h)
#define ftmacros_h











#if defined(sun) || defined(__sun)
#define __EXTENSIONS__






#define _XPG4_2
#elif defined(_hpux) || defined(hpux) || defined(__hpux)
#define _REENTRANT






#if !defined(_XOPEN_SOURCE_EXTENDED)
#define _XOPEN_SOURCE_EXTENDED
#endif



















#elif defined(__linux__) || defined(linux) || defined(__linux)








#define _GNU_SOURCE









#define _DEFAULT_SOURCE
#define _BSD_SOURCE
#endif

#endif
