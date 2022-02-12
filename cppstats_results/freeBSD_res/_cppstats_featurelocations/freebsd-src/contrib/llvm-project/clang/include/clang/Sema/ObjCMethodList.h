











#if !defined(LLVM_CLANG_SEMA_OBJCMETHODLIST_H)
#define LLVM_CLANG_SEMA_OBJCMETHODLIST_H

#include "clang/AST/DeclObjC.h"
#include "llvm/ADT/PointerIntPair.h"

namespace clang {

class ObjCMethodDecl;



struct ObjCMethodList {


llvm::PointerIntPair<ObjCMethodDecl *, 1> MethodAndHasMoreThanOneDecl;

llvm::PointerIntPair<ObjCMethodList *, 2> NextAndExtraBits;

ObjCMethodList() { }
ObjCMethodList(ObjCMethodDecl *M)
: MethodAndHasMoreThanOneDecl(M, 0) {}
ObjCMethodList(const ObjCMethodList &L)
: MethodAndHasMoreThanOneDecl(L.MethodAndHasMoreThanOneDecl),
NextAndExtraBits(L.NextAndExtraBits) {}

ObjCMethodList &operator=(const ObjCMethodList &L) {
MethodAndHasMoreThanOneDecl = L.MethodAndHasMoreThanOneDecl;
NextAndExtraBits = L.NextAndExtraBits;
return *this;
}

ObjCMethodList *getNext() const { return NextAndExtraBits.getPointer(); }
unsigned getBits() const { return NextAndExtraBits.getInt(); }
void setNext(ObjCMethodList *L) { NextAndExtraBits.setPointer(L); }
void setBits(unsigned B) { NextAndExtraBits.setInt(B); }

ObjCMethodDecl *getMethod() const {
return MethodAndHasMoreThanOneDecl.getPointer();
}
void setMethod(ObjCMethodDecl *M) {
return MethodAndHasMoreThanOneDecl.setPointer(M);
}

bool hasMoreThanOneDecl() const {
return MethodAndHasMoreThanOneDecl.getInt();
}
void setHasMoreThanOneDecl(bool B) {
return MethodAndHasMoreThanOneDecl.setInt(B);
}
};

}

#endif
