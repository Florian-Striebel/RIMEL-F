#if !defined(__CAPSICUM_LINUX_H__)
#define __CAPSICUM_LINUX_H__

#if defined(__linux__)



#include <errno.h>
#include <sys/procdesc.h>
#include <sys/capsicum.h>

#define HAVE_CAP_RIGHTS_LIMIT
#define HAVE_CAP_RIGHTS_GET
#define HAVE_CAP_FCNTLS_LIMIT
#define HAVE_CAP_IOCTLS_LIMIT
#define HAVE_PROC_FDINFO
#define HAVE_PDWAIT4
#define CAP_FROM_ACCEPT








#if defined(ENOTBENEATH)
#define E_NO_TRAVERSE_CAPABILITY ENOTBENEATH
#define E_NO_TRAVERSE_O_BENEATH ENOTBENEATH
#else
#define E_NO_TRAVERSE_CAPABILITY EPERM
#define E_NO_TRAVERSE_O_BENEATH EPERM
#endif


#define E_TOO_MANY_LINKS ELOOP

#endif

#endif
