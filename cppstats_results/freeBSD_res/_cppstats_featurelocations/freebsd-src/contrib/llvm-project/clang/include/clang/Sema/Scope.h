











#if !defined(LLVM_CLANG_SEMA_SCOPE_H)
#define LLVM_CLANG_SEMA_SCOPE_H

#include "clang/AST/Decl.h"
#include "clang/Basic/Diagnostic.h"
#include "llvm/ADT/PointerIntPair.h"
#include "llvm/ADT/SmallPtrSet.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/iterator_range.h"
#include <cassert>

namespace llvm {

class raw_ostream;

}

namespace clang {

class Decl;
class DeclContext;
class UsingDirectiveDecl;
class VarDecl;




class Scope {
public:


enum ScopeFlags {


FnScope = 0x01,



BreakScope = 0x02,



ContinueScope = 0x04,



DeclScope = 0x08,


ControlScope = 0x10,


ClassScope = 0x20,





BlockScope = 0x40,





TemplateParamScope = 0x80,



FunctionPrototypeScope = 0x100,





FunctionDeclarationScope = 0x200,



AtCatchScope = 0x400,



ObjCMethodScope = 0x800,


SwitchScope = 0x1000,


TryScope = 0x2000,


FnTryCatchScope = 0x4000,


OpenMPDirectiveScope = 0x8000,


OpenMPLoopDirectiveScope = 0x10000,




OpenMPSimdDirectiveScope = 0x20000,


EnumScope = 0x40000,


SEHTryScope = 0x80000,


SEHExceptScope = 0x100000,


SEHFilterScope = 0x200000,


CompoundStmtScope = 0x400000,



ClassInheritanceScope = 0x800000,


CatchScope = 0x1000000,




ConditionVarScope = 0x2000000,
};

private:


Scope *AnyParent;



unsigned Flags;



unsigned short Depth;



unsigned short MSLastManglingNumber;

unsigned short MSCurManglingNumber;



unsigned short PrototypeDepth;



unsigned short PrototypeIndex;



Scope *FnParent;
Scope *MSLastManglingParent;





Scope *BreakParent, *ContinueParent;



Scope *BlockParent;





Scope *TemplateParamParent;







using DeclSetTy = llvm::SmallPtrSet<Decl *, 32>;
DeclSetTy DeclsInScope;




DeclContext *Entity;

using UsingDirectivesTy = SmallVector<UsingDirectiveDecl *, 2>;
UsingDirectivesTy UsingDirectives;


DiagnosticErrorTrap ErrorTrap;



llvm::PointerIntPair<VarDecl *, 1, bool> NRVO;

void setFlags(Scope *Parent, unsigned F);

public:
Scope(Scope *Parent, unsigned ScopeFlags, DiagnosticsEngine &Diag)
: ErrorTrap(Diag) {
Init(Parent, ScopeFlags);
}


unsigned getFlags() const { return Flags; }

void setFlags(unsigned F) { setFlags(getParent(), F); }


bool isBlockScope() const { return Flags & BlockScope; }


const Scope *getParent() const { return AnyParent; }
Scope *getParent() { return AnyParent; }


const Scope *getFnParent() const { return FnParent; }
Scope *getFnParent() { return FnParent; }

const Scope *getMSLastManglingParent() const {
return MSLastManglingParent;
}
Scope *getMSLastManglingParent() { return MSLastManglingParent; }



Scope *getContinueParent() {
return ContinueParent;
}

const Scope *getContinueParent() const {
return const_cast<Scope*>(this)->getContinueParent();
}



void setIsConditionVarScope(bool InConditionVarScope) {
Flags = (Flags & ~ConditionVarScope) |
(InConditionVarScope ? ConditionVarScope : 0);
}

bool isConditionVarScope() const {
return Flags & ConditionVarScope;
}



Scope *getBreakParent() {
return BreakParent;
}
const Scope *getBreakParent() const {
return const_cast<Scope*>(this)->getBreakParent();
}

Scope *getBlockParent() { return BlockParent; }
const Scope *getBlockParent() const { return BlockParent; }

Scope *getTemplateParamParent() { return TemplateParamParent; }
const Scope *getTemplateParamParent() const { return TemplateParamParent; }


unsigned getDepth() const { return Depth; }



unsigned getFunctionPrototypeDepth() const {
return PrototypeDepth;
}



unsigned getNextFunctionPrototypeIndex() {
assert(isFunctionPrototypeScope());
return PrototypeIndex++;
}

using decl_range = llvm::iterator_range<DeclSetTy::iterator>;

decl_range decls() const {
return decl_range(DeclsInScope.begin(), DeclsInScope.end());
}

bool decl_empty() const { return DeclsInScope.empty(); }

void AddDecl(Decl *D) {
DeclsInScope.insert(D);
}

void RemoveDecl(Decl *D) {
DeclsInScope.erase(D);
}

void incrementMSManglingNumber() {
if (Scope *MSLMP = getMSLastManglingParent()) {
MSLMP->MSLastManglingNumber += 1;
MSCurManglingNumber += 1;
}
}

void decrementMSManglingNumber() {
if (Scope *MSLMP = getMSLastManglingParent()) {
MSLMP->MSLastManglingNumber -= 1;
MSCurManglingNumber -= 1;
}
}

unsigned getMSLastManglingNumber() const {
if (const Scope *MSLMP = getMSLastManglingParent())
return MSLMP->MSLastManglingNumber;
return 1;
}

unsigned getMSCurManglingNumber() const {
return MSCurManglingNumber;
}



bool isDeclScope(const Decl *D) const { return DeclsInScope.count(D) != 0; }


DeclContext *getEntity() const {
return isTemplateParamScope() ? nullptr : Entity;
}



DeclContext *getLookupEntity() const { return Entity; }

void setEntity(DeclContext *E) {
assert(!isTemplateParamScope() &&
"entity associated with template param scope");
Entity = E;
}
void setLookupEntity(DeclContext *E) { Entity = E; }





bool hasUnrecoverableErrorOccurred() const {
return ErrorTrap.hasUnrecoverableErrorOccurred();
}


bool isFunctionScope() const { return (getFlags() & Scope::FnScope); }


bool isClassScope() const {
return (getFlags() & Scope::ClassScope);
}



bool isInCXXInlineMethodScope() const {
if (const Scope *FnS = getFnParent()) {
assert(FnS->getParent() && "TUScope not created?");
return FnS->getParent()->isClassScope();
}
return false;
}



bool isInObjcMethodScope() const {
for (const Scope *S = this; S; S = S->getParent()) {

if (S->getFlags() & ObjCMethodScope)
return true;
}
return false;
}



bool isInObjcMethodOuterScope() const {
if (const Scope *S = this) {

if (S->getFlags() & ObjCMethodScope)
return true;
}
return false;
}



bool isTemplateParamScope() const {
return getFlags() & Scope::TemplateParamScope;
}



bool isFunctionPrototypeScope() const {
return getFlags() & Scope::FunctionPrototypeScope;
}



bool isFunctionDeclarationScope() const {
return getFlags() & Scope::FunctionDeclarationScope;
}


bool isAtCatchScope() const {
return getFlags() & Scope::AtCatchScope;
}


bool isSwitchScope() const {
for (const Scope *S = this; S; S = S->getParent()) {
if (S->getFlags() & Scope::SwitchScope)
return true;
else if (S->getFlags() & (Scope::FnScope | Scope::ClassScope |
Scope::BlockScope | Scope::TemplateParamScope |
Scope::FunctionPrototypeScope |
Scope::AtCatchScope | Scope::ObjCMethodScope))
return false;
}
return false;
}


bool isOpenMPDirectiveScope() const {
return (getFlags() & Scope::OpenMPDirectiveScope);
}



bool isOpenMPLoopDirectiveScope() const {
if (getFlags() & Scope::OpenMPLoopDirectiveScope) {
assert(isOpenMPDirectiveScope() &&
"OpenMP loop directive scope is not a directive scope");
return true;
}
return false;
}



bool isOpenMPSimdDirectiveScope() const {
return getFlags() & Scope::OpenMPSimdDirectiveScope;
}



bool isOpenMPLoopScope() const {
const Scope *P = getParent();
return P && P->isOpenMPLoopDirectiveScope();
}


bool isTryScope() const { return getFlags() & Scope::TryScope; }


bool isSEHTryScope() const { return getFlags() & Scope::SEHTryScope; }


bool isSEHExceptScope() const { return getFlags() & Scope::SEHExceptScope; }


bool isCompoundStmtScope() const {
return getFlags() & Scope::CompoundStmtScope;
}





bool Contains(const Scope& rhs) const { return Depth < rhs.Depth; }



bool containedInPrototypeScope() const;

void PushUsingDirective(UsingDirectiveDecl *UDir) {
UsingDirectives.push_back(UDir);
}

using using_directives_range =
llvm::iterator_range<UsingDirectivesTy::iterator>;

using_directives_range using_directives() {
return using_directives_range(UsingDirectives.begin(),
UsingDirectives.end());
}

void addNRVOCandidate(VarDecl *VD) {
if (NRVO.getInt())
return;
if (NRVO.getPointer() == nullptr) {
NRVO.setPointer(VD);
return;
}
if (NRVO.getPointer() != VD)
setNoNRVO();
}

void setNoNRVO() {
NRVO.setInt(true);
NRVO.setPointer(nullptr);
}

void mergeNRVOIntoParent();


void Init(Scope *parent, unsigned flags);



void AddFlags(unsigned Flags);

void dumpImpl(raw_ostream &OS) const;
void dump() const;
};

}

#endif
