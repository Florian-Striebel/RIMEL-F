







#if !defined(LLVM_CLANG_TESTING_TESTCLANGCONFIG_H)
#define LLVM_CLANG_TESTING_TESTCLANGCONFIG_H

#include "clang/Testing/CommandLineArgs.h"
#include "llvm/Support/raw_ostream.h"
#include <string>
#include <vector>

namespace clang {






struct TestClangConfig {
TestLanguage Language;


std::string Target;

bool isC() const { return Language == Lang_C89 || Language == Lang_C99; }

bool isC99OrLater() const { return Language == Lang_C99; }

bool isCXX() const {
return Language == Lang_CXX03 || Language == Lang_CXX11 ||
Language == Lang_CXX14 || Language == Lang_CXX17 ||
Language == Lang_CXX20;
}

bool isCXX11OrLater() const {
return Language == Lang_CXX11 || Language == Lang_CXX14 ||
Language == Lang_CXX17 || Language == Lang_CXX20;
}

bool isCXX14OrLater() const {
return Language == Lang_CXX14 || Language == Lang_CXX17 ||
Language == Lang_CXX20;
}

bool isCXX17OrLater() const {
return Language == Lang_CXX17 || Language == Lang_CXX20;
}

bool isCXX20OrLater() const { return Language == Lang_CXX20; }

bool supportsCXXDynamicExceptionSpecification() const {
return Language == Lang_CXX03 || Language == Lang_CXX11 ||
Language == Lang_CXX14;
}

bool hasDelayedTemplateParsing() const {
return Target == "x86_64-pc-win32-msvc";
}

std::vector<std::string> getCommandLineArgs() const {
std::vector<std::string> Result = getCommandLineArgsForTesting(Language);
Result.push_back("-target");
Result.push_back(Target);
return Result;
}

std::string toString() const {
std::string Result;
llvm::raw_string_ostream OS(Result);
OS << "{ Language=" << Language << ", Target=" << Target << " }";
return OS.str();
}

friend std::ostream &operator<<(std::ostream &OS,
const TestClangConfig &ClangConfig) {
return OS << ClangConfig.toString();
}
};

}

#endif
