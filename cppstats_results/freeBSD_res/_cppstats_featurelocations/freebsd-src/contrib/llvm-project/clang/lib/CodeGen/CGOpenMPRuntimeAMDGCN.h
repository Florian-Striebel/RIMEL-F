












#if !defined(LLVM_CLANG_LIB_CODEGEN_CGOPENMPRUNTIMEAMDGCN_H)
#define LLVM_CLANG_LIB_CODEGEN_CGOPENMPRUNTIMEAMDGCN_H

#include "CGOpenMPRuntime.h"
#include "CGOpenMPRuntimeGPU.h"
#include "CodeGenFunction.h"
#include "clang/AST/StmtOpenMP.h"

namespace clang {
namespace CodeGen {

class CGOpenMPRuntimeAMDGCN final : public CGOpenMPRuntimeGPU {

public:
explicit CGOpenMPRuntimeAMDGCN(CodeGenModule &CGM);


llvm::Value *getGPUWarpSize(CodeGenFunction &CGF) override;


llvm::Value *getGPUThreadID(CodeGenFunction &CGF) override;


llvm::Value *getGPUNumThreads(CodeGenFunction &CGF) override;
};

}
}

#endif
