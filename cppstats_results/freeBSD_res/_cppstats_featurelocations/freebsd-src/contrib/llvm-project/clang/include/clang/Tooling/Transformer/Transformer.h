







#if !defined(LLVM_CLANG_TOOLING_TRANSFORMER_TRANSFORMER_H_)
#define LLVM_CLANG_TOOLING_TRANSFORMER_TRANSFORMER_H_

#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/Tooling/Refactoring/AtomicChange.h"
#include "clang/Tooling/Transformer/RewriteRule.h"
#include "llvm/Support/Error.h"
#include <functional>
#include <utility>

namespace clang {
namespace tooling {


class Transformer : public ast_matchers::MatchFinder::MatchCallback {
public:
using ChangeConsumer =
std::function<void(Expected<clang::tooling::AtomicChange> Change)>;






Transformer(transformer::RewriteRule Rule, ChangeConsumer Consumer)
: Rule(std::move(Rule)), Consumer(std::move(Consumer)) {}



void registerMatchers(ast_matchers::MatchFinder *MatchFinder);



void run(const ast_matchers::MatchFinder::MatchResult &Result) override;

private:
transformer::RewriteRule Rule;

ChangeConsumer Consumer;
};
}
}

#endif
