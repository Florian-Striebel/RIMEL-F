











#if !defined(LLVM_CLANG_AST_INTERP_STATE_H)
#define LLVM_CLANG_AST_INTERP_STATE_H

#include "clang/AST/ASTDiagnostic.h"
#include "clang/AST/Expr.h"
#include "clang/AST/OptionalDiagnostic.h"

namespace clang {





enum AccessKinds {
AK_Read,
AK_ReadObjectRepresentation,
AK_Assign,
AK_Increment,
AK_Decrement,
AK_MemberCall,
AK_DynamicCast,
AK_TypeId,
AK_Construct,
AK_Destroy,
};


enum CheckSubobjectKind {
CSK_Base,
CSK_Derived,
CSK_Field,
CSK_ArrayToPointer,
CSK_ArrayIndex,
CSK_Real,
CSK_Imag
};

namespace interp {
class Frame;
class SourceInfo;


class State {
public:
virtual ~State();

virtual bool checkingForUndefinedBehavior() const = 0;
virtual bool checkingPotentialConstantExpression() const = 0;
virtual bool noteUndefinedBehavior() = 0;
virtual bool keepEvaluatingAfterFailure() const = 0;
virtual Frame *getCurrentFrame() = 0;
virtual const Frame *getBottomFrame() const = 0;
virtual bool hasActiveDiagnostic() = 0;
virtual void setActiveDiagnostic(bool Flag) = 0;
virtual void setFoldFailureDiagnostic(bool Flag) = 0;
virtual Expr::EvalStatus &getEvalStatus() const = 0;
virtual ASTContext &getCtx() const = 0;
virtual bool hasPriorDiagnostic() = 0;
virtual unsigned getCallStackDepth() = 0;

public:

OptionalDiagnostic
FFDiag(SourceLocation Loc,
diag::kind DiagId = diag::note_invalid_subexpr_in_const_expr,
unsigned ExtraNotes = 0);

OptionalDiagnostic
FFDiag(const Expr *E,
diag::kind DiagId = diag::note_invalid_subexpr_in_const_expr,
unsigned ExtraNotes = 0);

OptionalDiagnostic
FFDiag(const SourceInfo &SI,
diag::kind DiagId = diag::note_invalid_subexpr_in_const_expr,
unsigned ExtraNotes = 0);






OptionalDiagnostic
CCEDiag(SourceLocation Loc,
diag::kind DiagId = diag::note_invalid_subexpr_in_const_expr,
unsigned ExtraNotes = 0);

OptionalDiagnostic
CCEDiag(const Expr *E,
diag::kind DiagId = diag::note_invalid_subexpr_in_const_expr,
unsigned ExtraNotes = 0);

OptionalDiagnostic
CCEDiag(const SourceInfo &SI,
diag::kind DiagId = diag::note_invalid_subexpr_in_const_expr,
unsigned ExtraNotes = 0);


OptionalDiagnostic Note(SourceLocation Loc, diag::kind DiagId);


void addNotes(ArrayRef<PartialDiagnosticAt> Diags);


DiagnosticBuilder report(SourceLocation Loc, diag::kind DiagId);

const LangOptions &getLangOpts() const;

private:
void addCallStack(unsigned Limit);

PartialDiagnostic &addDiag(SourceLocation Loc, diag::kind DiagId);

OptionalDiagnostic diag(SourceLocation Loc, diag::kind DiagId,
unsigned ExtraNotes, bool IsCCEDiag);
};

}
}

#endif
