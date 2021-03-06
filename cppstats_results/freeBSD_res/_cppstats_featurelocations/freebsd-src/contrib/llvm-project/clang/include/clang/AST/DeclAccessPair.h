















#if !defined(LLVM_CLANG_AST_DECLACCESSPAIR_H)
#define LLVM_CLANG_AST_DECLACCESSPAIR_H

#include "clang/Basic/Specifiers.h"
#include "llvm/Support/DataTypes.h"

namespace clang {

class NamedDecl;



class DeclAccessPair {
uintptr_t Ptr;

enum { Mask = 0x3 };

public:
static DeclAccessPair make(NamedDecl *D, AccessSpecifier AS) {
DeclAccessPair p;
p.set(D, AS);
return p;
}

NamedDecl *getDecl() const {
return reinterpret_cast<NamedDecl*>(~Mask & Ptr);
}
AccessSpecifier getAccess() const {
return AccessSpecifier(Mask & Ptr);
}

void setDecl(NamedDecl *D) {
set(D, getAccess());
}
void setAccess(AccessSpecifier AS) {
set(getDecl(), AS);
}
void set(NamedDecl *D, AccessSpecifier AS) {
Ptr = uintptr_t(AS) | reinterpret_cast<uintptr_t>(D);
}

operator NamedDecl*() const { return getDecl(); }
NamedDecl *operator->() const { return getDecl(); }
};
}

#endif
