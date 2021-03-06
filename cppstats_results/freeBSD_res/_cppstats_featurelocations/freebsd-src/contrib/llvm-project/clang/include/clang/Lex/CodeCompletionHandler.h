











#if !defined(LLVM_CLANG_LEX_CODECOMPLETIONHANDLER_H)
#define LLVM_CLANG_LEX_CODECOMPLETIONHANDLER_H

#include "llvm/ADT/StringRef.h"

namespace clang {

class IdentifierInfo;
class MacroInfo;



class CodeCompletionHandler {
public:
virtual ~CodeCompletionHandler();









virtual void CodeCompleteDirective(bool InConditional) { }



virtual void CodeCompleteInConditionalExclusion() { }






virtual void CodeCompleteMacroName(bool IsDefinition) { }



virtual void CodeCompletePreprocessorExpression() { }







virtual void CodeCompleteMacroArgument(IdentifierInfo *Macro,
MacroInfo *MacroInfo,
unsigned ArgumentIndex) { }




virtual void CodeCompleteIncludedFile(llvm::StringRef Dir, bool IsAngled) {}




virtual void CodeCompleteNaturalLanguage() { }
};

}

#endif
