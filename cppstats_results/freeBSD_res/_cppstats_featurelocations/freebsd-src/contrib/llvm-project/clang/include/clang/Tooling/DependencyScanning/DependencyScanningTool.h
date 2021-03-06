







#if !defined(LLVM_CLANG_TOOLING_DEPENDENCY_SCANNING_TOOL_H)
#define LLVM_CLANG_TOOLING_DEPENDENCY_SCANNING_TOOL_H

#include "clang/Tooling/DependencyScanning/DependencyScanningService.h"
#include "clang/Tooling/DependencyScanning/DependencyScanningWorker.h"
#include "clang/Tooling/DependencyScanning/ModuleDepCollector.h"
#include "clang/Tooling/JSONCompilationDatabase.h"
#include "llvm/ADT/StringSet.h"
#include <string>

namespace clang{
namespace tooling{
namespace dependencies{


struct FullDependencies {



ModuleID ID;



std::vector<std::string> FileDeps;



std::vector<PrebuiltModuleDep> PrebuiltModuleDeps;






std::vector<ModuleID> ClangModuleDeps;












std::vector<std::string> getAdditionalArgs(
std::function<StringRef(ModuleID)> LookupPCMPath,
std::function<const ModuleDeps &(ModuleID)> LookupModuleDeps) const;




std::vector<std::string> getAdditionalArgsWithoutModulePaths() const;
};

struct FullDependenciesResult {
FullDependencies FullDeps;
std::vector<ModuleDeps> DiscoveredModules;
};



class DependencyScanningTool {
public:

DependencyScanningTool(DependencyScanningService &Service);







llvm::Expected<std::string>
getDependencyFile(const tooling::CompilationDatabase &Compilations,
StringRef CWD);












llvm::Expected<FullDependenciesResult>
getFullDependencies(const tooling::CompilationDatabase &Compilations,
StringRef CWD, const llvm::StringSet<> &AlreadySeen);

private:
DependencyScanningWorker Worker;
};

}
}
}

#endif
