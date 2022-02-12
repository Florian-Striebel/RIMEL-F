







#if !defined(LLVM_CLANG_BASIC_TARGET_ID_H)
#define LLVM_CLANG_BASIC_TARGET_ID_H

#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringMap.h"
#include "llvm/ADT/Triple.h"
#include <set>

namespace clang {






const llvm::SmallVector<llvm::StringRef, 4>
getAllPossibleTargetIDFeatures(const llvm::Triple &T,
llvm::StringRef Processor);



llvm::StringRef getProcessorFromTargetID(const llvm::Triple &T,
llvm::StringRef OffloadArch);








llvm::Optional<llvm::StringRef>
parseTargetID(const llvm::Triple &T, llvm::StringRef OffloadArch,
llvm::StringMap<bool> *FeatureMap);



std::string getCanonicalTargetID(llvm::StringRef Processor,
const llvm::StringMap<bool> &Features);




llvm::Optional<std::pair<llvm::StringRef, llvm::StringRef>>
getConflictTargetIDCombination(const std::set<llvm::StringRef> &TargetIDs);
}

#endif
