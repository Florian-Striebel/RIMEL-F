#if !defined(__CAPSICUM_FREEBSD_H__)
#define __CAPSICUM_FREEBSD_H__
#if defined(__FreeBSD__)




#if defined(__cplusplus)
extern "C" {
#endif


#include <errno.h>
#include <sys/param.h>
#if __FreeBSD_version >= 1100014 || (__FreeBSD_version >= 1001511 && __FreeBSD_version < 1100000)

#include <sys/capsicum.h>
#else
#include <sys/capability.h>
#endif
#include <sys/procdesc.h>

#if __FreeBSD_version >= 1000000
#define AT_SYSCALLS_IN_CAPMODE
#define HAVE_CAP_RIGHTS_GET
#define HAVE_CAP_RIGHTS_LIMIT
#define HAVE_PROCDESC_FSTAT
#define HAVE_CAP_FCNTLS_LIMIT

typedef uint32_t cap_fcntl_t;
#define HAVE_CAP_IOCTLS_LIMIT

typedef unsigned long cap_ioctl_t;

#if __FreeBSD_version >= 1101000
#define HAVE_OPENAT_INTERMEDIATE_DOTDOT
#endif

#endif

#if defined(__cplusplus)
}
#endif


#define fexecve_(F, A, E) fexecve(F, A, E)

#if defined(ENOTBENEATH)
#define E_NO_TRAVERSE_CAPABILITY ENOTBENEATH
#define E_NO_TRAVERSE_O_BENEATH ENOTBENEATH
#else
#define E_NO_TRAVERSE_CAPABILITY ENOTCAPABLE
#define E_NO_TRAVERSE_O_BENEATH ENOTCAPABLE
#endif


#define CAP_IOCTLS_LIMIT_MAX 256


#define E_TOO_MANY_LINKS EMLINK










#endif

#endif
