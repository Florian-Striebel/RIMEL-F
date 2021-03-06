












#if !defined(LLVM_CLANG_SEMA_OVERLOAD_H)
#define LLVM_CLANG_SEMA_OVERLOAD_H

#include "clang/AST/Decl.h"
#include "clang/AST/DeclAccessPair.h"
#include "clang/AST/DeclBase.h"
#include "clang/AST/DeclCXX.h"
#include "clang/AST/DeclTemplate.h"
#include "clang/AST/Expr.h"
#include "clang/AST/Type.h"
#include "clang/Basic/LLVM.h"
#include "clang/Basic/SourceLocation.h"
#include "clang/Sema/SemaFixItUtils.h"
#include "clang/Sema/TemplateDeduction.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/None.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/ADT/SmallPtrSet.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/AlignOf.h"
#include "llvm/Support/Allocator.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/ErrorHandling.h"
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <utility>

namespace clang {

class APValue;
class ASTContext;
class Sema;



enum OverloadingResult {

OR_Success,


OR_No_Viable_Function,


OR_Ambiguous,


OR_Deleted
};

enum OverloadCandidateDisplayKind {


OCD_AllCandidates,


OCD_ViableCandidates,


OCD_AmbiguousCandidates
};









enum class OverloadCandidateParamOrder : char { Normal, Reversed };




enum OverloadCandidateRewriteKind : unsigned {

CRK_None = 0x0,


CRK_DifferentOperator = 0x1,


CRK_Reversed = 0x2,
};





enum ImplicitConversionKind {

ICK_Identity = 0,


ICK_Lvalue_To_Rvalue,


ICK_Array_To_Pointer,


ICK_Function_To_Pointer,


ICK_Function_Conversion,


ICK_Qualification,


ICK_Integral_Promotion,


ICK_Floating_Promotion,


ICK_Complex_Promotion,


ICK_Integral_Conversion,


ICK_Floating_Conversion,


ICK_Complex_Conversion,


ICK_Floating_Integral,


ICK_Pointer_Conversion,


ICK_Pointer_Member,


ICK_Boolean_Conversion,


ICK_Compatible_Conversion,


ICK_Derived_To_Base,


ICK_Vector_Conversion,


ICK_SVE_Vector_Conversion,


ICK_Vector_Splat,


ICK_Complex_Real,


ICK_Block_Pointer_Conversion,


ICK_TransparentUnionConversion,


ICK_Writeback_Conversion,


ICK_Zero_Event_Conversion,


ICK_Zero_Queue_Conversion,


ICK_C_Only_Conversion,


ICK_Incompatible_Pointer_Conversion,


ICK_Num_Conversion_Kinds,
};





enum ImplicitConversionRank {

ICR_Exact_Match = 0,


ICR_Promotion,


ICR_Conversion,


ICR_OCL_Scalar_Widening,


ICR_Complex_Real_Conversion,


ICR_Writeback_Conversion,


ICR_C_Conversion,



ICR_C_Conversion_Extension
};

ImplicitConversionRank GetConversionRank(ImplicitConversionKind Kind);



enum NarrowingKind {

NK_Not_Narrowing,


NK_Type_Narrowing,


NK_Constant_Narrowing,



NK_Variable_Narrowing,



NK_Dependent_Narrowing,
};









class StandardConversionSequence {
public:



ImplicitConversionKind First : 8;





ImplicitConversionKind Second : 8;



ImplicitConversionKind Third : 8;




unsigned DeprecatedStringLiteralToCharPtr : 1;



unsigned QualificationIncludesObjCLifetime : 1;



unsigned IncompatibleObjC : 1;



unsigned ReferenceBinding : 1;



unsigned DirectBinding : 1;



unsigned IsLvalueReference : 1;


unsigned BindsToFunctionLvalue : 1;


unsigned BindsToRvalue : 1;



unsigned BindsImplicitObjectArgumentWithoutRefQualifier : 1;



unsigned ObjCLifetimeConversionBinding : 1;




void *FromTypePtr;




void *ToTypePtrs[3];






CXXConstructorDecl *CopyConstructor;
DeclAccessPair FoundCopyConstructor;

void setFromType(QualType T) { FromTypePtr = T.getAsOpaquePtr(); }

void setToType(unsigned Idx, QualType T) {
assert(Idx < 3 && "To type index is out of range");
ToTypePtrs[Idx] = T.getAsOpaquePtr();
}

void setAllToTypes(QualType T) {
ToTypePtrs[0] = T.getAsOpaquePtr();
ToTypePtrs[1] = ToTypePtrs[0];
ToTypePtrs[2] = ToTypePtrs[0];
}

QualType getFromType() const {
return QualType::getFromOpaquePtr(FromTypePtr);
}

QualType getToType(unsigned Idx) const {
assert(Idx < 3 && "To type index is out of range");
return QualType::getFromOpaquePtr(ToTypePtrs[Idx]);
}

void setAsIdentityConversion();

bool isIdentityConversion() const {
return Second == ICK_Identity && Third == ICK_Identity;
}

ImplicitConversionRank getRank() const;
NarrowingKind
getNarrowingKind(ASTContext &Context, const Expr *Converted,
APValue &ConstantValue, QualType &ConstantType,
bool IgnoreFloatToIntegralConversion = false) const;
bool isPointerConversionToBool() const;
bool isPointerConversionToVoidPointer(ASTContext& Context) const;
void dump() const;
};



struct UserDefinedConversionSequence {











StandardConversionSequence Before;







bool EllipsisConversion : 1;




bool HadMultipleCandidates : 1;



StandardConversionSequence After;




FunctionDecl* ConversionFunction;




DeclAccessPair FoundConversionFunction;

void dump() const;
};


struct AmbiguousConversionSequence {
using ConversionSet =
SmallVector<std::pair<NamedDecl *, FunctionDecl *>, 4>;

void *FromTypePtr;
void *ToTypePtr;
char Buffer[sizeof(ConversionSet)];

QualType getFromType() const {
return QualType::getFromOpaquePtr(FromTypePtr);
}

QualType getToType() const {
return QualType::getFromOpaquePtr(ToTypePtr);
}

void setFromType(QualType T) { FromTypePtr = T.getAsOpaquePtr(); }
void setToType(QualType T) { ToTypePtr = T.getAsOpaquePtr(); }

ConversionSet &conversions() {
return *reinterpret_cast<ConversionSet*>(Buffer);
}

const ConversionSet &conversions() const {
return *reinterpret_cast<const ConversionSet*>(Buffer);
}

void addConversion(NamedDecl *Found, FunctionDecl *D) {
conversions().push_back(std::make_pair(Found, D));
}

using iterator = ConversionSet::iterator;

iterator begin() { return conversions().begin(); }
iterator end() { return conversions().end(); }

using const_iterator = ConversionSet::const_iterator;

const_iterator begin() const { return conversions().begin(); }
const_iterator end() const { return conversions().end(); }

void construct();
void destruct();
void copyFrom(const AmbiguousConversionSequence &);
};



struct BadConversionSequence {
enum FailureKind {
no_conversion,
unrelated_class,
bad_qualifiers,
lvalue_ref_to_rvalue,
rvalue_ref_to_lvalue
};


Expr *FromExpr;

FailureKind Kind;

private:

void *FromTy;


void *ToTy;

public:
void init(FailureKind K, Expr *From, QualType To) {
init(K, From->getType(), To);
FromExpr = From;
}

void init(FailureKind K, QualType From, QualType To) {
Kind = K;
FromExpr = nullptr;
setFromType(From);
setToType(To);
}

QualType getFromType() const { return QualType::getFromOpaquePtr(FromTy); }
QualType getToType() const { return QualType::getFromOpaquePtr(ToTy); }

void setFromExpr(Expr *E) {
FromExpr = E;
setFromType(E->getType());
}

void setFromType(QualType T) { FromTy = T.getAsOpaquePtr(); }
void setToType(QualType T) { ToTy = T.getAsOpaquePtr(); }
};





class ImplicitConversionSequence {
public:




enum Kind {
StandardConversion = 0,
UserDefinedConversion,
AmbiguousConversion,
EllipsisConversion,
BadConversion
};

private:
enum {
Uninitialized = BadConversion + 1
};


unsigned ConversionKind : 30;



unsigned StdInitializerListElement : 1;

void setKind(Kind K) {
destruct();
ConversionKind = K;
}

void destruct() {
if (ConversionKind == AmbiguousConversion) Ambiguous.destruct();
}

public:
union {


StandardConversionSequence Standard;



UserDefinedConversionSequence UserDefined;



AmbiguousConversionSequence Ambiguous;



BadConversionSequence Bad;
};

ImplicitConversionSequence()
: ConversionKind(Uninitialized), StdInitializerListElement(false) {
Standard.setAsIdentityConversion();
}

ImplicitConversionSequence(const ImplicitConversionSequence &Other)
: ConversionKind(Other.ConversionKind),
StdInitializerListElement(Other.StdInitializerListElement) {
switch (ConversionKind) {
case Uninitialized: break;
case StandardConversion: Standard = Other.Standard; break;
case UserDefinedConversion: UserDefined = Other.UserDefined; break;
case AmbiguousConversion: Ambiguous.copyFrom(Other.Ambiguous); break;
case EllipsisConversion: break;
case BadConversion: Bad = Other.Bad; break;
}
}

ImplicitConversionSequence &
operator=(const ImplicitConversionSequence &Other) {
destruct();
new (this) ImplicitConversionSequence(Other);
return *this;
}

~ImplicitConversionSequence() {
destruct();
}

Kind getKind() const {
assert(isInitialized() && "querying uninitialized conversion");
return Kind(ConversionKind);
}








unsigned getKindRank() const {
switch (getKind()) {
case StandardConversion:
return 0;

case UserDefinedConversion:
case AmbiguousConversion:
return 1;

case EllipsisConversion:
return 2;

case BadConversion:
return 3;
}

llvm_unreachable("Invalid ImplicitConversionSequence::Kind!");
}

bool isBad() const { return getKind() == BadConversion; }
bool isStandard() const { return getKind() == StandardConversion; }
bool isEllipsis() const { return getKind() == EllipsisConversion; }
bool isAmbiguous() const { return getKind() == AmbiguousConversion; }
bool isUserDefined() const { return getKind() == UserDefinedConversion; }
bool isFailure() const { return isBad() || isAmbiguous(); }




bool isInitialized() const { return ConversionKind != Uninitialized; }


void setBad(BadConversionSequence::FailureKind Failure,
Expr *FromExpr, QualType ToType) {
setKind(BadConversion);
Bad.init(Failure, FromExpr, ToType);
}


void setBad(BadConversionSequence::FailureKind Failure,
QualType FromType, QualType ToType) {
setKind(BadConversion);
Bad.init(Failure, FromType, ToType);
}

void setStandard() { setKind(StandardConversion); }
void setEllipsis() { setKind(EllipsisConversion); }
void setUserDefined() { setKind(UserDefinedConversion); }

void setAmbiguous() {
if (ConversionKind == AmbiguousConversion) return;
ConversionKind = AmbiguousConversion;
Ambiguous.construct();
}

void setAsIdentityConversion(QualType T) {
setStandard();
Standard.setAsIdentityConversion();
Standard.setFromType(T);
Standard.setAllToTypes(T);
}



bool isStdInitializerListElement() const {
return StdInitializerListElement;
}

void setStdInitializerListElement(bool V = true) {
StdInitializerListElement = V;
}



static ImplicitConversionSequence getNullptrToBool(QualType SourceType,
QualType DestType,
bool NeedLValToRVal) {
ImplicitConversionSequence ICS;
ICS.setStandard();
ICS.Standard.setAsIdentityConversion();
ICS.Standard.setFromType(SourceType);
if (NeedLValToRVal)
ICS.Standard.First = ICK_Lvalue_To_Rvalue;
ICS.Standard.setToType(0, SourceType);
ICS.Standard.Second = ICK_Boolean_Conversion;
ICS.Standard.setToType(1, DestType);
ICS.Standard.setToType(2, DestType);
return ICS;
}




enum CompareKind {
Better = -1,
Indistinguishable = 0,
Worse = 1
};

void DiagnoseAmbiguousConversion(Sema &S,
SourceLocation CaretLoc,
const PartialDiagnostic &PDiag) const;

void dump() const;
};

enum OverloadFailureKind {
ovl_fail_too_many_arguments,
ovl_fail_too_few_arguments,
ovl_fail_bad_conversion,
ovl_fail_bad_deduction,




ovl_fail_trivial_conversion,










ovl_fail_illegal_constructor,



ovl_fail_bad_final_conversion,



ovl_fail_final_conversion_not_exact,




ovl_fail_bad_target,



ovl_fail_enable_if,



ovl_fail_explicit,


ovl_fail_addr_not_available,



ovl_fail_inhctor_slice,



ovl_non_default_multiversion_function,




ovl_fail_object_addrspace_mismatch,



ovl_fail_constraints_not_satisfied,
};



using ConversionSequenceList =
llvm::MutableArrayRef<ImplicitConversionSequence>;


struct OverloadCandidate {




FunctionDecl *Function;




DeclAccessPair FoundDecl;



QualType BuiltinParamTypes[3];



CXXConversionDecl *Surrogate;




ConversionSequenceList Conversions;


ConversionFixItGenerator Fix;


bool Viable : 1;








bool Best : 1;




bool IsSurrogate : 1;








bool IgnoreObjectArgument : 1;


CallExpr::ADLCallKind IsADLCandidate : 1;


unsigned RewriteKind : 2;



unsigned char FailureKind;



unsigned ExplicitCallArguments;

union {
DeductionFailureInfo DeductionFailure;





StandardConversionSequence FinalConversion;
};



OverloadCandidateRewriteKind getRewriteKind() const {
return static_cast<OverloadCandidateRewriteKind>(RewriteKind);
}

bool isReversed() const { return getRewriteKind() & CRK_Reversed; }



bool hasAmbiguousConversion() const {
for (auto &C : Conversions) {
if (!C.isInitialized()) return false;
if (C.isAmbiguous()) return true;
}
return false;
}

bool TryToFixBadConversion(unsigned Idx, Sema &S) {
bool CanFix = Fix.tryToFixConversion(
Conversions[Idx].Bad.FromExpr,
Conversions[Idx].Bad.getFromType(),
Conversions[Idx].Bad.getToType(), S);


if (!CanFix)
Fix.clear();

return CanFix;
}

unsigned getNumParams() const {
if (IsSurrogate) {
QualType STy = Surrogate->getConversionType();
while (STy->isPointerType() || STy->isReferenceType())
STy = STy->getPointeeType();
return STy->castAs<FunctionProtoType>()->getNumParams();
}
if (Function)
return Function->getNumParams();
return ExplicitCallArguments;
}

private:
friend class OverloadCandidateSet;
OverloadCandidate()
: IsSurrogate(false), IsADLCandidate(CallExpr::NotADL), RewriteKind(CRK_None) {}
};



class OverloadCandidateSet {
public:
enum CandidateSetKind {

CSK_Normal,






CSK_Operator,




CSK_InitByUserDefinedConversion,




CSK_InitByConstructor,
};



struct OperatorRewriteInfo {
OperatorRewriteInfo()
: OriginalOperator(OO_None), AllowRewrittenCandidates(false) {}
OperatorRewriteInfo(OverloadedOperatorKind Op, bool AllowRewritten)
: OriginalOperator(Op), AllowRewrittenCandidates(AllowRewritten) {}


OverloadedOperatorKind OriginalOperator;

bool AllowRewrittenCandidates;



bool isRewrittenOperator(const FunctionDecl *FD) {
return OriginalOperator &&
FD->getDeclName().getCXXOverloadedOperator() != OriginalOperator;
}

bool isAcceptableCandidate(const FunctionDecl *FD) {
if (!OriginalOperator)
return true;




OverloadedOperatorKind OO =
FD->getDeclName().getCXXOverloadedOperator();
return OO && (OO == OriginalOperator ||
(AllowRewrittenCandidates &&
OO == getRewrittenOverloadedOperator(OriginalOperator)));
}



OverloadCandidateRewriteKind
getRewriteKind(const FunctionDecl *FD, OverloadCandidateParamOrder PO) {
OverloadCandidateRewriteKind CRK = CRK_None;
if (isRewrittenOperator(FD))
CRK = OverloadCandidateRewriteKind(CRK | CRK_DifferentOperator);
if (PO == OverloadCandidateParamOrder::Reversed)
CRK = OverloadCandidateRewriteKind(CRK | CRK_Reversed);
return CRK;
}



bool isReversible() {
return AllowRewrittenCandidates && OriginalOperator &&
(getRewrittenOverloadedOperator(OriginalOperator) != OO_None ||
shouldAddReversed(OriginalOperator));
}



bool shouldAddReversed(OverloadedOperatorKind Op);



bool shouldAddReversed(ASTContext &Ctx, const FunctionDecl *FD);
};

private:
SmallVector<OverloadCandidate, 16> Candidates;
llvm::SmallPtrSet<uintptr_t, 16> Functions;



llvm::BumpPtrAllocator SlabAllocator;

SourceLocation Loc;
CandidateSetKind Kind;
OperatorRewriteInfo RewriteInfo;

constexpr static unsigned NumInlineBytes =
24 * sizeof(ImplicitConversionSequence);
unsigned NumInlineBytesUsed = 0;
alignas(void *) char InlineSpace[NumInlineBytes];


LangAS DestAS = LangAS::Default;







template <typename T>
T *slabAllocate(unsigned N) {

static_assert(alignof(T) == alignof(void *),
"Only works for pointer-aligned types.");
static_assert(std::is_trivial<T>::value ||
std::is_same<ImplicitConversionSequence, T>::value,
"Add destruction logic to OverloadCandidateSet::clear().");

unsigned NBytes = sizeof(T) * N;
if (NBytes > NumInlineBytes - NumInlineBytesUsed)
return SlabAllocator.Allocate<T>(N);
char *FreeSpaceStart = InlineSpace + NumInlineBytesUsed;
assert(uintptr_t(FreeSpaceStart) % alignof(void *) == 0 &&
"Misaligned storage!");

NumInlineBytesUsed += NBytes;
return reinterpret_cast<T *>(FreeSpaceStart);
}

void destroyCandidates();

public:
OverloadCandidateSet(SourceLocation Loc, CandidateSetKind CSK,
OperatorRewriteInfo RewriteInfo = {})
: Loc(Loc), Kind(CSK), RewriteInfo(RewriteInfo) {}
OverloadCandidateSet(const OverloadCandidateSet &) = delete;
OverloadCandidateSet &operator=(const OverloadCandidateSet &) = delete;
~OverloadCandidateSet() { destroyCandidates(); }

SourceLocation getLocation() const { return Loc; }
CandidateSetKind getKind() const { return Kind; }
OperatorRewriteInfo getRewriteInfo() const { return RewriteInfo; }


bool shouldDeferDiags(Sema &S, ArrayRef<Expr *> Args, SourceLocation OpLoc);



bool isNewCandidate(Decl *F, OverloadCandidateParamOrder PO =
OverloadCandidateParamOrder::Normal) {
uintptr_t Key = reinterpret_cast<uintptr_t>(F->getCanonicalDecl());
Key |= static_cast<uintptr_t>(PO);
return Functions.insert(Key).second;
}


void exclude(Decl *F) {
isNewCandidate(F, OverloadCandidateParamOrder::Normal);
isNewCandidate(F, OverloadCandidateParamOrder::Reversed);
}


void clear(CandidateSetKind CSK);

using iterator = SmallVectorImpl<OverloadCandidate>::iterator;

iterator begin() { return Candidates.begin(); }
iterator end() { return Candidates.end(); }

size_t size() const { return Candidates.size(); }
bool empty() const { return Candidates.empty(); }



ConversionSequenceList
allocateConversionSequences(unsigned NumConversions) {
ImplicitConversionSequence *Conversions =
slabAllocate<ImplicitConversionSequence>(NumConversions);


for (unsigned I = 0; I != NumConversions; ++I)
new (&Conversions[I]) ImplicitConversionSequence();

return ConversionSequenceList(Conversions, NumConversions);
}



OverloadCandidate &addCandidate(unsigned NumConversions = 0,
ConversionSequenceList Conversions = None) {
assert((Conversions.empty() || Conversions.size() == NumConversions) &&
"preallocated conversion sequence has wrong length");

Candidates.push_back(OverloadCandidate());
OverloadCandidate &C = Candidates.back();
C.Conversions = Conversions.empty()
? allocateConversionSequences(NumConversions)
: Conversions;
return C;
}


OverloadingResult BestViableFunction(Sema &S, SourceLocation Loc,
OverloadCandidateSet::iterator& Best);

SmallVector<OverloadCandidate *, 32> CompleteCandidates(
Sema &S, OverloadCandidateDisplayKind OCD, ArrayRef<Expr *> Args,
SourceLocation OpLoc = SourceLocation(),
llvm::function_ref<bool(OverloadCandidate &)> Filter =
[](OverloadCandidate &) { return true; });

void NoteCandidates(
PartialDiagnosticAt PA, Sema &S, OverloadCandidateDisplayKind OCD,
ArrayRef<Expr *> Args, StringRef Opc = "",
SourceLocation Loc = SourceLocation(),
llvm::function_ref<bool(OverloadCandidate &)> Filter =
[](OverloadCandidate &) { return true; });

void NoteCandidates(Sema &S, ArrayRef<Expr *> Args,
ArrayRef<OverloadCandidate *> Cands,
StringRef Opc = "",
SourceLocation OpLoc = SourceLocation());

LangAS getDestAS() { return DestAS; }

void setDestAS(LangAS AS) {
assert((Kind == CSK_InitByConstructor ||
Kind == CSK_InitByUserDefinedConversion) &&
"can't set the destination address space when not constructing an "
"object");
DestAS = AS;
}

};

bool isBetterOverloadCandidate(Sema &S,
const OverloadCandidate &Cand1,
const OverloadCandidate &Cand2,
SourceLocation Loc,
OverloadCandidateSet::CandidateSetKind Kind);

struct ConstructorInfo {
DeclAccessPair FoundDecl;
CXXConstructorDecl *Constructor;
FunctionTemplateDecl *ConstructorTmpl;

explicit operator bool() const { return Constructor; }
};



inline ConstructorInfo getConstructorInfo(NamedDecl *ND) {
if (isa<UsingDecl>(ND))
return ConstructorInfo{};



auto *D = ND->getUnderlyingDecl();
ConstructorInfo Info = {DeclAccessPair::make(ND, D->getAccess()), nullptr,
nullptr};
Info.ConstructorTmpl = dyn_cast<FunctionTemplateDecl>(D);
if (Info.ConstructorTmpl)
D = Info.ConstructorTmpl->getTemplatedDecl();
Info.Constructor = dyn_cast<CXXConstructorDecl>(D);
return Info;
}

}

#endif
