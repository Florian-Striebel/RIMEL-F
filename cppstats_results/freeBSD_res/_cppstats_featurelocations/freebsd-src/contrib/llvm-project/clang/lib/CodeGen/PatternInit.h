







#if !defined(LLVM_CLANG_LIB_CODEGEN_PATTERNINIT_H)
#define LLVM_CLANG_LIB_CODEGEN_PATTERNINIT_H

namespace llvm {
class Constant;
class Type;
}

namespace clang {
namespace CodeGen {

class CodeGenModule;

llvm::Constant *initializationPatternFor(CodeGenModule &, llvm::Type *);

}
}

#endif
