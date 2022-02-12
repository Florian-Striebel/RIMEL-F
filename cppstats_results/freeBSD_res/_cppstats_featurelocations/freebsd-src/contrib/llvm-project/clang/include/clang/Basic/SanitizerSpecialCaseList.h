












#if !defined(LLVM_CLANG_BASIC_SANITIZERSPECIALCASELIST_H)
#define LLVM_CLANG_BASIC_SANITIZERSPECIALCASELIST_H

#include "clang/Basic/LLVM.h"
#include "clang/Basic/Sanitizers.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/SpecialCaseList.h"
#include <memory>
#include <vector>

namespace llvm {
namespace vfs {
class FileSystem;
}
}

namespace clang {

class SanitizerSpecialCaseList : public llvm::SpecialCaseList {
public:
static std::unique_ptr<SanitizerSpecialCaseList>
create(const std::vector<std::string> &Paths, llvm::vfs::FileSystem &VFS,
std::string &Error);

static std::unique_ptr<SanitizerSpecialCaseList>
createOrDie(const std::vector<std::string> &Paths,
llvm::vfs::FileSystem &VFS);


bool inSection(SanitizerMask Mask, StringRef Prefix, StringRef Query,
StringRef Category = StringRef()) const;

protected:

void createSanitizerSections();

struct SanitizerSection {
SanitizerSection(SanitizerMask SM, SectionEntries &E)
: Mask(SM), Entries(E){};

SanitizerMask Mask;
SectionEntries &Entries;
};

std::vector<SanitizerSection> SanitizerSections;
};

}

#endif
