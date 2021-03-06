







#if !defined(LLVM_CLANG_CODEGEN_BACKENDUTIL_H)
#define LLVM_CLANG_CODEGEN_BACKENDUTIL_H

#include "clang/Basic/LLVM.h"
#include "llvm/IR/ModuleSummaryIndex.h"
#include <memory>

namespace llvm {
class BitcodeModule;
template <typename T> class Expected;
class Module;
class MemoryBufferRef;
}

namespace clang {
class DiagnosticsEngine;
class HeaderSearchOptions;
class CodeGenOptions;
class TargetOptions;
class LangOptions;

enum BackendAction {
Backend_EmitAssembly,
Backend_EmitBC,
Backend_EmitLL,
Backend_EmitNothing,
Backend_EmitMCNull,
Backend_EmitObj
};

void EmitBackendOutput(DiagnosticsEngine &Diags, const HeaderSearchOptions &,
const CodeGenOptions &CGOpts,
const TargetOptions &TOpts, const LangOptions &LOpts,
StringRef TDesc, llvm::Module *M, BackendAction Action,
std::unique_ptr<raw_pwrite_stream> OS);

void EmbedBitcode(llvm::Module *M, const CodeGenOptions &CGOpts,
llvm::MemoryBufferRef Buf);
}

#endif
