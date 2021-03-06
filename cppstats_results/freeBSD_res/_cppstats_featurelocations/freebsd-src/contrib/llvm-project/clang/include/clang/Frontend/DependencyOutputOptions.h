







#if !defined(LLVM_CLANG_FRONTEND_DEPENDENCYOUTPUTOPTIONS_H)
#define LLVM_CLANG_FRONTEND_DEPENDENCYOUTPUTOPTIONS_H

#include <string>
#include <vector>

namespace clang {


enum class ShowIncludesDestination { None, Stdout, Stderr };


enum class DependencyOutputFormat { Make, NMake };


enum ExtraDepKind {
EDK_SanitizeIgnorelist,
EDK_ProfileList,
EDK_ModuleFile,
EDK_DepFileEntry,
};



class DependencyOutputOptions {
public:
unsigned IncludeSystemHeaders : 1;
unsigned ShowHeaderIncludes : 1;
unsigned UsePhonyTargets : 1;


unsigned AddMissingHeaderDeps : 1;
unsigned IncludeModuleFiles : 1;
unsigned ShowSkippedHeaderIncludes : 1;





ShowIncludesDestination ShowIncludesDest = ShowIncludesDestination::None;


DependencyOutputFormat OutputFormat = DependencyOutputFormat::Make;


std::string OutputFile;





std::string HeaderIncludeOutputFile;



std::vector<std::string> Targets;



std::vector<std::pair<std::string, ExtraDepKind>> ExtraDeps;


std::string ShowIncludesPretendHeader;


std::string DOTOutputFile;


std::string ModuleDependencyOutputDir;

public:
DependencyOutputOptions()
: IncludeSystemHeaders(0), ShowHeaderIncludes(0), UsePhonyTargets(0),
AddMissingHeaderDeps(0), IncludeModuleFiles(0),
ShowSkippedHeaderIncludes(0) {}
};

}

#endif
