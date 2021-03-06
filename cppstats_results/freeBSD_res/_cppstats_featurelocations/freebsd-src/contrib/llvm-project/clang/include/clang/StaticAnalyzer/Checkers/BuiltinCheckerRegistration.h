












#if !defined(LLVM_CLANG_LIB_STATICANALYZER_CHECKERS_CLANGSACHECKERS_H)
#define LLVM_CLANG_LIB_STATICANALYZER_CHECKERS_CLANGSACHECKERS_H

#include "clang/StaticAnalyzer/Core/BugReporter/CommonBugCategories.h"

namespace clang {

class LangOptions;

namespace ento {

class CheckerManager;
class CheckerRegistry;

#define GET_CHECKERS
#define CHECKER(FULLNAME, CLASS, HELPTEXT, DOC_URI, IS_HIDDEN) void register##CLASS(CheckerManager &mgr); bool shouldRegister##CLASS(const CheckerManager &mgr);


#include "clang/StaticAnalyzer/Checkers/Checkers.inc"
#undef CHECKER
#undef GET_CHECKERS

}

}

#endif
