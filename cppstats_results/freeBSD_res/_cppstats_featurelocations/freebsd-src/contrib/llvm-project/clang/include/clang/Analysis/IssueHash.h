






#if !defined(LLVM_CLANG_STATICANALYZER_CORE_ISSUE_HASH_H)
#define LLVM_CLANG_STATICANALYZER_CORE_ISSUE_HASH_H

#include "llvm/ADT/SmallString.h"

namespace clang {
class Decl;
class FullSourceLoc;
class LangOptions;

















llvm::SmallString<32> getIssueHash(const FullSourceLoc &IssueLoc,
llvm::StringRef CheckerName,
llvm::StringRef WarningMessage,
const Decl *IssueDecl,
const LangOptions &LangOpts);




std::string getIssueString(const FullSourceLoc &IssueLoc,
llvm::StringRef CheckerName,
llvm::StringRef WarningMessage,
const Decl *IssueDecl, const LangOptions &LangOpts);
}

#endif
