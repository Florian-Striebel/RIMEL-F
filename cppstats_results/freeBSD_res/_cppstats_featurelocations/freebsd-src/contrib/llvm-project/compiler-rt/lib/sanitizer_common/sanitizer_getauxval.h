














#if !defined(SANITIZER_GETAUXVAL_H)
#define SANITIZER_GETAUXVAL_H

#include "sanitizer_platform.h"
#include "sanitizer_glibc_version.h"

#if SANITIZER_LINUX || SANITIZER_FUCHSIA

#if (__GLIBC_PREREQ(2, 16) || (SANITIZER_ANDROID && __ANDROID_API__ >= 21) || SANITIZER_FUCHSIA) && !SANITIZER_GO


#define SANITIZER_USE_GETAUXVAL 1
#else
#define SANITIZER_USE_GETAUXVAL 0
#endif

#if SANITIZER_USE_GETAUXVAL
#include <sys/auxv.h>
#else



extern "C" SANITIZER_WEAK_ATTRIBUTE unsigned long getauxval(unsigned long type);
#endif

#elif SANITIZER_NETBSD

#define SANITIZER_USE_GETAUXVAL 1

#include <dlfcn.h>
#include <elf.h>

static inline decltype(AuxInfo::a_v) getauxval(decltype(AuxInfo::a_type) type) {
for (const AuxInfo *aux = (const AuxInfo *)_dlauxinfo();
aux->a_type != AT_NULL; ++aux) {
if (type == aux->a_type)
return aux->a_v;
}

return 0;
}

#endif

#endif
