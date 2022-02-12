











#if !defined(LLVM_CLANG_SEMA_CODECOMPLETECONSUMER_H)
#define LLVM_CLANG_SEMA_CODECOMPLETECONSUMER_H

#include "clang-c/Index.h"
#include "clang/AST/Type.h"
#include "clang/Basic/LLVM.h"
#include "clang/Lex/MacroInfo.h"
#include "clang/Sema/CodeCompleteOptions.h"
#include "clang/Sema/DeclSpec.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/None.h"
#include "llvm/ADT/Optional.h"
#include "llvm/ADT/SmallPtrSet.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/Allocator.h"
#include "llvm/Support/type_traits.h"
#include <cassert>
#include <memory>
#include <string>
#include <utility>

namespace clang {

class ASTContext;
class Decl;
class DeclContext;
class FunctionDecl;
class FunctionTemplateDecl;
class IdentifierInfo;
class LangOptions;
class NamedDecl;
class NestedNameSpecifier;
class Preprocessor;
class RawComment;
class Sema;
class UsingShadowDecl;



enum {


CCP_NextInitializer = 7,



CCP_EnumInCase = 7,


CCP_SuperCompletion = 20,


CCP_LocalDeclaration = 34,



CCP_MemberDeclaration = 35,



CCP_Keyword = 40,


CCP_CodePattern = 40,


CCP_Declaration = 50,


CCP_Type = CCP_Declaration,


CCP_Constant = 65,


CCP_Macro = 70,


CCP_NestedNameSpecifier = 75,



CCP_Unlikely = 80,


CCP_ObjC_cmd = CCP_Unlikely
};



enum {

CCD_InBaseClass = 2,



CCD_ObjectQualifierMatch = -1,




CCD_SelectorMatch = -3,



CCD_bool_in_ObjC = 1,



CCD_ProbablyNotObjCCollection = 15,


CCD_MethodAsProperty = 2,



CCD_BlockPropertySetter = 3
};



enum {


CCF_ExactTypeMatch = 4,




CCF_SimilarTypeMatch = 2
};



enum SimplifiedTypeClass {
STC_Arithmetic,
STC_Array,
STC_Block,
STC_Function,
STC_ObjectiveC,
STC_Other,
STC_Pointer,
STC_Record,
STC_Void
};


SimplifiedTypeClass getSimplifiedTypeClass(CanQualType T);



QualType getDeclUsageType(ASTContext &C, const NamedDecl *ND);










unsigned getMacroUsagePriority(StringRef MacroName,
const LangOptions &LangOpts,
bool PreferredTypeIsPointer = false);



CXCursorKind getCursorKindForDecl(const Decl *D);



class CodeCompletionContext {
public:
enum Kind {

CCC_Other,



CCC_OtherWithMacros,



CCC_TopLevel,



CCC_ObjCInterface,



CCC_ObjCImplementation,



CCC_ObjCIvarList,


CCC_ClassStructUnion,



CCC_Statement,


CCC_Expression,



CCC_ObjCMessageReceiver,







CCC_DotMemberAccess,







CCC_ArrowMemberAccess,







CCC_ObjCPropertyAccess,



CCC_EnumTag,



CCC_UnionTag,



CCC_ClassOrStructTag,


CCC_ObjCProtocolName,



CCC_Namespace,


CCC_Type,


CCC_NewName,



CCC_SymbolOrNewName,



CCC_Symbol,


CCC_MacroName,



CCC_MacroNameUse,


CCC_PreprocessorExpression,



CCC_PreprocessorDirective,






CCC_NaturalLanguage,


CCC_SelectorName,


CCC_TypeQualifiers,



CCC_ParenthesizedExpression,



CCC_ObjCInstanceMessage,


CCC_ObjCClassMessage,



CCC_ObjCInterfaceName,


CCC_ObjCCategoryName,


CCC_IncludedFile,



CCC_Recovery
};

using VisitedContextSet = llvm::SmallPtrSet<DeclContext *, 8>;

private:
Kind CCKind;




bool IsUsingDeclaration;



QualType PreferredType;


QualType BaseType;


ArrayRef<IdentifierInfo *> SelIdents;



llvm::Optional<CXXScopeSpec> ScopeSpecifier;



VisitedContextSet VisitedContexts;

public:

CodeCompletionContext(Kind CCKind)
: CCKind(CCKind), IsUsingDeclaration(false), SelIdents(None) {}


CodeCompletionContext(Kind CCKind, QualType T,
ArrayRef<IdentifierInfo *> SelIdents = None)
: CCKind(CCKind), IsUsingDeclaration(false), SelIdents(SelIdents) {
if (CCKind == CCC_DotMemberAccess || CCKind == CCC_ArrowMemberAccess ||
CCKind == CCC_ObjCPropertyAccess || CCKind == CCC_ObjCClassMessage ||
CCKind == CCC_ObjCInstanceMessage)
BaseType = T;
else
PreferredType = T;
}

bool isUsingDeclaration() const { return IsUsingDeclaration; }
void setIsUsingDeclaration(bool V) { IsUsingDeclaration = V; }


Kind getKind() const { return CCKind; }




QualType getPreferredType() const { return PreferredType; }
void setPreferredType(QualType T) { PreferredType = T; }



QualType getBaseType() const { return BaseType; }


ArrayRef<IdentifierInfo *> getSelIdents() const { return SelIdents; }



bool wantConstructorResults() const;




void setCXXScopeSpecifier(CXXScopeSpec SS) {
this->ScopeSpecifier = std::move(SS);
}


void addVisitedContext(DeclContext *Ctx) {
VisitedContexts.insert(Ctx);
}


const VisitedContextSet &getVisitedContexts() const {
return VisitedContexts;
}

llvm::Optional<const CXXScopeSpec *> getCXXScopeSpecifier() {
if (ScopeSpecifier)
return ScopeSpecifier.getPointer();
return llvm::None;
}
};


llvm::StringRef getCompletionKindString(CodeCompletionContext::Kind Kind);








class CodeCompletionString {
public:


enum ChunkKind {



CK_TypedText,



CK_Text,




CK_Optional,



CK_Placeholder,



CK_Informative,


CK_ResultType,




CK_CurrentParameter,


CK_LeftParen,


CK_RightParen,


CK_LeftBracket,


CK_RightBracket,


CK_LeftBrace,


CK_RightBrace,


CK_LeftAngle,


CK_RightAngle,


CK_Comma,


CK_Colon,


CK_SemiColon,


CK_Equal,


CK_HorizontalSpace,



CK_VerticalSpace
};


struct Chunk {


ChunkKind Kind = CK_Text;

union {




const char *Text;




CodeCompletionString *Optional;
};

Chunk() : Text(nullptr) {}

explicit Chunk(ChunkKind Kind, const char *Text = "");


static Chunk CreateText(const char *Text);


static Chunk CreateOptional(CodeCompletionString *Optional);


static Chunk CreatePlaceholder(const char *Placeholder);


static Chunk CreateInformative(const char *Informative);


static Chunk CreateResultType(const char *ResultType);


static Chunk CreateCurrentParameter(const char *CurrentParameter);
};

private:
friend class CodeCompletionBuilder;
friend class CodeCompletionResult;


unsigned NumChunks : 16;


unsigned NumAnnotations : 16;


unsigned Priority : 16;


unsigned Availability : 2;


StringRef ParentName;



const char *BriefComment;

CodeCompletionString(const Chunk *Chunks, unsigned NumChunks,
unsigned Priority, CXAvailabilityKind Availability,
const char **Annotations, unsigned NumAnnotations,
StringRef ParentName,
const char *BriefComment);
~CodeCompletionString() = default;

public:
CodeCompletionString(const CodeCompletionString &) = delete;
CodeCompletionString &operator=(const CodeCompletionString &) = delete;

using iterator = const Chunk *;

iterator begin() const { return reinterpret_cast<const Chunk *>(this + 1); }
iterator end() const { return begin() + NumChunks; }
bool empty() const { return NumChunks == 0; }
unsigned size() const { return NumChunks; }

const Chunk &operator[](unsigned I) const {
assert(I < size() && "Chunk index out-of-range");
return begin()[I];
}


const char *getTypedText() const;


unsigned getPriority() const { return Priority; }


unsigned getAvailability() const { return Availability; }


unsigned getAnnotationCount() const;


const char *getAnnotation(unsigned AnnotationNr) const;


StringRef getParentContextName() const {
return ParentName;
}

const char *getBriefComment() const {
return BriefComment;
}



std::string getAsString() const;
};


class CodeCompletionAllocator : public llvm::BumpPtrAllocator {
public:

const char *CopyString(const Twine &String);
};


class GlobalCodeCompletionAllocator : public CodeCompletionAllocator {};

class CodeCompletionTUInfo {
llvm::DenseMap<const DeclContext *, StringRef> ParentNames;
std::shared_ptr<GlobalCodeCompletionAllocator> AllocatorRef;

public:
explicit CodeCompletionTUInfo(
std::shared_ptr<GlobalCodeCompletionAllocator> Allocator)
: AllocatorRef(std::move(Allocator)) {}

std::shared_ptr<GlobalCodeCompletionAllocator> getAllocatorRef() const {
return AllocatorRef;
}

CodeCompletionAllocator &getAllocator() const {
assert(AllocatorRef);
return *AllocatorRef;
}

StringRef getParentName(const DeclContext *DC);
};

}

