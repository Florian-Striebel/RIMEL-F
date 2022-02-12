











#if !defined(LLVM_CLANG_REWRITE_FRONTEND_ASTCONSUMERS_H)
#define LLVM_CLANG_REWRITE_FRONTEND_ASTCONSUMERS_H

#include "clang/Basic/LLVM.h"
#include <memory>
#include <string>

namespace clang {

class ASTConsumer;
class DiagnosticsEngine;
class LangOptions;
class Preprocessor;



std::unique_ptr<ASTConsumer>
CreateObjCRewriter(const std::string &InFile, std::unique_ptr<raw_ostream> OS,
DiagnosticsEngine &Diags, const LangOptions &LOpts,
bool SilenceRewriteMacroWarning);
std::unique_ptr<ASTConsumer>
CreateModernObjCRewriter(const std::string &InFile,
std::unique_ptr<raw_ostream> OS,
DiagnosticsEngine &Diags, const LangOptions &LOpts,
bool SilenceRewriteMacroWarning, bool LineInfo);



std::unique_ptr<ASTConsumer> CreateHTMLPrinter(std::unique_ptr<raw_ostream> OS,
Preprocessor &PP,
bool SyntaxHighlight = true,
bool HighlightMacros = true);

}

#endif
