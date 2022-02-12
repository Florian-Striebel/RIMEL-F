







#if !defined(LLVM_CLANG_BASIC_JSONSUPPORT_H)
#define LLVM_CLANG_BASIC_JSONSUPPORT_H

#include "clang/Basic/LLVM.h"
#include "clang/Basic/SourceManager.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/raw_ostream.h"
#include <iterator>

namespace clang {

inline raw_ostream &Indent(raw_ostream &Out, const unsigned int Space,
bool IsDot) {
for (unsigned int I = 0; I < Space * 2; ++I)
Out << (IsDot ? "&nbsp;" : " ");
return Out;
}

inline std::string JsonFormat(StringRef RawSR, bool AddQuotes) {
if (RawSR.empty())
return "null";


std::string Str = RawSR.trim().str();
size_t Pos = 0;


while (true) {
Pos = Str.find('\\', Pos);
if (Pos == std::string::npos)
break;


size_t TempPos = (Pos != 0) ? Pos - 1 : 0;


if (TempPos != Str.find("\\\\", Pos)) {
Str.insert(Pos, "\\");
++Pos;
}

++Pos;
}


Pos = 0;
while (true) {
Pos = Str.find('\"', Pos);
if (Pos == std::string::npos)
break;


size_t TempPos = (Pos != 0) ? Pos - 1 : 0;


if (TempPos != Str.find("\\\"", Pos)) {
Str.insert(Pos, "\\");
++Pos;
}

++Pos;
}


Str.erase(std::remove(Str.begin(), Str.end(), '\n'), Str.end());

if (!AddQuotes)
return Str;

return '\"' + Str + '\"';
}

inline void printSourceLocationAsJson(raw_ostream &Out, SourceLocation Loc,
const SourceManager &SM,
bool AddBraces = true) {

if (!Loc.isValid()) {
Out << "null";
return;
}

if (Loc.isFileID()) {
PresumedLoc PLoc = SM.getPresumedLoc(Loc);

if (PLoc.isInvalid()) {
Out << "null";
return;
}

if (AddBraces)
Out << "{ ";
std::string filename(PLoc.getFilename());
#if defined(_WIN32)

auto RemoveIt =
std::remove_if(filename.begin(), filename.end(), [](auto Char) {
static const char ForbiddenChars[] = "<>*?\"|";
return std::find(std::begin(ForbiddenChars), std::end(ForbiddenChars),
Char) != std::end(ForbiddenChars);
});
filename.erase(RemoveIt, filename.end());

std::replace(filename.begin(), filename.end(), '\\', '/');
#endif
Out << "\"line\": " << PLoc.getLine()
<< ", \"column\": " << PLoc.getColumn()
<< ", \"file\": \"" << filename << "\"";
if (AddBraces)
Out << " }";
return;
}




Out << "{ ";
printSourceLocationAsJson(Out, SM.getExpansionLoc(Loc), SM, false);
Out << ", \"spelling\": ";
printSourceLocationAsJson(Out, SM.getSpellingLoc(Loc), SM, true);
Out << " }";
}
}

#endif
