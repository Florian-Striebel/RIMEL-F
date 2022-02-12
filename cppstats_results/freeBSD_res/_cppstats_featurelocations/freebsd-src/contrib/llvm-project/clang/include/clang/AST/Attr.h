











#if !defined(LLVM_CLANG_AST_ATTR_H)
#define LLVM_CLANG_AST_ATTR_H

#include "clang/AST/ASTFwd.h"
#include "clang/AST/AttrIterator.h"
#include "clang/AST/Decl.h"
#include "clang/AST/Type.h"
#include "clang/Basic/AttrKinds.h"
#include "clang/Basic/AttributeCommonInfo.h"
#include "clang/Basic/LangOptions.h"
#include "clang/Basic/LLVM.h"
#include "clang/Basic/OpenMPKinds.h"
#include "clang/Basic/Sanitizers.h"
#include "clang/Basic/SourceLocation.h"
#include "llvm/ADT/StringSwitch.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/VersionTuple.h"
#include "llvm/Support/raw_ostream.h"
#include <algorithm>
#include <cassert>

namespace clang {
class ASTContext;
class AttributeCommonInfo;
class IdentifierInfo;
class ObjCInterfaceDecl;
class Expr;
class QualType;
class FunctionDecl;
class TypeSourceInfo;
class OMPTraitInfo;


class Attr : public AttributeCommonInfo {
private:
unsigned AttrKind : 16;

protected:


unsigned Inherited : 1;
unsigned IsPackExpansion : 1;
unsigned Implicit : 1;


unsigned IsLateParsed : 1;
unsigned InheritEvenIfAlreadyPresent : 1;

void *operator new(size_t bytes) noexcept {
llvm_unreachable("Attrs cannot be allocated with regular 'new'.");
}
void operator delete(void *data) noexcept {
llvm_unreachable("Attrs cannot be released with regular 'delete'.");
}

public:

void *operator new(size_t Bytes, ASTContext &C,
size_t Alignment = 8) noexcept {
return ::operator new(Bytes, C, Alignment);
}
void operator delete(void *Ptr, ASTContext &C, size_t Alignment) noexcept {
return ::operator delete(Ptr, C, Alignment);
}

protected:
Attr(ASTContext &Context, const AttributeCommonInfo &CommonInfo,
attr::Kind AK, bool IsLateParsed)
: AttributeCommonInfo(CommonInfo), AttrKind(AK), Inherited(false),
IsPackExpansion(false), Implicit(false), IsLateParsed(IsLateParsed),
InheritEvenIfAlreadyPresent(false) {}

public:
attr::Kind getKind() const { return static_cast<attr::Kind>(AttrKind); }

unsigned getSpellingListIndex() const {
return getAttributeSpellingListIndex();
}
const char *getSpelling() const;

SourceLocation getLocation() const { return getRange().getBegin(); }

bool isInherited() const { return Inherited; }



bool isImplicit() const { return Implicit; }
void setImplicit(bool I) { Implicit = I; }

void setPackExpansion(bool PE) { IsPackExpansion = PE; }
bool isPackExpansion() const { return IsPackExpansion; }


Attr *clone(ASTContext &C) const;

bool isLateParsed() const { return IsLateParsed; }


void printPretty(raw_ostream &OS, const PrintingPolicy &Policy) const;
};

class TypeAttr : public Attr {
protected:
TypeAttr(ASTContext &Context, const AttributeCommonInfo &CommonInfo,
attr::Kind AK, bool IsLateParsed)
: Attr(Context, CommonInfo, AK, IsLateParsed) {}

public:
static bool classof(const Attr *A) {
return A->getKind() >= attr::FirstTypeAttr &&
A->getKind() <= attr::LastTypeAttr;
}
};

class StmtAttr : public Attr {
protected:
StmtAttr(ASTContext &Context, const AttributeCommonInfo &CommonInfo,
attr::Kind AK, bool IsLateParsed)
: Attr(Context, CommonInfo, AK, IsLateParsed) {}

public:
static bool classof(const Attr *A) {
return A->getKind() >= attr::FirstStmtAttr &&
A->getKind() <= attr::LastStmtAttr;
}
};

class InheritableAttr : public Attr {
protected:
InheritableAttr(ASTContext &Context, const AttributeCommonInfo &CommonInfo,
attr::Kind AK, bool IsLateParsed,
bool InheritEvenIfAlreadyPresent)
: Attr(Context, CommonInfo, AK, IsLateParsed) {
this->InheritEvenIfAlreadyPresent = InheritEvenIfAlreadyPresent;
}

public:
void setInherited(bool I) { Inherited = I; }



bool shouldInheritEvenIfAlreadyPresent() const {
return InheritEvenIfAlreadyPresent;
}


static bool classof(const Attr *A) {
return A->getKind() >= attr::FirstInheritableAttr &&
A->getKind() <= attr::LastInheritableAttr;
}
};

class DeclOrStmtAttr : public InheritableAttr {
protected:
DeclOrStmtAttr(ASTContext &Context, const AttributeCommonInfo &CommonInfo,
attr::Kind AK, bool IsLateParsed,
bool InheritEvenIfAlreadyPresent)
: InheritableAttr(Context, CommonInfo, AK, IsLateParsed,
InheritEvenIfAlreadyPresent) {}

public:
static bool classof(const Attr *A) {
return A->getKind() >= attr::FirstDeclOrStmtAttr &&
A->getKind() <= attr::LastDeclOrStmtAttr;
}
};

class InheritableParamAttr : public InheritableAttr {
protected:
InheritableParamAttr(ASTContext &Context,
const AttributeCommonInfo &CommonInfo, attr::Kind AK,
bool IsLateParsed, bool InheritEvenIfAlreadyPresent)
: InheritableAttr(Context, CommonInfo, AK, IsLateParsed,
InheritEvenIfAlreadyPresent) {}

public:

static bool classof(const Attr *A) {
return A->getKind() >= attr::FirstInheritableParamAttr &&
A->getKind() <= attr::LastInheritableParamAttr;
}
};



class ParameterABIAttr : public InheritableParamAttr {
protected:
ParameterABIAttr(ASTContext &Context, const AttributeCommonInfo &CommonInfo,
attr::Kind AK, bool IsLateParsed,
bool InheritEvenIfAlreadyPresent)
: InheritableParamAttr(Context, CommonInfo, AK, IsLateParsed,
InheritEvenIfAlreadyPresent) {}

public:
ParameterABI getABI() const {
switch (getKind()) {
case attr::SwiftContext:
return ParameterABI::SwiftContext;
case attr::SwiftAsyncContext:
return ParameterABI::SwiftAsyncContext;
case attr::SwiftErrorResult:
return ParameterABI::SwiftErrorResult;
case attr::SwiftIndirectResult:
return ParameterABI::SwiftIndirectResult;
default:
llvm_unreachable("bad parameter ABI attribute kind");
}
}

static bool classof(const Attr *A) {
return A->getKind() >= attr::FirstParameterABIAttr &&
A->getKind() <= attr::LastParameterABIAttr;
}
};



class ParamIdx {

unsigned Idx : 30;
unsigned HasThis : 1;
unsigned IsValid : 1;

void assertComparable(const ParamIdx &I) const {
assert(isValid() && I.isValid() &&
"ParamIdx must be valid to be compared");




assert(HasThis == I.HasThis &&
"ParamIdx must be for the same function to be compared");
}

public:


ParamIdx() : Idx(0), HasThis(false), IsValid(false) {}







ParamIdx(unsigned Idx, const Decl *D)
: Idx(Idx), HasThis(false), IsValid(true) {
assert(Idx >= 1 && "Idx must be one-origin");
if (const auto *FD = dyn_cast<FunctionDecl>(D))
HasThis = FD->isCXXInstanceMember();
}





typedef uint32_t SerialType;



SerialType serialize() const {
return *reinterpret_cast<const SerialType *>(this);
}


static ParamIdx deserialize(SerialType S) {


void *ParamIdxPtr = static_cast<void *>(&S);
ParamIdx P(*static_cast<ParamIdx *>(ParamIdxPtr));
assert((!P.IsValid || P.Idx >= 1) && "valid Idx must be one-origin");
return P;
}


bool isValid() const { return IsValid; }







unsigned getSourceIndex() const {
assert(isValid() && "ParamIdx must be valid");
return Idx;
}







unsigned getASTIndex() const {
assert(isValid() && "ParamIdx must be valid");
assert(Idx >= 1 + HasThis &&
"stored index must be base-1 and not specify C++ implicit this");
return Idx - 1 - HasThis;
}





unsigned getLLVMIndex() const {
assert(isValid() && "ParamIdx must be valid");
assert(Idx >= 1 && "stored index must be base-1");
return Idx - 1;
}

bool operator==(const ParamIdx &I) const {
assertComparable(I);
return Idx == I.Idx;
}
bool operator!=(const ParamIdx &I) const {
assertComparable(I);
return Idx != I.Idx;
}
bool operator<(const ParamIdx &I) const {
assertComparable(I);
return Idx < I.Idx;
}
bool operator>(const ParamIdx &I) const {
assertComparable(I);
return Idx > I.Idx;
}
bool operator<=(const ParamIdx &I) const {
assertComparable(I);
return Idx <= I.Idx;
}
bool operator>=(const ParamIdx &I) const {
assertComparable(I);
return Idx >= I.Idx;
}
};

static_assert(sizeof(ParamIdx) == sizeof(ParamIdx::SerialType),
"ParamIdx does not fit its serialization type");


struct ParsedTargetAttr {
std::vector<std::string> Features;
StringRef Architecture;
StringRef Tune;
StringRef BranchProtection;
bool DuplicateArchitecture = false;
bool DuplicateTune = false;
bool operator ==(const ParsedTargetAttr &Other) const {
return DuplicateArchitecture == Other.DuplicateArchitecture &&
DuplicateTune == Other.DuplicateTune &&
Architecture == Other.Architecture &&
Tune == Other.Tune &&
BranchProtection == Other.BranchProtection &&
Features == Other.Features;
}
};

#include "clang/AST/Attrs.inc"

inline const StreamingDiagnostic &operator<<(const StreamingDiagnostic &DB,
const Attr *At) {
DB.AddTaggedVal(reinterpret_cast<intptr_t>(At),
DiagnosticsEngine::ak_attr);
return DB;
}
}

#endif
