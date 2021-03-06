











#if !defined(LLVM_CLANG_PARSE_PARSEAST_H)
#define LLVM_CLANG_PARSE_PARSEAST_H

#include "clang/Basic/LangOptions.h"

namespace clang {
class Preprocessor;
class ASTConsumer;
class ASTContext;
class CodeCompleteConsumer;
class Sema;














void ParseAST(Preprocessor &pp, ASTConsumer *C,
ASTContext &Ctx, bool PrintStats = false,
TranslationUnitKind TUKind = TU_Complete,
CodeCompleteConsumer *CompletionConsumer = nullptr,
bool SkipFunctionBodies = false);



void ParseAST(Sema &S, bool PrintStats = false,
bool SkipFunctionBodies = false);

}

#endif
