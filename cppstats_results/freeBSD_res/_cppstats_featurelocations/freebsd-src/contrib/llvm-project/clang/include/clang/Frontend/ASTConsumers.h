











#if !defined(LLVM_CLANG_FRONTEND_ASTCONSUMERS_H)
#define LLVM_CLANG_FRONTEND_ASTCONSUMERS_H

#include "clang/AST/ASTDumperUtils.h"
#include "clang/Basic/LLVM.h"
#include <memory>

namespace clang {

class ASTConsumer;
class CodeGenOptions;
class DiagnosticsEngine;
class FileManager;
class LangOptions;
class Preprocessor;
class TargetOptions;





std::unique_ptr<ASTConsumer> CreateASTPrinter(std::unique_ptr<raw_ostream> OS,
StringRef FilterString);



std::unique_ptr<ASTConsumer>
CreateASTDumper(std::unique_ptr<raw_ostream> OS, StringRef FilterString,
bool DumpDecls, bool Deserialize, bool DumpLookups,
bool DumpDeclTypes, ASTDumpOutputFormat Format);



std::unique_ptr<ASTConsumer> CreateASTDeclNodeLister();




std::unique_ptr<ASTConsumer> CreateASTViewer();

}

#endif
