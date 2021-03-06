







#if !defined(LLVM_CLANG_SEMA_CODECOMPLETEOPTIONS_H)
#define LLVM_CLANG_SEMA_CODECOMPLETEOPTIONS_H

namespace clang {


class CodeCompleteOptions {
public:

unsigned IncludeMacros : 1;


unsigned IncludeCodePatterns : 1;


unsigned IncludeGlobals : 1;







unsigned IncludeNamespaceLevelDecls : 1;


unsigned IncludeBriefComments : 1;




unsigned LoadExternal : 1;



unsigned IncludeFixIts : 1;

CodeCompleteOptions()
: IncludeMacros(0), IncludeCodePatterns(0), IncludeGlobals(1),
IncludeNamespaceLevelDecls(1), IncludeBriefComments(0),
LoadExternal(1), IncludeFixIts(0) {}
};

}

#endif

