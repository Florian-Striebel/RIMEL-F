













#if !defined(LLVM_CLANG_LIB_FORMAT_FORMATINTERNAL_H)
#define LLVM_CLANG_LIB_FORMAT_FORMATINTERNAL_H

#include "BreakableToken.h"
#include <utility>

namespace clang {
namespace format {
namespace internal {















































std::pair<tooling::Replacements, unsigned>
reformat(const FormatStyle &Style, StringRef Code,
ArrayRef<tooling::Range> Ranges, unsigned FirstStartColumn,
unsigned NextStartColumn, unsigned LastStartColumn, StringRef FileName,
FormattingAttemptStatus *Status);

}
}
}

#endif
