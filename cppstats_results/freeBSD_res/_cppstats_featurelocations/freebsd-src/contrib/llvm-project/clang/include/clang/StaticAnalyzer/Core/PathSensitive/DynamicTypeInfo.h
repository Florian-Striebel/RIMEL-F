







#if !defined(LLVM_CLANG_STATICANALYZER_CORE_PATHSENSITIVE_DYNAMICTYPEINFO_H)
#define LLVM_CLANG_STATICANALYZER_CORE_PATHSENSITIVE_DYNAMICTYPEINFO_H

#include "clang/AST/Type.h"

namespace clang {
namespace ento {



class DynamicTypeInfo {
public:
DynamicTypeInfo() : DynTy(QualType()) {}

DynamicTypeInfo(QualType Ty, bool CanBeSub = true)
: DynTy(Ty), CanBeASubClass(CanBeSub) {}



bool canBeASubClass() const { return CanBeASubClass; }


bool isValid() const { return !DynTy.isNull(); }


QualType getType() const { return DynTy; }

operator bool() const { return isValid(); }

bool operator==(const DynamicTypeInfo &RHS) const {
return DynTy == RHS.DynTy && CanBeASubClass == RHS.CanBeASubClass;
}

void Profile(llvm::FoldingSetNodeID &ID) const {
ID.Add(DynTy);
ID.AddBoolean(CanBeASubClass);
}

private:
QualType DynTy;
bool CanBeASubClass;
};

}
}

#endif
