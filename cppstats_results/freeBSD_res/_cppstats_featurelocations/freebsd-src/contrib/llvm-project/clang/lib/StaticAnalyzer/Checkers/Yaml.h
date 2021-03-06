












#if !defined(LLVM_CLANG_LIB_STATICANALYZER_CHECKER_YAML_H)
#define LLVM_CLANG_LIB_STATICANALYZER_CHECKER_YAML_H

#include "clang/StaticAnalyzer/Core/CheckerManager.h"
#include "llvm/Support/VirtualFileSystem.h"
#include "llvm/Support/YAMLTraits.h"

namespace clang {
namespace ento {




template <class T, class Checker>
llvm::Optional<T> getConfiguration(CheckerManager &Mgr, Checker *Chk,
StringRef Option, StringRef ConfigFile) {
if (ConfigFile.trim().empty())
return None;

llvm::vfs::FileSystem *FS = llvm::vfs::getRealFileSystem().get();
llvm::ErrorOr<std::unique_ptr<llvm::MemoryBuffer>> Buffer =
FS->getBufferForFile(ConfigFile.str());

if (std::error_code ec = Buffer.getError()) {
Mgr.reportInvalidCheckerOptionValue(Chk, Option,
"a valid filename instead of '" +
std::string(ConfigFile) + "'");
return None;
}

llvm::yaml::Input Input(Buffer.get()->getBuffer());
T Config;
Input >> Config;

if (std::error_code ec = Input.error()) {
Mgr.reportInvalidCheckerOptionValue(Chk, Option,
"a valid yaml file: " + ec.message());
return None;
}

return Config;
}

}
}

#endif
