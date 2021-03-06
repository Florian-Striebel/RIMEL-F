












#if !defined(LLVM_CLANG_ASTMATCHERS_DYNAMIC_DIAGNOSTICS_H)
#define LLVM_CLANG_ASTMATCHERS_DYNAMIC_DIAGNOSTICS_H

#include "clang/ASTMatchers/Dynamic/VariantValue.h"
#include "clang/Basic/LLVM.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/Twine.h"
#include "llvm/Support/raw_ostream.h"
#include <string>
#include <vector>

namespace clang {
namespace ast_matchers {
namespace dynamic {

struct SourceLocation {
SourceLocation() : Line(), Column() {}
unsigned Line;
unsigned Column;
};

struct SourceRange {
SourceLocation Start;
SourceLocation End;
};


struct ParserValue {
ParserValue() : Text(), Range(), Value() {}
StringRef Text;
SourceRange Range;
VariantValue Value;
};


class Diagnostics {
public:

enum ContextType {
CT_MatcherArg = 0,
CT_MatcherConstruct = 1
};


enum ErrorType {
ET_None = 0,

ET_RegistryMatcherNotFound = 1,
ET_RegistryWrongArgCount = 2,
ET_RegistryWrongArgType = 3,
ET_RegistryNotBindable = 4,
ET_RegistryAmbiguousOverload = 5,
ET_RegistryValueNotFound = 6,
ET_RegistryUnknownEnumWithReplace = 7,
ET_RegistryNonNodeMatcher = 8,
ET_RegistryMatcherNoWithSupport = 9,

ET_ParserStringError = 100,
ET_ParserNoOpenParen = 101,
ET_ParserNoCloseParen = 102,
ET_ParserNoComma = 103,
ET_ParserNoCode = 104,
ET_ParserNotAMatcher = 105,
ET_ParserInvalidToken = 106,
ET_ParserMalformedBindExpr = 107,
ET_ParserTrailingCode = 108,
ET_ParserNumberError = 109,
ET_ParserOverloadedType = 110,
ET_ParserMalformedChainedExpr = 111,
ET_ParserFailedToBuildMatcher = 112
};


class ArgStream {
public:
ArgStream(std::vector<std::string> *Out) : Out(Out) {}
template <class T> ArgStream &operator<<(const T &Arg) {
return operator<<(Twine(Arg));
}
ArgStream &operator<<(const Twine &Arg);

private:
std::vector<std::string> *Out;
};







struct Context {
public:

enum ConstructMatcherEnum { ConstructMatcher };
Context(ConstructMatcherEnum, Diagnostics *Error, StringRef MatcherName,
SourceRange MatcherRange);

enum MatcherArgEnum { MatcherArg };
Context(MatcherArgEnum, Diagnostics *Error, StringRef MatcherName,
SourceRange MatcherRange, unsigned ArgNumber);
~Context();

private:
Diagnostics *const Error;
};





struct OverloadContext {
public:
OverloadContext(Diagnostics* Error);
~OverloadContext();


void revertErrors();

private:
Diagnostics *const Error;
unsigned BeginIndex;
};






ArgStream addError(SourceRange Range, ErrorType Error);


struct ContextFrame {
ContextType Type;
SourceRange Range;
std::vector<std::string> Args;
};


struct ErrorContent {
std::vector<ContextFrame> ContextStack;
struct Message {
SourceRange Range;
ErrorType Type;
std::vector<std::string> Args;
};
std::vector<Message> Messages;
};
ArrayRef<ErrorContent> errors() const { return Errors; }




void printToStream(llvm::raw_ostream &OS) const;
std::string toString() const;




void printToStreamFull(llvm::raw_ostream &OS) const;
std::string toStringFull() const;

private:

ArgStream pushContextFrame(ContextType Type, SourceRange Range);

std::vector<ContextFrame> ContextStack;
std::vector<ErrorContent> Errors;
};

}
}
}

#endif
