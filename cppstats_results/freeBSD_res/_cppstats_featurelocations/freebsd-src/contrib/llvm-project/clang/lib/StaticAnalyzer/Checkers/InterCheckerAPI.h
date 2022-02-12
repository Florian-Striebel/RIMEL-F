










#if !defined(LLVM_CLANG_LIB_STATICANALYZER_CHECKERS_INTERCHECKERAPI_H)
#define LLVM_CLANG_LIB_STATICANALYZER_CHECKERS_INTERCHECKERAPI_H




namespace clang {
namespace ento {

class CheckerManager;


void registerInnerPointerCheckerAux(CheckerManager &Mgr);

}
}

#endif
