













#if !defined(LLVM_CLANG_LIB_FORMAT_WHITESPACEMANAGER_H)
#define LLVM_CLANG_LIB_FORMAT_WHITESPACEMANAGER_H

#include "TokenAnnotator.h"
#include "clang/Basic/SourceManager.h"
#include "clang/Format/Format.h"
#include "llvm/ADT/SmallVector.h"
#include <algorithm>
#include <string>
#include <tuple>

namespace clang {
namespace format {












class WhitespaceManager {
public:
WhitespaceManager(const SourceManager &SourceMgr, const FormatStyle &Style,
bool UseCRLF)
: SourceMgr(SourceMgr), Style(Style), UseCRLF(UseCRLF) {}

bool useCRLF() const { return UseCRLF; }







void replaceWhitespace(FormatToken &Tok, unsigned Newlines, unsigned Spaces,
unsigned StartOfTokenColumn, bool isAligned = false,
bool InPPDirective = false);





void addUntouchableToken(const FormatToken &Tok, bool InPPDirective);

llvm::Error addReplacement(const tooling::Replacement &Replacement);















void replaceWhitespaceInToken(const FormatToken &Tok, unsigned Offset,
unsigned ReplaceChars,
StringRef PreviousPostfix,
StringRef CurrentPrefix, bool InPPDirective,
unsigned Newlines, int Spaces);


const tooling::Replacements &generateReplacements();



struct Change {

class IsBeforeInFile {
public:
IsBeforeInFile(const SourceManager &SourceMgr) : SourceMgr(SourceMgr) {}
bool operator()(const Change &C1, const Change &C2) const;

private:
const SourceManager &SourceMgr;
};










Change(const FormatToken &Tok, bool CreateReplacement,
SourceRange OriginalWhitespaceRange, int Spaces,
unsigned StartOfTokenColumn, unsigned NewlinesBefore,
StringRef PreviousLinePostfix, StringRef CurrentLinePrefix,
bool IsAligned, bool ContinuesPPDirective, bool IsInsideToken);





const FormatToken *Tok;

bool CreateReplacement;


SourceRange OriginalWhitespaceRange;
unsigned StartOfTokenColumn;
unsigned NewlinesBefore;
std::string PreviousLinePostfix;
std::string CurrentLinePrefix;
bool IsAligned;
bool ContinuesPPDirective;






int Spaces;



bool IsInsideToken;




bool IsTrailingComment;
unsigned TokenLength;
unsigned PreviousEndOfTokenColumn;
unsigned EscapedNewlineColumn;









const Change *StartOfBlockComment;
int IndentationOffset;



int ConditionalsLevel;




std::tuple<unsigned, unsigned, unsigned> indentAndNestingLevel() const {
return std::make_tuple(Tok->IndentLevel, Tok->NestingLevel,
ConditionalsLevel);
}
};

private:
struct CellDescription {
unsigned Index = 0;
unsigned Cell = 0;
unsigned EndIndex = 0;
bool HasSplit = false;
CellDescription *NextColumnElement = nullptr;

constexpr bool operator==(const CellDescription &Other) const {
return Index == Other.Index && Cell == Other.Cell &&
EndIndex == Other.EndIndex;
}
constexpr bool operator!=(const CellDescription &Other) const {
return !(*this == Other);
}
};

struct CellDescriptions {
SmallVector<CellDescription> Cells;
unsigned CellCount = 0;
unsigned InitialSpaces = 0;
};




void calculateLineBreakInformation();


void alignConsecutiveMacros();


void alignConsecutiveAssignments();


void alignConsecutiveBitFields();


void alignConsecutiveDeclarations();


void alignChainedConditionals();


void alignTrailingComments();



void alignTrailingComments(unsigned Start, unsigned End, unsigned Column);


void alignEscapedNewlines();



void alignEscapedNewlines(unsigned Start, unsigned End, unsigned Column);


void alignArrayInitializers();



void alignArrayInitializers(unsigned Start, unsigned End);



void alignArrayInitializersRightJustified(CellDescriptions &&CellDescs);



void alignArrayInitializersLeftJustified(CellDescriptions &&CellDescs);


unsigned calculateCellWidth(unsigned Start, unsigned End,
bool WithSpaces = false) const;



CellDescriptions getCells(unsigned Start, unsigned End);


static bool isSplitCell(const CellDescription &Cell);


template <typename I>
auto getNetWidth(const I &Start, const I &End, unsigned InitialSpaces) const {
auto NetWidth = InitialSpaces;
for (auto PrevIter = Start; PrevIter != End; ++PrevIter) {


if (Changes[PrevIter->Index].NewlinesBefore > 0)
NetWidth = 0;
NetWidth +=
calculateCellWidth(PrevIter->Index, PrevIter->EndIndex, true) + 1;
}
return NetWidth;
}


template <typename I>
unsigned getMaximumCellWidth(I CellIter, unsigned NetWidth) const {
unsigned CellWidth =
calculateCellWidth(CellIter->Index, CellIter->EndIndex, true);
if (Changes[CellIter->Index].NewlinesBefore == 0)
CellWidth += NetWidth;
for (const auto *Next = CellIter->NextColumnElement; Next != nullptr;
Next = Next->NextColumnElement) {
auto ThisWidth = calculateCellWidth(Next->Index, Next->EndIndex, true);
if (Changes[Next->Index].NewlinesBefore == 0)
ThisWidth += NetWidth;
CellWidth = std::max(CellWidth, ThisWidth);
}
return CellWidth;
}


template <typename I>
unsigned getMaximumNetWidth(const I &CellStart, const I &CellStop,
unsigned InitialSpaces,
unsigned CellCount) const {
auto MaxNetWidth = getNetWidth(CellStart, CellStop, InitialSpaces);
auto RowCount = 1U;
auto Offset = std::distance(CellStart, CellStop);
for (const auto *Next = CellStop->NextColumnElement; Next != nullptr;
Next = Next->NextColumnElement) {
auto Start = (CellStart + RowCount * CellCount);
auto End = Start + Offset;
MaxNetWidth =
std::max(MaxNetWidth, getNetWidth(Start, End, InitialSpaces));
++RowCount;
}
return MaxNetWidth;
}


void alignToStartOfCell(unsigned Start, unsigned End);


static CellDescriptions linkCells(CellDescriptions &&CellDesc);


void generateChanges();


void storeReplacement(SourceRange Range, StringRef Text);
void appendNewlineText(std::string &Text, unsigned Newlines);
void appendEscapedNewlineText(std::string &Text, unsigned Newlines,
unsigned PreviousEndOfTokenColumn,
unsigned EscapedNewlineColumn);
void appendIndentText(std::string &Text, unsigned IndentLevel,
unsigned Spaces, unsigned WhitespaceStartColumn,
bool IsAligned);
unsigned appendTabIndent(std::string &Text, unsigned Spaces,
unsigned Indentation);

SmallVector<Change, 16> Changes;
const SourceManager &SourceMgr;
tooling::Replacements Replaces;
const FormatStyle &Style;
bool UseCRLF;
};

}
}

#endif
