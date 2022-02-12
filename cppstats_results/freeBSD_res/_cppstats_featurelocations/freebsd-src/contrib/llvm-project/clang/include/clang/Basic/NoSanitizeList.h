












#if !defined(LLVM_CLANG_BASIC_NOSANITIZELIST_H)
#define LLVM_CLANG_BASIC_NOSANITIZELIST_H

#include "clang/Basic/LLVM.h"
#include "clang/Basic/SourceLocation.h"
#include "llvm/ADT/StringRef.h"
#include <memory>
#include <vector>

namespace clang {

class SanitizerMask;
class SourceManager;
class SanitizerSpecialCaseList;

class NoSanitizeList {
std::unique_ptr<SanitizerSpecialCaseList> SSCL;
SourceManager &SM;

public:
NoSanitizeList(const std::vector<std::string> &NoSanitizeListPaths,
SourceManager &SM);
~NoSanitizeList();
bool containsGlobal(SanitizerMask Mask, StringRef GlobalName,
StringRef Category = StringRef()) const;
bool containsType(SanitizerMask Mask, StringRef MangledTypeName,
StringRef Category = StringRef()) const;
bool containsFunction(SanitizerMask Mask, StringRef FunctionName) const;
bool containsFile(SanitizerMask Mask, StringRef FileName,
StringRef Category = StringRef()) const;
bool containsLocation(SanitizerMask Mask, SourceLocation Loc,
StringRef Category = StringRef()) const;
};

}

#endif
