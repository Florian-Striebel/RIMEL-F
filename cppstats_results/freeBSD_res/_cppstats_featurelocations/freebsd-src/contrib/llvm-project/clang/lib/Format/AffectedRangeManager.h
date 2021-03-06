












#if !defined(LLVM_CLANG_LIB_FORMAT_AFFECTEDRANGEMANAGER_H)
#define LLVM_CLANG_LIB_FORMAT_AFFECTEDRANGEMANAGER_H

#include "clang/Basic/SourceManager.h"

namespace clang {
namespace format {

struct FormatToken;
class AnnotatedLine;

class AffectedRangeManager {
public:
AffectedRangeManager(const SourceManager &SourceMgr,
const ArrayRef<CharSourceRange> Ranges)
: SourceMgr(SourceMgr), Ranges(Ranges.begin(), Ranges.end()) {}




bool computeAffectedLines(SmallVectorImpl<AnnotatedLine *> &Lines);


bool affectsCharSourceRange(const CharSourceRange &Range);

private:


bool affectsTokenRange(const FormatToken &First, const FormatToken &Last,
bool IncludeLeadingNewlines);



bool affectsLeadingEmptyLines(const FormatToken &Tok);


void markAllAsAffected(SmallVectorImpl<AnnotatedLine *>::iterator I,
SmallVectorImpl<AnnotatedLine *>::iterator E);



bool nonPPLineAffected(AnnotatedLine *Line, const AnnotatedLine *PreviousLine,
SmallVectorImpl<AnnotatedLine *> &Lines);

const SourceManager &SourceMgr;
const SmallVector<CharSourceRange, 8> Ranges;
};

}
}

#endif
