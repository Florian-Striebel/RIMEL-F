













#if !defined(LLVM_CLANG_BASIC_ALIGNED_ALLOCATION_H)
#define LLVM_CLANG_BASIC_ALIGNED_ALLOCATION_H

#include "llvm/ADT/Triple.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/VersionTuple.h"

namespace clang {

inline llvm::VersionTuple alignedAllocMinVersion(llvm::Triple::OSType OS) {
switch (OS) {
default:
break;
case llvm::Triple::Darwin:
case llvm::Triple::MacOSX:
return llvm::VersionTuple(10U, 14U);
case llvm::Triple::IOS:
case llvm::Triple::TvOS:
return llvm::VersionTuple(11U);
case llvm::Triple::WatchOS:
return llvm::VersionTuple(4U);
case llvm::Triple::ZOS:
return llvm::VersionTuple();
}

llvm_unreachable("Unexpected OS");
}

}

#endif
