







#if !defined(LLD_FILESYSTEM_H)
#define LLD_FILESYSTEM_H

#include "lld/Common/LLVM.h"
#include <system_error>

namespace lld {
void unlinkAsync(StringRef path);
std::error_code tryCreateFile(StringRef path);
}

#endif
