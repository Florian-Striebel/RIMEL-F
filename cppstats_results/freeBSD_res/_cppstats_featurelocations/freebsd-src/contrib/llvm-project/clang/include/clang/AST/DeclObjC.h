











#if !defined(LLVM_CLANG_AST_DECLOBJC_H)
#define LLVM_CLANG_AST_DECLOBJC_H

#include "clang/AST/Decl.h"
#include "clang/AST/DeclBase.h"
#include "clang/AST/DeclObjCCommon.h"
#include "clang/AST/ExternalASTSource.h"
#include "clang/AST/Redeclarable.h"
#include "clang/AST/SelectorLocationsKind.h"
#include "clang/AST/Type.h"
#include "clang/Basic/IdentifierTable.h"
#include "clang/Basic/LLVM.h"
#include "clang/Basic/SourceLocation.h"
#include "clang/Basic/Specifiers.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/DenseSet.h"
#include "llvm/ADT/None.h"
#include "llvm/ADT/PointerIntPair.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/iterator_range.h"
#include "llvm/Support/Compiler.h"
#include "llvm/Support/TrailingObjects.h"
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <string>
#include <utility>

namespace clang {

class ASTContext;
class CompoundStmt;
class CXXCtorInitializer;
class Expr;
class ObjCCategoryDecl;
class ObjCCategoryImplDecl;
class ObjCImplementationDecl;
class ObjCInterfaceDecl;
class ObjCIvarDecl;
class ObjCPropertyDecl;
class ObjCPropertyImplDecl;
class ObjCProtocolDecl;
class Stmt;

class ObjCListBase {
protected:

void **List = nullptr;
unsigned NumElts = 0;

public:
ObjCListBase() = default;
ObjCListBase(const ObjCListBase &) = delete;
ObjCListBase &operator=(const ObjCListBase &) = delete;

unsigned size() const { return NumElts; }
bool empty() const { return NumElts == 0; }

protected:
void set(void *const* InList, unsigned Elts, ASTContext &Ctx);
};





template <typename T>
class ObjCList : public ObjCListBase {
public:
void set(T* const* InList, unsigned Elts, ASTContext &Ctx) {
ObjCListBase::set(reinterpret_cast<void*const*>(InList), Elts, Ctx);
}

using iterator = T* const *;

iterator begin() const { return (iterator)List; }
iterator end() const { return (iterator)List+NumElts; }

T* operator[](unsigned Idx) const {
assert(Idx < NumElts && "Invalid access");
return (T*)List[Idx];
}
};



class ObjCProtocolList : public ObjCList<ObjCProtocolDecl> {
SourceLocation *Locations = nullptr;

using ObjCList<ObjCProtocolDecl>::set;

public:
ObjCProtocolList() = default;

using loc_iterator = const SourceLocation *;

loc_iterator loc_begin() const { return Locations; }
loc_iterator loc_end() const { return Locations + size(); }

void set(ObjCProtocolDecl* const* InList, unsigned Elts,
const SourceLocation *Locs, ASTContext &Ctx);
};





















class ObjCMethodDecl : public NamedDecl, public DeclContext {



public:
enum ImplementationControl { None, Required, Optional };

private:

QualType MethodDeclType;


TypeSourceInfo *ReturnTInfo;



void *ParamsAndSelLocs = nullptr;
unsigned NumParams = 0;


SourceLocation DeclEndLoc;


LazyDeclStmtPtr Body;



ImplicitParamDecl *SelfDecl = nullptr;



ImplicitParamDecl *CmdDecl = nullptr;

ObjCMethodDecl(SourceLocation beginLoc, SourceLocation endLoc,
Selector SelInfo, QualType T, TypeSourceInfo *ReturnTInfo,
DeclContext *contextDecl, bool isInstance = true,
bool isVariadic = false, bool isPropertyAccessor = false,
bool isSynthesizedAccessorStub = false,
bool isImplicitlyDeclared = false, bool isDefined = false,
ImplementationControl impControl = None,
bool HasRelatedResultType = false);

SelectorLocationsKind getSelLocsKind() const {
return static_cast<SelectorLocationsKind>(ObjCMethodDeclBits.SelLocsKind);
}

void setSelLocsKind(SelectorLocationsKind Kind) {
ObjCMethodDeclBits.SelLocsKind = Kind;
}

bool hasStandardSelLocs() const {
return getSelLocsKind() != SelLoc_NonStandard;
}



SourceLocation *getStoredSelLocs() {
return reinterpret_cast<SourceLocation *>(getParams() + NumParams);
}
const SourceLocation *getStoredSelLocs() const {
return reinterpret_cast<const SourceLocation *>(getParams() + NumParams);
}



ParmVarDecl **getParams() {
return reinterpret_cast<ParmVarDecl **>(ParamsAndSelLocs);
}
const ParmVarDecl *const *getParams() const {
return reinterpret_cast<const ParmVarDecl *const *>(ParamsAndSelLocs);
}



unsigned getNumStoredSelLocs() const {
if (hasStandardSelLocs())
return 0;
return getNumSelectorLocs();
}

void setParamsAndSelLocs(ASTContext &C,
ArrayRef<ParmVarDecl*> Params,
ArrayRef<SourceLocation> SelLocs);




ObjCMethodDecl *getNextRedeclarationImpl() override;

public:
friend class ASTDeclReader;
friend class ASTDeclWriter;

static ObjCMethodDecl *
Create(ASTContext &C, SourceLocation beginLoc, SourceLocation endLoc,
Selector SelInfo, QualType T, TypeSourceInfo *ReturnTInfo,
DeclContext *contextDecl, bool isInstance = true,
bool isVariadic = false, bool isPropertyAccessor = false,
bool isSynthesizedAccessorStub = false,
bool isImplicitlyDeclared = false, bool isDefined = false,
ImplementationControl impControl = None,
bool HasRelatedResultType = false);

static ObjCMethodDecl *CreateDeserialized(ASTContext &C, unsigned ID);

ObjCMethodDecl *getCanonicalDecl() override;
const ObjCMethodDecl *getCanonicalDecl() const {
return const_cast<ObjCMethodDecl*>(this)->getCanonicalDecl();
}

ObjCDeclQualifier getObjCDeclQualifier() const {
return static_cast<ObjCDeclQualifier>(ObjCMethodDeclBits.objcDeclQualifier);
}

void setObjCDeclQualifier(ObjCDeclQualifier QV) {
ObjCMethodDeclBits.objcDeclQualifier = QV;
}



bool hasRelatedResultType() const {
return ObjCMethodDeclBits.RelatedResultType;
}


void setRelatedResultType(bool RRT = true) {
ObjCMethodDeclBits.RelatedResultType = RRT;
}


bool isRedeclaration() const { return ObjCMethodDeclBits.IsRedeclaration; }
void setIsRedeclaration(bool RD) { ObjCMethodDeclBits.IsRedeclaration = RD; }
void setAsRedeclaration(const ObjCMethodDecl *PrevMethod);


bool hasRedeclaration() const { return ObjCMethodDeclBits.HasRedeclaration; }
void setHasRedeclaration(bool HRD) const {
ObjCMethodDeclBits.HasRedeclaration = HRD;
}




SourceLocation getDeclaratorEndLoc() const { return DeclEndLoc; }


SourceLocation getBeginLoc() const LLVM_READONLY { return getLocation(); }
SourceLocation getEndLoc() const LLVM_READONLY;
SourceRange getSourceRange() const override LLVM_READONLY {
return SourceRange(getLocation(), getEndLoc());
}

SourceLocation getSelectorStartLoc() const {
if (isImplicit())
return getBeginLoc();
return getSelectorLoc(0);
}

SourceLocation getSelectorLoc(unsigned Index) const {
assert(Index < getNumSelectorLocs() && "Index out of range!");
if (hasStandardSelLocs())
return getStandardSelectorLoc(Index, getSelector(),
getSelLocsKind() == SelLoc_StandardWithSpace,
parameters(),
DeclEndLoc);
return getStoredSelLocs()[Index];
}

void getSelectorLocs(SmallVectorImpl<SourceLocation> &SelLocs) const;

unsigned getNumSelectorLocs() const {
if (isImplicit())
return 0;
Selector Sel = getSelector();
if (Sel.isUnarySelector())
return 1;
return Sel.getNumArgs();
}

ObjCInterfaceDecl *getClassInterface();
const ObjCInterfaceDecl *getClassInterface() const {
return const_cast<ObjCMethodDecl*>(this)->getClassInterface();
}



ObjCCategoryDecl *getCategory();
const ObjCCategoryDecl *getCategory() const {
return const_cast<ObjCMethodDecl*>(this)->getCategory();
}

Selector getSelector() const { return getDeclName().getObjCSelector(); }

QualType getReturnType() const { return MethodDeclType; }
void setReturnType(QualType T) { MethodDeclType = T; }
SourceRange getReturnTypeSourceRange() const;





QualType getSendResultType() const;



QualType getSendResultType(QualType receiverType) const;

TypeSourceInfo *getReturnTypeSourceInfo() const { return ReturnTInfo; }
void setReturnTypeSourceInfo(TypeSourceInfo *TInfo) { ReturnTInfo = TInfo; }


unsigned param_size() const { return NumParams; }

using param_const_iterator = const ParmVarDecl *const *;
using param_iterator = ParmVarDecl *const *;
using param_range = llvm::iterator_range<param_iterator>;
using param_const_range = llvm::iterator_range<param_const_iterator>;

param_const_iterator param_begin() const {
return param_const_iterator(getParams());
}

param_const_iterator param_end() const {
return param_const_iterator(getParams() + NumParams);
}

param_iterator param_begin() { return param_iterator(getParams()); }
param_iterator param_end() { return param_iterator(getParams() + NumParams); }



param_const_iterator sel_param_end() const {
return param_begin() + getSelector().getNumArgs();
}



ArrayRef<ParmVarDecl*> parameters() const {
return llvm::makeArrayRef(const_cast<ParmVarDecl**>(getParams()),
NumParams);
}

ParmVarDecl *getParamDecl(unsigned Idx) {
assert(Idx < NumParams && "Index out of bounds!");
return getParams()[Idx];
}
const ParmVarDecl *getParamDecl(unsigned Idx) const {
return const_cast<ObjCMethodDecl *>(this)->getParamDecl(Idx);
}




void setMethodParams(ASTContext &C,
ArrayRef<ParmVarDecl*> Params,
ArrayRef<SourceLocation> SelLocs = llvm::None);


struct GetTypeFn {
QualType operator()(const ParmVarDecl *PD) const { return PD->getType(); }
};

using param_type_iterator =
llvm::mapped_iterator<param_const_iterator, GetTypeFn>;

param_type_iterator param_type_begin() const {
return llvm::map_iterator(param_begin(), GetTypeFn());
}

param_type_iterator param_type_end() const {
return llvm::map_iterator(param_end(), GetTypeFn());
}





void createImplicitParams(ASTContext &Context, const ObjCInterfaceDecl *ID);



QualType getSelfType(ASTContext &Context, const ObjCInterfaceDecl *OID,
bool &selfIsPseudoStrong, bool &selfIsConsumed) const;

ImplicitParamDecl * getSelfDecl() const { return SelfDecl; }
void setSelfDecl(ImplicitParamDecl *SD) { SelfDecl = SD; }
ImplicitParamDecl * getCmdDecl() const { return CmdDecl; }
void setCmdDecl(ImplicitParamDecl *CD) { CmdDecl = CD; }


ObjCMethodFamily getMethodFamily() const;

bool isInstanceMethod() const { return ObjCMethodDeclBits.IsInstance; }
void setInstanceMethod(bool isInst) {
ObjCMethodDeclBits.IsInstance = isInst;
}

bool isVariadic() const { return ObjCMethodDeclBits.IsVariadic; }
void setVariadic(bool isVar) { ObjCMethodDeclBits.IsVariadic = isVar; }

bool isClassMethod() const { return !isInstanceMethod(); }

bool isPropertyAccessor() const {
return ObjCMethodDeclBits.IsPropertyAccessor;
}

void setPropertyAccessor(bool isAccessor) {
ObjCMethodDeclBits.IsPropertyAccessor = isAccessor;
}

bool isSynthesizedAccessorStub() const {
return ObjCMethodDeclBits.IsSynthesizedAccessorStub;
}

void setSynthesizedAccessorStub(bool isSynthesizedAccessorStub) {
ObjCMethodDeclBits.IsSynthesizedAccessorStub = isSynthesizedAccessorStub;
}

bool isDefined() const { return ObjCMethodDeclBits.IsDefined; }
void setDefined(bool isDefined) { ObjCMethodDeclBits.IsDefined = isDefined; }








bool isOverriding() const { return ObjCMethodDeclBits.IsOverriding; }
void setOverriding(bool IsOver) { ObjCMethodDeclBits.IsOverriding = IsOver; }









void getOverriddenMethods(
SmallVectorImpl<const ObjCMethodDecl *> &Overridden) const;


bool hasSkippedBody() const { return ObjCMethodDeclBits.HasSkippedBody; }
void setHasSkippedBody(bool Skipped = true) {
ObjCMethodDeclBits.HasSkippedBody = Skipped;
}


bool isDirectMethod() const;






const ObjCPropertyDecl *findPropertyDecl(bool CheckOverrides = true) const;


void setDeclImplementation(ImplementationControl ic) {
ObjCMethodDeclBits.DeclImplementation = ic;
}

ImplementationControl getImplementationControl() const {
return ImplementationControl(ObjCMethodDeclBits.DeclImplementation);
}

bool isOptional() const {
return getImplementationControl() == Optional;
}



bool isThisDeclarationADesignatedInitializer() const;







bool isDesignatedInitializerForTheInterface(
const ObjCMethodDecl **InitMethod = nullptr) const;


bool hasBody() const override { return Body.isValid(); }


Stmt *getBody() const override;

void setLazyBody(uint64_t Offset) { Body = Offset; }

CompoundStmt *getCompoundBody() { return (CompoundStmt*)getBody(); }
void setBody(Stmt *B) { Body = B; }


bool isThisDeclarationADefinition() const { return hasBody(); }


bool definedInNSObject(const ASTContext &) const;


static bool classof(const Decl *D) { return classofKind(D->getKind()); }
static bool classofKind(Kind K) { return K == ObjCMethod; }

static DeclContext *castToDeclContext(const ObjCMethodDecl *D) {
return static_cast<DeclContext *>(const_cast<ObjCMethodDecl*>(D));
}

static ObjCMethodDecl *castFromDeclContext(const DeclContext *DC) {
return static_cast<ObjCMethodDecl *>(const_cast<DeclContext*>(DC));
}
};


enum class ObjCTypeParamVariance : uint8_t {

Invariant,



Covariant,



Contravariant,
};













class ObjCTypeParamDecl : public TypedefNameDecl {

unsigned Index : 14;


unsigned Variance : 2;


SourceLocation VarianceLoc;



SourceLocation ColonLoc;

ObjCTypeParamDecl(ASTContext &ctx, DeclContext *dc,
ObjCTypeParamVariance variance, SourceLocation varianceLoc,
unsigned index,
SourceLocation nameLoc, IdentifierInfo *name,
SourceLocation colonLoc, TypeSourceInfo *boundInfo)
: TypedefNameDecl(ObjCTypeParam, ctx, dc, nameLoc, nameLoc, name,
boundInfo),
Index(index), Variance(static_cast<unsigned>(variance)),
VarianceLoc(varianceLoc), ColonLoc(colonLoc) {}

void anchor() override;

public:
friend class ASTDeclReader;
friend class ASTDeclWriter;

static ObjCTypeParamDecl *Create(ASTContext &ctx, DeclContext *dc,
ObjCTypeParamVariance variance,
SourceLocation varianceLoc,
unsigned index,
SourceLocation nameLoc,
IdentifierInfo *name,
SourceLocation colonLoc,
TypeSourceInfo *boundInfo);
static ObjCTypeParamDecl *CreateDeserialized(ASTContext &ctx, unsigned ID);

SourceRange getSourceRange() const override LLVM_READONLY;


ObjCTypeParamVariance getVariance() const {
return static_cast<ObjCTypeParamVariance>(Variance);
}


void setVariance(ObjCTypeParamVariance variance) {
Variance = static_cast<unsigned>(variance);
}


SourceLocation getVarianceLoc() const { return VarianceLoc; }


unsigned getIndex() const { return Index; }



bool hasExplicitBound() const { return ColonLoc.isValid(); }



SourceLocation getColonLoc() const { return ColonLoc; }


static bool classof(const Decl *D) { return classofKind(D->getKind()); }
static bool classofKind(Kind K) { return K == ObjCTypeParam; }
};








class ObjCTypeParamList final
: private llvm::TrailingObjects<ObjCTypeParamList, ObjCTypeParamDecl *> {

SourceRange Brackets;

unsigned NumParams;

ObjCTypeParamList(SourceLocation lAngleLoc,
ArrayRef<ObjCTypeParamDecl *> typeParams,
SourceLocation rAngleLoc);

public:
friend TrailingObjects;


static ObjCTypeParamList *create(ASTContext &ctx,
SourceLocation lAngleLoc,
ArrayRef<ObjCTypeParamDecl *> typeParams,
SourceLocation rAngleLoc);


using iterator = ObjCTypeParamDecl **;

iterator begin() { return getTrailingObjects<ObjCTypeParamDecl *>(); }

iterator end() { return begin() + size(); }


unsigned size() const { return NumParams; }


using const_iterator = ObjCTypeParamDecl * const *;

const_iterator begin() const {
return getTrailingObjects<ObjCTypeParamDecl *>();
}

const_iterator end() const {
return begin() + size();
}

ObjCTypeParamDecl *front() const {
assert(size() > 0 && "empty Objective-C type parameter list");
return *begin();
}

ObjCTypeParamDecl *back() const {
assert(size() > 0 && "empty Objective-C type parameter list");
return *(end() - 1);
}

SourceLocation getLAngleLoc() const { return Brackets.getBegin(); }
SourceLocation getRAngleLoc() const { return Brackets.getEnd(); }
SourceRange getSourceRange() const { return Brackets; }



void gatherDefaultTypeArgs(SmallVectorImpl<QualType> &typeArgs) const;
};

enum class ObjCPropertyQueryKind : uint8_t {
OBJC_PR_query_unknown = 0x00,
OBJC_PR_query_instance,
OBJC_PR_query_class
};







class ObjCPropertyDecl : public NamedDecl {
void anchor() override;

public:
enum SetterKind { Assign, Retain, Copy, Weak };
enum PropertyControl { None, Required, Optional };

private:

SourceLocation AtLoc;


SourceLocation LParenLoc;

QualType DeclType;
TypeSourceInfo *DeclTypeSourceInfo;
unsigned PropertyAttributes : NumObjCPropertyAttrsBits;
unsigned PropertyAttributesAsWritten : NumObjCPropertyAttrsBits;


unsigned PropertyImplementation : 2;


Selector GetterName;


Selector SetterName;


SourceLocation GetterNameLoc;


SourceLocation SetterNameLoc;


ObjCMethodDecl *GetterMethodDecl = nullptr;


ObjCMethodDecl *SetterMethodDecl = nullptr;


ObjCIvarDecl *PropertyIvarDecl = nullptr;

ObjCPropertyDecl(DeclContext *DC, SourceLocation L, IdentifierInfo *Id,
SourceLocation AtLocation, SourceLocation LParenLocation,
QualType T, TypeSourceInfo *TSI, PropertyControl propControl)
: NamedDecl(ObjCProperty, DC, L, Id), AtLoc(AtLocation),
LParenLoc(LParenLocation), DeclType(T), DeclTypeSourceInfo(TSI),
PropertyAttributes(ObjCPropertyAttribute::kind_noattr),
PropertyAttributesAsWritten(ObjCPropertyAttribute::kind_noattr),
PropertyImplementation(propControl), GetterName(Selector()),
SetterName(Selector()) {}

public:
static ObjCPropertyDecl *Create(ASTContext &C, DeclContext *DC,
SourceLocation L,
IdentifierInfo *Id, SourceLocation AtLocation,
SourceLocation LParenLocation,
QualType T,
TypeSourceInfo *TSI,
PropertyControl propControl = None);

static ObjCPropertyDecl *CreateDeserialized(ASTContext &C, unsigned ID);

SourceLocation getAtLoc() const { return AtLoc; }
void setAtLoc(SourceLocation L) { AtLoc = L; }

SourceLocation getLParenLoc() const { return LParenLoc; }
void setLParenLoc(SourceLocation L) { LParenLoc = L; }

TypeSourceInfo *getTypeSourceInfo() const { return DeclTypeSourceInfo; }

QualType getType() const { return DeclType; }

void setType(QualType T, TypeSourceInfo *TSI) {
DeclType = T;
DeclTypeSourceInfo = TSI;
}



QualType getUsageType(QualType objectType) const;

ObjCPropertyAttribute::Kind getPropertyAttributes() const {
return ObjCPropertyAttribute::Kind(PropertyAttributes);
}

void setPropertyAttributes(ObjCPropertyAttribute::Kind PRVal) {
PropertyAttributes |= PRVal;
}

void overwritePropertyAttributes(unsigned PRVal) {
PropertyAttributes = PRVal;
}

ObjCPropertyAttribute::Kind getPropertyAttributesAsWritten() const {
return ObjCPropertyAttribute::Kind(PropertyAttributesAsWritten);
}

void setPropertyAttributesAsWritten(ObjCPropertyAttribute::Kind PRVal) {
PropertyAttributesAsWritten = PRVal;
}




bool isReadOnly() const {
return (PropertyAttributes & ObjCPropertyAttribute::kind_readonly);
}


bool isAtomic() const {
return (PropertyAttributes & ObjCPropertyAttribute::kind_atomic);
}


bool isRetaining() const {
return (PropertyAttributes & (ObjCPropertyAttribute::kind_retain |
ObjCPropertyAttribute::kind_strong |
ObjCPropertyAttribute::kind_copy));
}

bool isInstanceProperty() const { return !isClassProperty(); }
bool isClassProperty() const {
return PropertyAttributes & ObjCPropertyAttribute::kind_class;
}
bool isDirectProperty() const;

ObjCPropertyQueryKind getQueryKind() const {
return isClassProperty() ? ObjCPropertyQueryKind::OBJC_PR_query_class :
ObjCPropertyQueryKind::OBJC_PR_query_instance;
}

static ObjCPropertyQueryKind getQueryKind(bool isClassProperty) {
return isClassProperty ? ObjCPropertyQueryKind::OBJC_PR_query_class :
ObjCPropertyQueryKind::OBJC_PR_query_instance;
}




SetterKind getSetterKind() const {
if (PropertyAttributes & ObjCPropertyAttribute::kind_strong)
return getType()->isBlockPointerType() ? Copy : Retain;
if (PropertyAttributes & ObjCPropertyAttribute::kind_retain)
return Retain;
if (PropertyAttributes & ObjCPropertyAttribute::kind_copy)
return Copy;
if (PropertyAttributes & ObjCPropertyAttribute::kind_weak)
return Weak;
return Assign;
}

Selector getGetterName() const { return GetterName; }
SourceLocation getGetterNameLoc() const { return GetterNameLoc; }

void setGetterName(Selector Sel, SourceLocation Loc = SourceLocation()) {
GetterName = Sel;
GetterNameLoc = Loc;
}

Selector getSetterName() const { return SetterName; }
SourceLocation getSetterNameLoc() const { return SetterNameLoc; }

void setSetterName(Selector Sel, SourceLocation Loc = SourceLocation()) {
SetterName = Sel;
SetterNameLoc = Loc;
}

ObjCMethodDecl *getGetterMethodDecl() const { return GetterMethodDecl; }
void setGetterMethodDecl(ObjCMethodDecl *gDecl) { GetterMethodDecl = gDecl; }

ObjCMethodDecl *getSetterMethodDecl() const { return SetterMethodDecl; }
void setSetterMethodDecl(ObjCMethodDecl *gDecl) { SetterMethodDecl = gDecl; }


void setPropertyImplementation(PropertyControl pc) {
PropertyImplementation = pc;
}

PropertyControl getPropertyImplementation() const {
return PropertyControl(PropertyImplementation);
}

bool isOptional() const {
return getPropertyImplementation() == PropertyControl::Optional;
}

void setPropertyIvarDecl(ObjCIvarDecl *Ivar) {
PropertyIvarDecl = Ivar;
}

ObjCIvarDecl *getPropertyIvarDecl() const {
return PropertyIvarDecl;
}

SourceRange getSourceRange() const override LLVM_READONLY {
return SourceRange(AtLoc, getLocation());
}


IdentifierInfo *getDefaultSynthIvarName(ASTContext &Ctx) const;


static ObjCPropertyDecl *findPropertyDecl(const DeclContext *DC,
const IdentifierInfo *propertyID,
ObjCPropertyQueryKind queryKind);

static bool classof(const Decl *D) { return classofKind(D->getKind()); }
static bool classofKind(Kind K) { return K == ObjCProperty; }
};





class ObjCContainerDecl : public NamedDecl, public DeclContext {





SourceRange AtEnd;

void anchor() override;

public:
ObjCContainerDecl(Kind DK, DeclContext *DC, IdentifierInfo *Id,
SourceLocation nameLoc, SourceLocation atStartLoc);


using prop_iterator = specific_decl_iterator<ObjCPropertyDecl>;
using prop_range =
llvm::iterator_range<specific_decl_iterator<ObjCPropertyDecl>>;

prop_range properties() const { return prop_range(prop_begin(), prop_end()); }

prop_iterator prop_begin() const {
return prop_iterator(decls_begin());
}

prop_iterator prop_end() const {
return prop_iterator(decls_end());
}

using instprop_iterator =
filtered_decl_iterator<ObjCPropertyDecl,
&ObjCPropertyDecl::isInstanceProperty>;
using instprop_range = llvm::iterator_range<instprop_iterator>;

instprop_range instance_properties() const {
return instprop_range(instprop_begin(), instprop_end());
}

instprop_iterator instprop_begin() const {
return instprop_iterator(decls_begin());
}

instprop_iterator instprop_end() const {
return instprop_iterator(decls_end());
}

using classprop_iterator =
filtered_decl_iterator<ObjCPropertyDecl,
&ObjCPropertyDecl::isClassProperty>;
using classprop_range = llvm::iterator_range<classprop_iterator>;

classprop_range class_properties() const {
return classprop_range(classprop_begin(), classprop_end());
}

classprop_iterator classprop_begin() const {
return classprop_iterator(decls_begin());
}

classprop_iterator classprop_end() const {
return classprop_iterator(decls_end());
}


using method_iterator = specific_decl_iterator<ObjCMethodDecl>;
using method_range =
llvm::iterator_range<specific_decl_iterator<ObjCMethodDecl>>;

method_range methods() const {
return method_range(meth_begin(), meth_end());
}

method_iterator meth_begin() const {
return method_iterator(decls_begin());
}

method_iterator meth_end() const {
return method_iterator(decls_end());
}

using instmeth_iterator =
filtered_decl_iterator<ObjCMethodDecl,
&ObjCMethodDecl::isInstanceMethod>;
using instmeth_range = llvm::iterator_range<instmeth_iterator>;

instmeth_range instance_methods() const {
return instmeth_range(instmeth_begin(), instmeth_end());
}

instmeth_iterator instmeth_begin() const {
return instmeth_iterator(decls_begin());
}

instmeth_iterator instmeth_end() const {
return instmeth_iterator(decls_end());
}

using classmeth_iterator =
filtered_decl_iterator<ObjCMethodDecl,
&ObjCMethodDecl::isClassMethod>;
using classmeth_range = llvm::iterator_range<classmeth_iterator>;

classmeth_range class_methods() const {
return classmeth_range(classmeth_begin(), classmeth_end());
}

classmeth_iterator classmeth_begin() const {
return classmeth_iterator(decls_begin());
}

classmeth_iterator classmeth_end() const {
return classmeth_iterator(decls_end());
}


ObjCMethodDecl *getMethod(Selector Sel, bool isInstance,
bool AllowHidden = false) const;

ObjCMethodDecl *getInstanceMethod(Selector Sel,
bool AllowHidden = false) const {
return getMethod(Sel, true, AllowHidden);
}

ObjCMethodDecl *getClassMethod(Selector Sel, bool AllowHidden = false) const {
return getMethod(Sel, false, AllowHidden);
}

bool HasUserDeclaredSetterMethod(const ObjCPropertyDecl *P) const;
ObjCIvarDecl *getIvarDecl(IdentifierInfo *Id) const;

ObjCPropertyDecl *
FindPropertyDeclaration(const IdentifierInfo *PropertyId,
ObjCPropertyQueryKind QueryKind) const;

using PropertyMap =
llvm::DenseMap<std::pair<IdentifierInfo *, unsigned>,
ObjCPropertyDecl *>;
using ProtocolPropertySet = llvm::SmallDenseSet<const ObjCProtocolDecl *, 8>;
using PropertyDeclOrder = llvm::SmallVector<ObjCPropertyDecl *, 8>;




virtual void collectPropertiesToImplement(PropertyMap &PM,
PropertyDeclOrder &PO) const {}

SourceLocation getAtStartLoc() const { return ObjCContainerDeclBits.AtStart; }

void setAtStartLoc(SourceLocation Loc) {
ObjCContainerDeclBits.AtStart = Loc;
}


SourceRange getAtEndRange() const { return AtEnd; }

void setAtEndRange(SourceRange atEnd) { AtEnd = atEnd; }

SourceRange getSourceRange() const override LLVM_READONLY {
return SourceRange(getAtStartLoc(), getAtEndRange().getEnd());
}


static bool classof(const Decl *D) { return classofKind(D->getKind()); }

static bool classofKind(Kind K) {
return K >= firstObjCContainer &&
K <= lastObjCContainer;
}

static DeclContext *castToDeclContext(const ObjCContainerDecl *D) {
return static_cast<DeclContext *>(const_cast<ObjCContainerDecl*>(D));
}

static ObjCContainerDecl *castFromDeclContext(const DeclContext *DC) {
return static_cast<ObjCContainerDecl *>(const_cast<DeclContext*>(DC));
}
};


























class ObjCInterfaceDecl : public ObjCContainerDecl
, public Redeclarable<ObjCInterfaceDecl> {
friend class ASTContext;



mutable const Type *TypeForDecl = nullptr;

struct DefinitionData {


ObjCInterfaceDecl *Definition = nullptr;


TypeSourceInfo *SuperClassTInfo = nullptr;


ObjCProtocolList ReferencedProtocols;


ObjCList<ObjCProtocolDecl> AllReferencedProtocols;






ObjCCategoryDecl *CategoryList = nullptr;



ObjCIvarDecl *IvarList = nullptr;



mutable unsigned ExternallyCompleted : 1;



mutable unsigned IvarListMissingImplementation : 1;



unsigned HasDesignatedInitializers : 1;

enum InheritedDesignatedInitializersState {


IDI_Unknown = 0,


IDI_Inherited = 1,


IDI_NotInherited = 2
};


mutable unsigned InheritedDesignatedInitializers : 2;




SourceLocation EndLoc;

DefinitionData()
: ExternallyCompleted(false), IvarListMissingImplementation(true),
HasDesignatedInitializers(false),
InheritedDesignatedInitializers(IDI_Unknown) {}
};


ObjCTypeParamList *TypeParamList = nullptr;






llvm::PointerIntPair<DefinitionData *, 1, bool> Data;

ObjCInterfaceDecl(const ASTContext &C, DeclContext *DC, SourceLocation AtLoc,
IdentifierInfo *Id, ObjCTypeParamList *typeParamList,
SourceLocation CLoc, ObjCInterfaceDecl *PrevDecl,
bool IsInternal);

void anchor() override;

void LoadExternalDefinition() const;

DefinitionData &data() const {
assert(Data.getPointer() && "Declaration has no definition!");
return *Data.getPointer();
}


void allocateDefinitionData();

using redeclarable_base = Redeclarable<ObjCInterfaceDecl>;

ObjCInterfaceDecl *getNextRedeclarationImpl() override {
return getNextRedeclaration();
}

ObjCInterfaceDecl *getPreviousDeclImpl() override {
return getPreviousDecl();
}

ObjCInterfaceDecl *getMostRecentDeclImpl() override {
return getMostRecentDecl();
}

public:
static ObjCInterfaceDecl *Create(const ASTContext &C, DeclContext *DC,
SourceLocation atLoc,
IdentifierInfo *Id,
ObjCTypeParamList *typeParamList,
ObjCInterfaceDecl *PrevDecl,
SourceLocation ClassLoc = SourceLocation(),
bool isInternal = false);

static ObjCInterfaceDecl *CreateDeserialized(const ASTContext &C, unsigned ID);







ObjCTypeParamList *getTypeParamList() const;





void setTypeParamList(ObjCTypeParamList *TPL);



ObjCTypeParamList *getTypeParamListAsWritten() const {
return TypeParamList;
}

SourceRange getSourceRange() const override LLVM_READONLY {
if (isThisDeclarationADefinition())
return ObjCContainerDecl::getSourceRange();

return SourceRange(getAtStartLoc(), getLocation());
}




void setExternallyCompleted();



void setHasDesignatedInitializers();



bool hasDesignatedInitializers() const;



bool declaresOrInheritsDesignatedInitializers() const {
return hasDesignatedInitializers() || inheritsDesignatedInitializers();
}

const ObjCProtocolList &getReferencedProtocols() const {
assert(hasDefinition() && "Caller did not check for forward reference!");
if (data().ExternallyCompleted)
LoadExternalDefinition();

return data().ReferencedProtocols;
}

ObjCImplementationDecl *getImplementation() const;
void setImplementation(ObjCImplementationDecl *ImplD);

ObjCCategoryDecl *FindCategoryDeclaration(IdentifierInfo *CategoryId) const;


ObjCMethodDecl *getCategoryInstanceMethod(Selector Sel) const;
ObjCMethodDecl *getCategoryClassMethod(Selector Sel) const;

ObjCMethodDecl *getCategoryMethod(Selector Sel, bool isInstance) const {
return isInstance ? getCategoryInstanceMethod(Sel)
: getCategoryClassMethod(Sel);
}

using protocol_iterator = ObjCProtocolList::iterator;
using protocol_range = llvm::iterator_range<protocol_iterator>;

protocol_range protocols() const {
return protocol_range(protocol_begin(), protocol_end());
}

protocol_iterator protocol_begin() const {

if (!hasDefinition())
return protocol_iterator();

if (data().ExternallyCompleted)
LoadExternalDefinition();

return data().ReferencedProtocols.begin();
}

protocol_iterator protocol_end() const {

if (!hasDefinition())
return protocol_iterator();

if (data().ExternallyCompleted)
LoadExternalDefinition();

return data().ReferencedProtocols.end();
}

using protocol_loc_iterator = ObjCProtocolList::loc_iterator;
using protocol_loc_range = llvm::iterator_range<protocol_loc_iterator>;

protocol_loc_range protocol_locs() const {
return protocol_loc_range(protocol_loc_begin(), protocol_loc_end());
}

protocol_loc_iterator protocol_loc_begin() const {

if (!hasDefinition())
return protocol_loc_iterator();

if (data().ExternallyCompleted)
LoadExternalDefinition();

return data().ReferencedProtocols.loc_begin();
}

protocol_loc_iterator protocol_loc_end() const {

if (!hasDefinition())
return protocol_loc_iterator();

if (data().ExternallyCompleted)
LoadExternalDefinition();

return data().ReferencedProtocols.loc_end();
}

using all_protocol_iterator = ObjCList<ObjCProtocolDecl>::iterator;
using all_protocol_range = llvm::iterator_range<all_protocol_iterator>;

all_protocol_range all_referenced_protocols() const {
return all_protocol_range(all_referenced_protocol_begin(),
all_referenced_protocol_end());
}

all_protocol_iterator all_referenced_protocol_begin() const {

if (!hasDefinition())
return all_protocol_iterator();

if (data().ExternallyCompleted)
LoadExternalDefinition();

return data().AllReferencedProtocols.empty()
? protocol_begin()
: data().AllReferencedProtocols.begin();
}

all_protocol_iterator all_referenced_protocol_end() const {

if (!hasDefinition())
return all_protocol_iterator();

if (data().ExternallyCompleted)
LoadExternalDefinition();

return data().AllReferencedProtocols.empty()
? protocol_end()
: data().AllReferencedProtocols.end();
}

using ivar_iterator = specific_decl_iterator<ObjCIvarDecl>;
using ivar_range = llvm::iterator_range<specific_decl_iterator<ObjCIvarDecl>>;

ivar_range ivars() const { return ivar_range(ivar_begin(), ivar_end()); }

ivar_iterator ivar_begin() const {
if (const ObjCInterfaceDecl *Def = getDefinition())
return ivar_iterator(Def->decls_begin());


return ivar_iterator();
}

ivar_iterator ivar_end() const {
if (const ObjCInterfaceDecl *Def = getDefinition())
return ivar_iterator(Def->decls_end());


return ivar_iterator();
}

unsigned ivar_size() const {
return std::distance(ivar_begin(), ivar_end());
}

bool ivar_empty() const { return ivar_begin() == ivar_end(); }

ObjCIvarDecl *all_declared_ivar_begin();
const ObjCIvarDecl *all_declared_ivar_begin() const {


return const_cast<ObjCInterfaceDecl *>(this)->all_declared_ivar_begin();
}
void setIvarList(ObjCIvarDecl *ivar) { data().IvarList = ivar; }



void setProtocolList(ObjCProtocolDecl *const* List, unsigned Num,
const SourceLocation *Locs, ASTContext &C) {
data().ReferencedProtocols.set(List, Num, Locs, C);
}



void mergeClassExtensionProtocolList(ObjCProtocolDecl *const* List,
unsigned Num,
ASTContext &C);



StringRef getObjCRuntimeNameAsString() const;






void getDesignatedInitializers(
llvm::SmallVectorImpl<const ObjCMethodDecl *> &Methods) const;










bool
isDesignatedInitializer(Selector Sel,
const ObjCMethodDecl **InitMethod = nullptr) const;



bool isThisDeclarationADefinition() const {
return getDefinition() == this;
}


bool hasDefinition() const {




if (!Data.getOpaqueValue())
getMostRecentDecl();

return Data.getPointer();
}




ObjCInterfaceDecl *getDefinition() {
return hasDefinition()? Data.getPointer()->Definition : nullptr;
}




const ObjCInterfaceDecl *getDefinition() const {
return hasDefinition()? Data.getPointer()->Definition : nullptr;
}



void startDefinition();


const ObjCObjectType *getSuperClassType() const {
if (TypeSourceInfo *TInfo = getSuperClassTInfo())
return TInfo->getType()->castAs<ObjCObjectType>();

return nullptr;
}


TypeSourceInfo *getSuperClassTInfo() const {

if (!hasDefinition())
return nullptr;

if (data().ExternallyCompleted)
LoadExternalDefinition();

return data().SuperClassTInfo;
}



ObjCInterfaceDecl *getSuperClass() const;

void setSuperClass(TypeSourceInfo *superClass) {
data().SuperClassTInfo = superClass;
}






template<bool (*Filter)(ObjCCategoryDecl *)>
class filtered_category_iterator {
ObjCCategoryDecl *Current = nullptr;

void findAcceptableCategory();

public:
using value_type = ObjCCategoryDecl *;
using reference = value_type;
using pointer = value_type;
using difference_type = std::ptrdiff_t;
using iterator_category = std::input_iterator_tag;

filtered_category_iterator() = default;
explicit filtered_category_iterator(ObjCCategoryDecl *Current)
: Current(Current) {
findAcceptableCategory();
}

reference operator*() const { return Current; }
pointer operator->() const { return Current; }

filtered_category_iterator &operator++();

filtered_category_iterator operator++(int) {
filtered_category_iterator Tmp = *this;
++(*this);
return Tmp;
}

friend bool operator==(filtered_category_iterator X,
filtered_category_iterator Y) {
return X.Current == Y.Current;
}

friend bool operator!=(filtered_category_iterator X,
filtered_category_iterator Y) {
return X.Current != Y.Current;
}
};

private:



static bool isVisibleCategory(ObjCCategoryDecl *Cat);

public:


using visible_categories_iterator =
filtered_category_iterator<isVisibleCategory>;

using visible_categories_range =
llvm::iterator_range<visible_categories_iterator>;

visible_categories_range visible_categories() const {
return visible_categories_range(visible_categories_begin(),
visible_categories_end());
}



visible_categories_iterator visible_categories_begin() const {
return visible_categories_iterator(getCategoryListRaw());
}


visible_categories_iterator visible_categories_end() const {
return visible_categories_iterator();
}


bool visible_categories_empty() const {
return visible_categories_begin() == visible_categories_end();
}

private:



static bool isKnownCategory(ObjCCategoryDecl *) { return true; }

public:


using known_categories_iterator = filtered_category_iterator<isKnownCategory>;
using known_categories_range =
llvm::iterator_range<known_categories_iterator>;

known_categories_range known_categories() const {
return known_categories_range(known_categories_begin(),
known_categories_end());
}



known_categories_iterator known_categories_begin() const {
return known_categories_iterator(getCategoryListRaw());
}


known_categories_iterator known_categories_end() const {
return known_categories_iterator();
}


bool known_categories_empty() const {
return known_categories_begin() == known_categories_end();
}

private:



static bool isVisibleExtension(ObjCCategoryDecl *Cat);

public:


using visible_extensions_iterator =
filtered_category_iterator<isVisibleExtension>;

using visible_extensions_range =
llvm::iterator_range<visible_extensions_iterator>;

visible_extensions_range visible_extensions() const {
return visible_extensions_range(visible_extensions_begin(),
visible_extensions_end());
}



visible_extensions_iterator visible_extensions_begin() const {
return visible_extensions_iterator(getCategoryListRaw());
}


visible_extensions_iterator visible_extensions_end() const {
return visible_extensions_iterator();
}


bool visible_extensions_empty() const {
return visible_extensions_begin() == visible_extensions_end();
}

private:



static bool isKnownExtension(ObjCCategoryDecl *Cat);

public:
friend class ASTDeclReader;
friend class ASTDeclWriter;
friend class ASTReader;


using known_extensions_iterator =
filtered_category_iterator<isKnownExtension>;
using known_extensions_range =
llvm::iterator_range<known_extensions_iterator>;

known_extensions_range known_extensions() const {
return known_extensions_range(known_extensions_begin(),
known_extensions_end());
}



known_extensions_iterator known_extensions_begin() const {
return known_extensions_iterator(getCategoryListRaw());
}


known_extensions_iterator known_extensions_end() const {
return known_extensions_iterator();
}


bool known_extensions_empty() const {
return known_extensions_begin() == known_extensions_end();
}



ObjCCategoryDecl* getCategoryListRaw() const {

if (!hasDefinition())
return nullptr;

if (data().ExternallyCompleted)
LoadExternalDefinition();

return data().CategoryList;
}



void setCategoryListRaw(ObjCCategoryDecl *category) {
data().CategoryList = category;
}

ObjCPropertyDecl
*FindPropertyVisibleInPrimaryClass(IdentifierInfo *PropertyId,
ObjCPropertyQueryKind QueryKind) const;

void collectPropertiesToImplement(PropertyMap &PM,
PropertyDeclOrder &PO) const override;



bool isSuperClassOf(const ObjCInterfaceDecl *I) const {

while (I != nullptr) {
if (declaresSameEntity(this, I))
return true;

I = I->getSuperClass();
}
return false;
}



bool isArcWeakrefUnavailable() const;




const ObjCInterfaceDecl *isObjCRequiresPropertyDefs() const;

ObjCIvarDecl *lookupInstanceVariable(IdentifierInfo *IVarName,
ObjCInterfaceDecl *&ClassDeclared);
ObjCIvarDecl *lookupInstanceVariable(IdentifierInfo *IVarName) {
ObjCInterfaceDecl *ClassDeclared;
return lookupInstanceVariable(IVarName, ClassDeclared);
}

ObjCProtocolDecl *lookupNestedProtocol(IdentifierInfo *Name);



ObjCMethodDecl *lookupMethod(Selector Sel, bool isInstance,
bool shallowCategoryLookup = false,
bool followSuper = true,
const ObjCCategoryDecl *C = nullptr) const;


ObjCMethodDecl *lookupInstanceMethod(Selector Sel) const {
return lookupMethod(Sel, true);
}


ObjCMethodDecl *lookupClassMethod(Selector Sel) const {
return lookupMethod(Sel, false);
}

ObjCInterfaceDecl *lookupInheritedClass(const IdentifierInfo *ICName);


ObjCMethodDecl *lookupPrivateMethod(const Selector &Sel,
bool Instance=true) const;

ObjCMethodDecl *lookupPrivateClassMethod(const Selector &Sel) {
return lookupPrivateMethod(Sel, false);
}




ObjCMethodDecl *lookupPropertyAccessor(const Selector Sel,
const ObjCCategoryDecl *Cat,
bool IsClassProperty) const {
return lookupMethod(Sel, !IsClassProperty,
false,
true ,
Cat);
}

SourceLocation getEndOfDefinitionLoc() const {
if (!hasDefinition())
return getLocation();

return data().EndLoc;
}

void setEndOfDefinitionLoc(SourceLocation LE) { data().EndLoc = LE; }


SourceLocation getSuperClassLoc() const;




bool isImplicitInterfaceDecl() const {
return hasDefinition() ? data().Definition->isImplicit() : isImplicit();
}




bool ClassImplementsProtocol(ObjCProtocolDecl *lProto,
bool lookupCategory,
bool RHSIsQualifiedID = false);

using redecl_range = redeclarable_base::redecl_range;
using redecl_iterator = redeclarable_base::redecl_iterator;

using redeclarable_base::redecls_begin;
using redeclarable_base::redecls_end;
using redeclarable_base::redecls;
using redeclarable_base::getPreviousDecl;
using redeclarable_base::getMostRecentDecl;
using redeclarable_base::isFirstDecl;


ObjCInterfaceDecl *getCanonicalDecl() override { return getFirstDecl(); }
const ObjCInterfaceDecl *getCanonicalDecl() const { return getFirstDecl(); }


const Type *getTypeForDecl() const { return TypeForDecl; }
void setTypeForDecl(const Type *TD) const { TypeForDecl = TD; }

static bool classof(const Decl *D) { return classofKind(D->getKind()); }
static bool classofKind(Kind K) { return K == ObjCInterface; }

private:
const ObjCInterfaceDecl *findInterfaceWithDesignatedInitializers() const;
bool inheritsDesignatedInitializers() const;
};
















class ObjCIvarDecl : public FieldDecl {
void anchor() override;

public:
enum AccessControl {
None, Private, Protected, Public, Package
};

private:
ObjCIvarDecl(ObjCContainerDecl *DC, SourceLocation StartLoc,
SourceLocation IdLoc, IdentifierInfo *Id,
QualType T, TypeSourceInfo *TInfo, AccessControl ac, Expr *BW,
bool synthesized)
: FieldDecl(ObjCIvar, DC, StartLoc, IdLoc, Id, T, TInfo, BW,
false, ICIS_NoInit),
DeclAccess(ac), Synthesized(synthesized) {}

public:
static ObjCIvarDecl *Create(ASTContext &C, ObjCContainerDecl *DC,
SourceLocation StartLoc, SourceLocation IdLoc,
IdentifierInfo *Id, QualType T,
TypeSourceInfo *TInfo,
AccessControl ac, Expr *BW = nullptr,
bool synthesized=false);

static ObjCIvarDecl *CreateDeserialized(ASTContext &C, unsigned ID);





const ObjCInterfaceDecl *getContainingInterface() const;

ObjCIvarDecl *getNextIvar() { return NextIvar; }
const ObjCIvarDecl *getNextIvar() const { return NextIvar; }
void setNextIvar(ObjCIvarDecl *ivar) { NextIvar = ivar; }

void setAccessControl(AccessControl ac) { DeclAccess = ac; }

AccessControl getAccessControl() const { return AccessControl(DeclAccess); }

AccessControl getCanonicalAccessControl() const {
return DeclAccess == None ? Protected : AccessControl(DeclAccess);
}

void setSynthesize(bool synth) { Synthesized = synth; }
bool getSynthesize() const { return Synthesized; }



QualType getUsageType(QualType objectType) const;


static bool classof(const Decl *D) { return classofKind(D->getKind()); }
static bool classofKind(Kind K) { return K == ObjCIvar; }

private:


ObjCIvarDecl *NextIvar = nullptr;


unsigned DeclAccess : 3;
unsigned Synthesized : 1;
};


class ObjCAtDefsFieldDecl : public FieldDecl {
ObjCAtDefsFieldDecl(DeclContext *DC, SourceLocation StartLoc,
SourceLocation IdLoc, IdentifierInfo *Id,
QualType T, Expr *BW)
: FieldDecl(ObjCAtDefsField, DC, StartLoc, IdLoc, Id, T,
nullptr,
BW, false, ICIS_NoInit) {}

void anchor() override;

public:
static ObjCAtDefsFieldDecl *Create(ASTContext &C, DeclContext *DC,
SourceLocation StartLoc,
SourceLocation IdLoc, IdentifierInfo *Id,
QualType T, Expr *BW);

static ObjCAtDefsFieldDecl *CreateDeserialized(ASTContext &C, unsigned ID);


static bool classof(const Decl *D) { return classofKind(D->getKind()); }
static bool classofKind(Kind K) { return K == ObjCAtDefsField; }
};






























class ObjCProtocolDecl : public ObjCContainerDecl,
public Redeclarable<ObjCProtocolDecl> {
struct DefinitionData {

ObjCProtocolDecl *Definition;


ObjCProtocolList ReferencedProtocols;
};






llvm::PointerIntPair<DefinitionData *, 1, bool> Data;

ObjCProtocolDecl(ASTContext &C, DeclContext *DC, IdentifierInfo *Id,
SourceLocation nameLoc, SourceLocation atStartLoc,
ObjCProtocolDecl *PrevDecl);

void anchor() override;

DefinitionData &data() const {
assert(Data.getPointer() && "Objective-C protocol has no definition!");
return *Data.getPointer();
}

void allocateDefinitionData();

using redeclarable_base = Redeclarable<ObjCProtocolDecl>;

ObjCProtocolDecl *getNextRedeclarationImpl() override {
return getNextRedeclaration();
}

ObjCProtocolDecl *getPreviousDeclImpl() override {
return getPreviousDecl();
}

ObjCProtocolDecl *getMostRecentDeclImpl() override {
return getMostRecentDecl();
}

public:
friend class ASTDeclReader;
friend class ASTDeclWriter;
friend class ASTReader;

static ObjCProtocolDecl *Create(ASTContext &C, DeclContext *DC,
IdentifierInfo *Id,
SourceLocation nameLoc,
SourceLocation atStartLoc,
ObjCProtocolDecl *PrevDecl);

static ObjCProtocolDecl *CreateDeserialized(ASTContext &C, unsigned ID);

const ObjCProtocolList &getReferencedProtocols() const {
assert(hasDefinition() && "No definition available!");
return data().ReferencedProtocols;
}

using protocol_iterator = ObjCProtocolList::iterator;
using protocol_range = llvm::iterator_range<protocol_iterator>;

protocol_range protocols() const {
return protocol_range(protocol_begin(), protocol_end());
}

protocol_iterator protocol_begin() const {
if (!hasDefinition())
return protocol_iterator();

return data().ReferencedProtocols.begin();
}

protocol_iterator protocol_end() const {
if (!hasDefinition())
return protocol_iterator();

return data().ReferencedProtocols.end();
}

using protocol_loc_iterator = ObjCProtocolList::loc_iterator;
using protocol_loc_range = llvm::iterator_range<protocol_loc_iterator>;

protocol_loc_range protocol_locs() const {
return protocol_loc_range(protocol_loc_begin(), protocol_loc_end());
}

protocol_loc_iterator protocol_loc_begin() const {
if (!hasDefinition())
return protocol_loc_iterator();

return data().ReferencedProtocols.loc_begin();
}

protocol_loc_iterator protocol_loc_end() const {
if (!hasDefinition())
return protocol_loc_iterator();

return data().ReferencedProtocols.loc_end();
}

unsigned protocol_size() const {
if (!hasDefinition())
return 0;

return data().ReferencedProtocols.size();
}



void setProtocolList(ObjCProtocolDecl *const*List, unsigned Num,
const SourceLocation *Locs, ASTContext &C) {
assert(hasDefinition() && "Protocol is not defined");
data().ReferencedProtocols.set(List, Num, Locs, C);
}



bool isNonRuntimeProtocol() const;



void getImpliedProtocols(llvm::DenseSet<const ObjCProtocolDecl *> &IPs) const;

ObjCProtocolDecl *lookupProtocolNamed(IdentifierInfo *PName);



ObjCMethodDecl *lookupMethod(Selector Sel, bool isInstance) const;

ObjCMethodDecl *lookupInstanceMethod(Selector Sel) const {
return lookupMethod(Sel, true);
}

ObjCMethodDecl *lookupClassMethod(Selector Sel) const {
return lookupMethod(Sel, false);
}


bool hasDefinition() const {




if (!Data.getOpaqueValue())
getMostRecentDecl();

return Data.getPointer();
}


ObjCProtocolDecl *getDefinition() {
return hasDefinition()? Data.getPointer()->Definition : nullptr;
}


const ObjCProtocolDecl *getDefinition() const {
return hasDefinition()? Data.getPointer()->Definition : nullptr;
}



bool isThisDeclarationADefinition() const {
return getDefinition() == this;
}


void startDefinition();



StringRef getObjCRuntimeNameAsString() const;

SourceRange getSourceRange() const override LLVM_READONLY {
if (isThisDeclarationADefinition())
return ObjCContainerDecl::getSourceRange();

return SourceRange(getAtStartLoc(), getLocation());
}

using redecl_range = redeclarable_base::redecl_range;
using redecl_iterator = redeclarable_base::redecl_iterator;

using redeclarable_base::redecls_begin;
using redeclarable_base::redecls_end;
using redeclarable_base::redecls;
using redeclarable_base::getPreviousDecl;
using redeclarable_base::getMostRecentDecl;
using redeclarable_base::isFirstDecl;


ObjCProtocolDecl *getCanonicalDecl() override { return getFirstDecl(); }
const ObjCProtocolDecl *getCanonicalDecl() const { return getFirstDecl(); }

void collectPropertiesToImplement(PropertyMap &PM,
PropertyDeclOrder &PO) const override;

void collectInheritedProtocolProperties(const ObjCPropertyDecl *Property,
ProtocolPropertySet &PS,
PropertyDeclOrder &PO) const;

static bool classof(const Decl *D) { return classofKind(D->getKind()); }
static bool classofKind(Kind K) { return K == ObjCProtocol; }
};

















class ObjCCategoryDecl : public ObjCContainerDecl {

ObjCInterfaceDecl *ClassInterface;


ObjCTypeParamList *TypeParamList = nullptr;


ObjCProtocolList ReferencedProtocols;



ObjCCategoryDecl *NextClassCategory = nullptr;


SourceLocation CategoryNameLoc;


SourceLocation IvarLBraceLoc;
SourceLocation IvarRBraceLoc;

ObjCCategoryDecl(DeclContext *DC, SourceLocation AtLoc,
SourceLocation ClassNameLoc, SourceLocation CategoryNameLoc,
IdentifierInfo *Id, ObjCInterfaceDecl *IDecl,
ObjCTypeParamList *typeParamList,
SourceLocation IvarLBraceLoc = SourceLocation(),
SourceLocation IvarRBraceLoc = SourceLocation());

void anchor() override;

public:
friend class ASTDeclReader;
friend class ASTDeclWriter;

static ObjCCategoryDecl *Create(ASTContext &C, DeclContext *DC,
SourceLocation AtLoc,
SourceLocation ClassNameLoc,
SourceLocation CategoryNameLoc,
IdentifierInfo *Id,
ObjCInterfaceDecl *IDecl,
ObjCTypeParamList *typeParamList,
SourceLocation IvarLBraceLoc=SourceLocation(),
SourceLocation IvarRBraceLoc=SourceLocation());
static ObjCCategoryDecl *CreateDeserialized(ASTContext &C, unsigned ID);

ObjCInterfaceDecl *getClassInterface() { return ClassInterface; }
const ObjCInterfaceDecl *getClassInterface() const { return ClassInterface; }



ObjCTypeParamList *getTypeParamList() const { return TypeParamList; }





void setTypeParamList(ObjCTypeParamList *TPL);


ObjCCategoryImplDecl *getImplementation() const;
void setImplementation(ObjCCategoryImplDecl *ImplD);



void setProtocolList(ObjCProtocolDecl *const*List, unsigned Num,
const SourceLocation *Locs, ASTContext &C) {
ReferencedProtocols.set(List, Num, Locs, C);
}

const ObjCProtocolList &getReferencedProtocols() const {
return ReferencedProtocols;
}

using protocol_iterator = ObjCProtocolList::iterator;
using protocol_range = llvm::iterator_range<protocol_iterator>;

protocol_range protocols() const {
return protocol_range(protocol_begin(), protocol_end());
}

protocol_iterator protocol_begin() const {
return ReferencedProtocols.begin();
}

protocol_iterator protocol_end() const { return ReferencedProtocols.end(); }
unsigned protocol_size() const { return ReferencedProtocols.size(); }

using protocol_loc_iterator = ObjCProtocolList::loc_iterator;
using protocol_loc_range = llvm::iterator_range<protocol_loc_iterator>;

protocol_loc_range protocol_locs() const {
return protocol_loc_range(protocol_loc_begin(), protocol_loc_end());
}

protocol_loc_iterator protocol_loc_begin() const {
return ReferencedProtocols.loc_begin();
}

protocol_loc_iterator protocol_loc_end() const {
return ReferencedProtocols.loc_end();
}

ObjCCategoryDecl *getNextClassCategory() const { return NextClassCategory; }



ObjCCategoryDecl *getNextClassCategoryRaw() const {
return NextClassCategory;
}

bool IsClassExtension() const { return getIdentifier() == nullptr; }

using ivar_iterator = specific_decl_iterator<ObjCIvarDecl>;
using ivar_range = llvm::iterator_range<specific_decl_iterator<ObjCIvarDecl>>;

ivar_range ivars() const { return ivar_range(ivar_begin(), ivar_end()); }

ivar_iterator ivar_begin() const {
return ivar_iterator(decls_begin());
}

ivar_iterator ivar_end() const {
return ivar_iterator(decls_end());
}

unsigned ivar_size() const {
return std::distance(ivar_begin(), ivar_end());
}

bool ivar_empty() const {
return ivar_begin() == ivar_end();
}

SourceLocation getCategoryNameLoc() const { return CategoryNameLoc; }
void setCategoryNameLoc(SourceLocation Loc) { CategoryNameLoc = Loc; }

void setIvarLBraceLoc(SourceLocation Loc) { IvarLBraceLoc = Loc; }
SourceLocation getIvarLBraceLoc() const { return IvarLBraceLoc; }
void setIvarRBraceLoc(SourceLocation Loc) { IvarRBraceLoc = Loc; }
SourceLocation getIvarRBraceLoc() const { return IvarRBraceLoc; }

static bool classof(const Decl *D) { return classofKind(D->getKind()); }
static bool classofKind(Kind K) { return K == ObjCCategory; }
};

class ObjCImplDecl : public ObjCContainerDecl {

ObjCInterfaceDecl *ClassInterface;

void anchor() override;

protected:
ObjCImplDecl(Kind DK, DeclContext *DC,
ObjCInterfaceDecl *classInterface,
IdentifierInfo *Id,
SourceLocation nameLoc, SourceLocation atStartLoc)
: ObjCContainerDecl(DK, DC, Id, nameLoc, atStartLoc),
ClassInterface(classInterface) {}

public:
const ObjCInterfaceDecl *getClassInterface() const { return ClassInterface; }
ObjCInterfaceDecl *getClassInterface() { return ClassInterface; }
void setClassInterface(ObjCInterfaceDecl *IFace);

void addInstanceMethod(ObjCMethodDecl *method) {

method->setLexicalDeclContext(this);
addDecl(method);
}

void addClassMethod(ObjCMethodDecl *method) {

method->setLexicalDeclContext(this);
addDecl(method);
}

void addPropertyImplementation(ObjCPropertyImplDecl *property);

ObjCPropertyImplDecl *FindPropertyImplDecl(IdentifierInfo *propertyId,
ObjCPropertyQueryKind queryKind) const;
ObjCPropertyImplDecl *FindPropertyImplIvarDecl(IdentifierInfo *ivarId) const;


using propimpl_iterator = specific_decl_iterator<ObjCPropertyImplDecl>;
using propimpl_range =
llvm::iterator_range<specific_decl_iterator<ObjCPropertyImplDecl>>;

propimpl_range property_impls() const {
return propimpl_range(propimpl_begin(), propimpl_end());
}

propimpl_iterator propimpl_begin() const {
return propimpl_iterator(decls_begin());
}

propimpl_iterator propimpl_end() const {
return propimpl_iterator(decls_end());
}

static bool classof(const Decl *D) { return classofKind(D->getKind()); }

static bool classofKind(Kind K) {
return K >= firstObjCImpl && K <= lastObjCImpl;
}
};














class ObjCCategoryImplDecl : public ObjCImplDecl {

SourceLocation CategoryNameLoc;

ObjCCategoryImplDecl(DeclContext *DC, IdentifierInfo *Id,
ObjCInterfaceDecl *classInterface,
SourceLocation nameLoc, SourceLocation atStartLoc,
SourceLocation CategoryNameLoc)
: ObjCImplDecl(ObjCCategoryImpl, DC, classInterface, Id,
nameLoc, atStartLoc),
CategoryNameLoc(CategoryNameLoc) {}

void anchor() override;

public:
friend class ASTDeclReader;
friend class ASTDeclWriter;

static ObjCCategoryImplDecl *Create(ASTContext &C, DeclContext *DC,
IdentifierInfo *Id,
ObjCInterfaceDecl *classInterface,
SourceLocation nameLoc,
SourceLocation atStartLoc,
SourceLocation CategoryNameLoc);
static ObjCCategoryImplDecl *CreateDeserialized(ASTContext &C, unsigned ID);

ObjCCategoryDecl *getCategoryDecl() const;

SourceLocation getCategoryNameLoc() const { return CategoryNameLoc; }

static bool classof(const Decl *D) { return classofKind(D->getKind()); }
static bool classofKind(Kind K) { return K == ObjCCategoryImpl;}
};

raw_ostream &operator<<(raw_ostream &OS, const ObjCCategoryImplDecl &CID);


















class ObjCImplementationDecl : public ObjCImplDecl {

ObjCInterfaceDecl *SuperClass;
SourceLocation SuperLoc;


SourceLocation IvarLBraceLoc;
SourceLocation IvarRBraceLoc;



LazyCXXCtorInitializersPtr IvarInitializers;
unsigned NumIvarInitializers = 0;



bool HasNonZeroConstructors : 1;


bool HasDestructors : 1;

ObjCImplementationDecl(DeclContext *DC,
ObjCInterfaceDecl *classInterface,
ObjCInterfaceDecl *superDecl,
SourceLocation nameLoc, SourceLocation atStartLoc,
SourceLocation superLoc = SourceLocation(),
SourceLocation IvarLBraceLoc=SourceLocation(),
SourceLocation IvarRBraceLoc=SourceLocation())
: ObjCImplDecl(ObjCImplementation, DC, classInterface,
classInterface ? classInterface->getIdentifier()
: nullptr,
nameLoc, atStartLoc),
SuperClass(superDecl), SuperLoc(superLoc),
IvarLBraceLoc(IvarLBraceLoc), IvarRBraceLoc(IvarRBraceLoc),
HasNonZeroConstructors(false), HasDestructors(false) {}

void anchor() override;

public:
friend class ASTDeclReader;
friend class ASTDeclWriter;

static ObjCImplementationDecl *Create(ASTContext &C, DeclContext *DC,
ObjCInterfaceDecl *classInterface,
ObjCInterfaceDecl *superDecl,
SourceLocation nameLoc,
SourceLocation atStartLoc,
SourceLocation superLoc = SourceLocation(),
SourceLocation IvarLBraceLoc=SourceLocation(),
SourceLocation IvarRBraceLoc=SourceLocation());

static ObjCImplementationDecl *CreateDeserialized(ASTContext &C, unsigned ID);


using init_iterator = CXXCtorInitializer **;


using init_const_iterator = CXXCtorInitializer * const *;

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
return init_begin() + NumIvarInitializers;
}


init_const_iterator init_end() const {
return init_begin() + NumIvarInitializers;
}


unsigned getNumIvarInitializers() const {
return NumIvarInitializers;
}

void setNumIvarInitializers(unsigned numNumIvarInitializers) {
NumIvarInitializers = numNumIvarInitializers;
}

void setIvarInitializers(ASTContext &C,
CXXCtorInitializer ** initializers,
unsigned numInitializers);



bool hasNonZeroConstructors() const { return HasNonZeroConstructors; }
void setHasNonZeroConstructors(bool val) { HasNonZeroConstructors = val; }



bool hasDestructors() const { return HasDestructors; }
void setHasDestructors(bool val) { HasDestructors = val; }



IdentifierInfo *getIdentifier() const {
return getClassInterface()->getIdentifier();
}






StringRef getName() const {
assert(getIdentifier() && "Name is not a simple identifier");
return getIdentifier()->getName();
}




std::string getNameAsString() const { return std::string(getName()); }



StringRef getObjCRuntimeNameAsString() const;

const ObjCInterfaceDecl *getSuperClass() const { return SuperClass; }
ObjCInterfaceDecl *getSuperClass() { return SuperClass; }
SourceLocation getSuperClassLoc() const { return SuperLoc; }

void setSuperClass(ObjCInterfaceDecl * superCls) { SuperClass = superCls; }

void setIvarLBraceLoc(SourceLocation Loc) { IvarLBraceLoc = Loc; }
SourceLocation getIvarLBraceLoc() const { return IvarLBraceLoc; }
void setIvarRBraceLoc(SourceLocation Loc) { IvarRBraceLoc = Loc; }
SourceLocation getIvarRBraceLoc() const { return IvarRBraceLoc; }

using ivar_iterator = specific_decl_iterator<ObjCIvarDecl>;
using ivar_range = llvm::iterator_range<specific_decl_iterator<ObjCIvarDecl>>;

ivar_range ivars() const { return ivar_range(ivar_begin(), ivar_end()); }

ivar_iterator ivar_begin() const {
return ivar_iterator(decls_begin());
}

ivar_iterator ivar_end() const {
return ivar_iterator(decls_end());
}

unsigned ivar_size() const {
return std::distance(ivar_begin(), ivar_end());
}

bool ivar_empty() const {
return ivar_begin() == ivar_end();
}

static bool classof(const Decl *D) { return classofKind(D->getKind()); }
static bool classofKind(Kind K) { return K == ObjCImplementation; }
};

