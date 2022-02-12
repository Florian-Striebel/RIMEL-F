













#if !defined(LLVM_CLANG_LIB_FORMAT_TOKENANNOTATOR_H)
#define LLVM_CLANG_LIB_FORMAT_TOKENANNOTATOR_H

#include "UnwrappedLineParser.h"
#include "clang/Format/Format.h"

namespace clang {
class SourceManager;

namespace format {

enum LineType {
LT_Invalid,
LT_ImportStatement,
LT_ObjCDecl,
LT_ObjCMethodDecl,
LT_ObjCProperty,
LT_Other,
LT_PreprocessorDirective,
LT_VirtualFunctionDecl,
LT_ArrayOfStructInitializer,
};

class AnnotatedLine {
public:
AnnotatedLine(const UnwrappedLine &Line)
: First(Line.Tokens.front().Tok), Level(Line.Level),
MatchingOpeningBlockLineIndex(Line.MatchingOpeningBlockLineIndex),
MatchingClosingBlockLineIndex(Line.MatchingClosingBlockLineIndex),
InPPDirective(Line.InPPDirective),
MustBeDeclaration(Line.MustBeDeclaration), MightBeFunctionDecl(false),
IsMultiVariableDeclStmt(false), Affected(false),
LeadingEmptyLinesAffected(false), ChildrenAffected(false),
FirstStartColumn(Line.FirstStartColumn) {
assert(!Line.Tokens.empty());




First->Previous = nullptr;
FormatToken *Current = First;
for (std::list<UnwrappedLineNode>::const_iterator I = ++Line.Tokens.begin(),
E = Line.Tokens.end();
I != E; ++I) {
const UnwrappedLineNode &Node = *I;
Current->Next = I->Tok;
I->Tok->Previous = Current;
Current = Current->Next;
Current->Children.clear();
for (const auto &Child : Node.Children) {
Children.push_back(new AnnotatedLine(Child));
Current->Children.push_back(Children.back());
}
}
Last = Current;
Last->Next = nullptr;
}

~AnnotatedLine() {
for (unsigned i = 0, e = Children.size(); i != e; ++i) {
delete Children[i];
}
FormatToken *Current = First;
while (Current) {
Current->Children.clear();
Current->Role.reset();
Current = Current->Next;
}
}



template <typename... Ts> bool startsWith(Ts... Tokens) const {
return First && First->startsSequence(Tokens...);
}





template <typename... Ts> bool endsWith(Ts... Tokens) const {
return Last && Last->endsSequence(Tokens...);
}



bool mightBeFunctionDefinition() const {
assert(MightBeFunctionDecl);










return !endsWith(tok::semi);
}


bool startsWithNamespace() const {
return startsWith(tok::kw_namespace) || startsWith(TT_NamespaceMacro) ||
startsWith(tok::kw_inline, tok::kw_namespace) ||
startsWith(tok::kw_export, tok::kw_namespace);
}

FormatToken *First;
FormatToken *Last;

SmallVector<AnnotatedLine *, 0> Children;

LineType Type;
unsigned Level;
size_t MatchingOpeningBlockLineIndex;
size_t MatchingClosingBlockLineIndex;
bool InPPDirective;
bool MustBeDeclaration;
bool MightBeFunctionDecl;
bool IsMultiVariableDeclStmt;



bool Affected;



bool LeadingEmptyLinesAffected;


bool ChildrenAffected;

unsigned FirstStartColumn;

private:

AnnotatedLine(const AnnotatedLine &) = delete;
void operator=(const AnnotatedLine &) = delete;
};



class TokenAnnotator {
public:
TokenAnnotator(const FormatStyle &Style, const AdditionalKeywords &Keywords)
: Style(Style), Keywords(Keywords) {}




void setCommentLineLevels(SmallVectorImpl<AnnotatedLine *> &Lines);

void annotate(AnnotatedLine &Line);
void calculateFormattingInformation(AnnotatedLine &Line);

private:

unsigned splitPenalty(const AnnotatedLine &Line, const FormatToken &Tok,
bool InFunctionDecl);

bool spaceRequiredBeforeParens(const FormatToken &Right) const;

bool spaceRequiredBetween(const AnnotatedLine &Line, const FormatToken &Left,
const FormatToken &Right);

bool spaceRequiredBefore(const AnnotatedLine &Line, const FormatToken &Right);

bool mustBreakBefore(const AnnotatedLine &Line, const FormatToken &Right);

bool canBreakBefore(const AnnotatedLine &Line, const FormatToken &Right);

bool mustBreakForReturnType(const AnnotatedLine &Line) const;

void printDebugInfo(const AnnotatedLine &Line);

void calculateUnbreakableTailLengths(AnnotatedLine &Line);

void calculateArrayInitializerColumnList(AnnotatedLine &Line);

FormatToken *calculateInitializerColumnList(AnnotatedLine &Line,
FormatToken *CurrentToken,
unsigned Depth);
FormatStyle::PointerAlignmentStyle
getTokenReferenceAlignment(const FormatToken &PointerOrReference);

FormatStyle::PointerAlignmentStyle
getTokenPointerOrReferenceAlignment(const FormatToken &PointerOrReference);

const FormatStyle &Style;

const AdditionalKeywords &Keywords;
};

}
}

#endif
