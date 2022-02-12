











#if !defined(LLVM_CLANG_TOOLING_TRANSFORMER_SOURCE_CODE_H)
#define LLVM_CLANG_TOOLING_TRANSFORMER_SOURCE_CODE_H

#include "clang/AST/ASTContext.h"
#include "clang/Basic/SourceLocation.h"
#include "clang/Basic/TokenKinds.h"

namespace clang {
namespace tooling {



CharSourceRange maybeExtendRange(CharSourceRange Range,
tok::TokenKind Terminator,
ASTContext &Context);




template <typename T>
CharSourceRange getExtendedRange(const T &Node, tok::TokenKind Next,
ASTContext &Context) {
return maybeExtendRange(CharSourceRange::getTokenRange(Node.getSourceRange()),
Next, Context);
}






CharSourceRange getAssociatedRange(const Decl &D, ASTContext &Context);


StringRef getText(CharSourceRange Range, const ASTContext &Context);


template <typename T>
StringRef getText(const T &Node, const ASTContext &Context) {
return getText(CharSourceRange::getTokenRange(Node.getSourceRange()),
Context);
}























template <typename T>
StringRef getExtendedText(const T &Node, tok::TokenKind Next,
ASTContext &Context) {
return getText(getExtendedRange(Node, Next, Context), Context);
}



llvm::Error validateEditRange(const CharSourceRange &Range,
const SourceManager &SM);





llvm::Optional<CharSourceRange>
getRangeForEdit(const CharSourceRange &EditRange, const SourceManager &SM,
const LangOptions &LangOpts);
inline llvm::Optional<CharSourceRange>
getRangeForEdit(const CharSourceRange &EditRange, const ASTContext &Context) {
return getRangeForEdit(EditRange, Context.getSourceManager(),
Context.getLangOpts());
}
}
}
#endif
