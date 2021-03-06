














#if !defined(LLVM_CLANG_TOOLING_ARGUMENTSADJUSTERS_H)
#define LLVM_CLANG_TOOLING_ARGUMENTSADJUSTERS_H

#include "clang/Basic/LLVM.h"
#include "llvm/ADT/StringRef.h"
#include <functional>
#include <string>
#include <vector>

namespace clang {
namespace tooling {


using CommandLineArguments = std::vector<std::string>;





using ArgumentsAdjuster = std::function<CommandLineArguments(
const CommandLineArguments &, StringRef Filename)>;



ArgumentsAdjuster getClangSyntaxOnlyAdjuster();



ArgumentsAdjuster getClangStripOutputAdjuster();



ArgumentsAdjuster getClangStripDependencyFileAdjuster();

enum class ArgumentInsertPosition { BEGIN, END };



ArgumentsAdjuster getInsertArgumentAdjuster(const CommandLineArguments &Extra,
ArgumentInsertPosition Pos);



ArgumentsAdjuster getInsertArgumentAdjuster(
const char *Extra,
ArgumentInsertPosition Pos = ArgumentInsertPosition::END);



ArgumentsAdjuster getStripPluginsAdjuster();



ArgumentsAdjuster combineAdjusters(ArgumentsAdjuster First,
ArgumentsAdjuster Second);

}
}

#endif
