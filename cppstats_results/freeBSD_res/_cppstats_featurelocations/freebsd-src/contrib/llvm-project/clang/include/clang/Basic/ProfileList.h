











#if !defined(LLVM_CLANG_BASIC_INSTRPROFLIST_H)
#define LLVM_CLANG_BASIC_INSTRPROFLIST_H

#include "clang/Basic/CodeGenOptions.h"
#include "clang/Basic/LLVM.h"
#include "clang/Basic/SourceLocation.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/Optional.h"
#include "llvm/ADT/StringRef.h"
#include <memory>

namespace llvm {
class SpecialCaseList;
}

namespace clang {

class ProfileSpecialCaseList;

class ProfileList {
std::unique_ptr<ProfileSpecialCaseList> SCL;
const bool Empty;
const bool Default;
SourceManager &SM;

public:
ProfileList(ArrayRef<std::string> Paths, SourceManager &SM);
~ProfileList();

bool isEmpty() const { return Empty; }
bool getDefault() const { return Default; }

llvm::Optional<bool>
isFunctionExcluded(StringRef FunctionName,
CodeGenOptions::ProfileInstrKind Kind) const;
llvm::Optional<bool>
isLocationExcluded(SourceLocation Loc,
CodeGenOptions::ProfileInstrKind Kind) const;
llvm::Optional<bool>
isFileExcluded(StringRef FileName,
CodeGenOptions::ProfileInstrKind Kind) const;
};

}

#endif
