







#if !defined(LLVM_CLANG_TOOLING_REFACTOR_REFACTORING_RULE_CONTEXT_H)
#define LLVM_CLANG_TOOLING_REFACTOR_REFACTORING_RULE_CONTEXT_H

#include "clang/Basic/DiagnosticError.h"
#include "clang/Basic/SourceManager.h"
#include "clang/Tooling/Refactoring/ASTSelection.h"

namespace clang {

class ASTContext;

namespace tooling {












class RefactoringRuleContext {
public:
RefactoringRuleContext(const SourceManager &SM) : SM(SM) {}

const SourceManager &getSources() const { return SM; }



SourceRange getSelectionRange() const { return SelectionRange; }

void setSelectionRange(SourceRange R) { SelectionRange = R; }

bool hasASTContext() const { return AST; }

ASTContext &getASTContext() const {
assert(AST && "no AST!");
return *AST;
}

void setASTContext(ASTContext &Context) { AST = &Context; }




llvm::Error createDiagnosticError(SourceLocation Loc, unsigned DiagID) {
return DiagnosticError::create(Loc, PartialDiagnostic(DiagID, DiagStorage));
}

llvm::Error createDiagnosticError(unsigned DiagID) {
return createDiagnosticError(SourceLocation(), DiagID);
}

void setASTSelection(std::unique_ptr<SelectedASTNode> Node) {
ASTNodeSelection = std::move(Node);
}

private:


const SourceManager &SM;


SourceRange SelectionRange;


ASTContext *AST = nullptr;

PartialDiagnostic::DiagStorageAllocator DiagStorage;


std::unique_ptr<SelectedASTNode> ASTNodeSelection;
};

}
}

#endif
