







#if !defined(LLVM_CLANG_BASIC_DIAGNOSTIC_ERROR_H)
#define LLVM_CLANG_BASIC_DIAGNOSTIC_ERROR_H

#include "clang/Basic/PartialDiagnostic.h"
#include "llvm/Support/Error.h"

namespace clang {




class DiagnosticError : public llvm::ErrorInfo<DiagnosticError> {
public:
DiagnosticError(PartialDiagnosticAt Diag) : Diag(std::move(Diag)) {}

void log(raw_ostream &OS) const override { OS << "clang diagnostic"; }

PartialDiagnosticAt &getDiagnostic() { return Diag; }
const PartialDiagnosticAt &getDiagnostic() const { return Diag; }



static llvm::Error create(SourceLocation Loc, PartialDiagnostic Diag) {
return llvm::make_error<DiagnosticError>(
PartialDiagnosticAt(Loc, std::move(Diag)));
}




static Optional<PartialDiagnosticAt> take(llvm::Error &Err) {
Optional<PartialDiagnosticAt> Result;
Err = llvm::handleErrors(std::move(Err), [&](DiagnosticError &E) {
Result = std::move(E.getDiagnostic());
});
return Result;
}

static char ID;

private:

std::error_code convertToErrorCode() const override {
return llvm::inconvertibleErrorCode();
}

PartialDiagnosticAt Diag;
};

}

#endif
