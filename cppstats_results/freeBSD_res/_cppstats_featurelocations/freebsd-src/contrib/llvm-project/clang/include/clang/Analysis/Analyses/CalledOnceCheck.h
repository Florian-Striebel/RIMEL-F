












#if !defined(LLVM_CLANG_ANALYSIS_ANALYSES_CALLEDONCECHECK_H)
#define LLVM_CLANG_ANALYSIS_ANALYSES_CALLEDONCECHECK_H

namespace clang {

class AnalysisDeclContext;
class BlockDecl;
class CFG;
class Decl;
class DeclContext;
class Expr;
class ParmVarDecl;
class Stmt;










enum class NeverCalledReason {
IfThen,
IfElse,
Switch,
SwitchSkipped,
LoopEntered,
LoopSkipped,
FallbackReason,
LARGEST_VALUE = FallbackReason
};

class CalledOnceCheckHandler {
public:
CalledOnceCheckHandler() = default;
virtual ~CalledOnceCheckHandler() = default;








virtual void handleDoubleCall(const ParmVarDecl *Parameter, const Expr *Call,
const Expr *PrevCall, bool IsCompletionHandler,
bool Poised) {}




virtual void handleNeverCalled(const ParmVarDecl *Parameter,
bool IsCompletionHandler) {}





virtual void handleCapturedNeverCalled(const ParmVarDecl *Parameter,
const Decl *Where,
bool IsCompletionHandler) {}













virtual void handleNeverCalled(const ParmVarDecl *Parameter,
const Decl *Function, const Stmt *Where,
NeverCalledReason Reason,
bool IsCalledDirectly,
bool IsCompletionHandler) {}




virtual void
handleBlockThatIsGuaranteedToBeCalledOnce(const BlockDecl *Block) {}





virtual void handleBlockWithNoGuarantees(const BlockDecl *Block) {}
};













void checkCalledOnceParameters(AnalysisDeclContext &AC,
CalledOnceCheckHandler &Handler,
bool CheckConventionalParameters);

}

#endif
