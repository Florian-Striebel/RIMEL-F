













#if !defined(LLVM_CLANG_LIB_BASIC_TARGETS_H)
#define LLVM_CLANG_LIB_BASIC_TARGETS_H

#include "clang/Basic/LangOptions.h"
#include "clang/Basic/MacroBuilder.h"
#include "clang/Basic/TargetInfo.h"
#include "llvm/ADT/StringRef.h"

namespace clang {
namespace targets {

LLVM_LIBRARY_VISIBILITY
clang::TargetInfo *AllocateTarget(const llvm::Triple &Triple,
const clang::TargetOptions &Opts);




LLVM_LIBRARY_VISIBILITY
void DefineStd(clang::MacroBuilder &Builder, llvm::StringRef MacroName,
const clang::LangOptions &Opts);

LLVM_LIBRARY_VISIBILITY
void defineCPUMacros(clang::MacroBuilder &Builder, llvm::StringRef CPUName,
bool Tuning = true);

LLVM_LIBRARY_VISIBILITY
void addCygMingDefines(const clang::LangOptions &Opts,
clang::MacroBuilder &Builder);
}
}
#endif
