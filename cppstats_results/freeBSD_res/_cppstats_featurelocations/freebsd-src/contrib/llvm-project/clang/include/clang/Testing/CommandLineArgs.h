











#if !defined(LLVM_CLANG_TESTING_COMMANDLINEARGS_H)
#define LLVM_CLANG_TESTING_COMMANDLINEARGS_H

#include "clang/Basic/LLVM.h"
#include "llvm/ADT/StringRef.h"
#include <string>
#include <vector>

namespace clang {

enum TestLanguage {
Lang_C89,
Lang_C99,
Lang_CXX03,
Lang_CXX11,
Lang_CXX14,
Lang_CXX17,
Lang_CXX20,
Lang_OpenCL,
Lang_OBJCXX
};

std::vector<std::string> getCommandLineArgsForTesting(TestLanguage Lang);

StringRef getFilenameForTesting(TestLanguage Lang);

}

#endif
