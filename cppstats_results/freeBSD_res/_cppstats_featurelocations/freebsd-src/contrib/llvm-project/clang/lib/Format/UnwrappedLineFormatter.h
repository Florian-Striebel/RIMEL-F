













#if !defined(LLVM_CLANG_LIB_FORMAT_UNWRAPPEDLINEFORMATTER_H)
#define LLVM_CLANG_LIB_FORMAT_UNWRAPPEDLINEFORMATTER_H

#include "ContinuationIndenter.h"
#include "clang/Format/Format.h"
#include <map>

namespace clang {
namespace format {

class ContinuationIndenter;
class WhitespaceManager;

class UnwrappedLineFormatter {
public:
UnwrappedLineFormatter(ContinuationIndenter *Indenter,
WhitespaceManager *Whitespaces,
const FormatStyle &Style,
const AdditionalKeywords &Keywords,
const SourceManager &SourceMgr,
FormattingAttemptStatus *Status)
: Indenter(Indenter), Whitespaces(Whitespaces), Style(Style),
Keywords(Keywords), SourceMgr(SourceMgr), Status(Status) {}


unsigned format(const SmallVectorImpl<AnnotatedLine *> &Lines,
bool DryRun = false, int AdditionalIndent = 0,
bool FixBadIndentation = false, unsigned FirstStartColumn = 0,
unsigned NextStartColumn = 0, unsigned LastStartColumn = 0);

private:


void formatFirstToken(const AnnotatedLine &Line,
const AnnotatedLine *PreviousLine,
const AnnotatedLine *PrevPrevLine,
const SmallVectorImpl<AnnotatedLine *> &Lines,
unsigned Indent, unsigned NewlineIndent);



unsigned getColumnLimit(bool InPPDirective,
const AnnotatedLine *NextLine) const;




std::map<std::pair<const SmallVectorImpl<AnnotatedLine *> *, unsigned>,
unsigned>
PenaltyCache;

ContinuationIndenter *Indenter;
WhitespaceManager *Whitespaces;
const FormatStyle &Style;
const AdditionalKeywords &Keywords;
const SourceManager &SourceMgr;
FormattingAttemptStatus *Status;
};
}
}

#endif
