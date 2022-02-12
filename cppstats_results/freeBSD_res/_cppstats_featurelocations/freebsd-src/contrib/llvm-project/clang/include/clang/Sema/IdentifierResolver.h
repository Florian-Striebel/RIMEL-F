












#if !defined(LLVM_CLANG_SEMA_IDENTIFIERRESOLVER_H)
#define LLVM_CLANG_SEMA_IDENTIFIERRESOLVER_H

#include "clang/Basic/LLVM.h"
#include "llvm/ADT/SmallVector.h"
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <iterator>

namespace clang {

class Decl;
class DeclarationName;
class DeclContext;
class IdentifierInfo;
class LangOptions;
class NamedDecl;
class Preprocessor;
class Scope;




class IdentifierResolver {




class IdDeclInfo {
public:
using DeclsTy = SmallVector<NamedDecl *, 2>;

DeclsTy::iterator decls_begin() { return Decls.begin(); }
DeclsTy::iterator decls_end() { return Decls.end(); }

void AddDecl(NamedDecl *D) { Decls.push_back(D); }



void RemoveDecl(NamedDecl *D);


void InsertDecl(DeclsTy::iterator Pos, NamedDecl *D) {
Decls.insert(Pos, D);
}

private:
DeclsTy Decls;
};

public:



class iterator {
public:
friend class IdentifierResolver;

using value_type = NamedDecl *;
using reference = NamedDecl *;
using pointer = NamedDecl *;
using iterator_category = std::input_iterator_tag;
using difference_type = std::ptrdiff_t;





uintptr_t Ptr = 0;
using BaseIter = IdDeclInfo::DeclsTy::iterator;


iterator(NamedDecl *D) {
Ptr = reinterpret_cast<uintptr_t>(D);
assert((Ptr & 0x1) == 0 && "Invalid Ptr!");
}



iterator(BaseIter I) {
Ptr = reinterpret_cast<uintptr_t>(I) | 0x1;
}

bool isIterator() const { return (Ptr & 0x1); }

BaseIter getIterator() const {
assert(isIterator() && "Ptr not an iterator!");
return reinterpret_cast<BaseIter>(Ptr & ~0x1);
}

void incrementSlowCase();

public:
iterator() = default;

NamedDecl *operator*() const {
if (isIterator())
return *getIterator();
else
return reinterpret_cast<NamedDecl*>(Ptr);
}

bool operator==(const iterator &RHS) const {
return Ptr == RHS.Ptr;
}
bool operator!=(const iterator &RHS) const {
return Ptr != RHS.Ptr;
}


iterator& operator++() {
if (!isIterator())
Ptr = 0;
else
incrementSlowCase();
return *this;
}
};

explicit IdentifierResolver(Preprocessor &PP);
~IdentifierResolver();


iterator begin(DeclarationName Name);


iterator end() {
return iterator();
}











bool isDeclInScope(Decl *D, DeclContext *Ctx, Scope *S = nullptr,
bool AllowInlineNamespace = false) const;


void AddDecl(NamedDecl *D);



void RemoveDecl(NamedDecl *D);



void InsertDeclAfter(iterator Pos, NamedDecl *D);









bool tryAddTopLevelDecl(NamedDecl *D, DeclarationName Name);

private:
const LangOptions &LangOpt;
Preprocessor &PP;

class IdDeclInfoMap;
IdDeclInfoMap *IdDeclInfos;

void updatingIdentifier(IdentifierInfo &II);
void readingIdentifier(IdentifierInfo &II);


static inline bool isDeclPtr(void *Ptr) {
return (reinterpret_cast<uintptr_t>(Ptr) & 0x1) == 0;
}


static inline IdDeclInfo *toIdDeclInfo(void *Ptr) {
assert((reinterpret_cast<uintptr_t>(Ptr) & 0x1) == 1
&& "Ptr not a IdDeclInfo* !");
return reinterpret_cast<IdDeclInfo*>(
reinterpret_cast<uintptr_t>(Ptr) & ~0x1);
}
};

}

#endif
