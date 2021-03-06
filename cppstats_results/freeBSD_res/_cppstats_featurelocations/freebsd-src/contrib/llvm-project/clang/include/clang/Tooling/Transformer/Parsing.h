














#if !defined(LLVM_CLANG_TOOLING_REFACTOR_PARSING_H_)
#define LLVM_CLANG_TOOLING_REFACTOR_PARSING_H_

#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/Basic/SourceLocation.h"
#include "clang/Tooling/Transformer/RangeSelector.h"
#include "llvm/Support/Error.h"
#include <functional>

namespace clang {
namespace transformer {








llvm::Expected<RangeSelector> parseRangeSelector(llvm::StringRef Input);

}
}

#endif
