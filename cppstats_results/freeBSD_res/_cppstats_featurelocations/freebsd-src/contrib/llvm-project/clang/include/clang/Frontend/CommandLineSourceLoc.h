












#if !defined(LLVM_CLANG_FRONTEND_COMMANDLINESOURCELOC_H)
#define LLVM_CLANG_FRONTEND_COMMANDLINESOURCELOC_H

#include "clang/Basic/LLVM.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/raw_ostream.h"

namespace clang {


struct ParsedSourceLocation {
std::string FileName;
unsigned Line;
unsigned Column;

public:


static ParsedSourceLocation FromString(StringRef Str) {
ParsedSourceLocation PSL;
std::pair<StringRef, StringRef> ColSplit = Str.rsplit(':');
std::pair<StringRef, StringRef> LineSplit =
ColSplit.first.rsplit(':');


if (!ColSplit.second.getAsInteger(10, PSL.Column) &&
!LineSplit.second.getAsInteger(10, PSL.Line)) {
PSL.FileName = std::string(LineSplit.first);



if (PSL.FileName == "-")
PSL.FileName = "<stdin>";
}

return PSL;
}


std::string ToString() const {
return (llvm::Twine(FileName == "<stdin>" ? "-" : FileName) + ":" +
Twine(Line) + ":" + Twine(Column))
.str();
}
};


struct ParsedSourceRange {
std::string FileName;


std::pair<unsigned, unsigned> Begin;


std::pair<unsigned, unsigned> End;










static Optional<ParsedSourceRange> fromString(StringRef Str) {
std::pair<StringRef, StringRef> RangeSplit = Str.rsplit('-');
unsigned EndLine, EndColumn;
bool HasEndLoc = false;
if (!RangeSplit.second.empty()) {
std::pair<StringRef, StringRef> Split = RangeSplit.second.rsplit(':');
if (Split.first.getAsInteger(10, EndLine) ||
Split.second.getAsInteger(10, EndColumn)) {



RangeSplit.first = Str;
} else
HasEndLoc = true;
}
auto Begin = ParsedSourceLocation::FromString(RangeSplit.first);
if (Begin.FileName.empty())
return None;
if (!HasEndLoc) {
EndLine = Begin.Line;
EndColumn = Begin.Column;
}
return ParsedSourceRange{std::move(Begin.FileName),
{Begin.Line, Begin.Column},
{EndLine, EndColumn}};
}
};
}

namespace llvm {
namespace cl {



template<>
class parser<clang::ParsedSourceLocation> final
: public basic_parser<clang::ParsedSourceLocation> {
public:
inline bool parse(Option &O, StringRef ArgName, StringRef ArgValue,
clang::ParsedSourceLocation &Val);
};

bool
parser<clang::ParsedSourceLocation>::
parse(Option &O, StringRef ArgName, StringRef ArgValue,
clang::ParsedSourceLocation &Val) {
using namespace clang;

Val = ParsedSourceLocation::FromString(ArgValue);
if (Val.FileName.empty()) {
errs() << "error: "
<< "source location must be of the form filename:line:column\n";
return true;
}

return false;
}
}
}

#endif
