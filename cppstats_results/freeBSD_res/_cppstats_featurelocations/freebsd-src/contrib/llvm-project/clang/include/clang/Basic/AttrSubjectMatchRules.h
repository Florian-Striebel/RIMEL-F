







#if !defined(LLVM_CLANG_BASIC_ATTR_SUBJECT_MATCH_RULES_H)
#define LLVM_CLANG_BASIC_ATTR_SUBJECT_MATCH_RULES_H

#include "clang/Basic/SourceLocation.h"
#include "llvm/ADT/DenseMap.h"

namespace clang {
namespace attr {


enum SubjectMatchRule {
#define ATTR_MATCH_RULE(X, Spelling, IsAbstract) X,
#include "clang/Basic/AttrSubMatchRulesList.inc"
};

const char *getSubjectMatchRuleSpelling(SubjectMatchRule Rule);

using ParsedSubjectMatchRuleSet = llvm::DenseMap<int, SourceRange>;

}
}

#endif
