












#if !defined(LLVM_CLANG_STATICANALYZER_CHECKERS_LOCALCHECKERS_H)
#define LLVM_CLANG_STATICANALYZER_CHECKERS_LOCALCHECKERS_H

namespace clang {
namespace ento {

class ExprEngine;

void RegisterCallInliner(ExprEngine &Eng);

}
}

#endif
