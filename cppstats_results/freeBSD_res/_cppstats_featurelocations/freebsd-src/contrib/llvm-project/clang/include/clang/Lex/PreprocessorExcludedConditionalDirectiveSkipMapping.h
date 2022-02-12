







#if !defined(LLVM_CLANG_LEX_PREPROCESSOR_EXCLUDED_COND_DIRECTIVE_SKIP_MAPPING_H)
#define LLVM_CLANG_LEX_PREPROCESSOR_EXCLUDED_COND_DIRECTIVE_SKIP_MAPPING_H

#include "clang/Basic/LLVM.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/Support/MemoryBuffer.h"

namespace clang {




using PreprocessorSkippedRangeMapping = llvm::DenseMap<unsigned, unsigned>;



using ExcludedPreprocessorDirectiveSkipMapping =
llvm::DenseMap<const char *, const PreprocessorSkippedRangeMapping *>;

}

#endif