raw_ostream &operator<<(raw_ostream &OS, const ObjCImplementationDecl &ID);



class ObjCCompatibleAliasDecl : public NamedDecl {

ObjCInterfaceDecl *AliasedClass;

ObjCCompatibleAliasDecl(DeclContext *DC, SourceLocation L, IdentifierInfo *Id,
ObjCInterfaceDecl* aliasedClass)
: NamedDecl(ObjCCompatibleAlias, DC, L, Id), AliasedClass(aliasedClass) {}

void anchor() override;

public:
static ObjCCompatibleAliasDecl *Create(ASTContext &C, DeclContext *DC,
SourceLocation L, IdentifierInfo *Id,
ObjCInterfaceDecl* aliasedClass);

static ObjCCompatibleAliasDecl *CreateDeserialized(ASTContext &C,
unsigned ID);

const ObjCInterfaceDecl *getClassInterface() const { return AliasedClass; }
ObjCInterfaceDecl *getClassInterface() { return AliasedClass; }
void setClassInterface(ObjCInterfaceDecl *D) { AliasedClass = D; }

static bool classof(const Decl *D) { return classofKind(D->getKind()); }
static bool classofKind(Kind K) { return K == ObjCCompatibleAlias; }
};





class ObjCPropertyImplDecl : public Decl {
public:
enum Kind {
Synthesize,
Dynamic
};

private:
SourceLocation AtLoc;







SourceLocation IvarLoc;


ObjCPropertyDecl *PropertyDecl;


ObjCIvarDecl *PropertyIvarDecl;


ObjCMethodDecl *GetterMethodDecl = nullptr;

ObjCMethodDecl *SetterMethodDecl = nullptr;



Expr *GetterCXXConstructor = nullptr;



Expr *SetterCXXAssignment = nullptr;

ObjCPropertyImplDecl(DeclContext *DC, SourceLocation atLoc, SourceLocation L,
ObjCPropertyDecl *property,
Kind PK,
ObjCIvarDecl *ivarDecl,
SourceLocation ivarLoc)
: Decl(ObjCPropertyImpl, DC, L), AtLoc(atLoc),
IvarLoc(ivarLoc), PropertyDecl(property), PropertyIvarDecl(ivarDecl) {
assert(PK == Dynamic || PropertyIvarDecl);
}

public:
friend class ASTDeclReader;

static ObjCPropertyImplDecl *Create(ASTContext &C, DeclContext *DC,
SourceLocation atLoc, SourceLocation L,
ObjCPropertyDecl *property,
Kind PK,
ObjCIvarDecl *ivarDecl,
SourceLocation ivarLoc);

static ObjCPropertyImplDecl *CreateDeserialized(ASTContext &C, unsigned ID);

SourceRange getSourceRange() const override LLVM_READONLY;

SourceLocation getBeginLoc() const LLVM_READONLY { return AtLoc; }
void setAtLoc(SourceLocation Loc) { AtLoc = Loc; }

ObjCPropertyDecl *getPropertyDecl() const {
return PropertyDecl;
}
void setPropertyDecl(ObjCPropertyDecl *Prop) { PropertyDecl = Prop; }

Kind getPropertyImplementation() const {
return PropertyIvarDecl ? Synthesize : Dynamic;
}

ObjCIvarDecl *getPropertyIvarDecl() const {
return PropertyIvarDecl;
}
SourceLocation getPropertyIvarDeclLoc() const { return IvarLoc; }

void setPropertyIvarDecl(ObjCIvarDecl *Ivar,
SourceLocation IvarLoc) {
PropertyIvarDecl = Ivar;
this->IvarLoc = IvarLoc;
}








bool isIvarNameSpecified() const {
return IvarLoc.isValid() && IvarLoc != getLocation();
}

ObjCMethodDecl *getGetterMethodDecl() const { return GetterMethodDecl; }
void setGetterMethodDecl(ObjCMethodDecl *MD) { GetterMethodDecl = MD; }

ObjCMethodDecl *getSetterMethodDecl() const { return SetterMethodDecl; }
void setSetterMethodDecl(ObjCMethodDecl *MD) { SetterMethodDecl = MD; }

Expr *getGetterCXXConstructor() const {
return GetterCXXConstructor;
}

void setGetterCXXConstructor(Expr *getterCXXConstructor) {
GetterCXXConstructor = getterCXXConstructor;
}

Expr *getSetterCXXAssignment() const {
return SetterCXXAssignment;
}

void setSetterCXXAssignment(Expr *setterCXXAssignment) {
SetterCXXAssignment = setterCXXAssignment;
}

static bool classof(const Decl *D) { return classofKind(D->getKind()); }
static bool classofKind(Decl::Kind K) { return K == ObjCPropertyImpl; }
};

template<bool (*Filter)(ObjCCategoryDecl *)>
void
ObjCInterfaceDecl::filtered_category_iterator<Filter>::
findAcceptableCategory() {
while (Current && !Filter(Current))
Current = Current->getNextClassCategoryRaw();
}

template<bool (*Filter)(ObjCCategoryDecl *)>
inline ObjCInterfaceDecl::filtered_category_iterator<Filter> &
ObjCInterfaceDecl::filtered_category_iterator<Filter>::operator++() {
Current = Current->getNextClassCategoryRaw();
findAcceptableCategory();
return *this;
}

inline bool ObjCInterfaceDecl::isVisibleCategory(ObjCCategoryDecl *Cat) {
return Cat->isUnconditionallyVisible();
}

inline bool ObjCInterfaceDecl::isVisibleExtension(ObjCCategoryDecl *Cat) {
return Cat->IsClassExtension() && Cat->isUnconditionallyVisible();
}

inline bool ObjCInterfaceDecl::isKnownExtension(ObjCCategoryDecl *Cat) {
return Cat->IsClassExtension();
}

}

#endif
