













#if !defined(LLVM_CLANG_LIB_FORMAT_FORMATTOKEN_H)
#define LLVM_CLANG_LIB_FORMAT_FORMATTOKEN_H

#include "clang/Basic/IdentifierTable.h"
#include "clang/Basic/OperatorPrecedence.h"
#include "clang/Format/Format.h"
#include "clang/Lex/Lexer.h"
#include <memory>
#include <unordered_set>

namespace clang {
namespace format {

#define LIST_TOKEN_TYPES TYPE(ArrayInitializerLSquare) TYPE(ArraySubscriptLSquare) TYPE(AttributeColon) TYPE(AttributeMacro) TYPE(AttributeParen) TYPE(AttributeSquare) TYPE(BinaryOperator) TYPE(BitFieldColon) TYPE(BlockComment) TYPE(CastRParen) TYPE(ConditionalExpr) TYPE(ConflictAlternative) TYPE(ConflictEnd) TYPE(ConflictStart) TYPE(ConstraintJunctions) TYPE(CtorInitializerColon) TYPE(CtorInitializerComma) TYPE(DesignatedInitializerLSquare) TYPE(DesignatedInitializerPeriod) TYPE(DictLiteral) TYPE(FatArrow) TYPE(ForEachMacro) TYPE(FunctionAnnotationRParen) TYPE(FunctionDeclarationName) TYPE(FunctionLBrace) TYPE(FunctionTypeLParen) TYPE(IfMacro) TYPE(ImplicitStringLiteral) TYPE(InheritanceColon) TYPE(InheritanceComma) TYPE(InlineASMBrace) TYPE(InlineASMColon) TYPE(InlineASMSymbolicNameLSquare) TYPE(JavaAnnotation) TYPE(JsComputedPropertyName) TYPE(JsExponentiation) TYPE(JsExponentiationEqual) TYPE(JsPipePipeEqual) TYPE(JsPrivateIdentifier) TYPE(JsTypeColon) TYPE(JsTypeOperator) TYPE(JsTypeOptionalQuestion) TYPE(JsAndAndEqual) TYPE(LambdaArrow) TYPE(LambdaLBrace) TYPE(LambdaLSquare) TYPE(LeadingJavaAnnotation) TYPE(LineComment) TYPE(MacroBlockBegin) TYPE(MacroBlockEnd) TYPE(NamespaceMacro) TYPE(NonNullAssertion) TYPE(NullCoalescingEqual) TYPE(NullCoalescingOperator) TYPE(NullPropagatingOperator) TYPE(ObjCBlockLBrace) TYPE(ObjCBlockLParen) TYPE(ObjCDecl) TYPE(ObjCForIn) TYPE(ObjCMethodExpr) TYPE(ObjCMethodSpecifier) TYPE(ObjCProperty) TYPE(ObjCStringLiteral) TYPE(OverloadedOperator) TYPE(OverloadedOperatorLParen) TYPE(PointerOrReference) TYPE(PureVirtualSpecifier) TYPE(RangeBasedForLoopColon) TYPE(RegexLiteral) TYPE(SelectorName) TYPE(StartOfName) TYPE(StatementAttributeLikeMacro) TYPE(StatementMacro) TYPE(StructuredBindingLSquare) TYPE(TemplateCloser) TYPE(TemplateOpener) TYPE(TemplateString) TYPE(ProtoExtensionLSquare) TYPE(TrailingAnnotation) TYPE(TrailingReturnArrow) TYPE(TrailingUnaryOperator) TYPE(TypeDeclarationParen) TYPE(TypenameMacro) TYPE(UnaryOperator) TYPE(UntouchableMacroFunc) TYPE(CSharpStringLiteral) TYPE(CSharpNamedArgumentColon) TYPE(CSharpNullable) TYPE(CSharpNullConditionalLSquare) TYPE(CSharpGenericTypeConstraint) TYPE(CSharpGenericTypeConstraintColon) TYPE(CSharpGenericTypeConstraintComma) TYPE(Unknown)
































































































enum TokenType : uint8_t {
#define TYPE(X) TT_##X,
LIST_TOKEN_TYPES
#undef TYPE
NUM_TOKEN_TYPES
};


const char *getTokenTypeName(TokenType Type);


enum BraceBlockKind { BK_Unknown, BK_Block, BK_BracedInit };


enum ParameterPackingKind { PPK_BinPacked, PPK_OnePerLine, PPK_Inconclusive };

enum FormatDecision { FD_Unformatted, FD_Continue, FD_Break };


enum MacroRole {


MR_ExpandedArg,


MR_UnexpandedArg,


MR_Hidden,
};

struct FormatToken;


























struct MacroExpansion {
MacroExpansion(MacroRole Role) : Role(Role) {}







MacroRole Role;


llvm::SmallVector<FormatToken *, 1> ExpandedFrom;


unsigned StartOfExpansion = 0;



unsigned EndOfExpansion = 0;
};

class TokenRole;
class AnnotatedLine;



struct FormatToken {
FormatToken()
: HasUnescapedNewline(false), IsMultiline(false), IsFirst(false),
MustBreakBefore(false), IsUnterminatedLiteral(false),
CanBreakBefore(false), ClosesTemplateDeclaration(false),
StartsBinaryExpression(false), EndsBinaryExpression(false),
PartOfMultiVariableDeclStmt(false), ContinuesLineCommentSection(false),
Finalized(false), BlockKind(BK_Unknown), Decision(FD_Unformatted),
PackingKind(PPK_Inconclusive), Type(TT_Unknown) {}


Token Tok;





StringRef TokenText;





std::shared_ptr<TokenRole> Role;


SourceRange WhitespaceRange;



unsigned HasUnescapedNewline : 1;


unsigned IsMultiline : 1;


unsigned IsFirst : 1;





unsigned MustBreakBefore : 1;


unsigned IsUnterminatedLiteral : 1;


unsigned CanBreakBefore : 1;


unsigned ClosesTemplateDeclaration : 1;



unsigned StartsBinaryExpression : 1;

unsigned EndsBinaryExpression : 1;




unsigned PartOfMultiVariableDeclStmt : 1;




unsigned ContinuesLineCommentSection : 1;




unsigned Finalized : 1;

private:

unsigned BlockKind : 2;

public:
BraceBlockKind getBlockKind() const {
return static_cast<BraceBlockKind>(BlockKind);
}
void setBlockKind(BraceBlockKind BBK) {
BlockKind = BBK;
assert(getBlockKind() == BBK && "BraceBlockKind overflow!");
}

private:

unsigned Decision : 2;

public:
FormatDecision getDecision() const {
return static_cast<FormatDecision>(Decision);
}
void setDecision(FormatDecision D) {
Decision = D;
assert(getDecision() == D && "FormatDecision overflow!");
}

private:

unsigned PackingKind : 2;

public:
ParameterPackingKind getPackingKind() const {
return static_cast<ParameterPackingKind>(PackingKind);
}
void setPackingKind(ParameterPackingKind K) {
PackingKind = K;
assert(getPackingKind() == K && "ParameterPackingKind overflow!");
}

private:
TokenType Type;

public:


TokenType getType() const { return Type; }
void setType(TokenType T) { Type = T; }





unsigned NewlinesBefore = 0;



unsigned LastNewlineOffset = 0;




unsigned ColumnWidth = 0;



unsigned LastLineColumnWidth = 0;


unsigned SpacesRequiredBefore = 0;


unsigned ParameterCount = 0;



unsigned BlockParameterCount = 0;



tok::TokenKind ParentBracket = tok::unknown;



unsigned TotalLength = 0;



unsigned OriginalColumn = 0;



unsigned UnbreakableTailLength = 0;




unsigned BindingStrength = 0;



unsigned NestingLevel = 0;


unsigned IndentLevel = 0;


unsigned SplitPenalty = 0;






unsigned LongestObjCSelectorName = 0;




unsigned ObjCSelectorNameParts = 0;




unsigned ParameterIndex = 0;






SmallVector<prec::Level, 4> FakeLParens;

unsigned FakeRParens = 0;



unsigned OperatorIndex = 0;



FormatToken *NextOperator = nullptr;


FormatToken *MatchingParen = nullptr;


FormatToken *Previous = nullptr;


FormatToken *Next = nullptr;


bool StartsColumn = false;


bool ArrayInitializerLineStart = false;


bool IsArrayInitializer = false;



SmallVector<AnnotatedLine *, 1> Children;



llvm::Optional<MacroExpansion> MacroCtx;

bool is(tok::TokenKind Kind) const { return Tok.is(Kind); }
bool is(TokenType TT) const { return getType() == TT; }
bool is(const IdentifierInfo *II) const {
return II && II == Tok.getIdentifierInfo();
}
bool is(tok::PPKeywordKind Kind) const {
return Tok.getIdentifierInfo() &&
Tok.getIdentifierInfo()->getPPKeywordID() == Kind;
}
bool is(BraceBlockKind BBK) const { return getBlockKind() == BBK; }
bool is(ParameterPackingKind PPK) const { return getPackingKind() == PPK; }

template <typename A, typename B> bool isOneOf(A K1, B K2) const {
return is(K1) || is(K2);
}
template <typename A, typename B, typename... Ts>
bool isOneOf(A K1, B K2, Ts... Ks) const {
return is(K1) || isOneOf(K2, Ks...);
}
template <typename T> bool isNot(T Kind) const { return !is(Kind); }

bool isIf(bool AllowConstexprMacro = true) const {
return is(tok::kw_if) || endsSequence(tok::kw_constexpr, tok::kw_if) ||
(endsSequence(tok::identifier, tok::kw_if) && AllowConstexprMacro);
}

bool closesScopeAfterBlock() const {
if (getBlockKind() == BK_Block)
return true;
if (closesScope())
return Previous->closesScopeAfterBlock();
return false;
}



template <typename A, typename... Ts>
bool startsSequence(A K1, Ts... Tokens) const {
return startsSequenceInternal(K1, Tokens...);
}







template <typename A, typename... Ts>
bool endsSequence(A K1, Ts... Tokens) const {
return endsSequenceInternal(K1, Tokens...);
}

bool isStringLiteral() const { return tok::isStringLiteral(Tok.getKind()); }

bool isObjCAtKeyword(tok::ObjCKeywordKind Kind) const {
return Tok.isObjCAtKeyword(Kind);
}

bool isAccessSpecifier(bool ColonRequired = true) const {
return isOneOf(tok::kw_public, tok::kw_protected, tok::kw_private) &&
(!ColonRequired || (Next && Next->is(tok::colon)));
}

bool canBePointerOrReferenceQualifier() const {
return isOneOf(tok::kw_const, tok::kw_restrict, tok::kw_volatile,
tok::kw___attribute, tok::kw__Nonnull, tok::kw__Nullable,
tok::kw__Null_unspecified, tok::kw___ptr32, tok::kw___ptr64,
TT_AttributeMacro);
}


bool isSimpleTypeSpecifier() const;

bool isObjCAccessSpecifier() const {
return is(tok::at) && Next &&
(Next->isObjCAtKeyword(tok::objc_public) ||
Next->isObjCAtKeyword(tok::objc_protected) ||
Next->isObjCAtKeyword(tok::objc_package) ||
Next->isObjCAtKeyword(tok::objc_private));
}



bool opensScope() const {
if (is(TT_TemplateString) && TokenText.endswith("${"))
return true;
if (is(TT_DictLiteral) && is(tok::less))
return true;
return isOneOf(tok::l_paren, tok::l_brace, tok::l_square,
TT_TemplateOpener);
}


bool closesScope() const {
if (is(TT_TemplateString) && TokenText.startswith("}"))
return true;
if (is(TT_DictLiteral) && is(tok::greater))
return true;
return isOneOf(tok::r_paren, tok::r_brace, tok::r_square,
TT_TemplateCloser);
}


bool isMemberAccess() const {
return isOneOf(tok::arrow, tok::period, tok::arrowstar) &&
!isOneOf(TT_DesignatedInitializerPeriod, TT_TrailingReturnArrow,
TT_LambdaArrow, TT_LeadingJavaAnnotation);
}

bool isUnaryOperator() const {
switch (Tok.getKind()) {
case tok::plus:
case tok::plusplus:
case tok::minus:
case tok::minusminus:
case tok::exclaim:
case tok::tilde:
case tok::kw_sizeof:
case tok::kw_alignof:
return true;
default:
return false;
}
}

bool isBinaryOperator() const {

return getPrecedence() > prec::Comma;
}

bool isTrailingComment() const {
return is(tok::comment) &&
(is(TT_LineComment) || !Next || Next->NewlinesBefore > 0);
}



bool isFunctionLikeKeyword() const {
switch (Tok.getKind()) {
case tok::kw_throw:
case tok::kw_typeid:
case tok::kw_return:
case tok::kw_sizeof:
case tok::kw_alignof:
case tok::kw_alignas:
case tok::kw_decltype:
case tok::kw_noexcept:
case tok::kw_static_assert:
case tok::kw__Atomic:
case tok::kw___attribute:
case tok::kw___underlying_type:
case tok::kw_requires:
return true;
default:
return false;
}
}



bool isLabelString() const {
if (!is(tok::string_literal))
return false;
StringRef Content = TokenText;
if (Content.startswith("\"") || Content.startswith("'"))
Content = Content.drop_front(1);
if (Content.endswith("\"") || Content.endswith("'"))
Content = Content.drop_back(1);
Content = Content.trim();
return Content.size() > 1 &&
(Content.back() == ':' || Content.back() == '=');
}






SourceLocation getStartOfNonWhitespace() const {
return WhitespaceRange.getEnd();
}

prec::Level getPrecedence() const {
return getBinOpPrecedence(Tok.getKind(), true,
true);
}


FormatToken *getPreviousNonComment() const {
FormatToken *Tok = Previous;
while (Tok && Tok->is(tok::comment))
Tok = Tok->Previous;
return Tok;
}


const FormatToken *getNextNonComment() const {
const FormatToken *Tok = Next;
while (Tok && Tok->is(tok::comment))
Tok = Tok->Next;
return Tok;
}



bool opensBlockOrBlockTypeList(const FormatStyle &Style) const {

if (is(tok::l_brace) && getBlockKind() == BK_BracedInit && Style.isCSharp())
return true;
if (is(TT_TemplateString) && opensScope())
return true;
return is(TT_ArrayInitializerLSquare) || is(TT_ProtoExtensionLSquare) ||
(is(tok::l_brace) &&
(getBlockKind() == BK_Block || is(TT_DictLiteral) ||
(!Style.Cpp11BracedListStyle && NestingLevel == 0))) ||
(is(tok::less) && (Style.Language == FormatStyle::LK_Proto ||
Style.Language == FormatStyle::LK_TextProto));
}



bool isCppStructuredBinding(const FormatStyle &Style) const {
if (!Style.isCpp() || isNot(tok::l_square))
return false;
const FormatToken *T = this;
do {
T = T->getPreviousNonComment();
} while (T && T->isOneOf(tok::kw_const, tok::kw_volatile, tok::amp,
tok::ampamp));
return T && T->is(tok::kw_auto);
}


bool closesBlockOrBlockTypeList(const FormatStyle &Style) const {
if (is(TT_TemplateString) && closesScope())
return true;
return MatchingParen && MatchingParen->opensBlockOrBlockTypeList(Style);
}



const FormatToken *getNamespaceToken() const {
const FormatToken *NamespaceTok = this;
if (is(tok::comment))
NamespaceTok = NamespaceTok->getNextNonComment();

if (NamespaceTok && NamespaceTok->isOneOf(tok::kw_inline, tok::kw_export))
NamespaceTok = NamespaceTok->getNextNonComment();
return NamespaceTok &&
NamespaceTok->isOneOf(tok::kw_namespace, TT_NamespaceMacro)
? NamespaceTok
: nullptr;
}

void copyFrom(const FormatToken &Tok) { *this = Tok; }

private:

FormatToken(const FormatToken &) = delete;
FormatToken &operator=(const FormatToken &) = default;

template <typename A, typename... Ts>
bool startsSequenceInternal(A K1, Ts... Tokens) const {
if (is(tok::comment) && Next)
return Next->startsSequenceInternal(K1, Tokens...);
return is(K1) && Next && Next->startsSequenceInternal(Tokens...);
}

template <typename A> bool startsSequenceInternal(A K1) const {
if (is(tok::comment) && Next)
return Next->startsSequenceInternal(K1);
return is(K1);
}

template <typename A, typename... Ts> bool endsSequenceInternal(A K1) const {
if (is(tok::comment) && Previous)
return Previous->endsSequenceInternal(K1);
return is(K1);
}

template <typename A, typename... Ts>
bool endsSequenceInternal(A K1, Ts... Tokens) const {
if (is(tok::comment) && Previous)
return Previous->endsSequenceInternal(K1, Tokens...);
return is(K1) && Previous && Previous->endsSequenceInternal(Tokens...);
}
};

class ContinuationIndenter;
struct LineState;

class TokenRole {
public:
TokenRole(const FormatStyle &Style) : Style(Style) {}
virtual ~TokenRole();



virtual void precomputeFormattingInfos(const FormatToken *Token);







virtual unsigned formatFromToken(LineState &State,
ContinuationIndenter *Indenter,
bool DryRun) {
return 0;
}



virtual unsigned formatAfterToken(LineState &State,
ContinuationIndenter *Indenter,
bool DryRun) {
return 0;
}


virtual void CommaFound(const FormatToken *Token) {}

virtual const FormatToken *lastComma() { return nullptr; }

protected:
const FormatStyle &Style;
};

class CommaSeparatedList : public TokenRole {
public:
CommaSeparatedList(const FormatStyle &Style)
: TokenRole(Style), HasNestedBracedList(false) {}

void precomputeFormattingInfos(const FormatToken *Token) override;

unsigned formatAfterToken(LineState &State, ContinuationIndenter *Indenter,
bool DryRun) override;

unsigned formatFromToken(LineState &State, ContinuationIndenter *Indenter,
bool DryRun) override;


void CommaFound(const FormatToken *Token) override {
Commas.push_back(Token);
}

const FormatToken *lastComma() override {
if (Commas.empty())
return nullptr;
return Commas.back();
}

private:


struct ColumnFormat {

unsigned Columns;


unsigned TotalWidth;


unsigned LineCount;


SmallVector<unsigned, 8> ColumnSizes;
};



const ColumnFormat *getColumnFormat(unsigned RemainingCharacters) const;


SmallVector<const FormatToken *, 8> Commas;



SmallVector<unsigned, 8> ItemLengths;


SmallVector<ColumnFormat, 4> Formats;

bool HasNestedBracedList;
};



struct AdditionalKeywords {
AdditionalKeywords(IdentifierTable &IdentTable) {
kw_final = &IdentTable.get("final");
kw_override = &IdentTable.get("override");
kw_in = &IdentTable.get("in");
kw_of = &IdentTable.get("of");
kw_CF_CLOSED_ENUM = &IdentTable.get("CF_CLOSED_ENUM");
kw_CF_ENUM = &IdentTable.get("CF_ENUM");
kw_CF_OPTIONS = &IdentTable.get("CF_OPTIONS");
kw_NS_CLOSED_ENUM = &IdentTable.get("NS_CLOSED_ENUM");
kw_NS_ENUM = &IdentTable.get("NS_ENUM");
kw_NS_OPTIONS = &IdentTable.get("NS_OPTIONS");

kw_as = &IdentTable.get("as");
kw_async = &IdentTable.get("async");
kw_await = &IdentTable.get("await");
kw_declare = &IdentTable.get("declare");
kw_finally = &IdentTable.get("finally");
kw_from = &IdentTable.get("from");
kw_function = &IdentTable.get("function");
kw_get = &IdentTable.get("get");
kw_import = &IdentTable.get("import");
kw_infer = &IdentTable.get("infer");
kw_is = &IdentTable.get("is");
kw_let = &IdentTable.get("let");
kw_module = &IdentTable.get("module");
kw_readonly = &IdentTable.get("readonly");
kw_set = &IdentTable.get("set");
kw_type = &IdentTable.get("type");
kw_typeof = &IdentTable.get("typeof");
kw_var = &IdentTable.get("var");
kw_yield = &IdentTable.get("yield");

kw_abstract = &IdentTable.get("abstract");
kw_assert = &IdentTable.get("assert");
kw_extends = &IdentTable.get("extends");
kw_implements = &IdentTable.get("implements");
kw_instanceof = &IdentTable.get("instanceof");
kw_interface = &IdentTable.get("interface");
kw_native = &IdentTable.get("native");
kw_package = &IdentTable.get("package");
kw_synchronized = &IdentTable.get("synchronized");
kw_throws = &IdentTable.get("throws");
kw___except = &IdentTable.get("__except");
kw___has_include = &IdentTable.get("__has_include");
kw___has_include_next = &IdentTable.get("__has_include_next");

kw_mark = &IdentTable.get("mark");

kw_extend = &IdentTable.get("extend");
kw_option = &IdentTable.get("option");
kw_optional = &IdentTable.get("optional");
kw_repeated = &IdentTable.get("repeated");
kw_required = &IdentTable.get("required");
kw_returns = &IdentTable.get("returns");

kw_signals = &IdentTable.get("signals");
kw_qsignals = &IdentTable.get("Q_SIGNALS");
kw_slots = &IdentTable.get("slots");
kw_qslots = &IdentTable.get("Q_SLOTS");


kw_dollar = &IdentTable.get("dollar");
kw_base = &IdentTable.get("base");
kw_byte = &IdentTable.get("byte");
kw_checked = &IdentTable.get("checked");
kw_decimal = &IdentTable.get("decimal");
kw_delegate = &IdentTable.get("delegate");
kw_event = &IdentTable.get("event");
kw_fixed = &IdentTable.get("fixed");
kw_foreach = &IdentTable.get("foreach");
kw_implicit = &IdentTable.get("implicit");
kw_internal = &IdentTable.get("internal");
kw_lock = &IdentTable.get("lock");
kw_null = &IdentTable.get("null");
kw_object = &IdentTable.get("object");
kw_out = &IdentTable.get("out");
kw_params = &IdentTable.get("params");
kw_ref = &IdentTable.get("ref");
kw_string = &IdentTable.get("string");
kw_stackalloc = &IdentTable.get("stackalloc");
kw_sbyte = &IdentTable.get("sbyte");
kw_sealed = &IdentTable.get("sealed");
kw_uint = &IdentTable.get("uint");
kw_ulong = &IdentTable.get("ulong");
kw_unchecked = &IdentTable.get("unchecked");
kw_unsafe = &IdentTable.get("unsafe");
kw_ushort = &IdentTable.get("ushort");
kw_when = &IdentTable.get("when");
kw_where = &IdentTable.get("where");




JsExtraKeywords = std::unordered_set<IdentifierInfo *>(
{kw_as, kw_async, kw_await, kw_declare, kw_finally, kw_from,
kw_function, kw_get, kw_import, kw_is, kw_let, kw_module, kw_readonly,
kw_set, kw_type, kw_typeof, kw_var, kw_yield,

kw_abstract, kw_extends, kw_implements, kw_instanceof, kw_interface});

CSharpExtraKeywords = std::unordered_set<IdentifierInfo *>(
{kw_base, kw_byte, kw_checked, kw_decimal, kw_delegate, kw_event,
kw_fixed, kw_foreach, kw_implicit, kw_in, kw_interface, kw_internal,
kw_is, kw_lock, kw_null, kw_object, kw_out, kw_override, kw_params,
kw_readonly, kw_ref, kw_string, kw_stackalloc, kw_sbyte, kw_sealed,
kw_uint, kw_ulong, kw_unchecked, kw_unsafe, kw_ushort, kw_when,
kw_where,

kw_as, kw_async, kw_await, kw_declare, kw_finally, kw_from,
kw_function, kw_get, kw_import, kw_is, kw_let, kw_module, kw_readonly,
kw_set, kw_type, kw_typeof, kw_var, kw_yield,

kw_abstract, kw_extends, kw_implements, kw_instanceof, kw_interface});
}


IdentifierInfo *kw_final;
IdentifierInfo *kw_override;
IdentifierInfo *kw_in;
IdentifierInfo *kw_of;
IdentifierInfo *kw_CF_CLOSED_ENUM;
IdentifierInfo *kw_CF_ENUM;
IdentifierInfo *kw_CF_OPTIONS;
IdentifierInfo *kw_NS_CLOSED_ENUM;
IdentifierInfo *kw_NS_ENUM;
IdentifierInfo *kw_NS_OPTIONS;
IdentifierInfo *kw___except;
IdentifierInfo *kw___has_include;
IdentifierInfo *kw___has_include_next;


IdentifierInfo *kw_as;
IdentifierInfo *kw_async;
IdentifierInfo *kw_await;
IdentifierInfo *kw_declare;
IdentifierInfo *kw_finally;
IdentifierInfo *kw_from;
IdentifierInfo *kw_function;
IdentifierInfo *kw_get;
IdentifierInfo *kw_import;
IdentifierInfo *kw_infer;
IdentifierInfo *kw_is;
IdentifierInfo *kw_let;
IdentifierInfo *kw_module;
IdentifierInfo *kw_readonly;
IdentifierInfo *kw_set;
IdentifierInfo *kw_type;
IdentifierInfo *kw_typeof;
IdentifierInfo *kw_var;
IdentifierInfo *kw_yield;


IdentifierInfo *kw_abstract;
IdentifierInfo *kw_assert;
IdentifierInfo *kw_extends;
IdentifierInfo *kw_implements;
IdentifierInfo *kw_instanceof;
IdentifierInfo *kw_interface;
IdentifierInfo *kw_native;
IdentifierInfo *kw_package;
IdentifierInfo *kw_synchronized;
IdentifierInfo *kw_throws;


IdentifierInfo *kw_mark;


IdentifierInfo *kw_extend;
IdentifierInfo *kw_option;
IdentifierInfo *kw_optional;
IdentifierInfo *kw_repeated;
IdentifierInfo *kw_required;
IdentifierInfo *kw_returns;


IdentifierInfo *kw_signals;
IdentifierInfo *kw_qsignals;
IdentifierInfo *kw_slots;
IdentifierInfo *kw_qslots;


IdentifierInfo *kw_dollar;
IdentifierInfo *kw_base;
IdentifierInfo *kw_byte;
IdentifierInfo *kw_checked;
IdentifierInfo *kw_decimal;
IdentifierInfo *kw_delegate;
IdentifierInfo *kw_event;
IdentifierInfo *kw_fixed;
IdentifierInfo *kw_foreach;
IdentifierInfo *kw_implicit;
IdentifierInfo *kw_internal;

IdentifierInfo *kw_lock;
IdentifierInfo *kw_null;
IdentifierInfo *kw_object;
IdentifierInfo *kw_out;

IdentifierInfo *kw_params;

IdentifierInfo *kw_ref;
IdentifierInfo *kw_string;
IdentifierInfo *kw_stackalloc;
IdentifierInfo *kw_sbyte;
IdentifierInfo *kw_sealed;
IdentifierInfo *kw_uint;
IdentifierInfo *kw_ulong;
IdentifierInfo *kw_unchecked;
IdentifierInfo *kw_unsafe;
IdentifierInfo *kw_ushort;
IdentifierInfo *kw_when;
IdentifierInfo *kw_where;






bool IsJavaScriptIdentifier(const FormatToken &Tok,
bool AcceptIdentifierName = true) const {


switch (Tok.Tok.getKind()) {
case tok::kw_break:
case tok::kw_case:
case tok::kw_catch:
case tok::kw_class:
case tok::kw_continue:
case tok::kw_const:
case tok::kw_default:
case tok::kw_delete:
case tok::kw_do:
case tok::kw_else:
case tok::kw_enum:
case tok::kw_export:
case tok::kw_false:
case tok::kw_for:
case tok::kw_if:
case tok::kw_import:
case tok::kw_module:
case tok::kw_new:
case tok::kw_private:
case tok::kw_protected:
case tok::kw_public:
case tok::kw_return:
case tok::kw_static:
case tok::kw_switch:
case tok::kw_this:
case tok::kw_throw:
case tok::kw_true:
case tok::kw_try:
case tok::kw_typeof:
case tok::kw_void:
case tok::kw_while:

return false;
case tok::identifier: {


bool IsPseudoKeyword =
JsExtraKeywords.find(Tok.Tok.getIdentifierInfo()) !=
JsExtraKeywords.end();
return AcceptIdentifierName || !IsPseudoKeyword;
}
default:


break;
}

switch (Tok.Tok.getKind()) {

#define KEYWORD(X, Y) case tok::kw_##X:
#include "clang/Basic/TokenKinds.def"


return true;
default:

return false;
}
}



bool isCSharpKeyword(const FormatToken &Tok) const {
switch (Tok.Tok.getKind()) {
case tok::kw_bool:
case tok::kw_break:
case tok::kw_case:
case tok::kw_catch:
case tok::kw_char:
case tok::kw_class:
case tok::kw_const:
case tok::kw_continue:
case tok::kw_default:
case tok::kw_do:
case tok::kw_double:
case tok::kw_else:
case tok::kw_enum:
case tok::kw_explicit:
case tok::kw_extern:
case tok::kw_false:
case tok::kw_float:
case tok::kw_for:
case tok::kw_goto:
case tok::kw_if:
case tok::kw_int:
case tok::kw_long:
case tok::kw_namespace:
case tok::kw_new:
case tok::kw_operator:
case tok::kw_private:
case tok::kw_protected:
case tok::kw_public:
case tok::kw_return:
case tok::kw_short:
case tok::kw_sizeof:
case tok::kw_static:
case tok::kw_struct:
case tok::kw_switch:
case tok::kw_this:
case tok::kw_throw:
case tok::kw_true:
case tok::kw_try:
case tok::kw_typeof:
case tok::kw_using:
case tok::kw_virtual:
case tok::kw_void:
case tok::kw_volatile:
case tok::kw_while:
return true;
default:
return Tok.is(tok::identifier) &&
CSharpExtraKeywords.find(Tok.Tok.getIdentifierInfo()) ==
CSharpExtraKeywords.end();
}
}

private:

std::unordered_set<IdentifierInfo *> JsExtraKeywords;


std::unordered_set<IdentifierInfo *> CSharpExtraKeywords;
};

}
}

#endif
