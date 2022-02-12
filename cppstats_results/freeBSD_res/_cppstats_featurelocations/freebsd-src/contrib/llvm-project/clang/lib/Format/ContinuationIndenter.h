













#if !defined(LLVM_CLANG_LIB_FORMAT_CONTINUATIONINDENTER_H)
#define LLVM_CLANG_LIB_FORMAT_CONTINUATIONINDENTER_H

#include "Encoding.h"
#include "FormatToken.h"
#include "clang/Format/Format.h"
#include "llvm/Support/Regex.h"
#include <map>
#include <tuple>

namespace clang {
class SourceManager;

namespace format {

class AnnotatedLine;
class BreakableToken;
struct FormatToken;
struct LineState;
struct ParenState;
struct RawStringFormatStyleManager;
class WhitespaceManager;

struct RawStringFormatStyleManager {
llvm::StringMap<FormatStyle> DelimiterStyle;
llvm::StringMap<FormatStyle> EnclosingFunctionStyle;

RawStringFormatStyleManager(const FormatStyle &CodeStyle);

llvm::Optional<FormatStyle> getDelimiterStyle(StringRef Delimiter) const;

llvm::Optional<FormatStyle>
getEnclosingFunctionStyle(StringRef EnclosingFunction) const;
};

class ContinuationIndenter {
public:


ContinuationIndenter(const FormatStyle &Style,
const AdditionalKeywords &Keywords,
const SourceManager &SourceMgr,
WhitespaceManager &Whitespaces,
encoding::Encoding Encoding,
bool BinPackInconclusiveFunctions);





LineState getInitialState(unsigned FirstIndent, unsigned FirstStartColumn,
const AnnotatedLine *Line, bool DryRun);




bool canBreak(const LineState &State);


bool mustBreak(const LineState &State);









unsigned addTokenToState(LineState &State, bool Newline, bool DryRun,
unsigned ExtraSpaces = 0);



unsigned getColumnLimit(const LineState &State) const;

private:


unsigned moveStateToNextToken(LineState &State, bool DryRun, bool Newline);


void moveStatePastFakeLParens(LineState &State, bool Newline);

void moveStatePastFakeRParens(LineState &State);


void moveStatePastScopeOpener(LineState &State, bool Newline);

void moveStatePastScopeCloser(LineState &State);

void moveStateToNewBlock(LineState &State);




unsigned reformatRawStringLiteral(const FormatToken &Current,
LineState &State,
const FormatStyle &RawStringStyle,
bool DryRun, bool Newline);



unsigned handleEndOfLine(const FormatToken &Current, LineState &State,
bool DryRun, bool AllowBreak, bool Newline);



llvm::Optional<FormatStyle> getRawStringStyle(const FormatToken &Current,
const LineState &State);



















std::pair<unsigned, bool> breakProtrudingToken(const FormatToken &Current,
LineState &State,
bool AllowBreak, bool DryRun,
bool Strict);



std::unique_ptr<BreakableToken>
createBreakableToken(const FormatToken &Current, LineState &State,
bool AllowBreak);








void addTokenOnCurrentLine(LineState &State, bool DryRun,
unsigned ExtraSpaces);








unsigned addTokenOnNewLine(LineState &State, bool DryRun);


unsigned getNewLineColumn(const LineState &State);






unsigned addMultilineToken(const FormatToken &Current, LineState &State);






bool nextIsMultilineString(const LineState &State);

FormatStyle Style;
const AdditionalKeywords &Keywords;
const SourceManager &SourceMgr;
WhitespaceManager &Whitespaces;
encoding::Encoding Encoding;
bool BinPackInconclusiveFunctions;
llvm::Regex CommentPragmasRegex;
const RawStringFormatStyleManager RawStringFormats;
};

struct ParenState {
ParenState(const FormatToken *Tok, unsigned Indent, unsigned LastSpace,
bool AvoidBinPacking, bool NoLineBreak)
: Tok(Tok), Indent(Indent), LastSpace(LastSpace),
NestedBlockIndent(Indent), IsAligned(false),
BreakBeforeClosingBrace(false), AvoidBinPacking(AvoidBinPacking),
BreakBeforeParameter(false), NoLineBreak(NoLineBreak),
NoLineBreakInOperand(false), LastOperatorWrapped(true),
ContainsLineBreak(false), ContainsUnwrappedBuilder(false),
AlignColons(true), ObjCSelectorNameFound(false),
HasMultipleNestedBlocks(false), NestedBlockInlined(false),
IsInsideObjCArrayLiteral(false), IsCSharpGenericTypeConstraint(false),
IsChainedConditional(false), IsWrappedConditional(false),
UnindentOperator(false) {}






const FormatToken *Tok;



unsigned Indent;






unsigned LastSpace;



unsigned NestedBlockIndent;





unsigned FirstLessLess = 0;


unsigned QuestionColumn = 0;


unsigned ColonPos = 0;


unsigned StartOfFunctionCall = 0;



unsigned StartOfArraySubscripts = 0;



unsigned NestedNameSpecifierContinuation = 0;



unsigned CallContinuation = 0;




unsigned VariablePos = 0;


bool IsAligned : 1;






bool BreakBeforeClosingBrace : 1;



bool AvoidBinPacking : 1;



bool BreakBeforeParameter : 1;


bool NoLineBreak : 1;



bool NoLineBreakInOperand : 1;



bool LastOperatorWrapped : 1;






bool ContainsLineBreak : 1;



bool ContainsUnwrappedBuilder : 1;






bool AlignColons : 1;






bool ObjCSelectorNameFound : 1;





bool HasMultipleNestedBlocks : 1;



bool NestedBlockInlined : 1;



bool IsInsideObjCArrayLiteral : 1;

bool IsCSharpGenericTypeConstraint : 1;



bool IsChainedConditional : 1;



bool IsWrappedConditional : 1;



bool UnindentOperator : 1;

bool operator<(const ParenState &Other) const {
if (Indent != Other.Indent)
return Indent < Other.Indent;
if (LastSpace != Other.LastSpace)
return LastSpace < Other.LastSpace;
if (NestedBlockIndent != Other.NestedBlockIndent)
return NestedBlockIndent < Other.NestedBlockIndent;
if (FirstLessLess != Other.FirstLessLess)
return FirstLessLess < Other.FirstLessLess;
if (IsAligned != Other.IsAligned)
return IsAligned;
if (BreakBeforeClosingBrace != Other.BreakBeforeClosingBrace)
return BreakBeforeClosingBrace;
if (QuestionColumn != Other.QuestionColumn)
return QuestionColumn < Other.QuestionColumn;
if (AvoidBinPacking != Other.AvoidBinPacking)
return AvoidBinPacking;
if (BreakBeforeParameter != Other.BreakBeforeParameter)
return BreakBeforeParameter;
if (NoLineBreak != Other.NoLineBreak)
return NoLineBreak;
if (LastOperatorWrapped != Other.LastOperatorWrapped)
return LastOperatorWrapped;
if (ColonPos != Other.ColonPos)
return ColonPos < Other.ColonPos;
if (StartOfFunctionCall != Other.StartOfFunctionCall)
return StartOfFunctionCall < Other.StartOfFunctionCall;
if (StartOfArraySubscripts != Other.StartOfArraySubscripts)
return StartOfArraySubscripts < Other.StartOfArraySubscripts;
if (CallContinuation != Other.CallContinuation)
return CallContinuation < Other.CallContinuation;
if (VariablePos != Other.VariablePos)
return VariablePos < Other.VariablePos;
if (ContainsLineBreak != Other.ContainsLineBreak)
return ContainsLineBreak;
if (ContainsUnwrappedBuilder != Other.ContainsUnwrappedBuilder)
return ContainsUnwrappedBuilder;
if (NestedBlockInlined != Other.NestedBlockInlined)
return NestedBlockInlined;
if (IsCSharpGenericTypeConstraint != Other.IsCSharpGenericTypeConstraint)
return IsCSharpGenericTypeConstraint;
if (IsChainedConditional != Other.IsChainedConditional)
return IsChainedConditional;
if (IsWrappedConditional != Other.IsWrappedConditional)
return IsWrappedConditional;
if (UnindentOperator != Other.UnindentOperator)
return UnindentOperator;
return false;
}
};




struct LineState {

unsigned Column;


FormatToken *NextToken;


bool LineContainsContinuedForLoopSection;


bool NoContinuation;


unsigned StartOfLineLevel;


unsigned LowestLevelOnLine;



unsigned StartOfStringLiteral;



std::vector<ParenState> Stack;














bool IgnoreStackForComparison;


unsigned FirstIndent;




const AnnotatedLine *Line;


bool operator<(const LineState &Other) const {
if (NextToken != Other.NextToken)
return NextToken < Other.NextToken;
if (Column != Other.Column)
return Column < Other.Column;
if (LineContainsContinuedForLoopSection !=
Other.LineContainsContinuedForLoopSection)
return LineContainsContinuedForLoopSection;
if (NoContinuation != Other.NoContinuation)
return NoContinuation;
if (StartOfLineLevel != Other.StartOfLineLevel)
return StartOfLineLevel < Other.StartOfLineLevel;
if (LowestLevelOnLine != Other.LowestLevelOnLine)
return LowestLevelOnLine < Other.LowestLevelOnLine;
if (StartOfStringLiteral != Other.StartOfStringLiteral)
return StartOfStringLiteral < Other.StartOfStringLiteral;
if (IgnoreStackForComparison || Other.IgnoreStackForComparison)
return false;
return Stack < Other.Stack;
}
};

}
}

#endif
