













#if !defined(LLVM_CLANG_LIB_FORMAT_UNWRAPPEDLINEPARSER_H)
#define LLVM_CLANG_LIB_FORMAT_UNWRAPPEDLINEPARSER_H

#include "FormatToken.h"
#include "clang/Basic/IdentifierTable.h"
#include "clang/Format/Format.h"
#include "llvm/Support/Regex.h"
#include <list>
#include <stack>

namespace clang {
namespace format {

struct UnwrappedLineNode;







struct UnwrappedLine {
UnwrappedLine();



std::list<UnwrappedLineNode> Tokens;


unsigned Level;


bool InPPDirective;

bool MustBeDeclaration;





size_t MatchingOpeningBlockLineIndex = kInvalidIndex;



size_t MatchingClosingBlockLineIndex = kInvalidIndex;

static const size_t kInvalidIndex = -1;

unsigned FirstStartColumn = 0;
};

class UnwrappedLineConsumer {
public:
virtual ~UnwrappedLineConsumer() {}
virtual void consumeUnwrappedLine(const UnwrappedLine &Line) = 0;
virtual void finishRun() = 0;
};

class FormatTokenSource;

class UnwrappedLineParser {
public:
UnwrappedLineParser(const FormatStyle &Style,
const AdditionalKeywords &Keywords,
unsigned FirstStartColumn, ArrayRef<FormatToken *> Tokens,
UnwrappedLineConsumer &Callback);

void parse();

private:
void reset();
void parseFile();
void parseLevel(bool HasOpeningBrace);
void parseBlock(bool MustBeDeclaration, unsigned AddLevels = 1u,
bool MunchSemi = true,
bool UnindentWhitesmithsBraces = false);
void parseChildBlock();
void parsePPDirective();
void parsePPDefine();
void parsePPIf(bool IfDef);
void parsePPElIf();
void parsePPElse();
void parsePPEndIf();
void parsePPUnknown();
void readTokenWithJavaScriptASI();
void parseStructuralElement(bool IsTopLevel = false);
bool tryToParseBracedList();
bool parseBracedList(bool ContinueOnSemicolons = false, bool IsEnum = false,
tok::TokenKind ClosingBraceKind = tok::r_brace);
void parseParens();
void parseSquare(bool LambdaIntroducer = false);
void parseIfThenElse();
void parseTryCatch();
void parseForOrWhileLoop();
void parseDoWhile();
void parseLabel(bool LeftAlignLabel = false);
void parseCaseLabel();
void parseSwitch();
void parseNamespace();
void parseNew();
void parseAccessSpecifier();
bool parseEnum();
bool parseStructLike();
void parseConcept();
void parseRequires();
void parseRequiresExpression(unsigned int OriginalLevel);
void parseConstraintExpression(unsigned int OriginalLevel);
void parseJavaEnumBody();



void parseRecord(bool ParseAsExpr = false);
void parseObjCLightweightGenerics();
void parseObjCMethod();
void parseObjCProtocolList();
void parseObjCUntilAtEnd();
void parseObjCInterfaceOrImplementation();
bool parseObjCProtocol();
void parseJavaScriptEs6ImportExport();
void parseStatementMacro();
void parseCSharpAttribute();



void parseCSharpGenericTypeConstraint();
bool tryToParseLambda();
bool tryToParseLambdaIntroducer();
bool tryToParsePropertyAccessor();
void tryToParseJSFunction();
bool tryToParseSimpleAttribute();



enum class LineLevel { Remove, Keep };

void addUnwrappedLine(LineLevel AdjustLevel = LineLevel::Remove);
bool eof() const;




void nextToken(int LevelDifference = 0);
void readToken(int LevelDifference = 0);











void distributeComments(const SmallVectorImpl<FormatToken *> &Comments,
const FormatToken *NextTok);


void flushComments(bool NewlineBeforeNext);
void pushToken(FormatToken *Tok);
void calculateBraceTypes(bool ExpectClassBody = false);





void conditionalCompilationCondition(bool Unreachable);
void conditionalCompilationStart(bool Unreachable);
void conditionalCompilationAlternative();
void conditionalCompilationEnd();

bool isOnNewLine(const FormatToken &FormatTok);




size_t computePPHash() const;




std::unique_ptr<UnwrappedLine> Line;





SmallVector<FormatToken *, 1> CommentsBeforeNextToken;
FormatToken *FormatTok;
bool MustBreakBeforeNextToken;


SmallVector<UnwrappedLine, 8> Lines;




SmallVector<UnwrappedLine, 4> PreprocessorDirectives;





SmallVectorImpl<UnwrappedLine> *CurrentLines;



std::vector<bool> DeclarationScopeStack;

const FormatStyle &Style;
const AdditionalKeywords &Keywords;

llvm::Regex CommentPragmasRegex;

FormatTokenSource *Tokens;
UnwrappedLineConsumer &Callback;




ArrayRef<FormatToken *> AllTokens;



enum PPBranchKind {
PP_Conditional,
PP_Unreachable
};

struct PPBranch {
PPBranch(PPBranchKind Kind, size_t Line) : Kind(Kind), Line(Line) {}
PPBranchKind Kind;
size_t Line;
};


SmallVector<PPBranch, 16> PPStack;







int PPBranchLevel;



SmallVector<int, 8> PPLevelBranchIndex;


SmallVector<int, 8> PPLevelBranchCount;





std::stack<int> PPChainBranchIndex;



enum IncludeGuardState {
IG_Inited,
IG_IfNdefed,
IG_Defined,
IG_Found,
IG_Rejected,
};


IncludeGuardState IncludeGuard;



FormatToken *IncludeGuardToken;




unsigned FirstStartColumn;

friend class ScopedLineState;
friend class CompoundStatementIndenter;
};

struct UnwrappedLineNode {
UnwrappedLineNode() : Tok(nullptr) {}
UnwrappedLineNode(FormatToken *Tok) : Tok(Tok) {}

FormatToken *Tok;
SmallVector<UnwrappedLine, 0> Children;
};

inline UnwrappedLine::UnwrappedLine()
: Level(0), InPPDirective(false), MustBeDeclaration(false),
MatchingOpeningBlockLineIndex(kInvalidIndex) {}

}
}

#endif
