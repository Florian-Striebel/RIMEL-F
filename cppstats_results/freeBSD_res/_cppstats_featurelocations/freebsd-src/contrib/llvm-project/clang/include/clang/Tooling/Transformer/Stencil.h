


















#if !defined(LLVM_CLANG_TOOLING_TRANSFORMER_STENCIL_H_)
#define LLVM_CLANG_TOOLING_TRANSFORMER_STENCIL_H_

#include "clang/AST/ASTContext.h"
#include "clang/AST/ASTTypeTraits.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/Tooling/Transformer/MatchConsumer.h"
#include "clang/Tooling/Transformer/RangeSelector.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/Error.h"
#include <string>
#include <vector>

namespace clang {
namespace transformer {

using StencilInterface = MatchComputation<std::string>;









using Stencil = std::shared_ptr<StencilInterface>;

namespace detail {




Stencil makeStencil(llvm::StringRef Text);
Stencil makeStencil(RangeSelector Selector);
inline Stencil makeStencil(Stencil S) { return S; }
}



Stencil catVector(std::vector<Stencil> Parts);



template <typename... Ts> Stencil cat(Ts &&... Parts) {
return catVector({detail::makeStencil(std::forward<Ts>(Parts))...});
}









Stencil expression(llvm::StringRef Id);



Stencil deref(llvm::StringRef ExprId);




Stencil maybeDeref(llvm::StringRef ExprId);




Stencil addressOf(llvm::StringRef ExprId);





Stencil maybeAddressOf(llvm::StringRef ExprId);






Stencil access(llvm::StringRef BaseId, Stencil Member);
inline Stencil access(llvm::StringRef BaseId, llvm::StringRef Member) {
return access(BaseId, detail::makeStencil(Member));
}



Stencil ifBound(llvm::StringRef Id, Stencil TrueStencil, Stencil FalseStencil);



inline Stencil ifBound(llvm::StringRef Id, llvm::StringRef TrueText,
llvm::StringRef FalseText) {
return ifBound(Id, detail::makeStencil(TrueText),
detail::makeStencil(FalseText));
}



Stencil run(MatchConsumer<std::string> C);








Stencil describe(llvm::StringRef Id);




Stencil dPrint(llvm::StringRef Id);
}
}
#endif
