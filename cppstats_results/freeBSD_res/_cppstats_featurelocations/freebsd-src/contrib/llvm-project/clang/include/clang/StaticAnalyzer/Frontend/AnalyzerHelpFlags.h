







#if !defined(LLVM_CLANG_STATICANALYZER_FRONTEND_ANALYZERHELPFLAGS_H)
#define LLVM_CLANG_STATICANALYZER_FRONTEND_ANALYZERHELPFLAGS_H

namespace llvm {
class raw_ostream;
}

namespace clang {

class CompilerInstance;

namespace ento {

void printCheckerHelp(llvm::raw_ostream &OS, CompilerInstance &CI);
void printEnabledCheckerList(llvm::raw_ostream &OS, CompilerInstance &CI);
void printAnalyzerConfigList(llvm::raw_ostream &OS);
void printCheckerConfigList(llvm::raw_ostream &OS, CompilerInstance &CI);

}
}

#endif
