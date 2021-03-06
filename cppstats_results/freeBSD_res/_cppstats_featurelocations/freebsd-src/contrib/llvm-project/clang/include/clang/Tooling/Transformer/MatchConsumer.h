













#if !defined(LLVM_CLANG_TOOLING_TRANSFORMER_MATCH_CONSUMER_H_)
#define LLVM_CLANG_TOOLING_TRANSFORMER_MATCH_CONSUMER_H_

#include "clang/AST/ASTTypeTraits.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/Errc.h"
#include "llvm/Support/Error.h"

namespace clang {
namespace transformer {







template <typename T>
using MatchConsumer =
std::function<Expected<T>(const ast_matchers::MatchFinder::MatchResult &)>;



inline llvm::Error notBoundError(llvm::StringRef Id) {
return llvm::make_error<llvm::StringError>(llvm::errc::invalid_argument,
"Id not bound: " + Id);
}



template <typename T>
MatchConsumer<T> ifBound(std::string ID, MatchConsumer<T> TrueC,
MatchConsumer<T> FalseC) {
return [=](const ast_matchers::MatchFinder::MatchResult &Result) {
auto &Map = Result.Nodes.getMap();
return (Map.find(ID) != Map.end() ? TrueC : FalseC)(Result);
};
}










template <typename T> class MatchComputation {
public:
virtual ~MatchComputation() = default;





virtual llvm::Error eval(const ast_matchers::MatchFinder::MatchResult &Match,
T *Result) const = 0;



llvm::Expected<T> eval(const ast_matchers::MatchFinder::MatchResult &R) const;




virtual std::string toString() const = 0;

protected:
MatchComputation() = default;



MatchComputation(const MatchComputation &) = default;
MatchComputation &operator=(const MatchComputation &) = default;
};

template <typename T>
llvm::Expected<T> MatchComputation<T>::eval(
const ast_matchers::MatchFinder::MatchResult &R) const {
T Output;
if (auto Err = eval(R, &Output))
return std::move(Err);
return Output;
}
}
}
#endif
