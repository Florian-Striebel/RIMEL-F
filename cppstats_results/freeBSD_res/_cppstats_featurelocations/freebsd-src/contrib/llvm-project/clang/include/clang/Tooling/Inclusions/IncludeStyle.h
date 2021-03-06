







#if !defined(LLVM_CLANG_TOOLING_INCLUSIONS_INCLUDESTYLE_H)
#define LLVM_CLANG_TOOLING_INCLUSIONS_INCLUDESTYLE_H

#include "llvm/Support/YAMLTraits.h"
#include <string>
#include <vector>

namespace clang {
namespace tooling {


struct IncludeStyle {

enum IncludeBlocksStyle {







IBS_Preserve,







IBS_Merge,









IBS_Regroup,
};



IncludeBlocksStyle IncludeBlocks;


struct IncludeCategory {

std::string Regex;

int Priority;

int SortPriority;

bool RegexIsCaseSensitive;
bool operator==(const IncludeCategory &Other) const {
return Regex == Other.Regex && Priority == Other.Priority &&
RegexIsCaseSensitive == Other.RegexIsCaseSensitive;
}
};














































std::vector<IncludeCategory> IncludeCategories;












std::string IncludeIsMainRegex;



















std::string IncludeIsMainSourceRegex;
};

}
}

LLVM_YAML_IS_SEQUENCE_VECTOR(clang::tooling::IncludeStyle::IncludeCategory)

namespace llvm {
namespace yaml {

template <>
struct MappingTraits<clang::tooling::IncludeStyle::IncludeCategory> {
static void mapping(IO &IO,
clang::tooling::IncludeStyle::IncludeCategory &Category);
};

template <>
struct ScalarEnumerationTraits<
clang::tooling::IncludeStyle::IncludeBlocksStyle> {
static void
enumeration(IO &IO, clang::tooling::IncludeStyle::IncludeBlocksStyle &Value);
};

}
}

#endif
