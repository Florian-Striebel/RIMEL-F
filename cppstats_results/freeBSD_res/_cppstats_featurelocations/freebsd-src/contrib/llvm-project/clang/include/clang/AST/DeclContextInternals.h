












#if !defined(LLVM_CLANG_AST_DECLCONTEXTINTERNALS_H)
#define LLVM_CLANG_AST_DECLCONTEXTINTERNALS_H

#include "clang/AST/ASTContext.h"
#include "clang/AST/Decl.h"
#include "clang/AST/DeclBase.h"
#include "clang/AST/DeclCXX.h"
#include "clang/AST/DeclarationName.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/PointerIntPair.h"
#include "llvm/ADT/PointerUnion.h"
#include <cassert>

namespace clang {

class DependentDiagnostic;



class StoredDeclsList {
using Decls = DeclListNode::Decls;



using DeclsAndHasExternalTy = llvm::PointerIntPair<Decls, 1, bool>;




DeclsAndHasExternalTy Data;

template<typename Fn>
void erase_if(Fn ShouldErase) {
Decls List = Data.getPointer();
if (!List)
return;
ASTContext &C = getASTContext();
DeclListNode::Decls NewHead = nullptr;
DeclListNode::Decls *NewLast = nullptr;
DeclListNode::Decls *NewTail = &NewHead;
while (true) {
if (!ShouldErase(*DeclListNode::iterator(List))) {
NewLast = NewTail;
*NewTail = List;
if (auto *Node = List.dyn_cast<DeclListNode*>()) {
NewTail = &Node->Rest;
List = Node->Rest;
} else {
break;
}
} else if (DeclListNode *N = List.dyn_cast<DeclListNode*>()) {
List = N->Rest;
C.DeallocateDeclListNode(N);
} else {



if (NewLast) {
DeclListNode *Node = NewLast->get<DeclListNode*>();
*NewLast = Node->D;
C.DeallocateDeclListNode(Node);
}
break;
}
}
Data.setPointer(NewHead);

assert(llvm::find_if(getLookupResult(), ShouldErase) ==
getLookupResult().end() && "Still exists!");
}

void erase(NamedDecl *ND) {
erase_if([ND](NamedDecl *D) { return D == ND; });
}

public:
StoredDeclsList() = default;

StoredDeclsList(StoredDeclsList &&RHS) : Data(RHS.Data) {
RHS.Data.setPointer(nullptr);
RHS.Data.setInt(0);
}

void MaybeDeallocList() {
if (isNull())
return;

ASTContext &C = getASTContext();
Decls List = Data.getPointer();
while (DeclListNode *ToDealloc = List.dyn_cast<DeclListNode *>()) {
List = ToDealloc->Rest;
C.DeallocateDeclListNode(ToDealloc);
}
}

~StoredDeclsList() {
MaybeDeallocList();
}

StoredDeclsList &operator=(StoredDeclsList &&RHS) {
MaybeDeallocList();

Data = RHS.Data;
RHS.Data.setPointer(nullptr);
RHS.Data.setInt(0);
return *this;
}

bool isNull() const { return Data.getPointer().isNull(); }

ASTContext &getASTContext() {
assert(!isNull() && "No ASTContext.");
if (NamedDecl *ND = getAsDecl())
return ND->getASTContext();
return getAsList()->D->getASTContext();
}

DeclsAndHasExternalTy getAsListAndHasExternal() const { return Data; }

NamedDecl *getAsDecl() const {
return getAsListAndHasExternal().getPointer().dyn_cast<NamedDecl *>();
}

DeclListNode *getAsList() const {
return getAsListAndHasExternal().getPointer().dyn_cast<DeclListNode*>();
}

bool hasExternalDecls() const {
return getAsListAndHasExternal().getInt();
}

void setHasExternalDecls() {
Data.setInt(1);
}

void remove(NamedDecl *D) {
assert(!isNull() && "removing from empty list");
erase(D);
}


void removeExternalDecls() {
erase_if([](NamedDecl *ND) { return ND->isFromASTFile(); });


Data.setInt(0);
}

void replaceExternalDecls(ArrayRef<NamedDecl*> Decls) {


erase_if([Decls](NamedDecl *ND) {
if (ND->isFromASTFile())
return true;
for (NamedDecl *D : Decls)
if (D->declarationReplaces(ND, false))
return true;
return false;
});


Data.setInt(0);

if (Decls.empty())
return;


ASTContext &C = Decls.front()->getASTContext();
DeclListNode::Decls DeclsAsList = Decls.back();
for (size_t I = Decls.size() - 1; I != 0; --I) {
DeclListNode *Node = C.AllocateDeclListNode(Decls[I - 1]);
Node->Rest = DeclsAsList;
DeclsAsList = Node;
}

DeclListNode::Decls Head = Data.getPointer();
if (Head.isNull()) {
Data.setPointer(DeclsAsList);
return;
}




DeclListNode::Decls *Tail = &Head;
while (DeclListNode *Node = Tail->dyn_cast<DeclListNode *>())
Tail = &Node->Rest;


DeclListNode *Node = C.AllocateDeclListNode(Tail->get<NamedDecl *>());
Node->Rest = DeclsAsList;
*Tail = Node;
Data.setPointer(Head);
}


DeclContext::lookup_result getLookupResult() const {
return DeclContext::lookup_result(Data.getPointer());
}



void addOrReplaceDecl(NamedDecl *D) {
const bool IsKnownNewer = true;

if (isNull()) {
Data.setPointer(D);
return;
}


if (NamedDecl *OldD = getAsDecl()) {
if (D->declarationReplaces(OldD, IsKnownNewer)) {
Data.setPointer(D);
return;
}


ASTContext &C = D->getASTContext();
DeclListNode *Node = C.AllocateDeclListNode(OldD);
Node->Rest = D;
Data.setPointer(Node);
return;
}



assert(!llvm::is_contained(getLookupResult(), D) && "Already exists!");

for (DeclListNode *N = getAsList(); ;
N = N->Rest.dyn_cast<DeclListNode *>()) {
if (D->declarationReplaces(N->D, IsKnownNewer)) {
N->D = D;
return;
}
if (auto *ND = N->Rest.dyn_cast<NamedDecl *>()) {
if (D->declarationReplaces(ND, IsKnownNewer)) {
N->Rest = D;
return;
}


ASTContext &C = D->getASTContext();
DeclListNode *Node = C.AllocateDeclListNode(ND);
N->Rest = Node;
Node->Rest = D;
return;
}
}
}


void prependDeclNoReplace(NamedDecl *D) {
if (isNull()) {
Data.setPointer(D);
return;
}

ASTContext &C = D->getASTContext();
DeclListNode *Node = C.AllocateDeclListNode(D);
Node->Rest = Data.getPointer();
Data.setPointer(Node);
}

LLVM_DUMP_METHOD void dump() const {
Decls D = Data.getPointer();
if (!D) {
llvm::errs() << "<null>\n";
return;
}

while (true) {
if (auto *Node = D.dyn_cast<DeclListNode*>()) {
llvm::errs() << '[' << Node->D << "] -> ";
D = Node->Rest;
} else {
llvm::errs() << '[' << D.get<NamedDecl*>() << "]\n";
return;
}
}
}
};

class StoredDeclsMap
: public llvm::SmallDenseMap<DeclarationName, StoredDeclsList, 4> {
friend class ASTContext;
friend class DeclContext;

llvm::PointerIntPair<StoredDeclsMap*, 1> Previous;
public:
static void DestroyAll(StoredDeclsMap *Map, bool Dependent);
};

class DependentStoredDeclsMap : public StoredDeclsMap {
friend class DeclContext;
friend class DependentDiagnostic;

DependentDiagnostic *FirstDiagnostic = nullptr;
public:
DependentStoredDeclsMap() = default;
};

}

#endif
