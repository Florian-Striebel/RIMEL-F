











#if !defined(LLVM_CLANG_CODEGEN_MODULEBUILDER_H)
#define LLVM_CLANG_CODEGEN_MODULEBUILDER_H

#include "clang/AST/ASTConsumer.h"

namespace llvm {
class Constant;
class LLVMContext;
class Module;
class StringRef;
}

namespace clang {
class CodeGenOptions;
class CoverageSourceInfo;
class Decl;
class DiagnosticsEngine;
class GlobalDecl;
class HeaderSearchOptions;
class LangOptions;
class PreprocessorOptions;

namespace CodeGen {
class CodeGenModule;
class CGDebugInfo;
}




class CodeGenerator : public ASTConsumer {
virtual void anchor();

public:



CodeGen::CodeGenModule &CGM();









llvm::Module *GetModule();





llvm::Module *ReleaseModule();


CodeGen::CGDebugInfo *getCGDebugInfo();





const Decl *GetDeclForMangledName(llvm::StringRef MangledName);









llvm::Constant *GetAddrOfGlobal(GlobalDecl decl, bool isForDefinition);



llvm::Module* StartModule(llvm::StringRef ModuleName, llvm::LLVMContext &C);
};




CodeGenerator *CreateLLVMCodeGen(DiagnosticsEngine &Diags,
llvm::StringRef ModuleName,
const HeaderSearchOptions &HeaderSearchOpts,
const PreprocessorOptions &PreprocessorOpts,
const CodeGenOptions &CGO,
llvm::LLVMContext& C,
CoverageSourceInfo *CoverageInfo = nullptr);

}

#endif
