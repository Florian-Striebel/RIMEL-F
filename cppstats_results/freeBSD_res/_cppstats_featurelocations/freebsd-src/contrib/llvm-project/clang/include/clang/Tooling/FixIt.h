

















#if !defined(LLVM_CLANG_TOOLING_FIXIT_H)
#define LLVM_CLANG_TOOLING_FIXIT_H

#include "clang/AST/ASTContext.h"

namespace clang {
namespace tooling {
namespace fixit {

namespace internal {
StringRef getText(CharSourceRange Range, const ASTContext &Context);


inline CharSourceRange getSourceRange(const SourceRange &Range) {
return CharSourceRange::getTokenRange(Range);
}


inline CharSourceRange getSourceRange(const SourceLocation &Loc) {
return CharSourceRange::getTokenRange(Loc, Loc);
}



template <typename T> CharSourceRange getSourceRange(const T &Node) {
return CharSourceRange::getTokenRange(Node.getSourceRange());
}
}


template <typename T>
StringRef getText(const T &Node, const ASTContext &Context) {
return internal::getText(internal::getSourceRange(Node), Context);
}



template <typename T> FixItHint createRemoval(const T &Node) {
return FixItHint::CreateRemoval(internal::getSourceRange(Node));
}


template <typename D, typename S>
FixItHint createReplacement(const D &Destination, const S &Source,
const ASTContext &Context) {
return FixItHint::CreateReplacement(internal::getSourceRange(Destination),
getText(Source, Context));
}


template <typename D>
FixItHint createReplacement(const D &Destination, StringRef Source) {
return FixItHint::CreateReplacement(internal::getSourceRange(Destination),
Source);
}

}
}
}

#endif
