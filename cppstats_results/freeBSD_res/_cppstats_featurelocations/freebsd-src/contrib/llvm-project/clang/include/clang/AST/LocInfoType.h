











#if !defined(LLVM_CLANG_SEMA_LOCINFOTYPE_H)
#define LLVM_CLANG_SEMA_LOCINFOTYPE_H

#include "clang/AST/Type.h"

namespace clang {

class TypeSourceInfo;







class LocInfoType : public Type {
enum {


LocInfo = Type::TypeLast + 1
};

TypeSourceInfo *DeclInfo;

LocInfoType(QualType ty, TypeSourceInfo *TInfo)
: Type((TypeClass)LocInfo, ty, ty->getDependence()), DeclInfo(TInfo) {
assert(getTypeClass() == (TypeClass)LocInfo && "LocInfo didn't fit in TC?");
}
friend class Sema;

public:
QualType getType() const { return getCanonicalTypeInternal(); }
TypeSourceInfo *getTypeSourceInfo() const { return DeclInfo; }

void getAsStringInternal(std::string &Str,
const PrintingPolicy &Policy) const;

static bool classof(const Type *T) {
return T->getTypeClass() == (TypeClass)LocInfo;
}
};

}

#endif
