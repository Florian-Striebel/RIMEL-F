













#if !defined(LLVM_CLANG_TOOLING_REFACTOR_RANGE_SELECTOR_H_)
#define LLVM_CLANG_TOOLING_REFACTOR_RANGE_SELECTOR_H_

#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/Basic/SourceLocation.h"
#include "clang/Tooling/Transformer/MatchConsumer.h"
#include "llvm/Support/Error.h"
#include <functional>
#include <string>

namespace clang {
namespace transformer {
using RangeSelector = MatchConsumer<CharSourceRange>;

inline RangeSelector charRange(CharSourceRange R) {
return [R](const ast_matchers::MatchFinder::MatchResult &)
-> Expected<CharSourceRange> { return R; };
}


RangeSelector enclose(RangeSelector Begin, RangeSelector End);


RangeSelector encloseNodes(std::string BeginID, std::string EndID);


inline RangeSelector range(RangeSelector Begin, RangeSelector End) {
return enclose(std::move(Begin), std::move(End));
}


inline RangeSelector range(std::string BeginID, std::string EndID) {
return encloseNodes(std::move(BeginID), std::move(EndID));
}


RangeSelector before(RangeSelector Selector);





RangeSelector after(RangeSelector Selector);


inline RangeSelector between(RangeSelector R1, RangeSelector R2) {
return enclose(after(std::move(R1)), before(std::move(R2)));
}



RangeSelector node(std::string ID);



RangeSelector statement(std::string ID);



RangeSelector member(std::string ID);








RangeSelector name(std::string ID);



RangeSelector callArgs(std::string ID);



RangeSelector statements(std::string ID);



RangeSelector initListElements(std::string ID);



RangeSelector elseBranch(std::string ID);




RangeSelector expansion(RangeSelector S);
}
}

#endif
