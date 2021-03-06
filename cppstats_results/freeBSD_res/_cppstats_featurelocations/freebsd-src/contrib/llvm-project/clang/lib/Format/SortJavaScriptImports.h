












#if !defined(LLVM_CLANG_LIB_FORMAT_SORTJAVASCRIPTIMPORTS_H)
#define LLVM_CLANG_LIB_FORMAT_SORTJAVASCRIPTIMPORTS_H

#include "clang/Basic/LLVM.h"
#include "clang/Format/Format.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/StringRef.h"

namespace clang {
namespace format {



tooling::Replacements sortJavaScriptImports(const FormatStyle &Style,
StringRef Code,
ArrayRef<tooling::Range> Ranges,
StringRef FileName);

}
}

#endif
