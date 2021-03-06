








#if !defined(_h_config)
#define _h_config




#define POSIX





#define POSIXJOBS















#define vfork fork





#define BSDJOBS






#define BSDTIMES




#define BSDLIMIT







#define TERMIO








#define SYSVREL 0





#undef YPBUGS



#if defined(__FreeBSD__)
#define NLS_BUGS
#define BSD_STYLE_COLORLS

#define MCLoadBySet NL_CAT_LOCALE
#define BUFSIZE 8192
#define UTMPX_FILE "/var/run/utx.active"
#endif

#if defined(__bsdi__)



#include <sys/param.h>
#if defined(_BSDI_VERSION >= 199701)
#define _PATH_TCSHELL "/bin/tcsh"
#undef SYSMALLOC
#define SYSMALLOC
#else
#define _PATH_TCSHELL "/usr/contrib/bin/tcsh"
#endif

#undef NLS
#undef NLS_CATALOGS

#elif defined(__APPLE__)
#define SYSMALLOC
#endif

#endif
