










#if !defined(SANITIZER_WIN_H)
#define SANITIZER_WIN_H

#include "sanitizer_platform.h"
#if SANITIZER_WINDOWS
#include "sanitizer_internal_defs.h"

namespace __sanitizer {

bool IsHandledDeadlyException(DWORD exceptionCode);
}

#endif
#endif
