












#if !defined(LLVM_CLANG_SEMA_CXXFIELDCOLLECTOR_H)
#define LLVM_CLANG_SEMA_CXXFIELDCOLLECTOR_H

#include "clang/Basic/LLVM.h"
#include "llvm/ADT/SmallVector.h"

namespace clang {
class FieldDecl;



class CXXFieldCollector {



SmallVector<FieldDecl*, 32> Fields;




SmallVector<size_t, 4> FieldCount;















public:

void StartClass() { FieldCount.push_back(0); }


void Add(FieldDecl *D) {
Fields.push_back(D);
++FieldCount.back();
}


size_t getCurNumFields() const {
assert(!FieldCount.empty() && "no currently-parsed class");
return FieldCount.back();
}



FieldDecl **getCurFields() { return &*(Fields.end() - getCurNumFields()); }


void FinishClass() {
Fields.resize(Fields.size() - getCurNumFields());
FieldCount.pop_back();
}
};

}

#endif
