














#if !defined(LLVM_CLANG_TOOLING_CORE_DIAGNOSTIC_H)
#define LLVM_CLANG_TOOLING_CORE_DIAGNOSTIC_H

#include "Replacement.h"
#include "clang/Basic/Diagnostic.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringMap.h"
#include "llvm/ADT/StringRef.h"
#include <string>

namespace clang {
namespace tooling {


struct FileByteRange {
FileByteRange() = default;

FileByteRange(const SourceManager &Sources, CharSourceRange Range);

std::string FilePath;
unsigned FileOffset;
unsigned Length;
};



struct DiagnosticMessage {
DiagnosticMessage(llvm::StringRef Message = "");







DiagnosticMessage(llvm::StringRef Message, const SourceManager &Sources,
SourceLocation Loc);

std::string Message;
std::string FilePath;
unsigned FileOffset;


llvm::StringMap<Replacements> Fix;



llvm::SmallVector<FileByteRange, 1> Ranges;
};



struct Diagnostic {
enum Level {
Remark = DiagnosticsEngine::Remark,
Warning = DiagnosticsEngine::Warning,
Error = DiagnosticsEngine::Error
};

Diagnostic() = default;

Diagnostic(llvm::StringRef DiagnosticName, Level DiagLevel,
StringRef BuildDirectory);

Diagnostic(llvm::StringRef DiagnosticName, const DiagnosticMessage &Message,
const SmallVector<DiagnosticMessage, 1> &Notes, Level DiagLevel,
llvm::StringRef BuildDirectory);


std::string DiagnosticName;


DiagnosticMessage Message;


SmallVector<DiagnosticMessage, 1> Notes;


Level DiagLevel;








std::string BuildDirectory;
};


struct TranslationUnitDiagnostics {

std::string MainSourceFile;
std::vector<Diagnostic> Diagnostics;
};



const llvm::StringMap<Replacements> *selectFirstFix(const Diagnostic& D);

}
}
#endif
