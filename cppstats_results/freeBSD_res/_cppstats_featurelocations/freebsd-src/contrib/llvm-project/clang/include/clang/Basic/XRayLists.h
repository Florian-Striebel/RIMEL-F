










#if !defined(LLVM_CLANG_BASIC_XRAYLISTS_H)
#define LLVM_CLANG_BASIC_XRAYLISTS_H

#include "clang/Basic/LLVM.h"
#include "clang/Basic/SourceLocation.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/StringRef.h"
#include <memory>

namespace llvm {
class SpecialCaseList;
}

namespace clang {

class SourceManager;

class XRayFunctionFilter {
std::unique_ptr<llvm::SpecialCaseList> AlwaysInstrument;
std::unique_ptr<llvm::SpecialCaseList> NeverInstrument;
std::unique_ptr<llvm::SpecialCaseList> AttrList;
SourceManager &SM;

public:
XRayFunctionFilter(ArrayRef<std::string> AlwaysInstrumentPaths,
ArrayRef<std::string> NeverInstrumentPaths,
ArrayRef<std::string> AttrListPaths, SourceManager &SM);
~XRayFunctionFilter();

enum class ImbueAttribute {
NONE,
ALWAYS,
NEVER,
ALWAYS_ARG1,
};

ImbueAttribute shouldImbueFunction(StringRef FunctionName) const;

ImbueAttribute
shouldImbueFunctionsInFile(StringRef Filename,
StringRef Category = StringRef()) const;

ImbueAttribute shouldImbueLocation(SourceLocation Loc,
StringRef Category = StringRef()) const;
};

}

#endif
