











#if !defined(LLVM_CLANG_AST_DECLARATIONNAME_H)
#define LLVM_CLANG_AST_DECLARATIONNAME_H

#include "clang/AST/Type.h"
#include "clang/Basic/Diagnostic.h"
#include "clang/Basic/IdentifierTable.h"
#include "clang/Basic/OperatorKinds.h"
#include "clang/Basic/PartialDiagnostic.h"
#include "clang/Basic/SourceLocation.h"
#include "llvm/ADT/DenseMapInfo.h"
#include "llvm/ADT/FoldingSet.h"
#include "llvm/Support/Compiler.h"
#include "llvm/Support/type_traits.h"
#include <cassert>
#include <cstdint>
#include <cstring>
#include <string>

namespace clang {

class ASTContext;
template <typename> class CanQual;
class DeclarationName;
class DeclarationNameTable;
class MultiKeywordSelector;
struct PrintingPolicy;
class TemplateDecl;
class TypeSourceInfo;
class UsingDirectiveDecl;

using CanQualType = CanQual<Type>;

namespace detail {







class alignas(IdentifierInfoAlignment) CXXSpecialNameExtra
: public llvm::FoldingSetNode {
friend class clang::DeclarationName;
friend class clang::DeclarationNameTable;


QualType Type;




void *FETokenInfo;

CXXSpecialNameExtra(QualType QT) : Type(QT), FETokenInfo(nullptr) {}

public:
void Profile(llvm::FoldingSetNodeID &ID) {
ID.AddPointer(Type.getAsOpaquePtr());
}
};


class alignas(IdentifierInfoAlignment) CXXDeductionGuideNameExtra
: public detail::DeclarationNameExtra,
public llvm::FoldingSetNode {
friend class clang::DeclarationName;
friend class clang::DeclarationNameTable;


TemplateDecl *Template;




void *FETokenInfo;

CXXDeductionGuideNameExtra(TemplateDecl *TD)
: DeclarationNameExtra(CXXDeductionGuideName), Template(TD),
FETokenInfo(nullptr) {}

public:
void Profile(llvm::FoldingSetNodeID &ID) { ID.AddPointer(Template); }
};





class alignas(IdentifierInfoAlignment) CXXOperatorIdName {
friend class clang::DeclarationName;
friend class clang::DeclarationNameTable;


OverloadedOperatorKind Kind = OO_None;




void *FETokenInfo = nullptr;
};



class alignas(IdentifierInfoAlignment) CXXLiteralOperatorIdName
: public detail::DeclarationNameExtra,
public llvm::FoldingSetNode {
friend class clang::DeclarationName;
friend class clang::DeclarationNameTable;

IdentifierInfo *ID;




void *FETokenInfo;

CXXLiteralOperatorIdName(IdentifierInfo *II)
: DeclarationNameExtra(CXXLiteralOperatorName), ID(II),
FETokenInfo(nullptr) {}

public:
void Profile(llvm::FoldingSetNodeID &FSID) { FSID.AddPointer(ID); }
};

}







class DeclarationName {
friend class DeclarationNameTable;
friend class NamedDecl;



























enum StoredNameKind {
StoredIdentifier = 0,
StoredObjCZeroArgSelector = Selector::ZeroArg,
StoredObjCOneArgSelector = Selector::OneArg,
StoredCXXConstructorName = 3,
StoredCXXDestructorName = 4,
StoredCXXConversionFunctionName = 5,
StoredCXXOperatorName = 6,
StoredDeclarationNameExtra = Selector::MultiArg,
PtrMask = 7,
UncommonNameKindOffset = 8
};

static_assert(alignof(IdentifierInfo) >= 8 &&
alignof(detail::DeclarationNameExtra) >= 8 &&
alignof(detail::CXXSpecialNameExtra) >= 8 &&
alignof(detail::CXXOperatorIdName) >= 8 &&
alignof(detail::CXXDeductionGuideNameExtra) >= 8 &&
alignof(detail::CXXLiteralOperatorIdName) >= 8,
"The various classes that DeclarationName::Ptr can point to"
" must be at least aligned to 8 bytes!");

public:




enum NameKind {
Identifier = StoredIdentifier,
ObjCZeroArgSelector = StoredObjCZeroArgSelector,
ObjCOneArgSelector = StoredObjCOneArgSelector,
CXXConstructorName = StoredCXXConstructorName,
CXXDestructorName = StoredCXXDestructorName,
CXXConversionFunctionName = StoredCXXConversionFunctionName,
CXXOperatorName = StoredCXXOperatorName,
CXXDeductionGuideName = UncommonNameKindOffset +
detail::DeclarationNameExtra::CXXDeductionGuideName,
CXXLiteralOperatorName =
UncommonNameKindOffset +
detail::DeclarationNameExtra::CXXLiteralOperatorName,
CXXUsingDirective = UncommonNameKindOffset +
detail::DeclarationNameExtra::CXXUsingDirective,
ObjCMultiArgSelector = UncommonNameKindOffset +
detail::DeclarationNameExtra::ObjCMultiArgSelector
};

private:
































uintptr_t Ptr = 0;

StoredNameKind getStoredNameKind() const {
return static_cast<StoredNameKind>(Ptr & PtrMask);
}

void *getPtr() const { return reinterpret_cast<void *>(Ptr & ~PtrMask); }

void setPtrAndKind(const void *P, StoredNameKind Kind) {
uintptr_t PAsInteger = reinterpret_cast<uintptr_t>(P);
assert((Kind & ~PtrMask) == 0 &&
"Invalid StoredNameKind in setPtrAndKind!");
assert((PAsInteger & PtrMask) == 0 &&
"Improperly aligned pointer in setPtrAndKind!");
Ptr = PAsInteger | Kind;
}


DeclarationName(detail::DeclarationNameExtra *Name) {
setPtrAndKind(Name, StoredDeclarationNameExtra);
}


DeclarationName(detail::CXXSpecialNameExtra *Name,
StoredNameKind StoredKind) {
assert((StoredKind == StoredCXXConstructorName ||
StoredKind == StoredCXXDestructorName ||
StoredKind == StoredCXXConversionFunctionName) &&
"Invalid StoredNameKind when constructing a DeclarationName"
" from a CXXSpecialNameExtra!");
setPtrAndKind(Name, StoredKind);
}


DeclarationName(detail::CXXOperatorIdName *Name) {
setPtrAndKind(Name, StoredCXXOperatorName);
}


IdentifierInfo *castAsIdentifierInfo() const {
assert((getStoredNameKind() == StoredIdentifier) &&
"DeclarationName does not store an IdentifierInfo!");
return static_cast<IdentifierInfo *>(getPtr());
}



detail::DeclarationNameExtra *castAsExtra() const {
assert((getStoredNameKind() == StoredDeclarationNameExtra) &&
"DeclarationName does not store an Extra structure!");
return static_cast<detail::DeclarationNameExtra *>(getPtr());
}



detail::CXXSpecialNameExtra *castAsCXXSpecialNameExtra() const {
assert((getStoredNameKind() == StoredCXXConstructorName ||
getStoredNameKind() == StoredCXXDestructorName ||
getStoredNameKind() == StoredCXXConversionFunctionName) &&
"DeclarationName does not store a CXXSpecialNameExtra!");
return static_cast<detail::CXXSpecialNameExtra *>(getPtr());
}



detail::CXXOperatorIdName *castAsCXXOperatorIdName() const {
assert((getStoredNameKind() == StoredCXXOperatorName) &&
"DeclarationName does not store a CXXOperatorIdName!");
return static_cast<detail::CXXOperatorIdName *>(getPtr());
}



detail::CXXDeductionGuideNameExtra *castAsCXXDeductionGuideNameExtra() const {
assert(getNameKind() == CXXDeductionGuideName &&
"DeclarationName does not store a CXXDeductionGuideNameExtra!");
return static_cast<detail::CXXDeductionGuideNameExtra *>(getPtr());
}



detail::CXXLiteralOperatorIdName *castAsCXXLiteralOperatorIdName() const {
assert(getNameKind() == CXXLiteralOperatorName &&
"DeclarationName does not store a CXXLiteralOperatorIdName!");
return static_cast<detail::CXXLiteralOperatorIdName *>(getPtr());
}



void *getFETokenInfoSlow() const;
void setFETokenInfoSlow(void *T);

public:

DeclarationName() { setPtrAndKind(nullptr, StoredIdentifier); }


DeclarationName(const IdentifierInfo *II) {
setPtrAndKind(II, StoredIdentifier);
}


DeclarationName(Selector Sel) : Ptr(Sel.InfoPtr) {}


static DeclarationName getUsingDirectiveName() {

static detail::DeclarationNameExtra UDirExtra(
detail::DeclarationNameExtra::CXXUsingDirective);
return DeclarationName(&UDirExtra);
}


explicit operator bool() const {
return getPtr() || (getStoredNameKind() != StoredIdentifier);
}


bool isEmpty() const { return !*this; }


bool isIdentifier() const { return getStoredNameKind() == StoredIdentifier; }
bool isObjCZeroArgSelector() const {
return getStoredNameKind() == StoredObjCZeroArgSelector;
}
bool isObjCOneArgSelector() const {
return getStoredNameKind() == StoredObjCOneArgSelector;
}


NameKind getNameKind() const {


StoredNameKind StoredKind = getStoredNameKind();
if (StoredKind != StoredDeclarationNameExtra)
return static_cast<NameKind>(StoredKind);



unsigned ExtraKind = castAsExtra()->getKind();
return static_cast<NameKind>(UncommonNameKindOffset + ExtraKind);
}







bool isDependentName() const;


std::string getAsString() const;



IdentifierInfo *getAsIdentifierInfo() const {
if (isIdentifier())
return castAsIdentifierInfo();
return nullptr;
}


uintptr_t getAsOpaqueInteger() const { return Ptr; }


void *getAsOpaquePtr() const { return reinterpret_cast<void *>(Ptr); }


static DeclarationName getFromOpaquePtr(void *P) {
DeclarationName N;
N.Ptr = reinterpret_cast<uintptr_t>(P);
return N;
}



static DeclarationName getFromOpaqueInteger(uintptr_t P) {
DeclarationName N;
N.Ptr = P;
return N;
}



QualType getCXXNameType() const {
if (getStoredNameKind() == StoredCXXConstructorName ||
getStoredNameKind() == StoredCXXDestructorName ||
getStoredNameKind() == StoredCXXConversionFunctionName) {
assert(getPtr() && "getCXXNameType on a null DeclarationName!");
return castAsCXXSpecialNameExtra()->Type;
}
return QualType();
}



TemplateDecl *getCXXDeductionGuideTemplate() const {
if (getNameKind() == CXXDeductionGuideName) {
assert(getPtr() &&
"getCXXDeductionGuideTemplate on a null DeclarationName!");
return castAsCXXDeductionGuideNameExtra()->Template;
}
return nullptr;
}



OverloadedOperatorKind getCXXOverloadedOperator() const {
if (getStoredNameKind() == StoredCXXOperatorName) {
assert(getPtr() && "getCXXOverloadedOperator on a null DeclarationName!");
return castAsCXXOperatorIdName()->Kind;
}
return OO_None;
}



IdentifierInfo *getCXXLiteralIdentifier() const {
if (getNameKind() == CXXLiteralOperatorName) {
assert(getPtr() && "getCXXLiteralIdentifier on a null DeclarationName!");
return castAsCXXLiteralOperatorIdName()->ID;
}
return nullptr;
}


Selector getObjCSelector() const {
assert((getNameKind() == ObjCZeroArgSelector ||
getNameKind() == ObjCOneArgSelector ||
getNameKind() == ObjCMultiArgSelector || !getPtr()) &&
"Not a selector!");
return Selector(Ptr);
}




void *getFETokenInfo() const {
assert(getPtr() && "getFETokenInfo on an empty DeclarationName!");
if (getStoredNameKind() == StoredIdentifier)
return castAsIdentifierInfo()->getFETokenInfo();
return getFETokenInfoSlow();
}

void setFETokenInfo(void *T) {
assert(getPtr() && "setFETokenInfo on an empty DeclarationName!");
if (getStoredNameKind() == StoredIdentifier)
castAsIdentifierInfo()->setFETokenInfo(T);
else
setFETokenInfoSlow(T);
}


friend bool operator==(DeclarationName LHS, DeclarationName RHS) {
return LHS.Ptr == RHS.Ptr;
}


friend bool operator!=(DeclarationName LHS, DeclarationName RHS) {
return LHS.Ptr != RHS.Ptr;
}

static DeclarationName getEmptyMarker() {
DeclarationName Name;
Name.Ptr = uintptr_t(-1);
return Name;
}

static DeclarationName getTombstoneMarker() {
DeclarationName Name;
Name.Ptr = uintptr_t(-2);
return Name;
}

static int compare(DeclarationName LHS, DeclarationName RHS);

void print(raw_ostream &OS, const PrintingPolicy &Policy) const;

void dump() const;
};

raw_ostream &operator<<(raw_ostream &OS, DeclarationName N);



inline bool operator<(DeclarationName LHS, DeclarationName RHS) {
return DeclarationName::compare(LHS, RHS) < 0;
}



inline bool operator>(DeclarationName LHS, DeclarationName RHS) {
return DeclarationName::compare(LHS, RHS) > 0;
}



inline bool operator<=(DeclarationName LHS, DeclarationName RHS) {
return DeclarationName::compare(LHS, RHS) <= 0;
}



inline bool operator>=(DeclarationName LHS, DeclarationName RHS) {
return DeclarationName::compare(LHS, RHS) >= 0;
}






class DeclarationNameTable {

const ASTContext &Ctx;




llvm::FoldingSet<detail::CXXSpecialNameExtra> CXXConstructorNames;




llvm::FoldingSet<detail::CXXSpecialNameExtra> CXXDestructorNames;





llvm::FoldingSet<detail::CXXSpecialNameExtra> CXXConversionFunctionNames;




detail::CXXOperatorIdName CXXOperatorNames[NUM_OVERLOADED_OPERATORS];





llvm::FoldingSet<detail::CXXLiteralOperatorIdName> CXXLiteralOperatorNames;





llvm::FoldingSet<detail::CXXDeductionGuideNameExtra> CXXDeductionGuideNames;

public:
DeclarationNameTable(const ASTContext &C);
DeclarationNameTable(const DeclarationNameTable &) = delete;
DeclarationNameTable &operator=(const DeclarationNameTable &) = delete;
DeclarationNameTable(DeclarationNameTable &&) = delete;
DeclarationNameTable &operator=(DeclarationNameTable &&) = delete;
~DeclarationNameTable() = default;


DeclarationName getIdentifier(const IdentifierInfo *ID) {
return DeclarationName(ID);
}


DeclarationName getCXXConstructorName(CanQualType Ty);


DeclarationName getCXXDestructorName(CanQualType Ty);


DeclarationName getCXXDeductionGuideName(TemplateDecl *TD);


DeclarationName getCXXConversionFunctionName(CanQualType Ty);







DeclarationName getCXXSpecialName(DeclarationName::NameKind Kind,
CanQualType Ty);


DeclarationName getCXXOperatorName(OverloadedOperatorKind Op) {
return DeclarationName(&CXXOperatorNames[Op]);
}


DeclarationName getCXXLiteralOperatorName(IdentifierInfo *II);
};




class DeclarationNameLoc {






struct NT {
TypeSourceInfo *TInfo;
};


struct CXXOpName {
SourceLocation::UIntTy BeginOpNameLoc;
SourceLocation::UIntTy EndOpNameLoc;
};


struct CXXLitOpName {
SourceLocation::UIntTy OpNameLoc;
};





union {
struct NT NamedType;
struct CXXOpName CXXOperatorName;
struct CXXLitOpName CXXLiteralOperatorName;
};

void setNamedTypeLoc(TypeSourceInfo *TInfo) { NamedType.TInfo = TInfo; }

void setCXXOperatorNameRange(SourceRange Range) {
CXXOperatorName.BeginOpNameLoc = Range.getBegin().getRawEncoding();
CXXOperatorName.EndOpNameLoc = Range.getEnd().getRawEncoding();
}

void setCXXLiteralOperatorNameLoc(SourceLocation Loc) {
CXXLiteralOperatorName.OpNameLoc = Loc.getRawEncoding();
}

public:
DeclarationNameLoc(DeclarationName Name);

DeclarationNameLoc() { memset((void*) this, 0, sizeof(*this)); }



TypeSourceInfo *getNamedTypeInfo() const { return NamedType.TInfo; }


SourceLocation getCXXOperatorNameBeginLoc() const {
return SourceLocation::getFromRawEncoding(CXXOperatorName.BeginOpNameLoc);
}


SourceLocation getCXXOperatorNameEndLoc() const {
return SourceLocation::getFromRawEncoding(CXXOperatorName.EndOpNameLoc);
}




SourceRange getCXXOperatorNameRange() const {
return SourceRange(getCXXOperatorNameBeginLoc(),
getCXXOperatorNameEndLoc());
}




SourceLocation getCXXLiteralOperatorNameLoc() const {
return SourceLocation::getFromRawEncoding(CXXLiteralOperatorName.OpNameLoc);
}



static DeclarationNameLoc makeNamedTypeLoc(TypeSourceInfo *TInfo) {
DeclarationNameLoc DNL;
DNL.setNamedTypeLoc(TInfo);
return DNL;
}


static DeclarationNameLoc makeCXXOperatorNameLoc(SourceLocation BeginLoc,
SourceLocation EndLoc) {
return makeCXXOperatorNameLoc(SourceRange(BeginLoc, EndLoc));
}


static DeclarationNameLoc makeCXXOperatorNameLoc(SourceRange Range) {
DeclarationNameLoc DNL;
DNL.setCXXOperatorNameRange(Range);
return DNL;
}


static DeclarationNameLoc makeCXXLiteralOperatorNameLoc(SourceLocation Loc) {
DeclarationNameLoc DNL;
DNL.setCXXLiteralOperatorNameLoc(Loc);
return DNL;
}
};



struct DeclarationNameInfo {
private:

DeclarationName Name;


SourceLocation NameLoc;


DeclarationNameLoc LocInfo;

public:

DeclarationNameInfo() = default;

DeclarationNameInfo(DeclarationName Name, SourceLocation NameLoc)
: Name(Name), NameLoc(NameLoc), LocInfo(Name) {}

DeclarationNameInfo(DeclarationName Name, SourceLocation NameLoc,
DeclarationNameLoc LocInfo)
: Name(Name), NameLoc(NameLoc), LocInfo(LocInfo) {}


DeclarationName getName() const { return Name; }


void setName(DeclarationName N) { Name = N; }


SourceLocation getLoc() const { return NameLoc; }


void setLoc(SourceLocation L) { NameLoc = L; }

const DeclarationNameLoc &getInfo() const { return LocInfo; }
void setInfo(const DeclarationNameLoc &Info) { LocInfo = Info; }



TypeSourceInfo *getNamedTypeInfo() const {
if (Name.getNameKind() != DeclarationName::CXXConstructorName &&
Name.getNameKind() != DeclarationName::CXXDestructorName &&
Name.getNameKind() != DeclarationName::CXXConversionFunctionName)
return nullptr;
return LocInfo.getNamedTypeInfo();
}



void setNamedTypeInfo(TypeSourceInfo *TInfo) {
assert(Name.getNameKind() == DeclarationName::CXXConstructorName ||
Name.getNameKind() == DeclarationName::CXXDestructorName ||
Name.getNameKind() == DeclarationName::CXXConversionFunctionName);
LocInfo = DeclarationNameLoc::makeNamedTypeLoc(TInfo);
}



SourceRange getCXXOperatorNameRange() const {
if (Name.getNameKind() != DeclarationName::CXXOperatorName)
return SourceRange();
return LocInfo.getCXXOperatorNameRange();
}



void setCXXOperatorNameRange(SourceRange R) {
assert(Name.getNameKind() == DeclarationName::CXXOperatorName);
LocInfo = DeclarationNameLoc::makeCXXOperatorNameLoc(R);
}




SourceLocation getCXXLiteralOperatorNameLoc() const {
if (Name.getNameKind() != DeclarationName::CXXLiteralOperatorName)
return SourceLocation();
return LocInfo.getCXXLiteralOperatorNameLoc();
}




void setCXXLiteralOperatorNameLoc(SourceLocation Loc) {
assert(Name.getNameKind() == DeclarationName::CXXLiteralOperatorName);
LocInfo = DeclarationNameLoc::makeCXXLiteralOperatorNameLoc(Loc);
}


bool isInstantiationDependent() const;



bool containsUnexpandedParameterPack() const;


std::string getAsString() const;


void printName(raw_ostream &OS, PrintingPolicy Policy) const;


SourceLocation getBeginLoc() const { return NameLoc; }


SourceRange getSourceRange() const LLVM_READONLY {
return SourceRange(getBeginLoc(), getEndLoc());
}

SourceLocation getEndLoc() const LLVM_READONLY {
SourceLocation EndLoc = getEndLocPrivate();
return EndLoc.isValid() ? EndLoc : getBeginLoc();
}

private:
SourceLocation getEndLocPrivate() const;
};



inline const StreamingDiagnostic &operator<<(const StreamingDiagnostic &PD,
DeclarationName N) {
PD.AddTaggedVal(N.getAsOpaqueInteger(),
DiagnosticsEngine::ak_declarationname);
return PD;
}

raw_ostream &operator<<(raw_ostream &OS, DeclarationNameInfo DNInfo);

}

