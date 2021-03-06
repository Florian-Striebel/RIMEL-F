













#if !defined(LLVM_CLANG_AST_DECLCXX_H)
#define LLVM_CLANG_AST_DECLCXX_H

#include "clang/AST/ASTUnresolvedSet.h"
#include "clang/AST/Decl.h"
#include "clang/AST/DeclBase.h"
#include "clang/AST/DeclarationName.h"
#include "clang/AST/Expr.h"
#include "clang/AST/ExternalASTSource.h"
#include "clang/AST/LambdaCapture.h"
#include "clang/AST/NestedNameSpecifier.h"
#include "clang/AST/Redeclarable.h"
#include "clang/AST/Stmt.h"
#include "clang/AST/Type.h"
#include "clang/AST/TypeLoc.h"
#include "clang/AST/UnresolvedSet.h"
#include "clang/Basic/LLVM.h"
#include "clang/Basic/Lambda.h"
#include "clang/Basic/LangOptions.h"
#include "clang/Basic/OperatorKinds.h"
#include "clang/Basic/SourceLocation.h"
#include "clang/Basic/Specifiers.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/PointerIntPair.h"
#include "llvm/ADT/PointerUnion.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/ADT/TinyPtrVector.h"
#include "llvm/ADT/iterator_range.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/Compiler.h"
#include "llvm/Support/PointerLikeTypeTraits.h"
#include "llvm/Support/TrailingObjects.h"
#include <cassert>
#include <cstddef>
#include <iterator>
#include <memory>
#include <vector>

