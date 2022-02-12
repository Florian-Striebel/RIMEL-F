




















#if !defined(LLVM_CLANG_SEMA_DECLSPEC_H)
#define LLVM_CLANG_SEMA_DECLSPEC_H

#include "clang/AST/DeclCXX.h"
#include "clang/AST/DeclObjCCommon.h"
#include "clang/AST/NestedNameSpecifier.h"
#include "clang/Basic/ExceptionSpecificationType.h"
#include "clang/Basic/Lambda.h"
#include "clang/Basic/OperatorKinds.h"
#include "clang/Basic/Specifiers.h"
#include "clang/Lex/Token.h"
#include "clang/Sema/Ownership.h"
#include "clang/Sema/ParsedAttr.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/Support/Compiler.h"
#include "llvm/Support/ErrorHandling.h"

namespace clang {
class ASTContext;
class CXXRecordDecl;
class TypeLoc;
class LangOptions;
class IdentifierInfo;
class NamespaceAliasDecl;
class NamespaceDecl;
class ObjCDeclSpec;
class Sema;
class Declarator;
struct TemplateIdAnnotation;













class CXXScopeSpec {
SourceRange Range;
NestedNameSpecifierLocBuilder Builder;

public:
SourceRange getRange() const { return Range; }
void setRange(SourceRange R) { Range = R; }
void setBeginLoc(SourceLocation Loc) { Range.setBegin(Loc); }
void setEndLoc(SourceLocation Loc) { Range.setEnd(Loc); }
SourceLocation getBeginLoc() const { return Range.getBegin(); }
SourceLocation getEndLoc() const { return Range.getEnd(); }


NestedNameSpecifier *getScopeRep() const {
return Builder.getRepresentation();
}












void Extend(ASTContext &Context, SourceLocation TemplateKWLoc, TypeLoc TL,
SourceLocation ColonColonLoc);












void Extend(ASTContext &Context, IdentifierInfo *Identifier,
SourceLocation IdentifierLoc, SourceLocation ColonColonLoc);












void Extend(ASTContext &Context, NamespaceDecl *Namespace,
SourceLocation NamespaceLoc, SourceLocation ColonColonLoc);













void Extend(ASTContext &Context, NamespaceAliasDecl *Alias,
SourceLocation AliasLoc, SourceLocation ColonColonLoc);



void MakeGlobal(ASTContext &Context, SourceLocation ColonColonLoc);














void MakeSuper(ASTContext &Context, CXXRecordDecl *RD,
SourceLocation SuperLoc, SourceLocation ColonColonLoc);







void MakeTrivial(ASTContext &Context, NestedNameSpecifier *Qualifier,
SourceRange R);



void Adopt(NestedNameSpecifierLoc Other);






NestedNameSpecifierLoc getWithLocInContext(ASTContext &Context) const;










SourceLocation getLastQualifierNameLoc() const;


bool isEmpty() const { return Range.isInvalid() && getScopeRep() == nullptr; }

bool isNotEmpty() const { return !isEmpty(); }


bool isInvalid() const { return Range.isValid() && getScopeRep() == nullptr; }

bool isValid() const { return getScopeRep() != nullptr; }


void SetInvalid(SourceRange R) {
assert(R.isValid() && "Must have a valid source range");
if (Range.getBegin().isInvalid())
Range.setBegin(R.getBegin());
Range.setEnd(R.getEnd());
Builder.Clear();
}



bool isSet() const { return getScopeRep() != nullptr; }

void clear() {
Range = SourceRange();
Builder.Clear();
}


char *location_data() const { return Builder.getBuffer().first; }



unsigned location_size() const { return Builder.getBuffer().second; }
};





class DeclSpec {
public:


enum SCS {
SCS_unspecified = 0,
SCS_typedef,
SCS_extern,
SCS_static,
SCS_auto,
SCS_register,
SCS_private_extern,
SCS_mutable
};



typedef ThreadStorageClassSpecifier TSCS;
static const TSCS TSCS_unspecified = clang::TSCS_unspecified;
static const TSCS TSCS___thread = clang::TSCS___thread;
static const TSCS TSCS_thread_local = clang::TSCS_thread_local;
static const TSCS TSCS__Thread_local = clang::TSCS__Thread_local;

enum TSC {
TSC_unspecified,
TSC_imaginary,
TSC_complex
};


typedef TypeSpecifierType TST;
static const TST TST_unspecified = clang::TST_unspecified;
static const TST TST_void = clang::TST_void;
static const TST TST_char = clang::TST_char;
static const TST TST_wchar = clang::TST_wchar;
static const TST TST_char8 = clang::TST_char8;
static const TST TST_char16 = clang::TST_char16;
static const TST TST_char32 = clang::TST_char32;
static const TST TST_int = clang::TST_int;
static const TST TST_int128 = clang::TST_int128;
static const TST TST_extint = clang::TST_extint;
static const TST TST_half = clang::TST_half;
static const TST TST_BFloat16 = clang::TST_BFloat16;
static const TST TST_float = clang::TST_float;
static const TST TST_double = clang::TST_double;
static const TST TST_float16 = clang::TST_Float16;
static const TST TST_accum = clang::TST_Accum;
static const TST TST_fract = clang::TST_Fract;
static const TST TST_float128 = clang::TST_float128;
static const TST TST_bool = clang::TST_bool;
static const TST TST_decimal32 = clang::TST_decimal32;
static const TST TST_decimal64 = clang::TST_decimal64;
static const TST TST_decimal128 = clang::TST_decimal128;
static const TST TST_enum = clang::TST_enum;
static const TST TST_union = clang::TST_union;
static const TST TST_struct = clang::TST_struct;
static const TST TST_interface = clang::TST_interface;
static const TST TST_class = clang::TST_class;
static const TST TST_typename = clang::TST_typename;
static const TST TST_typeofType = clang::TST_typeofType;
static const TST TST_typeofExpr = clang::TST_typeofExpr;
static const TST TST_decltype = clang::TST_decltype;
static const TST TST_decltype_auto = clang::TST_decltype_auto;
static const TST TST_underlyingType = clang::TST_underlyingType;
static const TST TST_auto = clang::TST_auto;
static const TST TST_auto_type = clang::TST_auto_type;
static const TST TST_unknown_anytype = clang::TST_unknown_anytype;
static const TST TST_atomic = clang::TST_atomic;
#define GENERIC_IMAGE_TYPE(ImgType, Id) static const TST TST_##ImgType##_t = clang::TST_##ImgType##_t;

#include "clang/Basic/OpenCLImageTypes.def"
static const TST TST_error = clang::TST_error;


enum TQ {
TQ_unspecified = 0,
TQ_const = 1,
TQ_restrict = 2,
TQ_volatile = 4,
TQ_unaligned = 8,


TQ_atomic = 16
};



enum ParsedSpecifiers {
PQ_None = 0,
PQ_StorageClassSpecifier = 1,
PQ_TypeSpecifier = 2,
PQ_TypeQualifier = 4,
PQ_FunctionSpecifier = 8

};

private:

unsigned StorageClassSpec : 3;
unsigned ThreadStorageClassSpec : 2;
unsigned SCS_extern_in_linkage_spec : 1;


unsigned TypeSpecWidth : 2;
unsigned TypeSpecComplex : 2;
unsigned TypeSpecSign : 2;
unsigned TypeSpecType : 6;
unsigned TypeAltiVecVector : 1;
unsigned TypeAltiVecPixel : 1;
unsigned TypeAltiVecBool : 1;
unsigned TypeSpecOwned : 1;
unsigned TypeSpecPipe : 1;
unsigned TypeSpecSat : 1;
unsigned ConstrainedAuto : 1;


unsigned TypeQualifiers : 5;


unsigned FS_inline_specified : 1;
unsigned FS_forceinline_specified: 1;
unsigned FS_virtual_specified : 1;
unsigned FS_noreturn_specified : 1;


unsigned Friend_specified : 1;


unsigned ConstexprSpecifier : 2;

union {
UnionParsedType TypeRep;
Decl *DeclRep;
Expr *ExprRep;
TemplateIdAnnotation *TemplateIdRep;
};


ExplicitSpecifier FS_explicit_specifier;


ParsedAttributes Attrs;


CXXScopeSpec TypeScope;



SourceRange Range;

SourceLocation StorageClassSpecLoc, ThreadStorageClassSpecLoc;
SourceRange TSWRange;
SourceLocation TSCLoc, TSSLoc, TSTLoc, AltiVecLoc, TSSatLoc;




SourceLocation TSTNameLoc;
SourceRange TypeofParensRange;
SourceLocation TQ_constLoc, TQ_restrictLoc, TQ_volatileLoc, TQ_atomicLoc,
TQ_unalignedLoc;
SourceLocation FS_inlineLoc, FS_virtualLoc, FS_explicitLoc, FS_noreturnLoc;
SourceLocation FS_explicitCloseParenLoc;
SourceLocation FS_forceinlineLoc;
SourceLocation FriendLoc, ModulePrivateLoc, ConstexprLoc;
SourceLocation TQ_pipeLoc;

WrittenBuiltinSpecs writtenBS;
void SaveWrittenBuiltinSpecs();

ObjCDeclSpec *ObjCQualifiers;

static bool isTypeRep(TST T) {
return (T == TST_typename || T == TST_typeofType ||
T == TST_underlyingType || T == TST_atomic);
}
static bool isExprRep(TST T) {
return (T == TST_typeofExpr || T == TST_decltype || T == TST_extint);
}
static bool isTemplateIdRep(TST T) {
return (T == TST_auto || T == TST_decltype_auto);
}

DeclSpec(const DeclSpec &) = delete;
void operator=(const DeclSpec &) = delete;
public:
static bool isDeclRep(TST T) {
return (T == TST_enum || T == TST_struct ||
T == TST_interface || T == TST_union ||
T == TST_class);
}

DeclSpec(AttributeFactory &attrFactory)
: StorageClassSpec(SCS_unspecified),
ThreadStorageClassSpec(TSCS_unspecified),
SCS_extern_in_linkage_spec(false),
TypeSpecWidth(static_cast<unsigned>(TypeSpecifierWidth::Unspecified)),
TypeSpecComplex(TSC_unspecified),
TypeSpecSign(static_cast<unsigned>(TypeSpecifierSign::Unspecified)),
TypeSpecType(TST_unspecified), TypeAltiVecVector(false),
TypeAltiVecPixel(false), TypeAltiVecBool(false), TypeSpecOwned(false),
TypeSpecPipe(false), TypeSpecSat(false), ConstrainedAuto(false),
TypeQualifiers(TQ_unspecified), FS_inline_specified(false),
FS_forceinline_specified(false), FS_virtual_specified(false),
FS_noreturn_specified(false), Friend_specified(false),
ConstexprSpecifier(
static_cast<unsigned>(ConstexprSpecKind::Unspecified)),
FS_explicit_specifier(), Attrs(attrFactory), writtenBS(),
ObjCQualifiers(nullptr) {}


SCS getStorageClassSpec() const { return (SCS)StorageClassSpec; }
TSCS getThreadStorageClassSpec() const {
return (TSCS)ThreadStorageClassSpec;
}
bool isExternInLinkageSpec() const { return SCS_extern_in_linkage_spec; }
void setExternInLinkageSpec(bool Value) {
SCS_extern_in_linkage_spec = Value;
}

SourceLocation getStorageClassSpecLoc() const { return StorageClassSpecLoc; }
SourceLocation getThreadStorageClassSpecLoc() const {
return ThreadStorageClassSpecLoc;
}

void ClearStorageClassSpecs() {
StorageClassSpec = DeclSpec::SCS_unspecified;
ThreadStorageClassSpec = DeclSpec::TSCS_unspecified;
SCS_extern_in_linkage_spec = false;
StorageClassSpecLoc = SourceLocation();
ThreadStorageClassSpecLoc = SourceLocation();
}

void ClearTypeSpecType() {
TypeSpecType = DeclSpec::TST_unspecified;
TypeSpecOwned = false;
TSTLoc = SourceLocation();
}


TypeSpecifierWidth getTypeSpecWidth() const {
return static_cast<TypeSpecifierWidth>(TypeSpecWidth);
}
TSC getTypeSpecComplex() const { return (TSC)TypeSpecComplex; }
TypeSpecifierSign getTypeSpecSign() const {
return static_cast<TypeSpecifierSign>(TypeSpecSign);
}
TST getTypeSpecType() const { return (TST)TypeSpecType; }
bool isTypeAltiVecVector() const { return TypeAltiVecVector; }
bool isTypeAltiVecPixel() const { return TypeAltiVecPixel; }
bool isTypeAltiVecBool() const { return TypeAltiVecBool; }
bool isTypeSpecOwned() const { return TypeSpecOwned; }
bool isTypeRep() const { return isTypeRep((TST) TypeSpecType); }
bool isTypeSpecPipe() const { return TypeSpecPipe; }
bool isTypeSpecSat() const { return TypeSpecSat; }
bool isConstrainedAuto() const { return ConstrainedAuto; }

ParsedType getRepAsType() const {
assert(isTypeRep((TST) TypeSpecType) && "DeclSpec does not store a type");
return TypeRep;
}
Decl *getRepAsDecl() const {
assert(isDeclRep((TST) TypeSpecType) && "DeclSpec does not store a decl");
return DeclRep;
}
Expr *getRepAsExpr() const {
assert(isExprRep((TST) TypeSpecType) && "DeclSpec does not store an expr");
return ExprRep;
}
TemplateIdAnnotation *getRepAsTemplateId() const {
assert(isTemplateIdRep((TST) TypeSpecType) &&
"DeclSpec does not store a template id");
return TemplateIdRep;
}
CXXScopeSpec &getTypeSpecScope() { return TypeScope; }
const CXXScopeSpec &getTypeSpecScope() const { return TypeScope; }

SourceRange getSourceRange() const LLVM_READONLY { return Range; }
SourceLocation getBeginLoc() const LLVM_READONLY { return Range.getBegin(); }
SourceLocation getEndLoc() const LLVM_READONLY { return Range.getEnd(); }

SourceLocation getTypeSpecWidthLoc() const { return TSWRange.getBegin(); }
SourceRange getTypeSpecWidthRange() const { return TSWRange; }
SourceLocation getTypeSpecComplexLoc() const { return TSCLoc; }
SourceLocation getTypeSpecSignLoc() const { return TSSLoc; }
SourceLocation getTypeSpecTypeLoc() const { return TSTLoc; }
SourceLocation getAltiVecLoc() const { return AltiVecLoc; }
SourceLocation getTypeSpecSatLoc() const { return TSSatLoc; }

SourceLocation getTypeSpecTypeNameLoc() const {
assert(isDeclRep((TST) TypeSpecType) || TypeSpecType == TST_typename);
return TSTNameLoc;
}

SourceRange getTypeofParensRange() const { return TypeofParensRange; }
void setTypeofParensRange(SourceRange range) { TypeofParensRange = range; }

bool hasAutoTypeSpec() const {
return (TypeSpecType == TST_auto || TypeSpecType == TST_auto_type ||
TypeSpecType == TST_decltype_auto);
}

bool hasTagDefinition() const;


static const char *getSpecifierName(DeclSpec::TST T,
const PrintingPolicy &Policy);
static const char *getSpecifierName(DeclSpec::TQ Q);
static const char *getSpecifierName(TypeSpecifierSign S);
static const char *getSpecifierName(DeclSpec::TSC C);
static const char *getSpecifierName(TypeSpecifierWidth W);
static const char *getSpecifierName(DeclSpec::SCS S);
static const char *getSpecifierName(DeclSpec::TSCS S);
static const char *getSpecifierName(ConstexprSpecKind C);




unsigned getTypeQualifiers() const { return TypeQualifiers; }
SourceLocation getConstSpecLoc() const { return TQ_constLoc; }
SourceLocation getRestrictSpecLoc() const { return TQ_restrictLoc; }
SourceLocation getVolatileSpecLoc() const { return TQ_volatileLoc; }
SourceLocation getAtomicSpecLoc() const { return TQ_atomicLoc; }
SourceLocation getUnalignedSpecLoc() const { return TQ_unalignedLoc; }
SourceLocation getPipeLoc() const { return TQ_pipeLoc; }


void ClearTypeQualifiers() {
TypeQualifiers = 0;
TQ_constLoc = SourceLocation();
TQ_restrictLoc = SourceLocation();
TQ_volatileLoc = SourceLocation();
TQ_atomicLoc = SourceLocation();
TQ_unalignedLoc = SourceLocation();
TQ_pipeLoc = SourceLocation();
}


bool isInlineSpecified() const {
return FS_inline_specified | FS_forceinline_specified;
}
SourceLocation getInlineSpecLoc() const {
return FS_inline_specified ? FS_inlineLoc : FS_forceinlineLoc;
}

ExplicitSpecifier getExplicitSpecifier() const {
return FS_explicit_specifier;
}

bool isVirtualSpecified() const { return FS_virtual_specified; }
SourceLocation getVirtualSpecLoc() const { return FS_virtualLoc; }

bool hasExplicitSpecifier() const {
return FS_explicit_specifier.isSpecified();
}
SourceLocation getExplicitSpecLoc() const { return FS_explicitLoc; }
SourceRange getExplicitSpecRange() const {
return FS_explicit_specifier.getExpr()
? SourceRange(FS_explicitLoc, FS_explicitCloseParenLoc)
: SourceRange(FS_explicitLoc);
}

bool isNoreturnSpecified() const { return FS_noreturn_specified; }
SourceLocation getNoreturnSpecLoc() const { return FS_noreturnLoc; }

void ClearFunctionSpecs() {
FS_inline_specified = false;
FS_inlineLoc = SourceLocation();
FS_forceinline_specified = false;
FS_forceinlineLoc = SourceLocation();
FS_virtual_specified = false;
FS_virtualLoc = SourceLocation();
FS_explicit_specifier = ExplicitSpecifier();
FS_explicitLoc = SourceLocation();
FS_explicitCloseParenLoc = SourceLocation();
FS_noreturn_specified = false;
FS_noreturnLoc = SourceLocation();
}




void forEachCVRUQualifier(
llvm::function_ref<void(TQ, StringRef, SourceLocation)> Handle);




void forEachQualifier(
llvm::function_ref<void(TQ, StringRef, SourceLocation)> Handle);


bool hasTypeSpecifier() const {
return getTypeSpecType() != DeclSpec::TST_unspecified ||
getTypeSpecWidth() != TypeSpecifierWidth::Unspecified ||
getTypeSpecComplex() != DeclSpec::TSC_unspecified ||
getTypeSpecSign() != TypeSpecifierSign::Unspecified;
}



unsigned getParsedSpecifiers() const;



bool isEmpty() const {
return getParsedSpecifiers() == DeclSpec::PQ_None;
}

void SetRangeStart(SourceLocation Loc) { Range.setBegin(Loc); }
void SetRangeEnd(SourceLocation Loc) { Range.setEnd(Loc); }











bool SetStorageClassSpec(Sema &S, SCS SC, SourceLocation Loc,
const char *&PrevSpec, unsigned &DiagID,
const PrintingPolicy &Policy);
bool SetStorageClassSpecThread(TSCS TSC, SourceLocation Loc,
const char *&PrevSpec, unsigned &DiagID);
bool SetTypeSpecWidth(TypeSpecifierWidth W, SourceLocation Loc,
const char *&PrevSpec, unsigned &DiagID,
const PrintingPolicy &Policy);
bool SetTypeSpecComplex(TSC C, SourceLocation Loc, const char *&PrevSpec,
unsigned &DiagID);
bool SetTypeSpecSign(TypeSpecifierSign S, SourceLocation Loc,
const char *&PrevSpec, unsigned &DiagID);
bool SetTypeSpecType(TST T, SourceLocation Loc, const char *&PrevSpec,
unsigned &DiagID, const PrintingPolicy &Policy);
bool SetTypeSpecType(TST T, SourceLocation Loc, const char *&PrevSpec,
unsigned &DiagID, ParsedType Rep,
const PrintingPolicy &Policy);
bool SetTypeSpecType(TST T, SourceLocation Loc, const char *&PrevSpec,
unsigned &DiagID, TypeResult Rep,
const PrintingPolicy &Policy) {
if (Rep.isInvalid())
return SetTypeSpecError();
return SetTypeSpecType(T, Loc, PrevSpec, DiagID, Rep.get(), Policy);
}
bool SetTypeSpecType(TST T, SourceLocation Loc, const char *&PrevSpec,
unsigned &DiagID, Decl *Rep, bool Owned,
const PrintingPolicy &Policy);
bool SetTypeSpecType(TST T, SourceLocation TagKwLoc,
SourceLocation TagNameLoc, const char *&PrevSpec,
unsigned &DiagID, ParsedType Rep,
const PrintingPolicy &Policy);
bool SetTypeSpecType(TST T, SourceLocation TagKwLoc,
SourceLocation TagNameLoc, const char *&PrevSpec,
unsigned &DiagID, Decl *Rep, bool Owned,
const PrintingPolicy &Policy);
bool SetTypeSpecType(TST T, SourceLocation Loc, const char *&PrevSpec,
unsigned &DiagID, TemplateIdAnnotation *Rep,
const PrintingPolicy &Policy);

bool SetTypeSpecType(TST T, SourceLocation Loc, const char *&PrevSpec,
unsigned &DiagID, Expr *Rep,
const PrintingPolicy &policy);
bool SetTypeAltiVecVector(bool isAltiVecVector, SourceLocation Loc,
const char *&PrevSpec, unsigned &DiagID,
const PrintingPolicy &Policy);
bool SetTypeAltiVecPixel(bool isAltiVecPixel, SourceLocation Loc,
const char *&PrevSpec, unsigned &DiagID,
const PrintingPolicy &Policy);
bool SetTypeAltiVecBool(bool isAltiVecBool, SourceLocation Loc,
const char *&PrevSpec, unsigned &DiagID,
const PrintingPolicy &Policy);
bool SetTypePipe(bool isPipe, SourceLocation Loc,
const char *&PrevSpec, unsigned &DiagID,
const PrintingPolicy &Policy);
bool SetExtIntType(SourceLocation KWLoc, Expr *BitWidth,
const char *&PrevSpec, unsigned &DiagID,
const PrintingPolicy &Policy);
bool SetTypeSpecSat(SourceLocation Loc, const char *&PrevSpec,
unsigned &DiagID);
bool SetTypeSpecError();
void UpdateDeclRep(Decl *Rep) {
assert(isDeclRep((TST) TypeSpecType));
DeclRep = Rep;
}
void UpdateTypeRep(ParsedType Rep) {
assert(isTypeRep((TST) TypeSpecType));
TypeRep = Rep;
}
void UpdateExprRep(Expr *Rep) {
assert(isExprRep((TST) TypeSpecType));
ExprRep = Rep;
}

bool SetTypeQual(TQ T, SourceLocation Loc);

bool SetTypeQual(TQ T, SourceLocation Loc, const char *&PrevSpec,
unsigned &DiagID, const LangOptions &Lang);

bool setFunctionSpecInline(SourceLocation Loc, const char *&PrevSpec,
unsigned &DiagID);
bool setFunctionSpecForceInline(SourceLocation Loc, const char *&PrevSpec,
unsigned &DiagID);
bool setFunctionSpecVirtual(SourceLocation Loc, const char *&PrevSpec,
unsigned &DiagID);
bool setFunctionSpecExplicit(SourceLocation Loc, const char *&PrevSpec,
unsigned &DiagID, ExplicitSpecifier ExplicitSpec,
SourceLocation CloseParenLoc);
bool setFunctionSpecNoreturn(SourceLocation Loc, const char *&PrevSpec,
unsigned &DiagID);

bool SetFriendSpec(SourceLocation Loc, const char *&PrevSpec,
unsigned &DiagID);
bool setModulePrivateSpec(SourceLocation Loc, const char *&PrevSpec,
unsigned &DiagID);
bool SetConstexprSpec(ConstexprSpecKind ConstexprKind, SourceLocation Loc,
const char *&PrevSpec, unsigned &DiagID);

bool isFriendSpecified() const { return Friend_specified; }
SourceLocation getFriendSpecLoc() const { return FriendLoc; }

bool isModulePrivateSpecified() const { return ModulePrivateLoc.isValid(); }
SourceLocation getModulePrivateSpecLoc() const { return ModulePrivateLoc; }

ConstexprSpecKind getConstexprSpecifier() const {
return ConstexprSpecKind(ConstexprSpecifier);
}

SourceLocation getConstexprSpecLoc() const { return ConstexprLoc; }
bool hasConstexprSpecifier() const {
return getConstexprSpecifier() != ConstexprSpecKind::Unspecified;
}

void ClearConstexprSpec() {
ConstexprSpecifier = static_cast<unsigned>(ConstexprSpecKind::Unspecified);
ConstexprLoc = SourceLocation();
}

AttributePool &getAttributePool() const {
return Attrs.getPool();
}


















void addAttributes(ParsedAttributesView &AL) {
Attrs.addAll(AL.begin(), AL.end());
}

bool hasAttributes() const { return !Attrs.empty(); }

ParsedAttributes &getAttributes() { return Attrs; }
const ParsedAttributes &getAttributes() const { return Attrs; }

void takeAttributesFrom(ParsedAttributes &attrs) {
Attrs.takeAllFrom(attrs);
}




void Finish(Sema &S, const PrintingPolicy &Policy);

const WrittenBuiltinSpecs& getWrittenBuiltinSpecs() const {
return writtenBS;
}

ObjCDeclSpec *getObjCQualifiers() const { return ObjCQualifiers; }
void setObjCQualifiers(ObjCDeclSpec *quals) { ObjCQualifiers = quals; }




bool isMissingDeclaratorOk();
};



class ObjCDeclSpec {
public:






enum ObjCDeclQualifier {
DQ_None = 0x0,
DQ_In = 0x1,
DQ_Inout = 0x2,
DQ_Out = 0x4,
DQ_Bycopy = 0x8,
DQ_Byref = 0x10,
DQ_Oneway = 0x20,
DQ_CSNullability = 0x40
};

ObjCDeclSpec()
: objcDeclQualifier(DQ_None),
PropertyAttributes(ObjCPropertyAttribute::kind_noattr), Nullability(0),
GetterName(nullptr), SetterName(nullptr) {}

ObjCDeclQualifier getObjCDeclQualifier() const {
return (ObjCDeclQualifier)objcDeclQualifier;
}
void setObjCDeclQualifier(ObjCDeclQualifier DQVal) {
objcDeclQualifier = (ObjCDeclQualifier) (objcDeclQualifier | DQVal);
}
void clearObjCDeclQualifier(ObjCDeclQualifier DQVal) {
objcDeclQualifier = (ObjCDeclQualifier) (objcDeclQualifier & ~DQVal);
}

ObjCPropertyAttribute::Kind getPropertyAttributes() const {
return ObjCPropertyAttribute::Kind(PropertyAttributes);
}
void setPropertyAttributes(ObjCPropertyAttribute::Kind PRVal) {
PropertyAttributes =
(ObjCPropertyAttribute::Kind)(PropertyAttributes | PRVal);
}

NullabilityKind getNullability() const {
assert(
((getObjCDeclQualifier() & DQ_CSNullability) ||
(getPropertyAttributes() & ObjCPropertyAttribute::kind_nullability)) &&
"Objective-C declspec doesn't have nullability");
return static_cast<NullabilityKind>(Nullability);
}

SourceLocation getNullabilityLoc() const {
assert(
((getObjCDeclQualifier() & DQ_CSNullability) ||
(getPropertyAttributes() & ObjCPropertyAttribute::kind_nullability)) &&
"Objective-C declspec doesn't have nullability");
return NullabilityLoc;
}

void setNullability(SourceLocation loc, NullabilityKind kind) {
assert(
((getObjCDeclQualifier() & DQ_CSNullability) ||
(getPropertyAttributes() & ObjCPropertyAttribute::kind_nullability)) &&
"Set the nullability declspec or property attribute first");
Nullability = static_cast<unsigned>(kind);
NullabilityLoc = loc;
}

const IdentifierInfo *getGetterName() const { return GetterName; }
IdentifierInfo *getGetterName() { return GetterName; }
SourceLocation getGetterNameLoc() const { return GetterNameLoc; }
void setGetterName(IdentifierInfo *name, SourceLocation loc) {
GetterName = name;
GetterNameLoc = loc;
}

const IdentifierInfo *getSetterName() const { return SetterName; }
IdentifierInfo *getSetterName() { return SetterName; }
SourceLocation getSetterNameLoc() const { return SetterNameLoc; }
void setSetterName(IdentifierInfo *name, SourceLocation loc) {
SetterName = name;
SetterNameLoc = loc;
}

private:



unsigned objcDeclQualifier : 7;


unsigned PropertyAttributes : NumObjCPropertyAttrsBits;

unsigned Nullability : 2;

SourceLocation NullabilityLoc;

IdentifierInfo *GetterName;
IdentifierInfo *SetterName;
SourceLocation GetterNameLoc;
SourceLocation SetterNameLoc;

};


enum class UnqualifiedIdKind {

IK_Identifier,

IK_OperatorFunctionId,

IK_ConversionFunctionId,

IK_LiteralOperatorId,

IK_ConstructorName,

IK_ConstructorTemplateId,

IK_DestructorName,

IK_TemplateId,

IK_ImplicitSelfParam,

IK_DeductionGuideName
};


class UnqualifiedId {
private:
UnqualifiedId(const UnqualifiedId &Other) = delete;
const UnqualifiedId &operator=(const UnqualifiedId &) = delete;

public:

UnqualifiedIdKind Kind;

struct OFI {

OverloadedOperatorKind Operator;








SourceLocation SymbolLocations[3];
};



union {


IdentifierInfo *Identifier;



struct OFI OperatorFunctionId;



UnionParsedType ConversionFunctionId;



UnionParsedType ConstructorName;



UnionParsedType DestructorName;


UnionParsedTemplateTy TemplateName;




TemplateIdAnnotation *TemplateId;
};




SourceLocation StartLocation;


SourceLocation EndLocation;

UnqualifiedId()
: Kind(UnqualifiedIdKind::IK_Identifier), Identifier(nullptr) {}



void clear() {
Kind = UnqualifiedIdKind::IK_Identifier;
Identifier = nullptr;
StartLocation = SourceLocation();
EndLocation = SourceLocation();
}


bool isValid() const { return StartLocation.isValid(); }


bool isInvalid() const { return !isValid(); }


UnqualifiedIdKind getKind() const { return Kind; }





void setIdentifier(const IdentifierInfo *Id, SourceLocation IdLoc) {
Kind = UnqualifiedIdKind::IK_Identifier;
Identifier = const_cast<IdentifierInfo *>(Id);
StartLocation = EndLocation = IdLoc;
}










void setOperatorFunctionId(SourceLocation OperatorLoc,
OverloadedOperatorKind Op,
SourceLocation SymbolLocations[3]);









void setConversionFunctionId(SourceLocation OperatorLoc,
ParsedType Ty,
SourceLocation EndLoc) {
Kind = UnqualifiedIdKind::IK_ConversionFunctionId;
StartLocation = OperatorLoc;
EndLocation = EndLoc;
ConversionFunctionId = Ty;
}









void setLiteralOperatorId(const IdentifierInfo *Id, SourceLocation OpLoc,
SourceLocation IdLoc) {
Kind = UnqualifiedIdKind::IK_LiteralOperatorId;
Identifier = const_cast<IdentifierInfo *>(Id);
StartLocation = OpLoc;
EndLocation = IdLoc;
}








void setConstructorName(ParsedType ClassType,
SourceLocation ClassNameLoc,
SourceLocation EndLoc) {
Kind = UnqualifiedIdKind::IK_ConstructorName;
StartLocation = ClassNameLoc;
EndLocation = EndLoc;
ConstructorName = ClassType;
}







void setConstructorTemplateId(TemplateIdAnnotation *TemplateId);







void setDestructorName(SourceLocation TildeLoc,
ParsedType ClassType,
SourceLocation EndLoc) {
Kind = UnqualifiedIdKind::IK_DestructorName;
StartLocation = TildeLoc;
EndLocation = EndLoc;
DestructorName = ClassType;
}






void setTemplateId(TemplateIdAnnotation *TemplateId);






void setDeductionGuideName(ParsedTemplateTy Template,
SourceLocation TemplateLoc) {
Kind = UnqualifiedIdKind::IK_DeductionGuideName;
TemplateName = Template;
StartLocation = EndLocation = TemplateLoc;
}





void setImplicitSelfParam(const IdentifierInfo *Id) {
Kind = UnqualifiedIdKind::IK_ImplicitSelfParam;
Identifier = const_cast<IdentifierInfo *>(Id);
StartLocation = EndLocation = SourceLocation();
}


SourceRange getSourceRange() const LLVM_READONLY {
return SourceRange(StartLocation, EndLocation);
}
SourceLocation getBeginLoc() const LLVM_READONLY { return StartLocation; }
SourceLocation getEndLoc() const LLVM_READONLY { return EndLocation; }
};


typedef SmallVector<Token, 4> CachedTokens;





struct DeclaratorChunk {
DeclaratorChunk() {};

enum {
Pointer, Reference, Array, Function, BlockPointer, MemberPointer, Paren, Pipe
} Kind;


SourceLocation Loc;

SourceLocation EndLoc;

SourceRange getSourceRange() const {
if (EndLoc.isInvalid())
return SourceRange(Loc, Loc);
return SourceRange(Loc, EndLoc);
}

ParsedAttributesView AttrList;

struct PointerTypeInfo {

unsigned TypeQuals : 5;


SourceLocation ConstQualLoc;


SourceLocation VolatileQualLoc;


SourceLocation RestrictQualLoc;


SourceLocation AtomicQualLoc;


SourceLocation UnalignedQualLoc;

void destroy() {
}
};

struct ReferenceTypeInfo {

bool HasRestrict : 1;

bool LValueRef : 1;
void destroy() {
}
};

struct ArrayTypeInfo {


unsigned TypeQuals : 5;


unsigned hasStatic : 1;


unsigned isStar : 1;




Expr *NumElts;

void destroy() {}
};








struct ParamInfo {
IdentifierInfo *Ident;
SourceLocation IdentLoc;
Decl *Param;






std::unique_ptr<CachedTokens> DefaultArgTokens;

ParamInfo() = default;
ParamInfo(IdentifierInfo *ident, SourceLocation iloc,
Decl *param,
std::unique_ptr<CachedTokens> DefArgTokens = nullptr)
: Ident(ident), IdentLoc(iloc), Param(param),
DefaultArgTokens(std::move(DefArgTokens)) {}
};

struct TypeAndRange {
ParsedType Ty;
SourceRange Range;
};

struct FunctionTypeInfo {



unsigned hasPrototype : 1;




unsigned isVariadic : 1;


unsigned isAmbiguous : 1;



unsigned RefQualifierIsLValueRef : 1;


unsigned ExceptionSpecType : 4;


unsigned DeleteParams : 1;



unsigned HasTrailingReturnType : 1;


SourceLocation LParenLoc;


SourceLocation EllipsisLoc;


SourceLocation RParenLoc;



unsigned NumParams;




unsigned NumExceptionsOrDecls;




SourceLocation RefQualifierLoc;



SourceLocation MutableLoc;


SourceLocation ExceptionSpecLocBeg;


SourceLocation ExceptionSpecLocEnd;




ParamInfo *Params;


DeclSpec *MethodQualifiers;


AttributeFactory *QualAttrFactory;

union {



TypeAndRange *Exceptions;



Expr *NoexceptExpr;



CachedTokens *ExceptionSpecTokens;




NamedDecl **DeclsInPrototype;
};



UnionParsedType TrailingReturnType;



SourceLocation TrailingReturnTypeLoc;




void freeParams() {
for (unsigned I = 0; I < NumParams; ++I)
Params[I].DefaultArgTokens.reset();
if (DeleteParams) {
delete[] Params;
DeleteParams = false;
}
NumParams = 0;
}

void destroy() {
freeParams();
delete QualAttrFactory;
delete MethodQualifiers;
switch (getExceptionSpecType()) {
default:
break;
case EST_Dynamic:
delete[] Exceptions;
break;
case EST_Unparsed:
delete ExceptionSpecTokens;
break;
case EST_None:
if (NumExceptionsOrDecls != 0)
delete[] DeclsInPrototype;
break;
}
}

DeclSpec &getOrCreateMethodQualifiers() {
if (!MethodQualifiers) {
QualAttrFactory = new AttributeFactory();
MethodQualifiers = new DeclSpec(*QualAttrFactory);
}
return *MethodQualifiers;
}




bool isKNRPrototype() const { return !hasPrototype && NumParams != 0; }

SourceLocation getLParenLoc() const { return LParenLoc; }

SourceLocation getEllipsisLoc() const { return EllipsisLoc; }

SourceLocation getRParenLoc() const { return RParenLoc; }

SourceLocation getExceptionSpecLocBeg() const {
return ExceptionSpecLocBeg;
}

SourceLocation getExceptionSpecLocEnd() const {
return ExceptionSpecLocEnd;
}

SourceRange getExceptionSpecRange() const {
return SourceRange(getExceptionSpecLocBeg(), getExceptionSpecLocEnd());
}


SourceLocation getRefQualifierLoc() const { return RefQualifierLoc; }


SourceLocation getConstQualifierLoc() const {
assert(MethodQualifiers);
return MethodQualifiers->getConstSpecLoc();
}


SourceLocation getVolatileQualifierLoc() const {
assert(MethodQualifiers);
return MethodQualifiers->getVolatileSpecLoc();
}


SourceLocation getRestrictQualifierLoc() const {
assert(MethodQualifiers);
return MethodQualifiers->getRestrictSpecLoc();
}


SourceLocation getMutableLoc() const { return MutableLoc; }



bool hasRefQualifier() const { return getRefQualifierLoc().isValid(); }



bool hasMutableQualifier() const { return getMutableLoc().isValid(); }


bool hasMethodTypeQualifiers() const {
return MethodQualifiers && (MethodQualifiers->getTypeQualifiers() ||
MethodQualifiers->getAttributes().size());
}


ExceptionSpecificationType getExceptionSpecType() const {
return static_cast<ExceptionSpecificationType>(ExceptionSpecType);
}


unsigned getNumExceptions() const {
assert(ExceptionSpecType != EST_None);
return NumExceptionsOrDecls;
}



ArrayRef<NamedDecl *> getDeclsInPrototype() const {
assert(ExceptionSpecType == EST_None);
return llvm::makeArrayRef(DeclsInPrototype, NumExceptionsOrDecls);
}



bool hasTrailingReturnType() const { return HasTrailingReturnType; }


ParsedType getTrailingReturnType() const {
assert(HasTrailingReturnType);
return TrailingReturnType;
}


SourceLocation getTrailingReturnTypeLoc() const {
assert(HasTrailingReturnType);
return TrailingReturnTypeLoc;
}
};

struct BlockPointerTypeInfo {


unsigned TypeQuals : 5;

void destroy() {
}
};

struct MemberPointerTypeInfo {

unsigned TypeQuals : 5;

SourceLocation StarLoc;


alignas(CXXScopeSpec) char ScopeMem[sizeof(CXXScopeSpec)];
CXXScopeSpec &Scope() {
return *reinterpret_cast<CXXScopeSpec *>(ScopeMem);
}
const CXXScopeSpec &Scope() const {
return *reinterpret_cast<const CXXScopeSpec *>(ScopeMem);
}
void destroy() {
Scope().~CXXScopeSpec();
}
};

struct PipeTypeInfo {

unsigned AccessWrites : 3;

void destroy() {}
};

union {
PointerTypeInfo Ptr;
ReferenceTypeInfo Ref;
ArrayTypeInfo Arr;
FunctionTypeInfo Fun;
BlockPointerTypeInfo Cls;
MemberPointerTypeInfo Mem;
PipeTypeInfo PipeInfo;
};

void destroy() {
switch (Kind) {
case DeclaratorChunk::Function: return Fun.destroy();
case DeclaratorChunk::Pointer: return Ptr.destroy();
case DeclaratorChunk::BlockPointer: return Cls.destroy();
case DeclaratorChunk::Reference: return Ref.destroy();
case DeclaratorChunk::Array: return Arr.destroy();
case DeclaratorChunk::MemberPointer: return Mem.destroy();
case DeclaratorChunk::Paren: return;
case DeclaratorChunk::Pipe: return PipeInfo.destroy();
}
}



const ParsedAttributesView &getAttrs() const { return AttrList; }
ParsedAttributesView &getAttrs() { return AttrList; }


static DeclaratorChunk getPointer(unsigned TypeQuals, SourceLocation Loc,
SourceLocation ConstQualLoc,
SourceLocation VolatileQualLoc,
SourceLocation RestrictQualLoc,
SourceLocation AtomicQualLoc,
SourceLocation UnalignedQualLoc) {
DeclaratorChunk I;
I.Kind = Pointer;
I.Loc = Loc;
new (&I.Ptr) PointerTypeInfo;
I.Ptr.TypeQuals = TypeQuals;
I.Ptr.ConstQualLoc = ConstQualLoc;
I.Ptr.VolatileQualLoc = VolatileQualLoc;
I.Ptr.RestrictQualLoc = RestrictQualLoc;
I.Ptr.AtomicQualLoc = AtomicQualLoc;
I.Ptr.UnalignedQualLoc = UnalignedQualLoc;
return I;
}


static DeclaratorChunk getReference(unsigned TypeQuals, SourceLocation Loc,
bool lvalue) {
DeclaratorChunk I;
I.Kind = Reference;
I.Loc = Loc;
I.Ref.HasRestrict = (TypeQuals & DeclSpec::TQ_restrict) != 0;
I.Ref.LValueRef = lvalue;
return I;
}


static DeclaratorChunk getArray(unsigned TypeQuals,
bool isStatic, bool isStar, Expr *NumElts,
SourceLocation LBLoc, SourceLocation RBLoc) {
DeclaratorChunk I;
I.Kind = Array;
I.Loc = LBLoc;
I.EndLoc = RBLoc;
I.Arr.TypeQuals = TypeQuals;
I.Arr.hasStatic = isStatic;
I.Arr.isStar = isStar;
I.Arr.NumElts = NumElts;
return I;
}



static DeclaratorChunk getFunction(bool HasProto,
bool IsAmbiguous,
SourceLocation LParenLoc,
ParamInfo *Params, unsigned NumParams,
SourceLocation EllipsisLoc,
SourceLocation RParenLoc,
bool RefQualifierIsLvalueRef,
SourceLocation RefQualifierLoc,
SourceLocation MutableLoc,
ExceptionSpecificationType ESpecType,
SourceRange ESpecRange,
ParsedType *Exceptions,
SourceRange *ExceptionRanges,
unsigned NumExceptions,
Expr *NoexceptExpr,
CachedTokens *ExceptionSpecTokens,
ArrayRef<NamedDecl *> DeclsInPrototype,
SourceLocation LocalRangeBegin,
SourceLocation LocalRangeEnd,
Declarator &TheDeclarator,
TypeResult TrailingReturnType =
TypeResult(),
SourceLocation TrailingReturnTypeLoc =
SourceLocation(),
DeclSpec *MethodQualifiers = nullptr);


static DeclaratorChunk getBlockPointer(unsigned TypeQuals,
SourceLocation Loc) {
DeclaratorChunk I;
I.Kind = BlockPointer;
I.Loc = Loc;
I.Cls.TypeQuals = TypeQuals;
return I;
}


static DeclaratorChunk getPipe(unsigned TypeQuals,
SourceLocation Loc) {
DeclaratorChunk I;
I.Kind = Pipe;
I.Loc = Loc;
I.Cls.TypeQuals = TypeQuals;
return I;
}

static DeclaratorChunk getMemberPointer(const CXXScopeSpec &SS,
unsigned TypeQuals,
SourceLocation StarLoc,
SourceLocation EndLoc) {
DeclaratorChunk I;
I.Kind = MemberPointer;
I.Loc = SS.getBeginLoc();
I.EndLoc = EndLoc;
new (&I.Mem) MemberPointerTypeInfo;
I.Mem.StarLoc = StarLoc;
I.Mem.TypeQuals = TypeQuals;
new (I.Mem.ScopeMem) CXXScopeSpec(SS);
return I;
}


static DeclaratorChunk getParen(SourceLocation LParenLoc,
SourceLocation RParenLoc) {
DeclaratorChunk I;
I.Kind = Paren;
I.Loc = LParenLoc;
I.EndLoc = RParenLoc;
return I;
}

bool isParen() const {
return Kind == Paren;
}
};



class DecompositionDeclarator {
public:
struct Binding {
IdentifierInfo *Name;
SourceLocation NameLoc;
};

private:

SourceLocation LSquareLoc, RSquareLoc;


Binding *Bindings;
unsigned NumBindings : 31;
unsigned DeleteBindings : 1;

friend class Declarator;

public:
DecompositionDeclarator()
: Bindings(nullptr), NumBindings(0), DeleteBindings(false) {}
DecompositionDeclarator(const DecompositionDeclarator &G) = delete;
DecompositionDeclarator &operator=(const DecompositionDeclarator &G) = delete;
~DecompositionDeclarator() {
if (DeleteBindings)
delete[] Bindings;
}

void clear() {
LSquareLoc = RSquareLoc = SourceLocation();
if (DeleteBindings)
delete[] Bindings;
Bindings = nullptr;
NumBindings = 0;
DeleteBindings = false;
}

ArrayRef<Binding> bindings() const {
return llvm::makeArrayRef(Bindings, NumBindings);
}

bool isSet() const { return LSquareLoc.isValid(); }

SourceLocation getLSquareLoc() const { return LSquareLoc; }
SourceLocation getRSquareLoc() const { return RSquareLoc; }
SourceRange getSourceRange() const {
return SourceRange(LSquareLoc, RSquareLoc);
}
};



enum class FunctionDefinitionKind {
Declaration,
Definition,
Defaulted,
Deleted
};

enum class DeclaratorContext {
File,
Prototype,
ObjCResult,
ObjCParameter,
KNRTypeList,
TypeName,
FunctionalCast,
Member,
Block,
ForInit,
SelectionInit,
Condition,
TemplateParam,
CXXNew,
CXXCatch,
ObjCCatch,
BlockLiteral,
LambdaExpr,
LambdaExprParameter,
ConversionId,
TrailingReturn,
TrailingReturnVar,
TemplateArg,
TemplateTypeArg,
AliasDecl,
AliasTemplate,
RequiresExpr
};













class Declarator {

private:
const DeclSpec &DS;
CXXScopeSpec SS;
UnqualifiedId Name;
SourceRange Range;


DeclaratorContext Context;


DecompositionDeclarator BindingGroup;





SmallVector<DeclaratorChunk, 8> DeclTypeInfo;


unsigned InvalidType : 1;


unsigned GroupingParens : 1;





unsigned FunctionDefinition : 2;


unsigned Redeclaration : 1;


unsigned Extension : 1;


unsigned ObjCIvar : 1;


unsigned ObjCWeakProperty : 1;


unsigned InlineStorageUsed : 1;


unsigned HasInitializer : 1;


ParsedAttributes Attrs;


Expr *AsmLabel;



Expr *TrailingRequiresClause;


ArrayRef<TemplateParameterList *> TemplateParameterLists;




TemplateParameterList *InventedTemplateParameterList;

#if !defined(_MSC_VER)
union {
#endif



DeclaratorChunk::ParamInfo InlineParams[16];
DecompositionDeclarator::Binding InlineBindings[16];
#if !defined(_MSC_VER)
};
#endif



SourceLocation CommaLoc;



SourceLocation EllipsisLoc;

friend struct DeclaratorChunk;

public:
Declarator(const DeclSpec &ds, DeclaratorContext C)
: DS(ds), Range(ds.getSourceRange()), Context(C),
InvalidType(DS.getTypeSpecType() == DeclSpec::TST_error),
GroupingParens(false), FunctionDefinition(static_cast<unsigned>(
FunctionDefinitionKind::Declaration)),
Redeclaration(false), Extension(false), ObjCIvar(false),
ObjCWeakProperty(false), InlineStorageUsed(false),
HasInitializer(false), Attrs(ds.getAttributePool().getFactory()),
AsmLabel(nullptr), TrailingRequiresClause(nullptr),
InventedTemplateParameterList(nullptr) {}

~Declarator() {
clear();
}


const DeclSpec &getDeclSpec() const { return DS; }






DeclSpec &getMutableDeclSpec() { return const_cast<DeclSpec &>(DS); }

AttributePool &getAttributePool() const {
return Attrs.getPool();
}



const CXXScopeSpec &getCXXScopeSpec() const { return SS; }
CXXScopeSpec &getCXXScopeSpec() { return SS; }


UnqualifiedId &getName() { return Name; }

const DecompositionDeclarator &getDecompositionDeclarator() const {
return BindingGroup;
}

DeclaratorContext getContext() const { return Context; }

bool isPrototypeContext() const {
return (Context == DeclaratorContext::Prototype ||
Context == DeclaratorContext::ObjCParameter ||
Context == DeclaratorContext::ObjCResult ||
Context == DeclaratorContext::LambdaExprParameter);
}


SourceRange getSourceRange() const LLVM_READONLY { return Range; }
SourceLocation getBeginLoc() const LLVM_READONLY { return Range.getBegin(); }
SourceLocation getEndLoc() const LLVM_READONLY { return Range.getEnd(); }

void SetSourceRange(SourceRange R) { Range = R; }


void SetRangeBegin(SourceLocation Loc) {
if (!Loc.isInvalid())
Range.setBegin(Loc);
}

void SetRangeEnd(SourceLocation Loc) {
if (!Loc.isInvalid())
Range.setEnd(Loc);
}



void ExtendWithDeclSpec(const DeclSpec &DS) {
SourceRange SR = DS.getSourceRange();
if (Range.getBegin().isInvalid())
Range.setBegin(SR.getBegin());
if (!SR.getEnd().isInvalid())
Range.setEnd(SR.getEnd());
}


void clear() {
SS.clear();
Name.clear();
Range = DS.getSourceRange();
BindingGroup.clear();

for (unsigned i = 0, e = DeclTypeInfo.size(); i != e; ++i)
DeclTypeInfo[i].destroy();
DeclTypeInfo.clear();
Attrs.clear();
AsmLabel = nullptr;
InlineStorageUsed = false;
HasInitializer = false;
ObjCIvar = false;
ObjCWeakProperty = false;
CommaLoc = SourceLocation();
EllipsisLoc = SourceLocation();
}




bool mayOmitIdentifier() const {
switch (Context) {
case DeclaratorContext::File:
case DeclaratorContext::KNRTypeList:
case DeclaratorContext::Member:
case DeclaratorContext::Block:
case DeclaratorContext::ForInit:
case DeclaratorContext::SelectionInit:
case DeclaratorContext::Condition:
return false;

case DeclaratorContext::TypeName:
case DeclaratorContext::FunctionalCast:
case DeclaratorContext::AliasDecl:
case DeclaratorContext::AliasTemplate:
case DeclaratorContext::Prototype:
case DeclaratorContext::LambdaExprParameter:
case DeclaratorContext::ObjCParameter:
case DeclaratorContext::ObjCResult:
case DeclaratorContext::TemplateParam:
case DeclaratorContext::CXXNew:
case DeclaratorContext::CXXCatch:
case DeclaratorContext::ObjCCatch:
case DeclaratorContext::BlockLiteral:
case DeclaratorContext::LambdaExpr:
case DeclaratorContext::ConversionId:
case DeclaratorContext::TemplateArg:
case DeclaratorContext::TemplateTypeArg:
case DeclaratorContext::TrailingReturn:
case DeclaratorContext::TrailingReturnVar:
case DeclaratorContext::RequiresExpr:
return true;
}
llvm_unreachable("unknown context kind!");
}




bool mayHaveIdentifier() const {
switch (Context) {
case DeclaratorContext::File:
case DeclaratorContext::KNRTypeList:
case DeclaratorContext::Member:
case DeclaratorContext::Block:
case DeclaratorContext::ForInit:
case DeclaratorContext::SelectionInit:
case DeclaratorContext::Condition:
case DeclaratorContext::Prototype:
case DeclaratorContext::LambdaExprParameter:
case DeclaratorContext::TemplateParam:
case DeclaratorContext::CXXCatch:
case DeclaratorContext::ObjCCatch:
case DeclaratorContext::RequiresExpr:
return true;

case DeclaratorContext::TypeName:
case DeclaratorContext::FunctionalCast:
case DeclaratorContext::CXXNew:
case DeclaratorContext::AliasDecl:
case DeclaratorContext::AliasTemplate:
case DeclaratorContext::ObjCParameter:
case DeclaratorContext::ObjCResult:
case DeclaratorContext::BlockLiteral:
case DeclaratorContext::LambdaExpr:
case DeclaratorContext::ConversionId:
case DeclaratorContext::TemplateArg:
case DeclaratorContext::TemplateTypeArg:
case DeclaratorContext::TrailingReturn:
case DeclaratorContext::TrailingReturnVar:
return false;
}
llvm_unreachable("unknown context kind!");
}


bool mayHaveDecompositionDeclarator() const {
switch (Context) {
case DeclaratorContext::File:


case DeclaratorContext::Block:
case DeclaratorContext::ForInit:
case DeclaratorContext::SelectionInit:
case DeclaratorContext::Condition:
return true;

case DeclaratorContext::Member:
case DeclaratorContext::Prototype:
case DeclaratorContext::TemplateParam:
case DeclaratorContext::RequiresExpr:

return false;


case DeclaratorContext::KNRTypeList:
case DeclaratorContext::TypeName:
case DeclaratorContext::FunctionalCast:
case DeclaratorContext::AliasDecl:
case DeclaratorContext::AliasTemplate:
case DeclaratorContext::LambdaExprParameter:
case DeclaratorContext::ObjCParameter:
case DeclaratorContext::ObjCResult:
case DeclaratorContext::CXXNew:
case DeclaratorContext::CXXCatch:
case DeclaratorContext::ObjCCatch:
case DeclaratorContext::BlockLiteral:
case DeclaratorContext::LambdaExpr:
case DeclaratorContext::ConversionId:
case DeclaratorContext::TemplateArg:
case DeclaratorContext::TemplateTypeArg:
case DeclaratorContext::TrailingReturn:
case DeclaratorContext::TrailingReturnVar:
return false;
}
llvm_unreachable("unknown context kind!");
}



bool mayBeFollowedByCXXDirectInit() const {
if (hasGroupingParens()) return false;

if (getDeclSpec().getStorageClassSpec() == DeclSpec::SCS_typedef)
return false;

if (getDeclSpec().getStorageClassSpec() == DeclSpec::SCS_extern &&
Context != DeclaratorContext::File)
return false;


if (Name.getKind() != UnqualifiedIdKind::IK_Identifier)
return false;

switch (Context) {
case DeclaratorContext::File:
case DeclaratorContext::Block:
case DeclaratorContext::ForInit:
case DeclaratorContext::SelectionInit:
case DeclaratorContext::TrailingReturnVar:
return true;

case DeclaratorContext::Condition:



return true;

case DeclaratorContext::KNRTypeList:
case DeclaratorContext::Member:
case DeclaratorContext::Prototype:
case DeclaratorContext::LambdaExprParameter:
case DeclaratorContext::ObjCParameter:
case DeclaratorContext::ObjCResult:
case DeclaratorContext::TemplateParam:
case DeclaratorContext::CXXCatch:
case DeclaratorContext::ObjCCatch:
case DeclaratorContext::TypeName:
case DeclaratorContext::FunctionalCast:
case DeclaratorContext::CXXNew:
case DeclaratorContext::AliasDecl:
case DeclaratorContext::AliasTemplate:
case DeclaratorContext::BlockLiteral:
case DeclaratorContext::LambdaExpr:
case DeclaratorContext::ConversionId:
case DeclaratorContext::TemplateArg:
case DeclaratorContext::TemplateTypeArg:
case DeclaratorContext::TrailingReturn:
case DeclaratorContext::RequiresExpr:
return false;
}
llvm_unreachable("unknown context kind!");
}




bool isPastIdentifier() const { return Name.isValid(); }





bool hasName() const {
return Name.getKind() != UnqualifiedIdKind::IK_Identifier ||
Name.Identifier || isDecompositionDeclarator();
}


bool isDecompositionDeclarator() const {
return BindingGroup.isSet();
}

IdentifierInfo *getIdentifier() const {
if (Name.getKind() == UnqualifiedIdKind::IK_Identifier)
return Name.Identifier;

return nullptr;
}
SourceLocation getIdentifierLoc() const { return Name.StartLocation; }


void SetIdentifier(IdentifierInfo *Id, SourceLocation IdLoc) {
Name.setIdentifier(Id, IdLoc);
}


void
setDecompositionBindings(SourceLocation LSquareLoc,
ArrayRef<DecompositionDeclarator::Binding> Bindings,
SourceLocation RSquareLoc);





void AddTypeInfo(const DeclaratorChunk &TI, ParsedAttributes &&attrs,
SourceLocation EndLoc) {
DeclTypeInfo.push_back(TI);
DeclTypeInfo.back().getAttrs().addAll(attrs.begin(), attrs.end());
getAttributePool().takeAllFrom(attrs.getPool());

if (!EndLoc.isInvalid())
SetRangeEnd(EndLoc);
}



void AddTypeInfo(const DeclaratorChunk &TI, SourceLocation EndLoc) {
DeclTypeInfo.push_back(TI);

if (!EndLoc.isInvalid())
SetRangeEnd(EndLoc);
}


void AddInnermostTypeInfo(const DeclaratorChunk &TI) {
DeclTypeInfo.insert(DeclTypeInfo.begin(), TI);
}


unsigned getNumTypeObjects() const { return DeclTypeInfo.size(); }



const DeclaratorChunk &getTypeObject(unsigned i) const {
assert(i < DeclTypeInfo.size() && "Invalid type chunk");
return DeclTypeInfo[i];
}
DeclaratorChunk &getTypeObject(unsigned i) {
assert(i < DeclTypeInfo.size() && "Invalid type chunk");
return DeclTypeInfo[i];
}

typedef SmallVectorImpl<DeclaratorChunk>::const_iterator type_object_iterator;
typedef llvm::iterator_range<type_object_iterator> type_object_range;


type_object_range type_objects() const {
return type_object_range(DeclTypeInfo.begin(), DeclTypeInfo.end());
}

void DropFirstTypeObject() {
assert(!DeclTypeInfo.empty() && "No type chunks to drop.");
DeclTypeInfo.front().destroy();
DeclTypeInfo.erase(DeclTypeInfo.begin());
}




const DeclaratorChunk *getInnermostNonParenChunk() const {
for (unsigned i = 0, i_end = DeclTypeInfo.size(); i < i_end; ++i) {
if (!DeclTypeInfo[i].isParen())
return &DeclTypeInfo[i];
}
return nullptr;
}




const DeclaratorChunk *getOutermostNonParenChunk() const {
for (unsigned i = DeclTypeInfo.size(), i_end = 0; i != i_end; --i) {
if (!DeclTypeInfo[i-1].isParen())
return &DeclTypeInfo[i-1];
}
return nullptr;
}




bool isArrayOfUnknownBound() const {
const DeclaratorChunk *chunk = getInnermostNonParenChunk();
return (chunk && chunk->Kind == DeclaratorChunk::Array &&
!chunk->Arr.NumElts);
}





bool isFunctionDeclarator(unsigned& idx) const {
for (unsigned i = 0, i_end = DeclTypeInfo.size(); i < i_end; ++i) {
switch (DeclTypeInfo[i].Kind) {
case DeclaratorChunk::Function:
idx = i;
return true;
case DeclaratorChunk::Paren:
continue;
case DeclaratorChunk::Pointer:
case DeclaratorChunk::Reference:
case DeclaratorChunk::Array:
case DeclaratorChunk::BlockPointer:
case DeclaratorChunk::MemberPointer:
case DeclaratorChunk::Pipe:
return false;
}
llvm_unreachable("Invalid type chunk");
}
return false;
}




bool isFunctionDeclarator() const {
unsigned index;
return isFunctionDeclarator(index);
}



DeclaratorChunk::FunctionTypeInfo &getFunctionTypeInfo() {
assert(isFunctionDeclarator() && "Not a function declarator!");
unsigned index = 0;
isFunctionDeclarator(index);
return DeclTypeInfo[index].Fun;
}



const DeclaratorChunk::FunctionTypeInfo &getFunctionTypeInfo() const {
return const_cast<Declarator*>(this)->getFunctionTypeInfo();
}







bool isDeclarationOfFunction() const;



bool isFunctionDeclarationContext() const {
if (getDeclSpec().getStorageClassSpec() == DeclSpec::SCS_typedef)
return false;

switch (Context) {
case DeclaratorContext::File:
case DeclaratorContext::Member:
case DeclaratorContext::Block:
case DeclaratorContext::ForInit:
case DeclaratorContext::SelectionInit:
return true;

case DeclaratorContext::Condition:
case DeclaratorContext::KNRTypeList:
case DeclaratorContext::TypeName:
case DeclaratorContext::FunctionalCast:
case DeclaratorContext::AliasDecl:
case DeclaratorContext::AliasTemplate:
case DeclaratorContext::Prototype:
case DeclaratorContext::LambdaExprParameter:
case DeclaratorContext::ObjCParameter:
case DeclaratorContext::ObjCResult:
case DeclaratorContext::TemplateParam:
case DeclaratorContext::CXXNew:
case DeclaratorContext::CXXCatch:
case DeclaratorContext::ObjCCatch:
case DeclaratorContext::BlockLiteral:
case DeclaratorContext::LambdaExpr:
case DeclaratorContext::ConversionId:
case DeclaratorContext::TemplateArg:
case DeclaratorContext::TemplateTypeArg:
case DeclaratorContext::TrailingReturn:
case DeclaratorContext::TrailingReturnVar:
case DeclaratorContext::RequiresExpr:
return false;
}
llvm_unreachable("unknown context kind!");
}



bool isExpressionContext() const {
switch (Context) {
case DeclaratorContext::File:
case DeclaratorContext::KNRTypeList:
case DeclaratorContext::Member:


case DeclaratorContext::TypeName:

case DeclaratorContext::FunctionalCast:
case DeclaratorContext::AliasDecl:
case DeclaratorContext::AliasTemplate:
case DeclaratorContext::Prototype:
case DeclaratorContext::LambdaExprParameter:
case DeclaratorContext::ObjCParameter:
case DeclaratorContext::ObjCResult:
case DeclaratorContext::TemplateParam:
case DeclaratorContext::CXXNew:
case DeclaratorContext::CXXCatch:
case DeclaratorContext::ObjCCatch:
case DeclaratorContext::BlockLiteral:
case DeclaratorContext::LambdaExpr:
case DeclaratorContext::ConversionId:
case DeclaratorContext::TrailingReturn:
case DeclaratorContext::TrailingReturnVar:
case DeclaratorContext::TemplateTypeArg:
case DeclaratorContext::RequiresExpr:
return false;

case DeclaratorContext::Block:
case DeclaratorContext::ForInit:
case DeclaratorContext::SelectionInit:
case DeclaratorContext::Condition:
case DeclaratorContext::TemplateArg:
return true;
}

llvm_unreachable("unknown context kind!");
}



bool isFunctionDeclaratorAFunctionDeclaration() const {
if (!isFunctionDeclarationContext())
return false;

for (unsigned I = 0, N = getNumTypeObjects(); I != N; ++I)
if (getTypeObject(I).Kind != DeclaratorChunk::Paren)
return false;

return true;
}



bool hasTrailingReturnType() const {
for (const auto &Chunk : type_objects())
if (Chunk.Kind == DeclaratorChunk::Function &&
Chunk.Fun.hasTrailingReturnType())
return true;
return false;
}


ParsedType getTrailingReturnType() const {
for (const auto &Chunk : type_objects())
if (Chunk.Kind == DeclaratorChunk::Function &&
Chunk.Fun.hasTrailingReturnType())
return Chunk.Fun.getTrailingReturnType();
return ParsedType();
}


void setTrailingRequiresClause(Expr *TRC) {
TrailingRequiresClause = TRC;

SetRangeEnd(TRC->getEndLoc());
}


Expr *getTrailingRequiresClause() {
return TrailingRequiresClause;
}



bool hasTrailingRequiresClause() const {
return TrailingRequiresClause != nullptr;
}


void setTemplateParameterLists(ArrayRef<TemplateParameterList *> TPLs) {
TemplateParameterLists = TPLs;
}


ArrayRef<TemplateParameterList *> getTemplateParameterLists() const {
return TemplateParameterLists;
}




void setInventedTemplateParameterList(TemplateParameterList *Invented) {
InventedTemplateParameterList = Invented;
}




TemplateParameterList * getInventedTemplateParameterList() const {
return InventedTemplateParameterList;
}










void takeAttributes(ParsedAttributes &attrs, SourceLocation lastLoc) {
Attrs.takeAllFrom(attrs);

if (!lastLoc.isInvalid())
SetRangeEnd(lastLoc);
}

const ParsedAttributes &getAttributes() const { return Attrs; }
ParsedAttributes &getAttributes() { return Attrs; }


bool hasAttributes() const {
if (!getAttributes().empty() || getDeclSpec().hasAttributes())
return true;
for (unsigned i = 0, e = getNumTypeObjects(); i != e; ++i)
if (!getTypeObject(i).getAttrs().empty())
return true;
return false;
}



void getCXX11AttributeRanges(SmallVectorImpl<SourceRange> &Ranges) {
for (const ParsedAttr &AL : Attrs)
if (AL.isCXX11Attribute())
Ranges.push_back(AL.getRange());
}

void setAsmLabel(Expr *E) { AsmLabel = E; }
Expr *getAsmLabel() const { return AsmLabel; }

void setExtension(bool Val = true) { Extension = Val; }
bool getExtension() const { return Extension; }

void setObjCIvar(bool Val = true) { ObjCIvar = Val; }
bool isObjCIvar() const { return ObjCIvar; }

void setObjCWeakProperty(bool Val = true) { ObjCWeakProperty = Val; }
bool isObjCWeakProperty() const { return ObjCWeakProperty; }

void setInvalidType(bool Val = true) { InvalidType = Val; }
bool isInvalidType() const {
return InvalidType || DS.getTypeSpecType() == DeclSpec::TST_error;
}

void setGroupingParens(bool flag) { GroupingParens = flag; }
bool hasGroupingParens() const { return GroupingParens; }

bool isFirstDeclarator() const { return !CommaLoc.isValid(); }
SourceLocation getCommaLoc() const { return CommaLoc; }
void setCommaLoc(SourceLocation CL) { CommaLoc = CL; }

bool hasEllipsis() const { return EllipsisLoc.isValid(); }
SourceLocation getEllipsisLoc() const { return EllipsisLoc; }
void setEllipsisLoc(SourceLocation EL) { EllipsisLoc = EL; }

void setFunctionDefinitionKind(FunctionDefinitionKind Val) {
FunctionDefinition = static_cast<unsigned>(Val);
}

bool isFunctionDefinition() const {
return getFunctionDefinitionKind() != FunctionDefinitionKind::Declaration;
}

FunctionDefinitionKind getFunctionDefinitionKind() const {
return (FunctionDefinitionKind)FunctionDefinition;
}

void setHasInitializer(bool Val = true) { HasInitializer = Val; }
bool hasInitializer() const { return HasInitializer; }


bool isFirstDeclarationOfMember() {
return getContext() == DeclaratorContext::Member &&
!getDeclSpec().isFriendSpecified();
}




bool isStaticMember();


bool isCtorOrDtor();

void setRedeclaration(bool Val) { Redeclaration = Val; }
bool isRedeclaration() const { return Redeclaration; }
};



struct FieldDeclarator {
Declarator D;
Expr *BitfieldSize;
explicit FieldDeclarator(const DeclSpec &DS)
: D(DS, DeclaratorContext::Member), BitfieldSize(nullptr) {}
};


class VirtSpecifiers {
public:
enum Specifier {
VS_None = 0,
VS_Override = 1,
VS_Final = 2,
VS_Sealed = 4,

VS_GNU_Final = 8,
VS_Abstract = 16
};

VirtSpecifiers() : Specifiers(0), LastSpecifier(VS_None) { }

bool SetSpecifier(Specifier VS, SourceLocation Loc,
const char *&PrevSpec);

bool isUnset() const { return Specifiers == 0; }

bool isOverrideSpecified() const { return Specifiers & VS_Override; }
SourceLocation getOverrideLoc() const { return VS_overrideLoc; }

bool isFinalSpecified() const { return Specifiers & (VS_Final | VS_Sealed | VS_GNU_Final); }
bool isFinalSpelledSealed() const { return Specifiers & VS_Sealed; }
SourceLocation getFinalLoc() const { return VS_finalLoc; }
SourceLocation getAbstractLoc() const { return VS_abstractLoc; }

void clear() { Specifiers = 0; }

static const char *getSpecifierName(Specifier VS);

SourceLocation getFirstLocation() const { return FirstLocation; }
SourceLocation getLastLocation() const { return LastLocation; }
Specifier getLastSpecifier() const { return LastSpecifier; }

private:
unsigned Specifiers;
Specifier LastSpecifier;

SourceLocation VS_overrideLoc, VS_finalLoc, VS_abstractLoc;
SourceLocation FirstLocation;
SourceLocation LastLocation;
};

enum class LambdaCaptureInitKind {
NoInit,
CopyInit,
DirectInit,
ListInit
};


struct LambdaIntroducer {

struct LambdaCapture {
LambdaCaptureKind Kind;
SourceLocation Loc;
IdentifierInfo *Id;
SourceLocation EllipsisLoc;
LambdaCaptureInitKind InitKind;
ExprResult Init;
ParsedType InitCaptureType;
SourceRange ExplicitRange;

LambdaCapture(LambdaCaptureKind Kind, SourceLocation Loc,
IdentifierInfo *Id, SourceLocation EllipsisLoc,
LambdaCaptureInitKind InitKind, ExprResult Init,
ParsedType InitCaptureType,
SourceRange ExplicitRange)
: Kind(Kind), Loc(Loc), Id(Id), EllipsisLoc(EllipsisLoc),
InitKind(InitKind), Init(Init), InitCaptureType(InitCaptureType),
ExplicitRange(ExplicitRange) {}
};

SourceRange Range;
SourceLocation DefaultLoc;
LambdaCaptureDefault Default;
SmallVector<LambdaCapture, 4> Captures;

LambdaIntroducer()
: Default(LCD_None) {}


void addCapture(LambdaCaptureKind Kind,
SourceLocation Loc,
IdentifierInfo* Id,
SourceLocation EllipsisLoc,
LambdaCaptureInitKind InitKind,
ExprResult Init,
ParsedType InitCaptureType,
SourceRange ExplicitRange) {
Captures.push_back(LambdaCapture(Kind, Loc, Id, EllipsisLoc, InitKind, Init,
InitCaptureType, ExplicitRange));
}
};

struct InventedTemplateParameterInfo {



unsigned NumExplicitTemplateParams = 0;



unsigned AutoTemplateParameterDepth = 0;








SmallVector<NamedDecl*, 4> TemplateParams;
};

}

#endif
