













#if !defined(LLVM_CLANG_BASIC_VERSION_H)
#define LLVM_CLANG_BASIC_VERSION_H

#include "clang/Basic/Version.inc"
#include "llvm/ADT/StringRef.h"

namespace clang {



std::string getClangRepositoryPath();




std::string getLLVMRepositoryPath();



std::string getClangRevision();






std::string getLLVMRevision();



std::string getClangFullRepositoryVersion();




std::string getClangFullVersion();


std::string getClangToolFullVersion(llvm::StringRef ToolName);




std::string getClangFullCPPVersion();
}

#endif
