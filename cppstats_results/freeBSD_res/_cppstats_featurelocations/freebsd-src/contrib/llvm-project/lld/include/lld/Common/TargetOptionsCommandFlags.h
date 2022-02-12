











#if !defined(LLD_COMMON_TARGETOPTIONSCOMMANDFLAGS_H)
#define LLD_COMMON_TARGETOPTIONSCOMMANDFLAGS_H

#include "llvm/ADT/Optional.h"
#include "llvm/Support/CodeGen.h"
#include "llvm/Target/TargetOptions.h"

namespace lld {
llvm::TargetOptions initTargetOptionsFromCodeGenFlags();
llvm::Optional<llvm::Reloc::Model> getRelocModelFromCMModel();
llvm::Optional<llvm::CodeModel::Model> getCodeModelFromCMModel();
std::string getCPUStr();
std::vector<std::string> getMAttrs();
}

#endif