namespace llvm {



template<>
struct DenseMapInfo<clang::DeclarationName> {
static inline clang::DeclarationName getEmptyKey() {
return clang::DeclarationName::getEmptyMarker();
}

static inline clang::DeclarationName getTombstoneKey() {
return clang::DeclarationName::getTombstoneMarker();
}

static unsigned getHashValue(clang::DeclarationName Name) {
return DenseMapInfo<void*>::getHashValue(Name.getAsOpaquePtr());
}

static inline bool
isEqual(clang::DeclarationName LHS, clang::DeclarationName RHS) {
return LHS == RHS;
}
};

template <> struct PointerLikeTypeTraits<clang::DeclarationName> {
static inline void *getAsVoidPointer(clang::DeclarationName P) {
return P.getAsOpaquePtr();
}
static inline clang::DeclarationName getFromVoidPointer(void *P) {
return clang::DeclarationName::getFromOpaquePtr(P);
}
static constexpr int NumLowBitsAvailable = 0;
};

}



namespace clang {



class AssumedTemplateStorage : public UncommonTemplateNameStorage {
friend class ASTContext;

AssumedTemplateStorage(DeclarationName Name)
: UncommonTemplateNameStorage(Assumed, 0), Name(Name) {}
DeclarationName Name;

public:

DeclarationName getDeclName() const { return Name; }
};

}

#endif
