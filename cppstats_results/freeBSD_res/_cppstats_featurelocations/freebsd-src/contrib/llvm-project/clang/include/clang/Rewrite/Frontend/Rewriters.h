











#if !defined(LLVM_CLANG_REWRITE_FRONTEND_REWRITERS_H)
#define LLVM_CLANG_REWRITE_FRONTEND_REWRITERS_H

#include "clang/Basic/LLVM.h"

namespace clang {
class Preprocessor;
class PreprocessorOutputOptions;


void RewriteMacrosInInput(Preprocessor &PP, raw_ostream *OS);


void DoRewriteTest(Preprocessor &PP, raw_ostream *OS);


void RewriteIncludesInInput(Preprocessor &PP, raw_ostream *OS,
const PreprocessorOutputOptions &Opts);

}

#endif
