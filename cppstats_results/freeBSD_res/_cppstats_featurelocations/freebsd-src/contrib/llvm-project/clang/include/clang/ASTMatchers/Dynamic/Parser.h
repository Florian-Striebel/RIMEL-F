

































#if !defined(LLVM_CLANG_ASTMATCHERS_DYNAMIC_PARSER_H)
#define LLVM_CLANG_ASTMATCHERS_DYNAMIC_PARSER_H

#include "clang/ASTMatchers/ASTMatchersInternal.h"
#include "clang/ASTMatchers/Dynamic/Registry.h"
#include "clang/ASTMatchers/Dynamic/VariantValue.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/Optional.h"
#include "llvm/ADT/StringMap.h"
#include "llvm/ADT/StringRef.h"
#include <utility>
#include <vector>

namespace clang {
namespace ast_matchers {
namespace dynamic {

class Diagnostics;


class Parser {
public:










class Sema {
public:
virtual ~Sema();


















virtual VariantMatcher actOnMatcherExpression(MatcherCtor Ctor,
SourceRange NameRange,
StringRef BindID,
ArrayRef<ParserValue> Args,
Diagnostics *Error) = 0;







virtual llvm::Optional<MatcherCtor>
lookupMatcherCtor(StringRef MatcherName) = 0;

virtual bool isBuilderMatcher(MatcherCtor) const = 0;

virtual ASTNodeKind nodeMatcherType(MatcherCtor) const = 0;

virtual internal::MatcherDescriptorPtr
buildMatcherCtor(MatcherCtor, SourceRange NameRange,
ArrayRef<ParserValue> Args, Diagnostics *Error) const = 0;









virtual std::vector<ArgKind> getAcceptedCompletionTypes(
llvm::ArrayRef<std::pair<MatcherCtor, unsigned>> Context);










virtual std::vector<MatcherCompletion>
getMatcherCompletions(llvm::ArrayRef<ArgKind> AcceptedTypes);
};



class RegistrySema : public Parser::Sema {
public:
~RegistrySema() override;

llvm::Optional<MatcherCtor>
lookupMatcherCtor(StringRef MatcherName) override;

VariantMatcher actOnMatcherExpression(MatcherCtor Ctor,
SourceRange NameRange,
StringRef BindID,
ArrayRef<ParserValue> Args,
Diagnostics *Error) override;

std::vector<ArgKind> getAcceptedCompletionTypes(
llvm::ArrayRef<std::pair<MatcherCtor, unsigned>> Context) override;

bool isBuilderMatcher(MatcherCtor Ctor) const override;

ASTNodeKind nodeMatcherType(MatcherCtor) const override;

internal::MatcherDescriptorPtr
buildMatcherCtor(MatcherCtor, SourceRange NameRange,
ArrayRef<ParserValue> Args,
Diagnostics *Error) const override;

std::vector<MatcherCompletion>
getMatcherCompletions(llvm::ArrayRef<ArgKind> AcceptedTypes) override;
};

using NamedValueMap = llvm::StringMap<VariantValue>;
















static llvm::Optional<DynTypedMatcher>
parseMatcherExpression(StringRef &MatcherCode, Sema *S,
const NamedValueMap *NamedValues, Diagnostics *Error);
static llvm::Optional<DynTypedMatcher>
parseMatcherExpression(StringRef &MatcherCode, Sema *S, Diagnostics *Error) {
return parseMatcherExpression(MatcherCode, S, nullptr, Error);
}
static llvm::Optional<DynTypedMatcher>
parseMatcherExpression(StringRef &MatcherCode, Diagnostics *Error) {
return parseMatcherExpression(MatcherCode, nullptr, Error);
}













static bool parseExpression(StringRef &Code, Sema *S,
const NamedValueMap *NamedValues,
VariantValue *Value, Diagnostics *Error);
static bool parseExpression(StringRef &Code, Sema *S, VariantValue *Value,
Diagnostics *Error) {
return parseExpression(Code, S, nullptr, Value, Error);
}
static bool parseExpression(StringRef &Code, VariantValue *Value,
Diagnostics *Error) {
return parseExpression(Code, nullptr, Value, Error);
}












static std::vector<MatcherCompletion>
completeExpression(StringRef &Code, unsigned CompletionOffset, Sema *S,
const NamedValueMap *NamedValues);
static std::vector<MatcherCompletion>
completeExpression(StringRef &Code, unsigned CompletionOffset, Sema *S) {
return completeExpression(Code, CompletionOffset, S, nullptr);
}
static std::vector<MatcherCompletion>
completeExpression(StringRef &Code, unsigned CompletionOffset) {
return completeExpression(Code, CompletionOffset, nullptr);
}

private:
class CodeTokenizer;
struct ScopedContextEntry;
struct TokenInfo;

Parser(CodeTokenizer *Tokenizer, Sema *S,
const NamedValueMap *NamedValues,
Diagnostics *Error);

bool parseBindID(std::string &BindID);
bool parseExpressionImpl(VariantValue *Value);
bool parseMatcherBuilder(MatcherCtor Ctor, const TokenInfo &NameToken,
const TokenInfo &OpenToken, VariantValue *Value);
bool parseMatcherExpressionImpl(const TokenInfo &NameToken,
const TokenInfo &OpenToken,
llvm::Optional<MatcherCtor> Ctor,
VariantValue *Value);
bool parseIdentifierPrefixImpl(VariantValue *Value);

void addCompletion(const TokenInfo &CompToken,
const MatcherCompletion &Completion);
void addExpressionCompletions();

std::vector<MatcherCompletion>
getNamedValueCompletions(ArrayRef<ArgKind> AcceptedTypes);

CodeTokenizer *const Tokenizer;
Sema *const S;
const NamedValueMap *const NamedValues;
Diagnostics *const Error;

using ContextStackTy = std::vector<std::pair<MatcherCtor, unsigned>>;

ContextStackTy ContextStack;
std::vector<MatcherCompletion> Completions;
};

}
}
}

#endif
