




























#if !defined(_COMPAT_OPENSOLARIS_LIBPROC_H_)
#define _COMPAT_OPENSOLARIS_LIBPROC_H_

#include <fcntl.h>

#define ps_prochandle proc_handle
#define Lmid_t int

#define PR_RLC 0x0001
#define PR_KLC 0x0002

#include_next <libproc.h>

#endif
