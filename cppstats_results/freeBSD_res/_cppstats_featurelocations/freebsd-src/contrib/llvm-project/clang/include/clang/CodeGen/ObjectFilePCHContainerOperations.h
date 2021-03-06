







#if !defined(LLVM_CLANG_CODEGEN_OBJECT_FILE_PCH_CONTAINER_OPERATIONS_H)
#define LLVM_CLANG_CODEGEN_OBJECT_FILE_PCH_CONTAINER_OPERATIONS_H

#include "clang/Frontend/PCHContainerOperations.h"

namespace clang {



class ObjectFilePCHContainerWriter : public PCHContainerWriter {
StringRef getFormat() const override { return "obj"; }




std::unique_ptr<ASTConsumer>
CreatePCHContainerGenerator(CompilerInstance &CI,
const std::string &MainFileName,
const std::string &OutputFileName,
std::unique_ptr<llvm::raw_pwrite_stream> OS,
std::shared_ptr<PCHBuffer> Buffer) const override;
};



class ObjectFilePCHContainerReader : public PCHContainerReader {
StringRef getFormat() const override { return "obj"; }


StringRef ExtractPCH(llvm::MemoryBufferRef Buffer) const override;
};
}

#endif
