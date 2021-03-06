














#if !defined(LLVM_CLANG_ASTMATCHERS_DYNAMIC_REGISTRY_H)
#define LLVM_CLANG_ASTMATCHERS_DYNAMIC_REGISTRY_H

#include "clang/ASTMatchers/Dynamic/Diagnostics.h"
#include "clang/ASTMatchers/Dynamic/VariantValue.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/Optional.h"
#include "llvm/ADT/StringRef.h"
#include <string>
#include <utility>
#include <vector>

namespace clang {
namespace ast_matchers {
namespace dynamic {

namespace internal {

class MatcherDescriptor;



class MatcherDescriptorPtr {
public:
explicit MatcherDescriptorPtr(MatcherDescriptor *);
~MatcherDescriptorPtr();
MatcherDescriptorPtr(MatcherDescriptorPtr &&) = default;
MatcherDescriptorPtr &operator=(MatcherDescriptorPtr &&) = default;
MatcherDescriptorPtr(const MatcherDescriptorPtr &) = delete;
MatcherDescriptorPtr &operator=(const MatcherDescriptorPtr &) = delete;

MatcherDescriptor *get() { return Ptr; }

private:
MatcherDescriptor *Ptr;
};

}

using MatcherCtor = const internal::MatcherDescriptor *;

struct MatcherCompletion {
MatcherCompletion() = default;
MatcherCompletion(StringRef TypedText, StringRef MatcherDecl,
unsigned Specificity)
: TypedText(TypedText), MatcherDecl(MatcherDecl),
Specificity(Specificity) {}

bool operator==(const MatcherCompletion &Other) const {
return TypedText == Other.TypedText && MatcherDecl == Other.MatcherDecl;
}


std::string TypedText;


std::string MatcherDecl;






unsigned Specificity;
};

class Registry {
public:
Registry() = delete;

static ASTNodeKind nodeMatcherType(MatcherCtor);

static bool isBuilderMatcher(MatcherCtor Ctor);

static internal::MatcherDescriptorPtr
buildMatcherCtor(MatcherCtor, SourceRange NameRange,
ArrayRef<ParserValue> Args, Diagnostics *Error);





static llvm::Optional<MatcherCtor> lookupMatcherCtor(StringRef MatcherName);









static std::vector<ArgKind> getAcceptedCompletionTypes(
llvm::ArrayRef<std::pair<MatcherCtor, unsigned>> Context);










static std::vector<MatcherCompletion>
getMatcherCompletions(ArrayRef<ArgKind> AcceptedTypes);
















static VariantMatcher constructMatcher(MatcherCtor Ctor,
SourceRange NameRange,
ArrayRef<ParserValue> Args,
Diagnostics *Error);







static VariantMatcher constructBoundMatcher(MatcherCtor Ctor,
SourceRange NameRange,
StringRef BindID,
ArrayRef<ParserValue> Args,
Diagnostics *Error);
};

}
}
}

#endif