namespace clang {


class CodeCompletionBuilder {
public:
using Chunk = CodeCompletionString::Chunk;

private:
CodeCompletionAllocator &Allocator;
CodeCompletionTUInfo &CCTUInfo;
unsigned Priority = 0;
CXAvailabilityKind Availability = CXAvailability_Available;
StringRef ParentName;
const char *BriefComment = nullptr;


SmallVector<Chunk, 4> Chunks;

SmallVector<const char *, 2> Annotations;

public:
CodeCompletionBuilder(CodeCompletionAllocator &Allocator,
CodeCompletionTUInfo &CCTUInfo)
: Allocator(Allocator), CCTUInfo(CCTUInfo) {}

CodeCompletionBuilder(CodeCompletionAllocator &Allocator,
CodeCompletionTUInfo &CCTUInfo,
unsigned Priority, CXAvailabilityKind Availability)
: Allocator(Allocator), CCTUInfo(CCTUInfo), Priority(Priority),
Availability(Availability) {}



CodeCompletionAllocator &getAllocator() const { return Allocator; }

CodeCompletionTUInfo &getCodeCompletionTUInfo() const { return CCTUInfo; }




CodeCompletionString *TakeString();


void AddTypedTextChunk(const char *Text);


void AddTextChunk(const char *Text);


void AddOptionalChunk(CodeCompletionString *Optional);


void AddPlaceholderChunk(const char *Placeholder);


void AddInformativeChunk(const char *Text);


void AddResultTypeChunk(const char *ResultType);


void AddCurrentParameterChunk(const char *CurrentParameter);


void AddChunk(CodeCompletionString::ChunkKind CK, const char *Text = "");

void AddAnnotation(const char *A) { Annotations.push_back(A); }


void addParentContext(const DeclContext *DC);

const char *getBriefComment() const { return BriefComment; }
void addBriefComment(StringRef Comment);

StringRef getParentName() const { return ParentName; }
};


class CodeCompletionResult {
public:

enum ResultKind {

RK_Declaration = 0,


RK_Keyword,


RK_Macro,


RK_Pattern
};



const NamedDecl *Declaration = nullptr;

union {


const char *Keyword;



CodeCompletionString *Pattern;


const IdentifierInfo *Macro;
};


unsigned Priority;



unsigned StartParameter = 0;


ResultKind Kind;


CXCursorKind CursorKind;


CXAvailabilityKind Availability = CXAvailability_Available;





























std::vector<FixItHint> FixIts;


bool Hidden : 1;


bool InBaseClass : 1;


bool QualifierIsInformative : 1;



bool StartsNestedNameSpecifier : 1;



bool AllParametersAreInformative : 1;



bool DeclaringEntity : 1;




NestedNameSpecifier *Qualifier = nullptr;





const UsingShadowDecl *ShadowDecl = nullptr;




const MacroInfo *MacroDefInfo = nullptr;


CodeCompletionResult(const NamedDecl *Declaration, unsigned Priority,
NestedNameSpecifier *Qualifier = nullptr,
bool QualifierIsInformative = false,
bool Accessible = true,
std::vector<FixItHint> FixIts = std::vector<FixItHint>())
: Declaration(Declaration), Priority(Priority), Kind(RK_Declaration),
FixIts(std::move(FixIts)), Hidden(false), InBaseClass(false),
QualifierIsInformative(QualifierIsInformative),
StartsNestedNameSpecifier(false), AllParametersAreInformative(false),
DeclaringEntity(false), Qualifier(Qualifier) {

computeCursorKindAndAvailability(Accessible);
}


CodeCompletionResult(const char *Keyword, unsigned Priority = CCP_Keyword)
: Keyword(Keyword), Priority(Priority), Kind(RK_Keyword),
CursorKind(CXCursor_NotImplemented), Hidden(false), InBaseClass(false),
QualifierIsInformative(false), StartsNestedNameSpecifier(false),
AllParametersAreInformative(false), DeclaringEntity(false) {}


CodeCompletionResult(const IdentifierInfo *Macro,
const MacroInfo *MI = nullptr,
unsigned Priority = CCP_Macro)
: Macro(Macro), Priority(Priority), Kind(RK_Macro),
CursorKind(CXCursor_MacroDefinition), Hidden(false), InBaseClass(false),
QualifierIsInformative(false), StartsNestedNameSpecifier(false),
AllParametersAreInformative(false), DeclaringEntity(false),
MacroDefInfo(MI) {}


CodeCompletionResult(
CodeCompletionString *Pattern, unsigned Priority = CCP_CodePattern,
CXCursorKind CursorKind = CXCursor_NotImplemented,
CXAvailabilityKind Availability = CXAvailability_Available,
const NamedDecl *D = nullptr)
: Declaration(D), Pattern(Pattern), Priority(Priority), Kind(RK_Pattern),
CursorKind(CursorKind), Availability(Availability), Hidden(false),
InBaseClass(false), QualifierIsInformative(false),
StartsNestedNameSpecifier(false), AllParametersAreInformative(false),
DeclaringEntity(false) {}



CodeCompletionResult(CodeCompletionString *Pattern, const NamedDecl *D,
unsigned Priority)
: Declaration(D), Pattern(Pattern), Priority(Priority), Kind(RK_Pattern),
Hidden(false), InBaseClass(false), QualifierIsInformative(false),
StartsNestedNameSpecifier(false), AllParametersAreInformative(false),
DeclaringEntity(false) {
computeCursorKindAndAvailability();
}



const NamedDecl *getDeclaration() const {
assert(((Kind == RK_Declaration) || (Kind == RK_Pattern)) &&
"Not a declaration or pattern result");
return Declaration;
}


const char *getKeyword() const {
assert(Kind == RK_Keyword && "Not a keyword result");
return Keyword;
}








CodeCompletionString *CreateCodeCompletionString(Sema &S,
const CodeCompletionContext &CCContext,
CodeCompletionAllocator &Allocator,
CodeCompletionTUInfo &CCTUInfo,
bool IncludeBriefComments);
CodeCompletionString *CreateCodeCompletionString(ASTContext &Ctx,
Preprocessor &PP,
const CodeCompletionContext &CCContext,
CodeCompletionAllocator &Allocator,
CodeCompletionTUInfo &CCTUInfo,
bool IncludeBriefComments);



CodeCompletionString *
CreateCodeCompletionStringForMacro(Preprocessor &PP,
CodeCompletionAllocator &Allocator,
CodeCompletionTUInfo &CCTUInfo);

CodeCompletionString *createCodeCompletionStringForDecl(
Preprocessor &PP, ASTContext &Ctx, CodeCompletionBuilder &Result,
bool IncludeBriefComments, const CodeCompletionContext &CCContext,
PrintingPolicy &Policy);

CodeCompletionString *createCodeCompletionStringForOverride(
Preprocessor &PP, ASTContext &Ctx, CodeCompletionBuilder &Result,
bool IncludeBriefComments, const CodeCompletionContext &CCContext,
PrintingPolicy &Policy);





StringRef getOrderedName(std::string &Saved) const;

private:
void computeCursorKindAndAvailability(bool Accessible = true);
};

bool operator<(const CodeCompletionResult &X, const CodeCompletionResult &Y);

inline bool operator>(const CodeCompletionResult &X,
const CodeCompletionResult &Y) {
return Y < X;
}

inline bool operator<=(const CodeCompletionResult &X,
const CodeCompletionResult &Y) {
return !(Y < X);
}

inline bool operator>=(const CodeCompletionResult &X,
const CodeCompletionResult &Y) {
return !(X < Y);
}



class CodeCompleteConsumer {
protected:
const CodeCompleteOptions CodeCompleteOpts;

public:
class OverloadCandidate {
public:

enum CandidateKind {

CK_Function,


CK_FunctionTemplate,



CK_FunctionType
};

private:

CandidateKind Kind;

union {


FunctionDecl *Function;



FunctionTemplateDecl *FunctionTemplate;



const FunctionType *Type;
};

public:
OverloadCandidate(FunctionDecl *Function)
: Kind(CK_Function), Function(Function) {}

OverloadCandidate(FunctionTemplateDecl *FunctionTemplateDecl)
: Kind(CK_FunctionTemplate), FunctionTemplate(FunctionTemplateDecl) {}

OverloadCandidate(const FunctionType *Type)
: Kind(CK_FunctionType), Type(Type) {}


CandidateKind getKind() const { return Kind; }



FunctionDecl *getFunction() const;


FunctionTemplateDecl *getFunctionTemplate() const {
assert(getKind() == CK_FunctionTemplate && "Not a function template");
return FunctionTemplate;
}



const FunctionType *getFunctionType() const;



CodeCompletionString *CreateSignatureString(unsigned CurrentArg,
Sema &S,
CodeCompletionAllocator &Allocator,
CodeCompletionTUInfo &CCTUInfo,
bool IncludeBriefComments) const;
};

CodeCompleteConsumer(const CodeCompleteOptions &CodeCompleteOpts)
: CodeCompleteOpts(CodeCompleteOpts) {}


bool includeMacros() const {
return CodeCompleteOpts.IncludeMacros;
}


bool includeCodePatterns() const {
return CodeCompleteOpts.IncludeCodePatterns;
}


bool includeGlobals() const { return CodeCompleteOpts.IncludeGlobals; }




bool includeNamespaceLevelDecls() const {
return CodeCompleteOpts.IncludeNamespaceLevelDecls;
}



bool includeBriefComments() const {
return CodeCompleteOpts.IncludeBriefComments;
}



bool includeFixIts() const { return CodeCompleteOpts.IncludeFixIts; }



bool loadExternal() const {
return CodeCompleteOpts.LoadExternal;
}


virtual ~CodeCompleteConsumer();



virtual bool isResultFilteredOut(StringRef Filter,
CodeCompletionResult Results) {
return false;
}




virtual void ProcessCodeCompleteResults(Sema &S,
CodeCompletionContext Context,
CodeCompletionResult *Results,
unsigned NumResults) {}












virtual void ProcessOverloadCandidates(Sema &S, unsigned CurrentArg,
OverloadCandidate *Candidates,
unsigned NumCandidates,
SourceLocation OpenParLoc) {}




virtual CodeCompletionAllocator &getAllocator() = 0;

virtual CodeCompletionTUInfo &getCodeCompletionTUInfo() = 0;
};



const RawComment *getCompletionComment(const ASTContext &Ctx,
const NamedDecl *Decl);



const RawComment *getPatternCompletionComment(const ASTContext &Ctx,
const NamedDecl *Decl);



const RawComment *
getParameterComment(const ASTContext &Ctx,
const CodeCompleteConsumer::OverloadCandidate &Result,
unsigned ArgIndex);



class PrintingCodeCompleteConsumer : public CodeCompleteConsumer {

raw_ostream &OS;

CodeCompletionTUInfo CCTUInfo;

public:


PrintingCodeCompleteConsumer(const CodeCompleteOptions &CodeCompleteOpts,
raw_ostream &OS)
: CodeCompleteConsumer(CodeCompleteOpts), OS(OS),
CCTUInfo(std::make_shared<GlobalCodeCompletionAllocator>()) {}


void ProcessCodeCompleteResults(Sema &S, CodeCompletionContext Context,
CodeCompletionResult *Results,
unsigned NumResults) override;

void ProcessOverloadCandidates(Sema &S, unsigned CurrentArg,
OverloadCandidate *Candidates,
unsigned NumCandidates,
SourceLocation OpenParLoc) override;

bool isResultFilteredOut(StringRef Filter, CodeCompletionResult Results) override;

CodeCompletionAllocator &getAllocator() override {
return CCTUInfo.getAllocator();
}

CodeCompletionTUInfo &getCodeCompletionTUInfo() override { return CCTUInfo; }
};

}

#endif
