







#if !defined(LLVM_CLANG_TOOLING_COMPILATIONDATABASEPLUGINREGISTRY_H)
#define LLVM_CLANG_TOOLING_COMPILATIONDATABASEPLUGINREGISTRY_H

#include "clang/Tooling/CompilationDatabase.h"
#include "llvm/Support/Registry.h"

namespace clang {
namespace tooling {











class CompilationDatabasePlugin {
public:
virtual ~CompilationDatabasePlugin();




virtual std::unique_ptr<CompilationDatabase>
loadFromDirectory(StringRef Directory, std::string &ErrorMessage) = 0;
};

using CompilationDatabasePluginRegistry =
llvm::Registry<CompilationDatabasePlugin>;

}
}

#endif
