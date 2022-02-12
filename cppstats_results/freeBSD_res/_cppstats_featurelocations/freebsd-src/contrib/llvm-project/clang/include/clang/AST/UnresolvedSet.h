












#if !defined(LLVM_CLANG_AST_UNRESOLVEDSET_H)
#define LLVM_CLANG_AST_UNRESOLVEDSET_H

#include "clang/AST/DeclAccessPair.h"
#include "clang/Basic/LLVM.h"
#include "clang/Basic/Specifiers.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/iterator.h"
#include <cstddef>
#include <iterator>

namespace clang {

class NamedDecl;



class UnresolvedSetIterator : public llvm::iterator_adaptor_base<
UnresolvedSetIterator, DeclAccessPair *,
std::random_access_iterator_tag, NamedDecl *,
std::ptrdiff_t, NamedDecl *, NamedDecl *> {
friend class ASTUnresolvedSet;
friend class OverloadExpr;
friend class UnresolvedSetImpl;

explicit UnresolvedSetIterator(DeclAccessPair *Iter)
: iterator_adaptor_base(Iter) {}
explicit UnresolvedSetIterator(const DeclAccessPair *Iter)
: iterator_adaptor_base(const_cast<DeclAccessPair *>(Iter)) {}

public:


UnresolvedSetIterator() : iterator_adaptor_base(nullptr) {}

NamedDecl *getDecl() const { return I->getDecl(); }
void setDecl(NamedDecl *ND) const { return I->setDecl(ND); }
AccessSpecifier getAccess() const { return I->getAccess(); }
void setAccess(AccessSpecifier AS) { I->setAccess(AS); }
const DeclAccessPair &getPair() const { return *I; }

NamedDecl *operator*() const { return getDecl(); }
NamedDecl *operator->() const { return **this; }
};


class UnresolvedSetImpl {
using DeclsTy = SmallVectorImpl<DeclAccessPair>;



private:
template <unsigned N> friend class UnresolvedSet;

UnresolvedSetImpl() = default;
UnresolvedSetImpl(const UnresolvedSetImpl &) = default;
UnresolvedSetImpl &operator=(const UnresolvedSetImpl &) = default;


UnresolvedSetImpl(UnresolvedSetImpl &&) {}
UnresolvedSetImpl &operator=(UnresolvedSetImpl &&) { return *this; }

public:


using iterator = UnresolvedSetIterator;
using const_iterator = UnresolvedSetIterator;

iterator begin() { return iterator(decls().begin()); }
iterator end() { return iterator(decls().end()); }

const_iterator begin() const { return const_iterator(decls().begin()); }
const_iterator end() const { return const_iterator(decls().end()); }

ArrayRef<DeclAccessPair> pairs() const { return decls(); }

void addDecl(NamedDecl *D) {
addDecl(D, AS_none);
}

void addDecl(NamedDecl *D, AccessSpecifier AS) {
decls().push_back(DeclAccessPair::make(D, AS));
}




bool replace(const NamedDecl* Old, NamedDecl *New) {
for (DeclsTy::iterator I = decls().begin(), E = decls().end(); I != E; ++I)
if (I->getDecl() == Old)
return (I->setDecl(New), true);
return false;
}



void replace(iterator I, NamedDecl *New) { I.I->setDecl(New); }

void replace(iterator I, NamedDecl *New, AccessSpecifier AS) {
I.I->set(New, AS);
}

void erase(unsigned I) { decls()[I] = decls().pop_back_val(); }

void erase(iterator I) { *I.I = decls().pop_back_val(); }

void setAccess(iterator I, AccessSpecifier AS) { I.I->setAccess(AS); }

void clear() { decls().clear(); }
void set_size(unsigned N) { decls().set_size(N); }

bool empty() const { return decls().empty(); }
unsigned size() const { return decls().size(); }

void append(iterator I, iterator E) { decls().append(I.I, E.I); }

template<typename Iter> void assign(Iter I, Iter E) { decls().assign(I, E); }

DeclAccessPair &operator[](unsigned I) { return decls()[I]; }
const DeclAccessPair &operator[](unsigned I) const { return decls()[I]; }

private:


DeclsTy &decls() {
return *reinterpret_cast<DeclsTy*>(this);
}
const DeclsTy &decls() const {
return *reinterpret_cast<const DeclsTy*>(this);
}
};


template <unsigned InlineCapacity> class UnresolvedSet :
public UnresolvedSetImpl {
SmallVector<DeclAccessPair, InlineCapacity> Decls;
};


}

#endif
