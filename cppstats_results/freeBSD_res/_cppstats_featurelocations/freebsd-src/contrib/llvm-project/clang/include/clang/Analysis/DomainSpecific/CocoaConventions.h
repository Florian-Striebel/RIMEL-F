











#if !defined(LLVM_CLANG_ANALYSIS_DOMAINSPECIFIC_COCOACONVENTIONS_H)
#define LLVM_CLANG_ANALYSIS_DOMAINSPECIFIC_COCOACONVENTIONS_H

#include "clang/Basic/LLVM.h"
#include "llvm/ADT/StringRef.h"

namespace clang {
class FunctionDecl;
class QualType;

namespace ento {
namespace cocoa {

bool isRefType(QualType RetTy, StringRef Prefix,
StringRef Name = StringRef());

bool isCocoaObjectRef(QualType T);

}

namespace coreFoundation {
bool isCFObjectRef(QualType T);

bool followsCreateRule(const FunctionDecl *FD);
}

}}

#endif