namespace clang {

class ASTContext;
class ClassTemplateDecl;
class ConstructorUsingShadowDecl;
class CXXBasePath;
class CXXBasePaths;
class CXXConstructorDecl;
class CXXDestructorDecl;
class CXXFinalOverriderMap;
class CXXIndirectPrimaryBaseSet;
class CXXMethodDecl;
class DecompositionDecl;
class DiagnosticBuilder;
class FriendDecl;
class FunctionTemplateDecl;
class IdentifierInfo;
class MemberSpecializationInfo;
class BaseUsingDecl;
class TemplateDecl;
class TemplateParameterList;
class UsingDecl;











class AccessSpecDecl : public Decl {

SourceLocation ColonLoc;

AccessSpecDecl(AccessSpecifier AS, DeclContext *DC,
SourceLocation ASLoc, SourceLocation ColonLoc)
: Decl(AccessSpec, DC, ASLoc), ColonLoc(ColonLoc) {
setAccess(AS);
}

AccessSpecDecl(EmptyShell Empty) : Decl(AccessSpec, Empty) {}

virtual void anchor();

public:

SourceLocation getAccessSpecifierLoc() const { return getLocation(); }


void setAccessSpecifierLoc(SourceLocation ASLoc) { setLocation(ASLoc); }


SourceLocation getColonLoc() const { return ColonLoc; }


void setColonLoc(SourceLocation CLoc) { ColonLoc = CLoc; }

SourceRange getSourceRange() const override LLVM_READONLY {
return SourceRange(getAccessSpecifierLoc(), getColonLoc());
}

static AccessSpecDecl *Create(ASTContext &C, AccessSpecifier AS,
DeclContext *DC, SourceLocation ASLoc,
SourceLocation ColonLoc) {
return new (C, DC) AccessSpecDecl(AS, DC, ASLoc, ColonLoc);
}

static AccessSpecDecl *CreateDeserialized(ASTContext &C, unsigned ID);


static bool classof(const Decl *D) { return classofKind(D->getKind()); }
static bool classofKind(Kind K) { return K == AccessSpec; }
};

















class CXXBaseSpecifier {



SourceRange Range;



SourceLocation EllipsisLoc;


unsigned Virtual : 1;





unsigned BaseOfClass : 1;





unsigned Access : 2;



unsigned InheritConstructors : 1;





TypeSourceInfo *BaseTypeInfo;

public:
CXXBaseSpecifier() = default;
CXXBaseSpecifier(SourceRange R, bool V, bool BC, AccessSpecifier A,
TypeSourceInfo *TInfo, SourceLocation EllipsisLoc)
: Range(R), EllipsisLoc(EllipsisLoc), Virtual(V), BaseOfClass(BC),
Access(A), InheritConstructors(false), BaseTypeInfo(TInfo) {}


SourceRange getSourceRange() const LLVM_READONLY { return Range; }
SourceLocation getBeginLoc() const LLVM_READONLY { return Range.getBegin(); }
SourceLocation getEndLoc() const LLVM_READONLY { return Range.getEnd(); }


SourceLocation getBaseTypeLoc() const LLVM_READONLY {
return BaseTypeInfo->getTypeLoc().getBeginLoc();
}


bool isVirtual() const { return Virtual; }



bool isBaseOfClass() const { return BaseOfClass; }


bool isPackExpansion() const { return EllipsisLoc.isValid(); }


bool getInheritConstructors() const { return InheritConstructors; }


void setInheritConstructors(bool Inherit = true) {
InheritConstructors = Inherit;
}


SourceLocation getEllipsisLoc() const {
return EllipsisLoc;
}






AccessSpecifier getAccessSpecifier() const {
if ((AccessSpecifier)Access == AS_none)
return BaseOfClass? AS_private : AS_public;
else
return (AccessSpecifier)Access;
}






AccessSpecifier getAccessSpecifierAsWritten() const {
return (AccessSpecifier)Access;
}




QualType getType() const {
return BaseTypeInfo->getType().getUnqualifiedType();
}


TypeSourceInfo *getTypeSourceInfo() const { return BaseTypeInfo; }
};


class CXXRecordDecl : public RecordDecl {
friend class ASTDeclReader;
friend class ASTDeclWriter;
friend class ASTNodeImporter;
friend class ASTReader;
friend class ASTRecordWriter;
friend class ASTWriter;
friend class DeclContext;
friend class LambdaExpr;

friend void FunctionDecl::setPure(bool);
friend void TagDecl::startDefinition();


enum SpecialMemberFlags {
SMF_DefaultConstructor = 0x1,
SMF_CopyConstructor = 0x2,
SMF_MoveConstructor = 0x4,
SMF_CopyAssignment = 0x8,
SMF_MoveAssignment = 0x10,
SMF_Destructor = 0x20,
SMF_All = 0x3f
};

struct DefinitionData {
#define FIELD(Name, Width, Merge) unsigned Name : Width;

#include "CXXRecordDeclDefinitionBits.def"


unsigned IsLambda : 1;


unsigned IsParsingBaseSpecifiers : 1;



unsigned ComputedVisibleConversions : 1;

unsigned HasODRHash : 1;


unsigned ODRHash = 0;


unsigned NumBases = 0;


unsigned NumVBases = 0;




LazyCXXBaseSpecifiersPtr Bases;


LazyCXXBaseSpecifiersPtr VBases;





LazyASTUnresolvedSet Conversions;






LazyASTUnresolvedSet VisibleConversions;


CXXRecordDecl *Definition;





LazyDeclPtr FirstFriend;

DefinitionData(CXXRecordDecl *D);


CXXBaseSpecifier *getBases() const {
if (!Bases.isOffset())
return Bases.get(nullptr);
return getBasesSlowCase();
}


CXXBaseSpecifier *getVBases() const {
if (!VBases.isOffset())
return VBases.get(nullptr);
return getVBasesSlowCase();
}

ArrayRef<CXXBaseSpecifier> bases() const {
return llvm::makeArrayRef(getBases(), NumBases);
}

ArrayRef<CXXBaseSpecifier> vbases() const {
return llvm::makeArrayRef(getVBases(), NumVBases);
}

private:
CXXBaseSpecifier *getBasesSlowCase() const;
CXXBaseSpecifier *getVBasesSlowCase() const;
};

struct DefinitionData *DefinitionData;


struct LambdaDefinitionData : public DefinitionData {
using Capture = LambdaCapture;









unsigned Dependent : 1;


unsigned IsGenericLambda : 1;


unsigned CaptureDefault : 2;


unsigned NumCaptures : 15;


unsigned NumExplicitCaptures : 13;


unsigned HasKnownInternalLinkage : 1;



unsigned ManglingNumber : 31;





LazyDeclPtr ContextDecl;



Capture *Captures = nullptr;


TypeSourceInfo *MethodTyInfo;

LambdaDefinitionData(CXXRecordDecl *D, TypeSourceInfo *Info, bool Dependent,
bool IsGeneric, LambdaCaptureDefault CaptureDefault)
: DefinitionData(D), Dependent(Dependent), IsGenericLambda(IsGeneric),
CaptureDefault(CaptureDefault), NumCaptures(0),
NumExplicitCaptures(0), HasKnownInternalLinkage(0), ManglingNumber(0),
MethodTyInfo(Info) {
IsLambda = true;



Aggregate = false;
PlainOldData = false;
}
};

struct DefinitionData *dataPtr() const {

getMostRecentDecl();
return DefinitionData;
}

struct DefinitionData &data() const {
auto *DD = dataPtr();
assert(DD && "queried property of class with no definition");
return *DD;
}

struct LambdaDefinitionData &getLambdaData() const {


auto *DD = DefinitionData;
assert(DD && DD->IsLambda && "queried lambda property of non-lambda class");
return static_cast<LambdaDefinitionData&>(*DD);
}










llvm::PointerUnion<ClassTemplateDecl *, MemberSpecializationInfo *>
TemplateOrInstantiation;



void addedClassSubobject(CXXRecordDecl *Base);






void addedMember(Decl *D);

void markedVirtualFunctionPure();



FriendDecl *getFirstFriend() const;




bool hasSubobjectAtOffsetZeroOfEmptyBaseType(ASTContext &Ctx,
const CXXRecordDecl *X);

protected:
CXXRecordDecl(Kind K, TagKind TK, const ASTContext &C, DeclContext *DC,
SourceLocation StartLoc, SourceLocation IdLoc,
IdentifierInfo *Id, CXXRecordDecl *PrevDecl);

public:

using base_class_iterator = CXXBaseSpecifier *;


using base_class_const_iterator = const CXXBaseSpecifier *;

CXXRecordDecl *getCanonicalDecl() override {
return cast<CXXRecordDecl>(RecordDecl::getCanonicalDecl());
}

const CXXRecordDecl *getCanonicalDecl() const {
return const_cast<CXXRecordDecl*>(this)->getCanonicalDecl();
}

CXXRecordDecl *getPreviousDecl() {
return cast_or_null<CXXRecordDecl>(
static_cast<RecordDecl *>(this)->getPreviousDecl());
}

const CXXRecordDecl *getPreviousDecl() const {
return const_cast<CXXRecordDecl*>(this)->getPreviousDecl();
}

CXXRecordDecl *getMostRecentDecl() {
return cast<CXXRecordDecl>(
static_cast<RecordDecl *>(this)->getMostRecentDecl());
}

const CXXRecordDecl *getMostRecentDecl() const {
return const_cast<CXXRecordDecl*>(this)->getMostRecentDecl();
}

CXXRecordDecl *getMostRecentNonInjectedDecl() {
CXXRecordDecl *Recent =
static_cast<CXXRecordDecl *>(this)->getMostRecentDecl();
while (Recent->isInjectedClassName()) {

assert(Recent->getPreviousDecl());
Recent = Recent->getPreviousDecl();
}
return Recent;
}

const CXXRecordDecl *getMostRecentNonInjectedDecl() const {
return const_cast<CXXRecordDecl*>(this)->getMostRecentNonInjectedDecl();
}

CXXRecordDecl *getDefinition() const {


auto *DD = DefinitionData ? DefinitionData : dataPtr();
return DD ? DD->Definition : nullptr;
}

bool hasDefinition() const { return DefinitionData || dataPtr(); }

static CXXRecordDecl *Create(const ASTContext &C, TagKind TK, DeclContext *DC,
SourceLocation StartLoc, SourceLocation IdLoc,
IdentifierInfo *Id,
CXXRecordDecl *PrevDecl = nullptr,
bool DelayTypeCreation = false);
static CXXRecordDecl *CreateLambda(const ASTContext &C, DeclContext *DC,
TypeSourceInfo *Info, SourceLocation Loc,
bool DependentLambda, bool IsGeneric,
LambdaCaptureDefault CaptureDefault);
static CXXRecordDecl *CreateDeserialized(const ASTContext &C, unsigned ID);

bool isDynamicClass() const {
return data().Polymorphic || data().NumVBases != 0;
}



bool mayBeDynamicClass() const {
return !hasDefinition() || isDynamicClass() || hasAnyDependentBases();
}



bool mayBeNonDynamicClass() const {
return !hasDefinition() || !isDynamicClass() || hasAnyDependentBases();
}

void setIsParsingBaseSpecifiers() { data().IsParsingBaseSpecifiers = true; }

bool isParsingBaseSpecifiers() const {
return data().IsParsingBaseSpecifiers;
}

unsigned getODRHash() const;


void setBases(CXXBaseSpecifier const * const *Bases, unsigned NumBases);


unsigned getNumBases() const { return data().NumBases; }

using base_class_range = llvm::iterator_range<base_class_iterator>;
using base_class_const_range =
llvm::iterator_range<base_class_const_iterator>;

base_class_range bases() {
return base_class_range(bases_begin(), bases_end());
}
base_class_const_range bases() const {
return base_class_const_range(bases_begin(), bases_end());
}

base_class_iterator bases_begin() { return data().getBases(); }
base_class_const_iterator bases_begin() const { return data().getBases(); }
base_class_iterator bases_end() { return bases_begin() + data().NumBases; }
base_class_const_iterator bases_end() const {
return bases_begin() + data().NumBases;
}


unsigned getNumVBases() const { return data().NumVBases; }

base_class_range vbases() {
return base_class_range(vbases_begin(), vbases_end());
}
base_class_const_range vbases() const {
return base_class_const_range(vbases_begin(), vbases_end());
}

base_class_iterator vbases_begin() { return data().getVBases(); }
base_class_const_iterator vbases_begin() const { return data().getVBases(); }
base_class_iterator vbases_end() { return vbases_begin() + data().NumVBases; }
base_class_const_iterator vbases_end() const {
return vbases_begin() + data().NumVBases;
}



bool hasAnyDependentBases() const;




using method_iterator = specific_decl_iterator<CXXMethodDecl>;
using method_range =
llvm::iterator_range<specific_decl_iterator<CXXMethodDecl>>;

method_range methods() const {
return method_range(method_begin(), method_end());
}



method_iterator method_begin() const {
return method_iterator(decls_begin());
}


method_iterator method_end() const {
return method_iterator(decls_end());
}


using ctor_iterator = specific_decl_iterator<CXXConstructorDecl>;
using ctor_range =
llvm::iterator_range<specific_decl_iterator<CXXConstructorDecl>>;

ctor_range ctors() const { return ctor_range(ctor_begin(), ctor_end()); }

ctor_iterator ctor_begin() const {
return ctor_iterator(decls_begin());
}

ctor_iterator ctor_end() const {
return ctor_iterator(decls_end());
}



class friend_iterator;
using friend_range = llvm::iterator_range<friend_iterator>;

friend_range friends() const;
friend_iterator friend_begin() const;
friend_iterator friend_end() const;
void pushFriendDecl(FriendDecl *FD);


bool hasFriends() const {
return data().FirstFriend.isValid();
}



bool defaultedCopyConstructorIsDeleted() const {
assert((!needsOverloadResolutionForCopyConstructor() ||
(data().DeclaredSpecialMembers & SMF_CopyConstructor)) &&
"this property has not yet been computed by Sema");
return data().DefaultedCopyConstructorIsDeleted;
}



bool defaultedMoveConstructorIsDeleted() const {
assert((!needsOverloadResolutionForMoveConstructor() ||
(data().DeclaredSpecialMembers & SMF_MoveConstructor)) &&
"this property has not yet been computed by Sema");
return data().DefaultedMoveConstructorIsDeleted;
}


bool defaultedDestructorIsDeleted() const {
assert((!needsOverloadResolutionForDestructor() ||
(data().DeclaredSpecialMembers & SMF_Destructor)) &&
"this property has not yet been computed by Sema");
return data().DefaultedDestructorIsDeleted;
}



bool hasSimpleCopyConstructor() const {
return !hasUserDeclaredCopyConstructor() &&
!data().DefaultedCopyConstructorIsDeleted;
}



bool hasSimpleMoveConstructor() const {
return !hasUserDeclaredMoveConstructor() && hasMoveConstructor() &&
!data().DefaultedMoveConstructorIsDeleted;
}



bool hasSimpleCopyAssignment() const {
return !hasUserDeclaredCopyAssignment() &&
!data().DefaultedCopyAssignmentIsDeleted;
}



bool hasSimpleMoveAssignment() const {
return !hasUserDeclaredMoveAssignment() && hasMoveAssignment() &&
!data().DefaultedMoveAssignmentIsDeleted;
}



bool hasSimpleDestructor() const {
return !hasUserDeclaredDestructor() &&
!data().DefaultedDestructorIsDeleted;
}


bool hasDefaultConstructor() const {
return (data().DeclaredSpecialMembers & SMF_DefaultConstructor) ||
needsImplicitDefaultConstructor();
}





bool needsImplicitDefaultConstructor() const {
return (!data().UserDeclaredConstructor &&
!(data().DeclaredSpecialMembers & SMF_DefaultConstructor) &&
(!isLambda() || lambdaIsDefaultConstructibleAndAssignable())) ||



(data().HasInheritedDefaultConstructor &&
!(data().DeclaredSpecialMembers & SMF_DefaultConstructor));
}




bool hasUserDeclaredConstructor() const {
return data().UserDeclaredConstructor;
}



bool hasUserProvidedDefaultConstructor() const {
return data().UserProvidedDefaultConstructor;
}




bool hasUserDeclaredCopyConstructor() const {
return data().UserDeclaredSpecialMembers & SMF_CopyConstructor;
}



bool needsImplicitCopyConstructor() const {
return !(data().DeclaredSpecialMembers & SMF_CopyConstructor);
}



bool needsOverloadResolutionForCopyConstructor() const {






if (data().UserDeclaredSpecialMembers &
(SMF_MoveConstructor | SMF_MoveAssignment))
return true;
return data().NeedOverloadResolutionForCopyConstructor;
}



bool implicitCopyConstructorHasConstParam() const {
return data().ImplicitCopyConstructorCanHaveConstParamForNonVBase &&
(isAbstract() ||
data().ImplicitCopyConstructorCanHaveConstParamForVBase);
}



bool hasCopyConstructorWithConstParam() const {
return data().HasDeclaredCopyConstructorWithConstParam ||
(needsImplicitCopyConstructor() &&
implicitCopyConstructorHasConstParam());
}






bool hasUserDeclaredMoveOperation() const {
return data().UserDeclaredSpecialMembers &
(SMF_MoveConstructor | SMF_MoveAssignment);
}



bool hasUserDeclaredMoveConstructor() const {
return data().UserDeclaredSpecialMembers & SMF_MoveConstructor;
}


bool hasMoveConstructor() const {
return (data().DeclaredSpecialMembers & SMF_MoveConstructor) ||
needsImplicitMoveConstructor();
}



void setImplicitCopyConstructorIsDeleted() {
assert((data().DefaultedCopyConstructorIsDeleted ||
needsOverloadResolutionForCopyConstructor()) &&
"Copy constructor should not be deleted");
data().DefaultedCopyConstructorIsDeleted = true;
}



void setImplicitMoveConstructorIsDeleted() {
assert((data().DefaultedMoveConstructorIsDeleted ||
needsOverloadResolutionForMoveConstructor()) &&
"move constructor should not be deleted");
data().DefaultedMoveConstructorIsDeleted = true;
}



void setImplicitDestructorIsDeleted() {
assert((data().DefaultedDestructorIsDeleted ||
needsOverloadResolutionForDestructor()) &&
"destructor should not be deleted");
data().DefaultedDestructorIsDeleted = true;
}



bool needsImplicitMoveConstructor() const {
return !(data().DeclaredSpecialMembers & SMF_MoveConstructor) &&
!hasUserDeclaredCopyConstructor() &&
!hasUserDeclaredCopyAssignment() &&
!hasUserDeclaredMoveAssignment() &&
!hasUserDeclaredDestructor();
}



bool needsOverloadResolutionForMoveConstructor() const {
return data().NeedOverloadResolutionForMoveConstructor;
}





bool hasUserDeclaredCopyAssignment() const {
return data().UserDeclaredSpecialMembers & SMF_CopyAssignment;
}



void setImplicitCopyAssignmentIsDeleted() {
assert((data().DefaultedCopyAssignmentIsDeleted ||
needsOverloadResolutionForCopyAssignment()) &&
"copy assignment should not be deleted");
data().DefaultedCopyAssignmentIsDeleted = true;
}



bool needsImplicitCopyAssignment() const {
return !(data().DeclaredSpecialMembers & SMF_CopyAssignment);
}



bool needsOverloadResolutionForCopyAssignment() const {






if (data().UserDeclaredSpecialMembers &
(SMF_MoveConstructor | SMF_MoveAssignment))
return true;
return data().NeedOverloadResolutionForCopyAssignment;
}



bool implicitCopyAssignmentHasConstParam() const {
return data().ImplicitCopyAssignmentHasConstParam;
}




bool hasCopyAssignmentWithConstParam() const {
return data().HasDeclaredCopyAssignmentWithConstParam ||
(needsImplicitCopyAssignment() &&
implicitCopyAssignmentHasConstParam());
}



bool hasUserDeclaredMoveAssignment() const {
return data().UserDeclaredSpecialMembers & SMF_MoveAssignment;
}


bool hasMoveAssignment() const {
return (data().DeclaredSpecialMembers & SMF_MoveAssignment) ||
needsImplicitMoveAssignment();
}



void setImplicitMoveAssignmentIsDeleted() {
assert((data().DefaultedMoveAssignmentIsDeleted ||
needsOverloadResolutionForMoveAssignment()) &&
"move assignment should not be deleted");
data().DefaultedMoveAssignmentIsDeleted = true;
}




bool needsImplicitMoveAssignment() const {
return !(data().DeclaredSpecialMembers & SMF_MoveAssignment) &&
!hasUserDeclaredCopyConstructor() &&
!hasUserDeclaredCopyAssignment() &&
!hasUserDeclaredMoveConstructor() &&
!hasUserDeclaredDestructor() &&
(!isLambda() || lambdaIsDefaultConstructibleAndAssignable());
}



bool needsOverloadResolutionForMoveAssignment() const {
return data().NeedOverloadResolutionForMoveAssignment;
}




bool hasUserDeclaredDestructor() const {
return data().UserDeclaredSpecialMembers & SMF_Destructor;
}



bool needsImplicitDestructor() const {
return !(data().DeclaredSpecialMembers & SMF_Destructor);
}



bool needsOverloadResolutionForDestructor() const {
return data().NeedOverloadResolutionForDestructor;
}


bool isLambda() const {

auto *DD = DefinitionData;
return DD && DD->IsLambda;
}




bool isGenericLambda() const;



bool lambdaIsDefaultConstructibleAndAssignable() const;



CXXMethodDecl *getLambdaCallOperator() const;



FunctionTemplateDecl *getDependentLambdaCallOperator() const;








CXXMethodDecl *getLambdaStaticInvoker() const;
CXXMethodDecl *getLambdaStaticInvoker(CallingConv CC) const;




TemplateParameterList *getGenericLambdaTemplateParameterList() const;


ArrayRef<NamedDecl *> getLambdaExplicitTemplateParameters() const;

LambdaCaptureDefault getLambdaCaptureDefault() const {
assert(isLambda());
return static_cast<LambdaCaptureDefault>(getLambdaData().CaptureDefault);
}


void setCaptures(ASTContext &Context, ArrayRef<LambdaCapture> Captures);













void getCaptureFields(llvm::DenseMap<const VarDecl *, FieldDecl *> &Captures,
FieldDecl *&ThisCapture) const;

using capture_const_iterator = const LambdaCapture *;
using capture_const_range = llvm::iterator_range<capture_const_iterator>;

capture_const_range captures() const {
return capture_const_range(captures_begin(), captures_end());
}

capture_const_iterator captures_begin() const {
return isLambda() ? getLambdaData().Captures : nullptr;
}

capture_const_iterator captures_end() const {
return isLambda() ? captures_begin() + getLambdaData().NumCaptures
: nullptr;
}

unsigned capture_size() const { return getLambdaData().NumCaptures; }

using conversion_iterator = UnresolvedSetIterator;

conversion_iterator conversion_begin() const {
return data().Conversions.get(getASTContext()).begin();
}

conversion_iterator conversion_end() const {
return data().Conversions.get(getASTContext()).end();
}




void removeConversion(const NamedDecl *Old);



llvm::iterator_range<conversion_iterator>
getVisibleConversionFunctions() const;





bool isAggregate() const { return data().Aggregate; }




bool hasInClassInitializer() const { return data().HasInClassInitializer; }









bool hasUninitializedReferenceMember() const {
return !isUnion() && !hasUserDeclaredConstructor() &&
data().HasUninitializedReferenceMember;
}









bool isPOD() const { return data().PlainOldData; }



bool isCLike() const;










bool isEmpty() const { return data().Empty; }

bool hasPrivateFields() const {
return data().HasPrivateFields;
}

bool hasProtectedFields() const {
return data().HasProtectedFields;
}


bool hasDirectFields() const {
auto &D = data();
return D.HasPublicFields || D.HasProtectedFields || D.HasPrivateFields;
}



bool isPolymorphic() const { return data().Polymorphic; }






bool isAbstract() const { return data().Abstract; }



bool isStandardLayout() const { return data().IsStandardLayout; }



bool isCXX11StandardLayout() const { return data().IsCXX11StandardLayout; }



bool hasMutableFields() const { return data().HasMutableFields; }


bool hasVariantMembers() const { return data().HasVariantMembers; }



bool hasTrivialDefaultConstructor() const {
return hasDefaultConstructor() &&
(data().HasTrivialSpecialMembers & SMF_DefaultConstructor);
}



bool hasNonTrivialDefaultConstructor() const {
return (data().DeclaredNonTrivialSpecialMembers & SMF_DefaultConstructor) ||
(needsImplicitDefaultConstructor() &&
!(data().HasTrivialSpecialMembers & SMF_DefaultConstructor));
}



bool hasConstexprNonCopyMoveConstructor() const {
return data().HasConstexprNonCopyMoveConstructor ||
(needsImplicitDefaultConstructor() &&
defaultedDefaultConstructorIsConstexpr());
}



bool defaultedDefaultConstructorIsConstexpr() const {
return data().DefaultedDefaultConstructorIsConstexpr &&
(!isUnion() || hasInClassInitializer() || !hasVariantMembers() ||
getLangOpts().CPlusPlus20);
}


bool hasConstexprDefaultConstructor() const {
return data().HasConstexprDefaultConstructor ||
(needsImplicitDefaultConstructor() &&
defaultedDefaultConstructorIsConstexpr());
}



bool hasTrivialCopyConstructor() const {
return data().HasTrivialSpecialMembers & SMF_CopyConstructor;
}

bool hasTrivialCopyConstructorForCall() const {
return data().HasTrivialSpecialMembersForCall & SMF_CopyConstructor;
}



bool hasNonTrivialCopyConstructor() const {
return data().DeclaredNonTrivialSpecialMembers & SMF_CopyConstructor ||
!hasTrivialCopyConstructor();
}

bool hasNonTrivialCopyConstructorForCall() const {
return (data().DeclaredNonTrivialSpecialMembersForCall &
SMF_CopyConstructor) ||
!hasTrivialCopyConstructorForCall();
}



bool hasTrivialMoveConstructor() const {
return hasMoveConstructor() &&
(data().HasTrivialSpecialMembers & SMF_MoveConstructor);
}

bool hasTrivialMoveConstructorForCall() const {
return hasMoveConstructor() &&
(data().HasTrivialSpecialMembersForCall & SMF_MoveConstructor);
}



bool hasNonTrivialMoveConstructor() const {
return (data().DeclaredNonTrivialSpecialMembers & SMF_MoveConstructor) ||
(needsImplicitMoveConstructor() &&
!(data().HasTrivialSpecialMembers & SMF_MoveConstructor));
}

bool hasNonTrivialMoveConstructorForCall() const {
return (data().DeclaredNonTrivialSpecialMembersForCall &
SMF_MoveConstructor) ||
(needsImplicitMoveConstructor() &&
!(data().HasTrivialSpecialMembersForCall & SMF_MoveConstructor));
}



bool hasTrivialCopyAssignment() const {
return data().HasTrivialSpecialMembers & SMF_CopyAssignment;
}



bool hasNonTrivialCopyAssignment() const {
return data().DeclaredNonTrivialSpecialMembers & SMF_CopyAssignment ||
!hasTrivialCopyAssignment();
}



bool hasTrivialMoveAssignment() const {
return hasMoveAssignment() &&
(data().HasTrivialSpecialMembers & SMF_MoveAssignment);
}



bool hasNonTrivialMoveAssignment() const {
return (data().DeclaredNonTrivialSpecialMembers & SMF_MoveAssignment) ||
(needsImplicitMoveAssignment() &&
!(data().HasTrivialSpecialMembers & SMF_MoveAssignment));
}



bool defaultedDestructorIsConstexpr() const {
return data().DefaultedDestructorIsConstexpr &&
getLangOpts().CPlusPlus20;
}


bool hasConstexprDestructor() const;



bool hasTrivialDestructor() const {
return data().HasTrivialSpecialMembers & SMF_Destructor;
}

bool hasTrivialDestructorForCall() const {
return data().HasTrivialSpecialMembersForCall & SMF_Destructor;
}



bool hasNonTrivialDestructor() const {
return !(data().HasTrivialSpecialMembers & SMF_Destructor);
}

bool hasNonTrivialDestructorForCall() const {
return !(data().HasTrivialSpecialMembersForCall & SMF_Destructor);
}

void setHasTrivialSpecialMemberForCall() {
data().HasTrivialSpecialMembersForCall =
(SMF_CopyConstructor | SMF_MoveConstructor | SMF_Destructor);
}



bool allowConstDefaultInit() const {
return !data().HasUninitializedFields ||
!(data().HasDefaultedDefaultConstructor ||
needsImplicitDefaultConstructor());
}






bool hasIrrelevantDestructor() const {
return data().HasIrrelevantDestructor;
}



bool hasNonLiteralTypeFieldsOrBases() const {
return data().HasNonLiteralTypeFieldsOrBases;
}



bool hasInheritedConstructor() const {
return data().HasInheritedConstructor;
}



bool hasInheritedAssignment() const {
return data().HasInheritedAssignment;
}



bool isTriviallyCopyable() const;






bool isTrivial() const {
return isTriviallyCopyable() && hasTrivialDefaultConstructor();
}


















bool isLiteral() const {
const LangOptions &LangOpts = getLangOpts();
return (LangOpts.CPlusPlus20 ? hasConstexprDestructor()
: hasTrivialDestructor()) &&
(!isLambda() || LangOpts.CPlusPlus17) &&
!hasNonLiteralTypeFieldsOrBases() &&
(isAggregate() || isLambda() ||
hasConstexprNonCopyMoveConstructor() ||
hasTrivialDefaultConstructor());
}


bool isStructural() const {
return isLiteral() && data().StructuralIfLiteral;
}




















CXXRecordDecl *getInstantiatedFromMemberClass() const;




MemberSpecializationInfo *getMemberSpecializationInfo() const;



void setInstantiationOfMemberClass(CXXRecordDecl *RD,
TemplateSpecializationKind TSK);












ClassTemplateDecl *getDescribedClassTemplate() const;

void setDescribedClassTemplate(ClassTemplateDecl *Template);




TemplateSpecializationKind getTemplateSpecializationKind() const;


void setTemplateSpecializationKind(TemplateSpecializationKind TSK);



const CXXRecordDecl *getTemplateInstantiationPattern() const;

CXXRecordDecl *getTemplateInstantiationPattern() {
return const_cast<CXXRecordDecl *>(const_cast<const CXXRecordDecl *>(this)
->getTemplateInstantiationPattern());
}


CXXDestructorDecl *getDestructor() const;



bool isAnyDestructorNoReturn() const { return data().IsAnyDestructorNoReturn; }



const FunctionDecl *isLocalClass() const {
if (const auto *RD = dyn_cast<CXXRecordDecl>(getDeclContext()))
return RD->isLocalClass();

return dyn_cast<FunctionDecl>(getDeclContext());
}

FunctionDecl *isLocalClass() {
return const_cast<FunctionDecl*>(
const_cast<const CXXRecordDecl*>(this)->isLocalClass());
}



bool isCurrentInstantiation(const DeclContext *CurContext) const;











bool isDerivedFrom(const CXXRecordDecl *Base) const;

















bool isDerivedFrom(const CXXRecordDecl *Base, CXXBasePaths &Paths) const;














bool isVirtuallyDerivedFrom(const CXXRecordDecl *Base) const;



bool isProvablyNotDerivedFrom(const CXXRecordDecl *Base) const;






using ForallBasesCallback =
llvm::function_ref<bool(const CXXRecordDecl *BaseDefinition)>;









bool forallBases(ForallBasesCallback BaseMatches) const;











using BaseMatchesCallback =
llvm::function_ref<bool(const CXXBaseSpecifier *Specifier,
CXXBasePath &Path)>;






















bool lookupInBases(BaseMatchesCallback BaseMatches, CXXBasePaths &Paths,
bool LookupInDependent = false) const;








static bool FindBaseClass(const CXXBaseSpecifier *Specifier,
CXXBasePath &Path, const CXXRecordDecl *BaseRecord);










static bool FindVirtualBaseClass(const CXXBaseSpecifier *Specifier,
CXXBasePath &Path,
const CXXRecordDecl *BaseRecord);




void getFinalOverriders(CXXFinalOverriderMap &FinaOverriders) const;


void getIndirectPrimaryBases(CXXIndirectPrimaryBaseSet& Bases) const;







bool hasMemberName(DeclarationName N) const;





std::vector<const NamedDecl *>
lookupDependentName(DeclarationName Name,
llvm::function_ref<bool(const NamedDecl *ND)> Filter);




void viewInheritance(ASTContext& Context) const;



static AccessSpecifier MergeAccess(AccessSpecifier PathAccess,
AccessSpecifier DeclAccess) {
assert(DeclAccess != AS_none);
if (DeclAccess == AS_private) return AS_none;
return (PathAccess > DeclAccess ? PathAccess : DeclAccess);
}



void finishedDefaultedOrDeletedMember(CXXMethodDecl *MD);

void setTrivialForCallFlags(CXXMethodDecl *MD);


void completeDefinition() override;








void completeDefinition(CXXFinalOverriderMap *FinalOverriders);








bool mayBeAbstract() const;



bool isEffectivelyFinal() const;







unsigned getLambdaManglingNumber() const {
assert(isLambda() && "Not a lambda closure type!");
return getLambdaData().ManglingNumber;
}



bool hasKnownLambdaInternalLinkage() const {
assert(isLambda() && "Not a lambda closure type!");
return getLambdaData().HasKnownInternalLinkage;
}










Decl *getLambdaContextDecl() const;



void setLambdaMangling(unsigned ManglingNumber, Decl *ContextDecl,
bool HasKnownInternalLinkage = false) {
assert(isLambda() && "Not a lambda closure type!");
getLambdaData().ManglingNumber = ManglingNumber;
getLambdaData().ContextDecl = ContextDecl;
getLambdaData().HasKnownInternalLinkage = HasKnownInternalLinkage;
}


void setDeviceLambdaManglingNumber(unsigned Num) const;


unsigned getDeviceLambdaManglingNumber() const;


MSInheritanceModel getMSInheritanceModel() const;


MSInheritanceModel calculateInheritanceModel() const;







bool nullFieldOffsetIsZero() const;



MSVtorDispMode getMSVtorDispMode() const;












bool isDependentLambda() const {
return isLambda() && getLambdaData().Dependent;
}

TypeSourceInfo *getLambdaTypeInfo() const {
return getLambdaData().MethodTyInfo;
}



bool isInterfaceLike() const;

static bool classof(const Decl *D) { return classofKind(D->getKind()); }
static bool classofKind(Kind K) {
return K >= firstCXXRecord && K <= lastCXXRecord;
}
void markAbstract() { data().Abstract = true; }
};



class ExplicitSpecifier {
llvm::PointerIntPair<Expr *, 2, ExplicitSpecKind> ExplicitSpec{
nullptr, ExplicitSpecKind::ResolvedFalse};

public:
ExplicitSpecifier() = default;
ExplicitSpecifier(Expr *Expression, ExplicitSpecKind Kind)
: ExplicitSpec(Expression, Kind) {}
ExplicitSpecKind getKind() const { return ExplicitSpec.getInt(); }
const Expr *getExpr() const { return ExplicitSpec.getPointer(); }
Expr *getExpr() { return ExplicitSpec.getPointer(); }


bool isSpecified() const {
return ExplicitSpec.getInt() != ExplicitSpecKind::ResolvedFalse ||
ExplicitSpec.getPointer();
}



bool isEquivalent(const ExplicitSpecifier Other) const;



bool isExplicit() const {
return ExplicitSpec.getInt() == ExplicitSpecKind::ResolvedTrue;
}


bool isInvalid() const {
return ExplicitSpec.getInt() == ExplicitSpecKind::Unresolved &&
!ExplicitSpec.getPointer();
}
void setKind(ExplicitSpecKind Kind) { ExplicitSpec.setInt(Kind); }
void setExpr(Expr *E) { ExplicitSpec.setPointer(E); }

static ExplicitSpecifier getFromDecl(FunctionDecl *Function);
static const ExplicitSpecifier getFromDecl(const FunctionDecl *Function) {
return getFromDecl(const_cast<FunctionDecl *>(Function));
}
static ExplicitSpecifier Invalid() {
return ExplicitSpecifier(nullptr, ExplicitSpecKind::Unresolved);
}
};











class CXXDeductionGuideDecl : public FunctionDecl {
void anchor() override;

private:
CXXDeductionGuideDecl(ASTContext &C, DeclContext *DC, SourceLocation StartLoc,
ExplicitSpecifier ES,
const DeclarationNameInfo &NameInfo, QualType T,
TypeSourceInfo *TInfo, SourceLocation EndLocation,
CXXConstructorDecl *Ctor)
: FunctionDecl(CXXDeductionGuide, C, DC, StartLoc, NameInfo, T, TInfo,
SC_None, false, ConstexprSpecKind::Unspecified),
Ctor(Ctor), ExplicitSpec(ES) {
if (EndLocation.isValid())
setRangeEnd(EndLocation);
setIsCopyDeductionCandidate(false);
}

CXXConstructorDecl *Ctor;
ExplicitSpecifier ExplicitSpec;
void setExplicitSpecifier(ExplicitSpecifier ES) { ExplicitSpec = ES; }

public:
friend class ASTDeclReader;
friend class ASTDeclWriter;

static CXXDeductionGuideDecl *
Create(ASTContext &C, DeclContext *DC, SourceLocation StartLoc,
ExplicitSpecifier ES, const DeclarationNameInfo &NameInfo, QualType T,
TypeSourceInfo *TInfo, SourceLocation EndLocation,
CXXConstructorDecl *Ctor = nullptr);

static CXXDeductionGuideDecl *CreateDeserialized(ASTContext &C, unsigned ID);

ExplicitSpecifier getExplicitSpecifier() { return ExplicitSpec; }
const ExplicitSpecifier getExplicitSpecifier() const { return ExplicitSpec; }


bool isExplicit() const { return ExplicitSpec.isExplicit(); }


TemplateDecl *getDeducedTemplate() const {
return getDeclName().getCXXDeductionGuideTemplate();
}



CXXConstructorDecl *getCorrespondingConstructor() const {
return Ctor;
}

void setIsCopyDeductionCandidate(bool isCDC = true) {
FunctionDeclBits.IsCopyDeductionCandidate = isCDC;
}

bool isCopyDeductionCandidate() const {
return FunctionDeclBits.IsCopyDeductionCandidate;
}


static bool classof(const Decl *D) { return classofKind(D->getKind()); }
static bool classofKind(Kind K) { return K == CXXDeductionGuide; }
};














class RequiresExprBodyDecl : public Decl, public DeclContext {
RequiresExprBodyDecl(ASTContext &C, DeclContext *DC, SourceLocation StartLoc)
: Decl(RequiresExprBody, DC, StartLoc), DeclContext(RequiresExprBody) {}

public:
friend class ASTDeclReader;
friend class ASTDeclWriter;

static RequiresExprBodyDecl *Create(ASTContext &C, DeclContext *DC,
SourceLocation StartLoc);

static RequiresExprBodyDecl *CreateDeserialized(ASTContext &C, unsigned ID);


static bool classof(const Decl *D) { return classofKind(D->getKind()); }
static bool classofKind(Kind K) { return K == RequiresExprBody; }
};





class CXXMethodDecl : public FunctionDecl {
void anchor() override;

protected:
CXXMethodDecl(Kind DK, ASTContext &C, CXXRecordDecl *RD,
SourceLocation StartLoc, const DeclarationNameInfo &NameInfo,
QualType T, TypeSourceInfo *TInfo, StorageClass SC,
bool isInline, ConstexprSpecKind ConstexprKind,
SourceLocation EndLocation,
Expr *TrailingRequiresClause = nullptr)
: FunctionDecl(DK, C, RD, StartLoc, NameInfo, T, TInfo, SC, isInline,
ConstexprKind, TrailingRequiresClause) {
if (EndLocation.isValid())
setRangeEnd(EndLocation);
}

public:
static CXXMethodDecl *Create(ASTContext &C, CXXRecordDecl *RD,
SourceLocation StartLoc,
const DeclarationNameInfo &NameInfo, QualType T,
TypeSourceInfo *TInfo, StorageClass SC,
bool isInline, ConstexprSpecKind ConstexprKind,
SourceLocation EndLocation,
Expr *TrailingRequiresClause = nullptr);

static CXXMethodDecl *CreateDeserialized(ASTContext &C, unsigned ID);

bool isStatic() const;
bool isInstance() const { return !isStatic(); }



static bool isStaticOverloadedOperator(OverloadedOperatorKind OOK) {





return OOK == OO_New || OOK == OO_Array_New || OOK == OO_Delete ||
OOK == OO_Array_Delete;
}

bool isConst() const { return getType()->castAs<FunctionType>()->isConst(); }
bool isVolatile() const { return getType()->castAs<FunctionType>()->isVolatile(); }

bool isVirtual() const {
CXXMethodDecl *CD = const_cast<CXXMethodDecl*>(this)->getCanonicalDecl();



if (CD->isVirtualAsWritten() || CD->isPure())
return true;

return CD->size_overridden_methods() != 0;
}






CXXMethodDecl *getDevirtualizedMethod(const Expr *Base, bool IsAppleKext);

const CXXMethodDecl *getDevirtualizedMethod(const Expr *Base,
bool IsAppleKext) const {
return const_cast<CXXMethodDecl *>(this)->getDevirtualizedMethod(
Base, IsAppleKext);
}








bool isUsualDeallocationFunction(
SmallVectorImpl<const FunctionDecl *> &PreventedBy) const;



bool isCopyAssignmentOperator() const;


bool isMoveAssignmentOperator() const;

CXXMethodDecl *getCanonicalDecl() override {
return cast<CXXMethodDecl>(FunctionDecl::getCanonicalDecl());
}
const CXXMethodDecl *getCanonicalDecl() const {
return const_cast<CXXMethodDecl*>(this)->getCanonicalDecl();
}

CXXMethodDecl *getMostRecentDecl() {
return cast<CXXMethodDecl>(
static_cast<FunctionDecl *>(this)->getMostRecentDecl());
}
const CXXMethodDecl *getMostRecentDecl() const {
return const_cast<CXXMethodDecl*>(this)->getMostRecentDecl();
}

void addOverriddenMethod(const CXXMethodDecl *MD);

using method_iterator = const CXXMethodDecl *const *;

method_iterator begin_overridden_methods() const;
method_iterator end_overridden_methods() const;
unsigned size_overridden_methods() const;

using overridden_method_range = llvm::iterator_range<
llvm::TinyPtrVector<const CXXMethodDecl *>::const_iterator>;

overridden_method_range overridden_methods() const;



const CXXRecordDecl *getParent() const {
return cast<CXXRecordDecl>(FunctionDecl::getParent());
}



CXXRecordDecl *getParent() {
return const_cast<CXXRecordDecl *>(
cast<CXXRecordDecl>(FunctionDecl::getParent()));
}







QualType getThisType() const;




QualType getThisObjectType() const;

static QualType getThisType(const FunctionProtoType *FPT,
const CXXRecordDecl *Decl);

static QualType getThisObjectType(const FunctionProtoType *FPT,
const CXXRecordDecl *Decl);

Qualifiers getMethodQualifiers() const {
return getType()->castAs<FunctionProtoType>()->getMethodQuals();
}












RefQualifierKind getRefQualifier() const {
return getType()->castAs<FunctionProtoType>()->getRefQualifier();
}

bool hasInlineBody() const;








bool isLambdaStaticInvoker() const;






CXXMethodDecl *
getCorrespondingMethodInClass(const CXXRecordDecl *RD,
bool MayBeBase = false);

const CXXMethodDecl *
getCorrespondingMethodInClass(const CXXRecordDecl *RD,
bool MayBeBase = false) const {
return const_cast<CXXMethodDecl *>(this)
->getCorrespondingMethodInClass(RD, MayBeBase);
}



CXXMethodDecl *getCorrespondingMethodDeclaredInClass(const CXXRecordDecl *RD,
bool MayBeBase = false);
const CXXMethodDecl *
getCorrespondingMethodDeclaredInClass(const CXXRecordDecl *RD,
bool MayBeBase = false) const {
return const_cast<CXXMethodDecl *>(this)
->getCorrespondingMethodDeclaredInClass(RD, MayBeBase);
}


static bool classof(const Decl *D) { return classofKind(D->getKind()); }
static bool classofKind(Kind K) {
return K >= firstCXXMethod && K <= lastCXXMethod;
}
};
















class CXXCtorInitializer final {



llvm::PointerUnion<TypeSourceInfo *, FieldDecl *, IndirectFieldDecl *>
Initializee;



Stmt *Init;







SourceLocation MemberOrEllipsisLocation;


SourceLocation LParenLoc;


SourceLocation RParenLoc;



unsigned IsDelegating : 1;



unsigned IsVirtual : 1;



unsigned IsWritten : 1;



unsigned SourceOrder : 13;

public:

explicit
CXXCtorInitializer(ASTContext &Context, TypeSourceInfo *TInfo, bool IsVirtual,
SourceLocation L, Expr *Init, SourceLocation R,
SourceLocation EllipsisLoc);


explicit
CXXCtorInitializer(ASTContext &Context, FieldDecl *Member,
SourceLocation MemberLoc, SourceLocation L, Expr *Init,
SourceLocation R);


explicit
CXXCtorInitializer(ASTContext &Context, IndirectFieldDecl *Member,
SourceLocation MemberLoc, SourceLocation L, Expr *Init,
SourceLocation R);


explicit
CXXCtorInitializer(ASTContext &Context, TypeSourceInfo *TInfo,
SourceLocation L, Expr *Init, SourceLocation R);


int64_t getID(const ASTContext &Context) const;


bool isBaseInitializer() const {
return Initializee.is<TypeSourceInfo*>() && !IsDelegating;
}



bool isMemberInitializer() const { return Initializee.is<FieldDecl*>(); }

bool isAnyMemberInitializer() const {
return isMemberInitializer() || isIndirectMemberInitializer();
}

bool isIndirectMemberInitializer() const {
return Initializee.is<IndirectFieldDecl*>();
}







bool isInClassMemberInitializer() const {
return Init->getStmtClass() == Stmt::CXXDefaultInitExprClass;
}



bool isDelegatingInitializer() const {
return Initializee.is<TypeSourceInfo*>() && IsDelegating;
}


bool isPackExpansion() const {
return isBaseInitializer() && MemberOrEllipsisLocation.isValid();
}


SourceLocation getEllipsisLoc() const {
if (!isPackExpansion())
return {};
return MemberOrEllipsisLocation;
}




TypeLoc getBaseClassLoc() const;



const Type *getBaseClass() const;


bool isBaseVirtual() const {
assert(isBaseInitializer() && "Must call this on base initializer!");

return IsVirtual;
}



TypeSourceInfo *getTypeSourceInfo() const {
return Initializee.dyn_cast<TypeSourceInfo *>();
}



FieldDecl *getMember() const {
if (isMemberInitializer())
return Initializee.get<FieldDecl*>();
return nullptr;
}

FieldDecl *getAnyMember() const {
if (isMemberInitializer())
return Initializee.get<FieldDecl*>();
if (isIndirectMemberInitializer())
return Initializee.get<IndirectFieldDecl*>()->getAnonField();
return nullptr;
}

IndirectFieldDecl *getIndirectMember() const {
if (isIndirectMemberInitializer())
return Initializee.get<IndirectFieldDecl*>();
return nullptr;
}

SourceLocation getMemberLocation() const {
return MemberOrEllipsisLocation;
}


SourceLocation getSourceLocation() const;


SourceRange getSourceRange() const LLVM_READONLY;



bool isWritten() const { return IsWritten; }



int getSourceOrder() const {
return IsWritten ? static_cast<int>(SourceOrder) : -1;
}








void setSourceOrder(int Pos) {
assert(!IsWritten &&
"setSourceOrder() used on implicit initializer");
assert(SourceOrder == 0 &&
"calling twice setSourceOrder() on the same initializer");
assert(Pos >= 0 &&
"setSourceOrder() used to make an initializer implicit");
IsWritten = true;
SourceOrder = static_cast<unsigned>(Pos);
}

SourceLocation getLParenLoc() const { return LParenLoc; }
SourceLocation getRParenLoc() const { return RParenLoc; }


Expr *getInit() const { return static_cast<Expr *>(Init); }
};


class InheritedConstructor {
ConstructorUsingShadowDecl *Shadow = nullptr;
CXXConstructorDecl *BaseCtor = nullptr;

public:
InheritedConstructor() = default;
InheritedConstructor(ConstructorUsingShadowDecl *Shadow,
CXXConstructorDecl *BaseCtor)
: Shadow(Shadow), BaseCtor(BaseCtor) {}

explicit operator bool() const { return Shadow; }

ConstructorUsingShadowDecl *getShadowDecl() const { return Shadow; }
CXXConstructorDecl *getConstructor() const { return BaseCtor; }
};











class CXXConstructorDecl final
: public CXXMethodDecl,
private llvm::TrailingObjects<CXXConstructorDecl, InheritedConstructor,
ExplicitSpecifier> {






LazyCXXCtorInitializersPtr CtorInitializers;

CXXConstructorDecl(ASTContext &C, CXXRecordDecl *RD, SourceLocation StartLoc,
const DeclarationNameInfo &NameInfo, QualType T,
TypeSourceInfo *TInfo, ExplicitSpecifier ES, bool isInline,
bool isImplicitlyDeclared, ConstexprSpecKind ConstexprKind,
InheritedConstructor Inherited,
Expr *TrailingRequiresClause);

void anchor() override;

size_t numTrailingObjects(OverloadToken<InheritedConstructor>) const {
return CXXConstructorDeclBits.IsInheritingConstructor;
}
size_t numTrailingObjects(OverloadToken<ExplicitSpecifier>) const {
return CXXConstructorDeclBits.HasTrailingExplicitSpecifier;
}

ExplicitSpecifier getExplicitSpecifierInternal() const {
if (CXXConstructorDeclBits.HasTrailingExplicitSpecifier)
return *getTrailingObjects<ExplicitSpecifier>();
return ExplicitSpecifier(
nullptr, CXXConstructorDeclBits.IsSimpleExplicit
? ExplicitSpecKind::ResolvedTrue
: ExplicitSpecKind::ResolvedFalse);
}

enum TrailingAllocKind {
TAKInheritsConstructor = 1,
TAKHasTailExplicit = 1 << 1,
};

uint64_t getTrailingAllocKind() const {
return numTrailingObjects(OverloadToken<InheritedConstructor>()) |
(numTrailingObjects(OverloadToken<ExplicitSpecifier>()) << 1);
}

public:
friend class ASTDeclReader;
friend class ASTDeclWriter;
friend TrailingObjects;

static CXXConstructorDecl *CreateDeserialized(ASTContext &C, unsigned ID,
uint64_t AllocKind);
static CXXConstructorDecl *
Create(ASTContext &C, CXXRecordDecl *RD, SourceLocation StartLoc,
const DeclarationNameInfo &NameInfo, QualType T, TypeSourceInfo *TInfo,
ExplicitSpecifier ES, bool isInline, bool isImplicitlyDeclared,
ConstexprSpecKind ConstexprKind,
InheritedConstructor Inherited = InheritedConstructor(),
Expr *TrailingRequiresClause = nullptr);

void setExplicitSpecifier(ExplicitSpecifier ES) {
assert((!ES.getExpr() ||
CXXConstructorDeclBits.HasTrailingExplicitSpecifier) &&
"cannot set this explicit specifier. no trail-allocated space for "
"explicit");
if (ES.getExpr())
*getCanonicalDecl()->getTrailingObjects<ExplicitSpecifier>() = ES;
else
CXXConstructorDeclBits.IsSimpleExplicit = ES.isExplicit();
}

ExplicitSpecifier getExplicitSpecifier() {
return getCanonicalDecl()->getExplicitSpecifierInternal();
}
const ExplicitSpecifier getExplicitSpecifier() const {
return getCanonicalDecl()->getExplicitSpecifierInternal();
}


bool isExplicit() const { return getExplicitSpecifier().isExplicit(); }


using init_iterator = CXXCtorInitializer **;


using init_const_iterator = CXXCtorInitializer *const *;

using init_range = llvm::iterator_range<init_iterator>;
using init_const_range = llvm::iterator_range<init_const_iterator>;

init_range inits() { return init_range(init_begin(), init_end()); }
init_const_range inits() const {
return init_const_range(init_begin(), init_end());
}


init_iterator init_begin() {
const auto *ConstThis = this;
return const_cast<init_iterator>(ConstThis->init_begin());
}


init_const_iterator init_begin() const;


init_iterator init_end() {
return init_begin() + getNumCtorInitializers();
}


init_const_iterator init_end() const {
return init_begin() + getNumCtorInitializers();
}

using init_reverse_iterator = std::reverse_iterator<init_iterator>;
using init_const_reverse_iterator =
std::reverse_iterator<init_const_iterator>;

init_reverse_iterator init_rbegin() {
return init_reverse_iterator(init_end());
}
init_const_reverse_iterator init_rbegin() const {
return init_const_reverse_iterator(init_end());
}

init_reverse_iterator init_rend() {
return init_reverse_iterator(init_begin());
}
init_const_reverse_iterator init_rend() const {
return init_const_reverse_iterator(init_begin());
}



unsigned getNumCtorInitializers() const {
return CXXConstructorDeclBits.NumCtorInitializers;
}

void setNumCtorInitializers(unsigned numCtorInitializers) {
CXXConstructorDeclBits.NumCtorInitializers = numCtorInitializers;



assert(CXXConstructorDeclBits.NumCtorInitializers ==
numCtorInitializers && "NumCtorInitializers overflow!");
}

void setCtorInitializers(CXXCtorInitializer **Initializers) {
CtorInitializers = Initializers;
}


bool isDelegatingConstructor() const {
return (getNumCtorInitializers() == 1) &&
init_begin()[0]->isDelegatingInitializer();
}


CXXConstructorDecl *getTargetConstructor() const;




bool isDefaultConstructor() const;














bool isCopyConstructor(unsigned &TypeQuals) const;




bool isCopyConstructor() const {
unsigned TypeQuals = 0;
return isCopyConstructor(TypeQuals);
}






bool isMoveConstructor(unsigned &TypeQuals) const;



bool isMoveConstructor() const {
unsigned TypeQuals = 0;
return isMoveConstructor(TypeQuals);
}





bool isCopyOrMoveConstructor(unsigned &TypeQuals) const;


bool isCopyOrMoveConstructor() const {
unsigned Quals;
return isCopyOrMoveConstructor(Quals);
}




bool isConvertingConstructor(bool AllowExplicit) const;




bool isSpecializationCopyingObject() const;



bool isInheritingConstructor() const {
return CXXConstructorDeclBits.IsInheritingConstructor;
}



void setInheritingConstructor(bool isIC = true) {
CXXConstructorDeclBits.IsInheritingConstructor = isIC;
}


InheritedConstructor getInheritedConstructor() const {
return isInheritingConstructor() ?
*getTrailingObjects<InheritedConstructor>() : InheritedConstructor();
}

CXXConstructorDecl *getCanonicalDecl() override {
return cast<CXXConstructorDecl>(FunctionDecl::getCanonicalDecl());
}
const CXXConstructorDecl *getCanonicalDecl() const {
return const_cast<CXXConstructorDecl*>(this)->getCanonicalDecl();
}


static bool classof(const Decl *D) { return classofKind(D->getKind()); }
static bool classofKind(Kind K) { return K == CXXConstructor; }
};











class CXXDestructorDecl : public CXXMethodDecl {
friend class ASTDeclReader;
friend class ASTDeclWriter;



FunctionDecl *OperatorDelete = nullptr;
Expr *OperatorDeleteThisArg = nullptr;

CXXDestructorDecl(ASTContext &C, CXXRecordDecl *RD, SourceLocation StartLoc,
const DeclarationNameInfo &NameInfo, QualType T,
TypeSourceInfo *TInfo, bool isInline,
bool isImplicitlyDeclared, ConstexprSpecKind ConstexprKind,
Expr *TrailingRequiresClause = nullptr)
: CXXMethodDecl(CXXDestructor, C, RD, StartLoc, NameInfo, T, TInfo,
SC_None, isInline, ConstexprKind, SourceLocation(),
TrailingRequiresClause) {
setImplicit(isImplicitlyDeclared);
}

void anchor() override;

public:
static CXXDestructorDecl *Create(ASTContext &C, CXXRecordDecl *RD,
SourceLocation StartLoc,
const DeclarationNameInfo &NameInfo,
QualType T, TypeSourceInfo *TInfo,
bool isInline, bool isImplicitlyDeclared,
ConstexprSpecKind ConstexprKind,
Expr *TrailingRequiresClause = nullptr);
static CXXDestructorDecl *CreateDeserialized(ASTContext & C, unsigned ID);

void setOperatorDelete(FunctionDecl *OD, Expr *ThisArg);

const FunctionDecl *getOperatorDelete() const {
return getCanonicalDecl()->OperatorDelete;
}

Expr *getOperatorDeleteThisArg() const {
return getCanonicalDecl()->OperatorDeleteThisArg;
}

CXXDestructorDecl *getCanonicalDecl() override {
return cast<CXXDestructorDecl>(FunctionDecl::getCanonicalDecl());
}
const CXXDestructorDecl *getCanonicalDecl() const {
return const_cast<CXXDestructorDecl*>(this)->getCanonicalDecl();
}


static bool classof(const Decl *D) { return classofKind(D->getKind()); }
static bool classofKind(Kind K) { return K == CXXDestructor; }
};











class CXXConversionDecl : public CXXMethodDecl {
CXXConversionDecl(ASTContext &C, CXXRecordDecl *RD, SourceLocation StartLoc,
const DeclarationNameInfo &NameInfo, QualType T,
TypeSourceInfo *TInfo, bool isInline, ExplicitSpecifier ES,
ConstexprSpecKind ConstexprKind, SourceLocation EndLocation,
Expr *TrailingRequiresClause = nullptr)
: CXXMethodDecl(CXXConversion, C, RD, StartLoc, NameInfo, T, TInfo,
SC_None, isInline, ConstexprKind, EndLocation,
TrailingRequiresClause),
ExplicitSpec(ES) {}
void anchor() override;

ExplicitSpecifier ExplicitSpec;

public:
friend class ASTDeclReader;
friend class ASTDeclWriter;

static CXXConversionDecl *
Create(ASTContext &C, CXXRecordDecl *RD, SourceLocation StartLoc,
const DeclarationNameInfo &NameInfo, QualType T, TypeSourceInfo *TInfo,
bool isInline, ExplicitSpecifier ES, ConstexprSpecKind ConstexprKind,
SourceLocation EndLocation, Expr *TrailingRequiresClause = nullptr);
static CXXConversionDecl *CreateDeserialized(ASTContext &C, unsigned ID);

ExplicitSpecifier getExplicitSpecifier() {
return getCanonicalDecl()->ExplicitSpec;
}

const ExplicitSpecifier getExplicitSpecifier() const {
return getCanonicalDecl()->ExplicitSpec;
}


bool isExplicit() const { return getExplicitSpecifier().isExplicit(); }
void setExplicitSpecifier(ExplicitSpecifier ES) { ExplicitSpec = ES; }


QualType getConversionType() const {
return getType()->castAs<FunctionType>()->getReturnType();
}



bool isLambdaToBlockPointerConversion() const;

CXXConversionDecl *getCanonicalDecl() override {
return cast<CXXConversionDecl>(FunctionDecl::getCanonicalDecl());
}
const CXXConversionDecl *getCanonicalDecl() const {
return const_cast<CXXConversionDecl*>(this)->getCanonicalDecl();
}


static bool classof(const Decl *D) { return classofKind(D->getKind()); }
static bool classofKind(Kind K) { return K == CXXConversion; }
};







class LinkageSpecDecl : public Decl, public DeclContext {
virtual void anchor();


public:




enum LanguageIDs { lang_c = 1, lang_cxx = 2 };

private:

SourceLocation ExternLoc;


SourceLocation RBraceLoc;

LinkageSpecDecl(DeclContext *DC, SourceLocation ExternLoc,
SourceLocation LangLoc, LanguageIDs lang, bool HasBraces);

public:
static LinkageSpecDecl *Create(ASTContext &C, DeclContext *DC,
SourceLocation ExternLoc,
SourceLocation LangLoc, LanguageIDs Lang,
bool HasBraces);
static LinkageSpecDecl *CreateDeserialized(ASTContext &C, unsigned ID);


LanguageIDs getLanguage() const {
return static_cast<LanguageIDs>(LinkageSpecDeclBits.Language);
}


void setLanguage(LanguageIDs L) { LinkageSpecDeclBits.Language = L; }



bool hasBraces() const {
assert(!RBraceLoc.isValid() || LinkageSpecDeclBits.HasBraces);
return LinkageSpecDeclBits.HasBraces;
}

SourceLocation getExternLoc() const { return ExternLoc; }
SourceLocation getRBraceLoc() const { return RBraceLoc; }
void setExternLoc(SourceLocation L) { ExternLoc = L; }
void setRBraceLoc(SourceLocation L) {
RBraceLoc = L;
LinkageSpecDeclBits.HasBraces = RBraceLoc.isValid();
}

SourceLocation getEndLoc() const LLVM_READONLY {
if (hasBraces())
return getRBraceLoc();


return decls_empty() ? getLocation() : decls_begin()->getEndLoc();
}

SourceRange getSourceRange() const override LLVM_READONLY {
return SourceRange(ExternLoc, getEndLoc());
}

static bool classof(const Decl *D) { return classofKind(D->getKind()); }
static bool classofKind(Kind K) { return K == LinkageSpec; }

static DeclContext *castToDeclContext(const LinkageSpecDecl *D) {
return static_cast<DeclContext *>(const_cast<LinkageSpecDecl*>(D));
}

static LinkageSpecDecl *castFromDeclContext(const DeclContext *DC) {
return static_cast<LinkageSpecDecl *>(const_cast<DeclContext*>(DC));
}
};











class UsingDirectiveDecl : public NamedDecl {

SourceLocation UsingLoc;


SourceLocation NamespaceLoc;


NestedNameSpecifierLoc QualifierLoc;


NamedDecl *NominatedNamespace;



DeclContext *CommonAncestor;

UsingDirectiveDecl(DeclContext *DC, SourceLocation UsingLoc,
SourceLocation NamespcLoc,
NestedNameSpecifierLoc QualifierLoc,
SourceLocation IdentLoc,
NamedDecl *Nominated,
DeclContext *CommonAncestor)
: NamedDecl(UsingDirective, DC, IdentLoc, getName()), UsingLoc(UsingLoc),
NamespaceLoc(NamespcLoc), QualifierLoc(QualifierLoc),
NominatedNamespace(Nominated), CommonAncestor(CommonAncestor) {}





static DeclarationName getName() {
return DeclarationName::getUsingDirectiveName();
}

void anchor() override;

public:
friend class ASTDeclReader;


friend class DeclContext;



NestedNameSpecifierLoc getQualifierLoc() const { return QualifierLoc; }



NestedNameSpecifier *getQualifier() const {
return QualifierLoc.getNestedNameSpecifier();
}

NamedDecl *getNominatedNamespaceAsWritten() { return NominatedNamespace; }
const NamedDecl *getNominatedNamespaceAsWritten() const {
return NominatedNamespace;
}


NamespaceDecl *getNominatedNamespace();

const NamespaceDecl *getNominatedNamespace() const {
return const_cast<UsingDirectiveDecl*>(this)->getNominatedNamespace();
}



DeclContext *getCommonAncestor() { return CommonAncestor; }
const DeclContext *getCommonAncestor() const { return CommonAncestor; }


SourceLocation getUsingLoc() const { return UsingLoc; }



SourceLocation getNamespaceKeyLocation() const { return NamespaceLoc; }


SourceLocation getIdentLocation() const { return getLocation(); }

static UsingDirectiveDecl *Create(ASTContext &C, DeclContext *DC,
SourceLocation UsingLoc,
SourceLocation NamespaceLoc,
NestedNameSpecifierLoc QualifierLoc,
SourceLocation IdentLoc,
NamedDecl *Nominated,
DeclContext *CommonAncestor);
static UsingDirectiveDecl *CreateDeserialized(ASTContext &C, unsigned ID);

SourceRange getSourceRange() const override LLVM_READONLY {
return SourceRange(UsingLoc, getLocation());
}

static bool classof(const Decl *D) { return classofKind(D->getKind()); }
static bool classofKind(Kind K) { return K == UsingDirective; }
};








class NamespaceAliasDecl : public NamedDecl,
public Redeclarable<NamespaceAliasDecl> {
friend class ASTDeclReader;


SourceLocation NamespaceLoc;




SourceLocation IdentLoc;


NestedNameSpecifierLoc QualifierLoc;



NamedDecl *Namespace;

NamespaceAliasDecl(ASTContext &C, DeclContext *DC,
SourceLocation NamespaceLoc, SourceLocation AliasLoc,
IdentifierInfo *Alias, NestedNameSpecifierLoc QualifierLoc,
SourceLocation IdentLoc, NamedDecl *Namespace)
: NamedDecl(NamespaceAlias, DC, AliasLoc, Alias), redeclarable_base(C),
NamespaceLoc(NamespaceLoc), IdentLoc(IdentLoc),
QualifierLoc(QualifierLoc), Namespace(Namespace) {}

void anchor() override;

using redeclarable_base = Redeclarable<NamespaceAliasDecl>;

NamespaceAliasDecl *getNextRedeclarationImpl() override;
NamespaceAliasDecl *getPreviousDeclImpl() override;
NamespaceAliasDecl *getMostRecentDeclImpl() override;

public:
static NamespaceAliasDecl *Create(ASTContext &C, DeclContext *DC,
SourceLocation NamespaceLoc,
SourceLocation AliasLoc,
IdentifierInfo *Alias,
NestedNameSpecifierLoc QualifierLoc,
SourceLocation IdentLoc,
NamedDecl *Namespace);

static NamespaceAliasDecl *CreateDeserialized(ASTContext &C, unsigned ID);

using redecl_range = redeclarable_base::redecl_range;
using redecl_iterator = redeclarable_base::redecl_iterator;

using redeclarable_base::redecls_begin;
using redeclarable_base::redecls_end;
using redeclarable_base::redecls;
using redeclarable_base::getPreviousDecl;
using redeclarable_base::getMostRecentDecl;

NamespaceAliasDecl *getCanonicalDecl() override {
return getFirstDecl();
}
const NamespaceAliasDecl *getCanonicalDecl() const {
return getFirstDecl();
}



NestedNameSpecifierLoc getQualifierLoc() const { return QualifierLoc; }



NestedNameSpecifier *getQualifier() const {
return QualifierLoc.getNestedNameSpecifier();
}


NamespaceDecl *getNamespace() {
if (auto *AD = dyn_cast<NamespaceAliasDecl>(Namespace))
return AD->getNamespace();

return cast<NamespaceDecl>(Namespace);
}

const NamespaceDecl *getNamespace() const {
return const_cast<NamespaceAliasDecl *>(this)->getNamespace();
}



SourceLocation getAliasLoc() const { return getLocation(); }


SourceLocation getNamespaceLoc() const { return NamespaceLoc; }


SourceLocation getTargetNameLoc() const { return IdentLoc; }



NamedDecl *getAliasedNamespace() const { return Namespace; }

SourceRange getSourceRange() const override LLVM_READONLY {
return SourceRange(NamespaceLoc, IdentLoc);
}

static bool classof(const Decl *D) { return classofKind(D->getKind()); }
static bool classofKind(Kind K) { return K == NamespaceAlias; }
};



class LifetimeExtendedTemporaryDecl final
: public Decl,
public Mergeable<LifetimeExtendedTemporaryDecl> {
friend class MaterializeTemporaryExpr;
friend class ASTDeclReader;

Stmt *ExprWithTemporary = nullptr;



ValueDecl *ExtendingDecl = nullptr;
unsigned ManglingNumber;

mutable APValue *Value = nullptr;

virtual void anchor();

LifetimeExtendedTemporaryDecl(Expr *Temp, ValueDecl *EDecl, unsigned Mangling)
: Decl(Decl::LifetimeExtendedTemporary, EDecl->getDeclContext(),
EDecl->getLocation()),
ExprWithTemporary(Temp), ExtendingDecl(EDecl),
ManglingNumber(Mangling) {}

LifetimeExtendedTemporaryDecl(EmptyShell)
: Decl(Decl::LifetimeExtendedTemporary, EmptyShell{}) {}

public:
static LifetimeExtendedTemporaryDecl *Create(Expr *Temp, ValueDecl *EDec,
unsigned Mangling) {
return new (EDec->getASTContext(), EDec->getDeclContext())
LifetimeExtendedTemporaryDecl(Temp, EDec, Mangling);
}
static LifetimeExtendedTemporaryDecl *CreateDeserialized(ASTContext &C,
unsigned ID) {
return new (C, ID) LifetimeExtendedTemporaryDecl(EmptyShell{});
}

ValueDecl *getExtendingDecl() { return ExtendingDecl; }
const ValueDecl *getExtendingDecl() const { return ExtendingDecl; }


StorageDuration getStorageDuration() const;






Expr *getTemporaryExpr() { return cast<Expr>(ExprWithTemporary); }
const Expr *getTemporaryExpr() const { return cast<Expr>(ExprWithTemporary); }

unsigned getManglingNumber() const { return ManglingNumber; }



APValue *getOrCreateValue(bool MayCreate) const;

APValue *getValue() const { return Value; }


Stmt::child_range childrenExpr() {
return Stmt::child_range(&ExprWithTemporary, &ExprWithTemporary + 1);
}

Stmt::const_child_range childrenExpr() const {
return Stmt::const_child_range(&ExprWithTemporary, &ExprWithTemporary + 1);
}

static bool classof(const Decl *D) { return classofKind(D->getKind()); }
static bool classofKind(Kind K) {
return K == Decl::LifetimeExtendedTemporary;
}
};




















class UsingShadowDecl : public NamedDecl, public Redeclarable<UsingShadowDecl> {
friend class BaseUsingDecl;


NamedDecl *Underlying = nullptr;



NamedDecl *UsingOrNextShadow = nullptr;

void anchor() override;

using redeclarable_base = Redeclarable<UsingShadowDecl>;

UsingShadowDecl *getNextRedeclarationImpl() override {
return getNextRedeclaration();
}

UsingShadowDecl *getPreviousDeclImpl() override {
return getPreviousDecl();
}

UsingShadowDecl *getMostRecentDeclImpl() override {
return getMostRecentDecl();
}

protected:
UsingShadowDecl(Kind K, ASTContext &C, DeclContext *DC, SourceLocation Loc,
DeclarationName Name, BaseUsingDecl *Introducer,
NamedDecl *Target);
UsingShadowDecl(Kind K, ASTContext &C, EmptyShell);

public:
friend class ASTDeclReader;
friend class ASTDeclWriter;

static UsingShadowDecl *Create(ASTContext &C, DeclContext *DC,
SourceLocation Loc, DeclarationName Name,
BaseUsingDecl *Introducer, NamedDecl *Target) {
return new (C, DC)
UsingShadowDecl(UsingShadow, C, DC, Loc, Name, Introducer, Target);
}

static UsingShadowDecl *CreateDeserialized(ASTContext &C, unsigned ID);

using redecl_range = redeclarable_base::redecl_range;
using redecl_iterator = redeclarable_base::redecl_iterator;

using redeclarable_base::redecls_begin;
using redeclarable_base::redecls_end;
using redeclarable_base::redecls;
using redeclarable_base::getPreviousDecl;
using redeclarable_base::getMostRecentDecl;
using redeclarable_base::isFirstDecl;

UsingShadowDecl *getCanonicalDecl() override {
return getFirstDecl();
}
const UsingShadowDecl *getCanonicalDecl() const {
return getFirstDecl();
}



NamedDecl *getTargetDecl() const { return Underlying; }



void setTargetDecl(NamedDecl *ND) {
assert(ND && "Target decl is null!");
Underlying = ND;


IdentifierNamespace =
ND->getIdentifierNamespace() &
~(IDNS_OrdinaryFriend | IDNS_TagFriend | IDNS_LocalExtern);
}



BaseUsingDecl *getIntroducer() const;



UsingShadowDecl *getNextUsingShadowDecl() const {
return dyn_cast_or_null<UsingShadowDecl>(UsingOrNextShadow);
}

static bool classof(const Decl *D) { return classofKind(D->getKind()); }
static bool classofKind(Kind K) {
return K == Decl::UsingShadow || K == Decl::ConstructorUsingShadow;
}
};




class BaseUsingDecl : public NamedDecl {





llvm::PointerIntPair<UsingShadowDecl *, 1, bool> FirstUsingShadow;

protected:
BaseUsingDecl(Kind DK, DeclContext *DC, SourceLocation L, DeclarationName N)
: NamedDecl(DK, DC, L, N), FirstUsingShadow(nullptr, 0) {}

private:
void anchor() override;

protected:

bool getShadowFlag() const { return FirstUsingShadow.getInt(); }


void setShadowFlag(bool V) { FirstUsingShadow.setInt(V); }

public:
friend class ASTDeclReader;
friend class ASTDeclWriter;



class shadow_iterator {

UsingShadowDecl *Current = nullptr;

public:
using value_type = UsingShadowDecl *;
using reference = UsingShadowDecl *;
using pointer = UsingShadowDecl *;
using iterator_category = std::forward_iterator_tag;
using difference_type = std::ptrdiff_t;

shadow_iterator() = default;
explicit shadow_iterator(UsingShadowDecl *C) : Current(C) {}

reference operator*() const { return Current; }
pointer operator->() const { return Current; }

shadow_iterator &operator++() {
Current = Current->getNextUsingShadowDecl();
return *this;
}

shadow_iterator operator++(int) {
shadow_iterator tmp(*this);
++(*this);
return tmp;
}

friend bool operator==(shadow_iterator x, shadow_iterator y) {
return x.Current == y.Current;
}
friend bool operator!=(shadow_iterator x, shadow_iterator y) {
return x.Current != y.Current;
}
};

using shadow_range = llvm::iterator_range<shadow_iterator>;

shadow_range shadows() const {
return shadow_range(shadow_begin(), shadow_end());
}

shadow_iterator shadow_begin() const {
return shadow_iterator(FirstUsingShadow.getPointer());
}

shadow_iterator shadow_end() const { return shadow_iterator(); }



unsigned shadow_size() const {
return std::distance(shadow_begin(), shadow_end());
}

void addShadowDecl(UsingShadowDecl *S);
void removeShadowDecl(UsingShadowDecl *S);

static bool classof(const Decl *D) { return classofKind(D->getKind()); }
static bool classofKind(Kind K) { return K == Using || K == UsingEnum; }
};







class UsingDecl : public BaseUsingDecl, public Mergeable<UsingDecl> {

SourceLocation UsingLocation;


NestedNameSpecifierLoc QualifierLoc;



DeclarationNameLoc DNLoc;

UsingDecl(DeclContext *DC, SourceLocation UL,
NestedNameSpecifierLoc QualifierLoc,
const DeclarationNameInfo &NameInfo, bool HasTypenameKeyword)
: BaseUsingDecl(Using, DC, NameInfo.getLoc(), NameInfo.getName()),
UsingLocation(UL), QualifierLoc(QualifierLoc),
DNLoc(NameInfo.getInfo()) {
setShadowFlag(HasTypenameKeyword);
}

void anchor() override;

public:
friend class ASTDeclReader;
friend class ASTDeclWriter;


SourceLocation getUsingLoc() const { return UsingLocation; }


void setUsingLoc(SourceLocation L) { UsingLocation = L; }



NestedNameSpecifierLoc getQualifierLoc() const { return QualifierLoc; }


NestedNameSpecifier *getQualifier() const {
return QualifierLoc.getNestedNameSpecifier();
}

DeclarationNameInfo getNameInfo() const {
return DeclarationNameInfo(getDeclName(), getLocation(), DNLoc);
}


bool isAccessDeclaration() const { return UsingLocation.isInvalid(); }


bool hasTypename() const { return getShadowFlag(); }


void setTypename(bool TN) { setShadowFlag(TN); }

static UsingDecl *Create(ASTContext &C, DeclContext *DC,
SourceLocation UsingL,
NestedNameSpecifierLoc QualifierLoc,
const DeclarationNameInfo &NameInfo,
bool HasTypenameKeyword);

static UsingDecl *CreateDeserialized(ASTContext &C, unsigned ID);

SourceRange getSourceRange() const override LLVM_READONLY;


UsingDecl *getCanonicalDecl() override {
return cast<UsingDecl>(getFirstDecl());
}
const UsingDecl *getCanonicalDecl() const {
return cast<UsingDecl>(getFirstDecl());
}

static bool classof(const Decl *D) { return classofKind(D->getKind()); }
static bool classofKind(Kind K) { return K == Using; }
};











class ConstructorUsingShadowDecl final : public UsingShadowDecl {



ConstructorUsingShadowDecl *NominatedBaseClassShadowDecl = nullptr;





ConstructorUsingShadowDecl *ConstructedBaseClassShadowDecl = nullptr;




unsigned IsVirtual : 1;

ConstructorUsingShadowDecl(ASTContext &C, DeclContext *DC, SourceLocation Loc,
UsingDecl *Using, NamedDecl *Target,
bool TargetInVirtualBase)
: UsingShadowDecl(ConstructorUsingShadow, C, DC, Loc,
Using->getDeclName(), Using,
Target->getUnderlyingDecl()),
NominatedBaseClassShadowDecl(
dyn_cast<ConstructorUsingShadowDecl>(Target)),
ConstructedBaseClassShadowDecl(NominatedBaseClassShadowDecl),
IsVirtual(TargetInVirtualBase) {



if (NominatedBaseClassShadowDecl &&
NominatedBaseClassShadowDecl->constructsVirtualBase()) {
ConstructedBaseClassShadowDecl =
NominatedBaseClassShadowDecl->ConstructedBaseClassShadowDecl;
IsVirtual = true;
}
}

ConstructorUsingShadowDecl(ASTContext &C, EmptyShell Empty)
: UsingShadowDecl(ConstructorUsingShadow, C, Empty), IsVirtual(false) {}

void anchor() override;

public:
friend class ASTDeclReader;
friend class ASTDeclWriter;

static ConstructorUsingShadowDecl *Create(ASTContext &C, DeclContext *DC,
SourceLocation Loc,
UsingDecl *Using, NamedDecl *Target,
bool IsVirtual);
static ConstructorUsingShadowDecl *CreateDeserialized(ASTContext &C,
unsigned ID);



UsingDecl *getIntroducer() const {
return cast<UsingDecl>(UsingShadowDecl::getIntroducer());
}




const CXXRecordDecl *getParent() const {
return cast<CXXRecordDecl>(getDeclContext());
}
CXXRecordDecl *getParent() {
return cast<CXXRecordDecl>(getDeclContext());
}





ConstructorUsingShadowDecl *getNominatedBaseClassShadowDecl() const {
return NominatedBaseClassShadowDecl;
}



ConstructorUsingShadowDecl *getConstructedBaseClassShadowDecl() const {
return ConstructedBaseClassShadowDecl;
}



CXXRecordDecl *getNominatedBaseClass() const;



CXXRecordDecl *getConstructedBaseClass() const {
return cast<CXXRecordDecl>((ConstructedBaseClassShadowDecl
? ConstructedBaseClassShadowDecl
: getTargetDecl())
->getDeclContext());
}



bool constructsVirtualBase() const {
return IsVirtual;
}

static bool classof(const Decl *D) { return classofKind(D->getKind()); }
static bool classofKind(Kind K) { return K == ConstructorUsingShadow; }
};








class UsingEnumDecl : public BaseUsingDecl, public Mergeable<UsingEnumDecl> {

SourceLocation UsingLocation;


SourceLocation EnumLocation;


EnumDecl *Enum;

UsingEnumDecl(DeclContext *DC, DeclarationName DN, SourceLocation UL,
SourceLocation EL, SourceLocation NL, EnumDecl *ED)
: BaseUsingDecl(UsingEnum, DC, NL, DN), UsingLocation(UL),
EnumLocation(EL), Enum(ED) {}

void anchor() override;

public:
friend class ASTDeclReader;
friend class ASTDeclWriter;


SourceLocation getUsingLoc() const { return UsingLocation; }
void setUsingLoc(SourceLocation L) { UsingLocation = L; }


SourceLocation getEnumLoc() const { return EnumLocation; }
void setEnumLoc(SourceLocation L) { EnumLocation = L; }

public:
EnumDecl *getEnumDecl() const { return Enum; }

static UsingEnumDecl *Create(ASTContext &C, DeclContext *DC,
SourceLocation UsingL, SourceLocation EnumL,
SourceLocation NameL, EnumDecl *ED);

static UsingEnumDecl *CreateDeserialized(ASTContext &C, unsigned ID);

SourceRange getSourceRange() const override LLVM_READONLY;


UsingEnumDecl *getCanonicalDecl() override {
return cast<UsingEnumDecl>(getFirstDecl());
}
const UsingEnumDecl *getCanonicalDecl() const {
return cast<UsingEnumDecl>(getFirstDecl());
}

static bool classof(const Decl *D) { return classofKind(D->getKind()); }
static bool classofKind(Kind K) { return K == UsingEnum; }
};














class UsingPackDecl final
: public NamedDecl, public Mergeable<UsingPackDecl>,
private llvm::TrailingObjects<UsingPackDecl, NamedDecl *> {


NamedDecl *InstantiatedFrom;


unsigned NumExpansions;

UsingPackDecl(DeclContext *DC, NamedDecl *InstantiatedFrom,
ArrayRef<NamedDecl *> UsingDecls)
: NamedDecl(UsingPack, DC,
InstantiatedFrom ? InstantiatedFrom->getLocation()
: SourceLocation(),
InstantiatedFrom ? InstantiatedFrom->getDeclName()
: DeclarationName()),
InstantiatedFrom(InstantiatedFrom), NumExpansions(UsingDecls.size()) {
std::uninitialized_copy(UsingDecls.begin(), UsingDecls.end(),
getTrailingObjects<NamedDecl *>());
}

void anchor() override;

public:
friend class ASTDeclReader;
friend class ASTDeclWriter;
friend TrailingObjects;




NamedDecl *getInstantiatedFromUsingDecl() const { return InstantiatedFrom; }



ArrayRef<NamedDecl *> expansions() const {
return llvm::makeArrayRef(getTrailingObjects<NamedDecl *>(), NumExpansions);
}

static UsingPackDecl *Create(ASTContext &C, DeclContext *DC,
NamedDecl *InstantiatedFrom,
ArrayRef<NamedDecl *> UsingDecls);

static UsingPackDecl *CreateDeserialized(ASTContext &C, unsigned ID,
unsigned NumExpansions);

SourceRange getSourceRange() const override LLVM_READONLY {
return InstantiatedFrom->getSourceRange();
}

UsingPackDecl *getCanonicalDecl() override { return getFirstDecl(); }
const UsingPackDecl *getCanonicalDecl() const { return getFirstDecl(); }

static bool classof(const Decl *D) { return classofKind(D->getKind()); }
static bool classofKind(Kind K) { return K == UsingPack; }
};












class UnresolvedUsingValueDecl : public ValueDecl,
public Mergeable<UnresolvedUsingValueDecl> {

SourceLocation UsingLocation;


SourceLocation EllipsisLoc;


NestedNameSpecifierLoc QualifierLoc;



DeclarationNameLoc DNLoc;

UnresolvedUsingValueDecl(DeclContext *DC, QualType Ty,
SourceLocation UsingLoc,
NestedNameSpecifierLoc QualifierLoc,
const DeclarationNameInfo &NameInfo,
SourceLocation EllipsisLoc)
: ValueDecl(UnresolvedUsingValue, DC,
NameInfo.getLoc(), NameInfo.getName(), Ty),
UsingLocation(UsingLoc), EllipsisLoc(EllipsisLoc),
QualifierLoc(QualifierLoc), DNLoc(NameInfo.getInfo()) {}

void anchor() override;

public:
friend class ASTDeclReader;
friend class ASTDeclWriter;


SourceLocation getUsingLoc() const { return UsingLocation; }


void setUsingLoc(SourceLocation L) { UsingLocation = L; }


bool isAccessDeclaration() const { return UsingLocation.isInvalid(); }



NestedNameSpecifierLoc getQualifierLoc() const { return QualifierLoc; }


NestedNameSpecifier *getQualifier() const {
return QualifierLoc.getNestedNameSpecifier();
}

DeclarationNameInfo getNameInfo() const {
return DeclarationNameInfo(getDeclName(), getLocation(), DNLoc);
}


bool isPackExpansion() const {
return EllipsisLoc.isValid();
}


SourceLocation getEllipsisLoc() const {
return EllipsisLoc;
}

static UnresolvedUsingValueDecl *
Create(ASTContext &C, DeclContext *DC, SourceLocation UsingLoc,
NestedNameSpecifierLoc QualifierLoc,
const DeclarationNameInfo &NameInfo, SourceLocation EllipsisLoc);

static UnresolvedUsingValueDecl *
CreateDeserialized(ASTContext &C, unsigned ID);

SourceRange getSourceRange() const override LLVM_READONLY;


UnresolvedUsingValueDecl *getCanonicalDecl() override {
return getFirstDecl();
}
const UnresolvedUsingValueDecl *getCanonicalDecl() const {
return getFirstDecl();
}

static bool classof(const Decl *D) { return classofKind(D->getKind()); }
static bool classofKind(Kind K) { return K == UnresolvedUsingValue; }
};












class UnresolvedUsingTypenameDecl
: public TypeDecl,
public Mergeable<UnresolvedUsingTypenameDecl> {
friend class ASTDeclReader;


SourceLocation TypenameLocation;


SourceLocation EllipsisLoc;


NestedNameSpecifierLoc QualifierLoc;

UnresolvedUsingTypenameDecl(DeclContext *DC, SourceLocation UsingLoc,
SourceLocation TypenameLoc,
NestedNameSpecifierLoc QualifierLoc,
SourceLocation TargetNameLoc,
IdentifierInfo *TargetName,
SourceLocation EllipsisLoc)
: TypeDecl(UnresolvedUsingTypename, DC, TargetNameLoc, TargetName,
UsingLoc),
TypenameLocation(TypenameLoc), EllipsisLoc(EllipsisLoc),
QualifierLoc(QualifierLoc) {}

void anchor() override;

public:

SourceLocation getUsingLoc() const { return getBeginLoc(); }


SourceLocation getTypenameLoc() const { return TypenameLocation; }



NestedNameSpecifierLoc getQualifierLoc() const { return QualifierLoc; }


NestedNameSpecifier *getQualifier() const {
return QualifierLoc.getNestedNameSpecifier();
}

DeclarationNameInfo getNameInfo() const {
return DeclarationNameInfo(getDeclName(), getLocation());
}


bool isPackExpansion() const {
return EllipsisLoc.isValid();
}


SourceLocation getEllipsisLoc() const {
return EllipsisLoc;
}

static UnresolvedUsingTypenameDecl *
Create(ASTContext &C, DeclContext *DC, SourceLocation UsingLoc,
SourceLocation TypenameLoc, NestedNameSpecifierLoc QualifierLoc,
SourceLocation TargetNameLoc, DeclarationName TargetName,
SourceLocation EllipsisLoc);

static UnresolvedUsingTypenameDecl *
CreateDeserialized(ASTContext &C, unsigned ID);


UnresolvedUsingTypenameDecl *getCanonicalDecl() override {
return getFirstDecl();
}
const UnresolvedUsingTypenameDecl *getCanonicalDecl() const {
return getFirstDecl();
}

static bool classof(const Decl *D) { return classofKind(D->getKind()); }
static bool classofKind(Kind K) { return K == UnresolvedUsingTypename; }
};






class UnresolvedUsingIfExistsDecl final : public NamedDecl {
UnresolvedUsingIfExistsDecl(DeclContext *DC, SourceLocation Loc,
DeclarationName Name);

void anchor() override;

public:
static UnresolvedUsingIfExistsDecl *Create(ASTContext &Ctx, DeclContext *DC,
SourceLocation Loc,
DeclarationName Name);
static UnresolvedUsingIfExistsDecl *CreateDeserialized(ASTContext &Ctx,
unsigned ID);

static bool classof(const Decl *D) { return classofKind(D->getKind()); }
static bool classofKind(Kind K) { return K == Decl::UnresolvedUsingIfExists; }
};


class StaticAssertDecl : public Decl {
llvm::PointerIntPair<Expr *, 1, bool> AssertExprAndFailed;
StringLiteral *Message;
SourceLocation RParenLoc;

StaticAssertDecl(DeclContext *DC, SourceLocation StaticAssertLoc,
Expr *AssertExpr, StringLiteral *Message,
SourceLocation RParenLoc, bool Failed)
: Decl(StaticAssert, DC, StaticAssertLoc),
AssertExprAndFailed(AssertExpr, Failed), Message(Message),
RParenLoc(RParenLoc) {}

virtual void anchor();

public:
friend class ASTDeclReader;

static StaticAssertDecl *Create(ASTContext &C, DeclContext *DC,
SourceLocation StaticAssertLoc,
Expr *AssertExpr, StringLiteral *Message,
SourceLocation RParenLoc, bool Failed);
static StaticAssertDecl *CreateDeserialized(ASTContext &C, unsigned ID);

Expr *getAssertExpr() { return AssertExprAndFailed.getPointer(); }
const Expr *getAssertExpr() const { return AssertExprAndFailed.getPointer(); }

StringLiteral *getMessage() { return Message; }
const StringLiteral *getMessage() const { return Message; }

bool isFailed() const { return AssertExprAndFailed.getInt(); }

SourceLocation getRParenLoc() const { return RParenLoc; }

SourceRange getSourceRange() const override LLVM_READONLY {
return SourceRange(getLocation(), getRParenLoc());
}

static bool classof(const Decl *D) { return classofKind(D->getKind()); }
static bool classofKind(Kind K) { return K == StaticAssert; }
};









class BindingDecl : public ValueDecl {

ValueDecl *Decomp;




Expr *Binding = nullptr;

BindingDecl(DeclContext *DC, SourceLocation IdLoc, IdentifierInfo *Id)
: ValueDecl(Decl::Binding, DC, IdLoc, Id, QualType()) {}

void anchor() override;

public:
friend class ASTDeclReader;

static BindingDecl *Create(ASTContext &C, DeclContext *DC,
SourceLocation IdLoc, IdentifierInfo *Id);
static BindingDecl *CreateDeserialized(ASTContext &C, unsigned ID);




Expr *getBinding() const { return Binding; }



ValueDecl *getDecomposedDecl() const { return Decomp; }



VarDecl *getHoldingVar() const;




void setBinding(QualType DeclaredType, Expr *Binding) {
setType(DeclaredType);
this->Binding = Binding;
}


void setDecomposedDecl(ValueDecl *Decomposed) { Decomp = Decomposed; }

static bool classof(const Decl *D) { return classofKind(D->getKind()); }
static bool classofKind(Kind K) { return K == Decl::Binding; }
};









class DecompositionDecl final
: public VarDecl,
private llvm::TrailingObjects<DecompositionDecl, BindingDecl *> {

unsigned NumBindings;

DecompositionDecl(ASTContext &C, DeclContext *DC, SourceLocation StartLoc,
SourceLocation LSquareLoc, QualType T,
TypeSourceInfo *TInfo, StorageClass SC,
ArrayRef<BindingDecl *> Bindings)
: VarDecl(Decomposition, C, DC, StartLoc, LSquareLoc, nullptr, T, TInfo,
SC),
NumBindings(Bindings.size()) {
std::uninitialized_copy(Bindings.begin(), Bindings.end(),
getTrailingObjects<BindingDecl *>());
for (auto *B : Bindings)
B->setDecomposedDecl(this);
}

void anchor() override;

public:
friend class ASTDeclReader;
friend TrailingObjects;

static DecompositionDecl *Create(ASTContext &C, DeclContext *DC,
SourceLocation StartLoc,
SourceLocation LSquareLoc,
QualType T, TypeSourceInfo *TInfo,
StorageClass S,
ArrayRef<BindingDecl *> Bindings);
static DecompositionDecl *CreateDeserialized(ASTContext &C, unsigned ID,
unsigned NumBindings);

ArrayRef<BindingDecl *> bindings() const {
return llvm::makeArrayRef(getTrailingObjects<BindingDecl *>(), NumBindings);
}

void printName(raw_ostream &os) const override;

static bool classof(const Decl *D) { return classofKind(D->getKind()); }
static bool classofKind(Kind K) { return K == Decomposition; }
};




























class MSPropertyDecl : public DeclaratorDecl {
IdentifierInfo *GetterId, *SetterId;

MSPropertyDecl(DeclContext *DC, SourceLocation L, DeclarationName N,
QualType T, TypeSourceInfo *TInfo, SourceLocation StartL,
IdentifierInfo *Getter, IdentifierInfo *Setter)
: DeclaratorDecl(MSProperty, DC, L, N, T, TInfo, StartL),
GetterId(Getter), SetterId(Setter) {}

void anchor() override;
public:
friend class ASTDeclReader;

static MSPropertyDecl *Create(ASTContext &C, DeclContext *DC,
SourceLocation L, DeclarationName N, QualType T,
TypeSourceInfo *TInfo, SourceLocation StartL,
IdentifierInfo *Getter, IdentifierInfo *Setter);
static MSPropertyDecl *CreateDeserialized(ASTContext &C, unsigned ID);

static bool classof(const Decl *D) { return D->getKind() == MSProperty; }

bool hasGetter() const { return GetterId != nullptr; }
IdentifierInfo* getGetterId() const { return GetterId; }
bool hasSetter() const { return SetterId != nullptr; }
IdentifierInfo* getSetterId() const { return SetterId; }
};



struct MSGuidDeclParts {

uint32_t Part1;

uint16_t Part2;

uint16_t Part3;

uint8_t Part4And5[8];

uint64_t getPart4And5AsUint64() const {
uint64_t Val;
memcpy(&Val, &Part4And5, sizeof(Part4And5));
return Val;
}
};







class MSGuidDecl : public ValueDecl,
public Mergeable<MSGuidDecl>,
public llvm::FoldingSetNode {
public:
using Parts = MSGuidDeclParts;

private:

Parts PartVal;



mutable APValue APVal;

void anchor() override;

MSGuidDecl(DeclContext *DC, QualType T, Parts P);

static MSGuidDecl *Create(const ASTContext &C, QualType T, Parts P);
static MSGuidDecl *CreateDeserialized(ASTContext &C, unsigned ID);


friend class ASTContext;
friend class ASTReader;
friend class ASTDeclReader;

public:

void printName(llvm::raw_ostream &OS) const override;


Parts getParts() const { return PartVal; }




APValue &getAsAPValue() const;

static void Profile(llvm::FoldingSetNodeID &ID, Parts P) {
ID.AddInteger(P.Part1);
ID.AddInteger(P.Part2);
ID.AddInteger(P.Part3);
ID.AddInteger(P.getPart4And5AsUint64());
}
void Profile(llvm::FoldingSetNodeID &ID) { Profile(ID, PartVal); }

static bool classof(const Decl *D) { return classofKind(D->getKind()); }
static bool classofKind(Kind K) { return K == Decl::MSGuid; }
};



const StreamingDiagnostic &operator<<(const StreamingDiagnostic &DB,
AccessSpecifier AS);

}

#endif
