







#if !defined(LLVM_CLANG_TOOLING_INCLUSIONS_HEADERINCLUDES_H)
#define LLVM_CLANG_TOOLING_INCLUSIONS_HEADERINCLUDES_H

#include "clang/Basic/SourceManager.h"
#include "clang/Tooling/Core/Replacement.h"
#include "clang/Tooling/Inclusions/IncludeStyle.h"
#include "llvm/Support/Path.h"
#include "llvm/Support/Regex.h"
#include <unordered_map>

namespace clang {
namespace tooling {





class IncludeCategoryManager {
public:
IncludeCategoryManager(const IncludeStyle &Style, StringRef FileName);





int getIncludePriority(StringRef IncludeName, bool CheckMainHeader) const;
int getSortIncludePriority(StringRef IncludeName, bool CheckMainHeader) const;

private:
bool isMainHeader(StringRef IncludeName) const;

const IncludeStyle Style;
bool IsMainFile;
std::string FileName;
SmallVector<llvm::Regex, 4> CategoryRegexs;
};



class HeaderIncludes {
public:
HeaderIncludes(llvm::StringRef FileName, llvm::StringRef Code,
const IncludeStyle &Style);




















llvm::Optional<tooling::Replacement> insert(llvm::StringRef Header,
bool IsAngled) const;





tooling::Replacements remove(llvm::StringRef Header, bool IsAngled) const;

private:
struct Include {
Include(StringRef Name, tooling::Range R) : Name(Name), R(R) {}


std::string Name;


tooling::Range R;
};

void addExistingInclude(Include IncludeToAdd, unsigned NextLineOffset);

std::string FileName;
std::string Code;




llvm::StringMap<llvm::SmallVector<Include, 1>> ExistingIncludes;







std::unordered_map<int, llvm::SmallVector<const Include *, 8>>
IncludesByPriority;

int FirstIncludeOffset;


unsigned MinInsertOffset;



unsigned MaxInsertOffset;
IncludeCategoryManager Categories;

std::unordered_map<int, int> CategoryEndOffsets;


std::set<int> Priorities;


llvm::Regex IncludeRegex;
};


}
}

#endif
