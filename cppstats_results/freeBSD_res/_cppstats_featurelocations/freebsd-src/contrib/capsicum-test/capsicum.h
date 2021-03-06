


#if !defined(__CAPSICUM_H__)
#define __CAPSICUM_H__

#if defined(__FreeBSD__)
#include "capsicum-freebsd.h"
#endif

#if defined(__linux__)
#include "capsicum-linux.h"
#endif





#if !defined(CAP_SET_ALL)
#if defined(CAP_RIGHTS_VERSION)
#define CAP_SET_ALL(rights) CAP_ALL(rights)
#else
#define CAP_SET_ALL(rights) *(rights) = CAP_MASK_VALID
#endif
#endif

#if !defined(CAP_SET_NONE)
#if defined(CAP_RIGHTS_VERSION)
#define CAP_SET_NONE(rights) CAP_NONE(rights)
#else
#define CAP_SET_NONE(rights) *(rights) = 0
#endif
#endif






#include "capsicum-rights.h"





#if !defined(CAP_RENAMEAT_SOURCE)
#define CAP_RENAMEAT_SOURCE CAP_RENAMEAT
#endif
#if !defined(CAP_RENAMEAT_TARGET)
#define CAP_RENAMEAT_TARGET CAP_LINKAT
#endif




#if !defined(CAP_LINKAT_SOURCE)
#define CAP_LINKAT_SOURCE CAP_LOOKUP
#endif
#if !defined(CAP_LINKAT_TARGET)
#define CAP_LINKAT_TARGET CAP_LINKAT
#endif

#if defined(CAP_PREAD)

#define CAP_SEEK_ASWAS 0
#else

#define CAP_SEEK_ASWAS CAP_SEEK
#define CAP_PREAD CAP_READ
#define CAP_PWRITE CAP_WRITE
#endif

#if !defined(CAP_MMAP_R)
#define CAP_MMAP_R (CAP_READ|CAP_MMAP)
#define CAP_MMAP_W (CAP_WRITE|CAP_MMAP)
#define CAP_MMAP_X (CAP_MAPEXEC|CAP_MMAP)
#define CAP_MMAP_RW (CAP_MMAP_R|CAP_MMAP_W)
#define CAP_MMAP_RX (CAP_MMAP_R|CAP_MMAP_X)
#define CAP_MMAP_WX (CAP_MMAP_W|CAP_MMAP_X)
#define CAP_MMAP_RWX (CAP_MMAP_R|CAP_MMAP_W|CAP_MMAP_X)
#endif

#if !defined(CAP_MKFIFOAT)
#define CAP_MKFIFOAT CAP_MKFIFO
#endif

#if !defined(CAP_MKNODAT)
#define CAP_MKNODAT CAP_MKFIFOAT
#endif

#if !defined(CAP_MKDIRAT)
#define CAP_MKDIRAT CAP_MKDIR
#endif

#if !defined(CAP_UNLINKAT)
#define CAP_UNLINKAT CAP_RMDIR
#endif

#if !defined(CAP_SOCK_CLIENT)
#define CAP_SOCK_CLIENT (CAP_CONNECT | CAP_GETPEERNAME | CAP_GETSOCKNAME | CAP_GETSOCKOPT | CAP_PEELOFF | CAP_READ | CAP_WRITE | CAP_SETSOCKOPT | CAP_SHUTDOWN)


#endif

#if !defined(CAP_SOCK_SERVER)
#define CAP_SOCK_SERVER (CAP_ACCEPT | CAP_BIND | CAP_GETPEERNAME | CAP_GETSOCKNAME | CAP_GETSOCKOPT | CAP_LISTEN | CAP_PEELOFF | CAP_READ | CAP_WRITE | CAP_SETSOCKOPT | CAP_SHUTDOWN)



#endif

#if !defined(CAP_EVENT)
#define CAP_EVENT CAP_POLL_EVENT
#endif





#if !defined(HAVE_CAP_RIGHTS_GET)

inline int cap_rights_get(int fd, cap_rights_t *rights) {
return cap_getrights(fd, rights);
}
#endif

#if !defined(HAVE_CAP_RIGHTS_LIMIT)

#include <unistd.h>
inline int cap_rights_limit(int fd, const cap_rights_t *rights) {
int cap = cap_new(fd, *rights);
if (cap < 0) return cap;
int rc = dup2(cap, fd);
if (rc < 0) return rc;
close(cap);
return rc;
}
#endif

#include <stdio.h>
#if defined(CAP_RIGHTS_VERSION)

static inline void cap_rights_describe(const cap_rights_t *rights, char *buffer) {
int ii;
for (ii = 0; ii < (CAP_RIGHTS_VERSION+2); ii++) {
int len = sprintf(buffer, "0x%016llx ", (unsigned long long)rights->cr_rights[ii]);
buffer += len;
}
}

#if defined(__cplusplus)
#include <iostream>
#include <iomanip>
inline std::ostream& operator<<(std::ostream& os, cap_rights_t rights) {
for (int ii = 0; ii < (CAP_RIGHTS_VERSION+2); ii++) {
os << std::hex << std::setw(16) << std::setfill('0') << (unsigned long long)rights.cr_rights[ii] << " ";
}
return os;
}
#endif

#else

static inline void cap_rights_describe(const cap_rights_t *rights, char *buffer) {
sprintf(buffer, "0x%016llx", (*rights));
}

#endif

#if defined(__cplusplus)
#include <string>
extern std::string capsicum_test_bindir;
#endif

#endif
