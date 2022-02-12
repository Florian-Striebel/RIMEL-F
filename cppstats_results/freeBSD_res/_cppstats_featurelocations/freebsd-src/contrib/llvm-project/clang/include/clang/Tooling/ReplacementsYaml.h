













#if !defined(LLVM_CLANG_TOOLING_REPLACEMENTSYAML_H)
#define LLVM_CLANG_TOOLING_REPLACEMENTSYAML_H

#include "clang/Tooling/Refactoring.h"
#include "llvm/Support/YAMLTraits.h"
#include <string>

LLVM_YAML_IS_SEQUENCE_VECTOR(clang::tooling::Replacement)

namespace llvm {
namespace yaml {



template <> struct MappingTraits<clang::tooling::Replacement> {


struct NormalizedReplacement {
NormalizedReplacement(const IO &)
: FilePath(""), Offset(0), Length(0), ReplacementText("") {}

NormalizedReplacement(const IO &, const clang::tooling::Replacement &R)
: FilePath(R.getFilePath()), Offset(R.getOffset()),
Length(R.getLength()), ReplacementText(R.getReplacementText()) {}

clang::tooling::Replacement denormalize(const IO &) {
return clang::tooling::Replacement(FilePath, Offset, Length,
ReplacementText);
}

std::string FilePath;
unsigned int Offset;
unsigned int Length;
std::string ReplacementText;
};

static void mapping(IO &Io, clang::tooling::Replacement &R) {
MappingNormalization<NormalizedReplacement, clang::tooling::Replacement>
Keys(Io, R);
Io.mapRequired("FilePath", Keys->FilePath);
Io.mapRequired("Offset", Keys->Offset);
Io.mapRequired("Length", Keys->Length);
Io.mapRequired("ReplacementText", Keys->ReplacementText);
}
};



template <> struct MappingTraits<clang::tooling::TranslationUnitReplacements> {
static void mapping(IO &Io,
clang::tooling::TranslationUnitReplacements &Doc) {
Io.mapRequired("MainSourceFile", Doc.MainSourceFile);
Io.mapRequired("Replacements", Doc.Replacements);
}
};
}
}

#endif
