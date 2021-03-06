







#if !defined(LLVM_CLANG_LEX_HEADERSEARCHOPTIONS_H)
#define LLVM_CLANG_LEX_HEADERSEARCHOPTIONS_H

#include "clang/Basic/LLVM.h"
#include "llvm/ADT/CachedHashString.h"
#include "llvm/ADT/Hashing.h"
#include "llvm/ADT/SetVector.h"
#include "llvm/ADT/StringRef.h"
#include <cstdint>
#include <string>
#include <vector>
#include <map>

namespace clang {

namespace frontend {






enum IncludeDirGroup {

Quoted = 0,


Angled,


IndexHeaderMap,


System,


ExternCSystem,


CSystem,


CXXSystem,


ObjCSystem,


ObjCXXSystem,


After
};

}



class HeaderSearchOptions {
public:
struct Entry {
std::string Path;
frontend::IncludeDirGroup Group;
unsigned IsFramework : 1;




unsigned IgnoreSysRoot : 1;

Entry(StringRef path, frontend::IncludeDirGroup group, bool isFramework,
bool ignoreSysRoot)
: Path(path), Group(group), IsFramework(isFramework),
IgnoreSysRoot(ignoreSysRoot) {}
};

struct SystemHeaderPrefix {

std::string Prefix;



bool IsSystemHeader;

SystemHeaderPrefix(StringRef Prefix, bool IsSystemHeader)
: Prefix(Prefix), IsSystemHeader(IsSystemHeader) {}
};



std::string Sysroot;


std::vector<Entry> UserEntries;


std::vector<SystemHeaderPrefix> SystemHeaderPrefixes;



std::string ResourceDir;


std::string ModuleCachePath;


std::string ModuleUserBuildPath;


std::map<std::string, std::string, std::less<>> PrebuiltModuleFiles;


std::vector<std::string> PrebuiltModulePaths;


std::string ModuleFormat;





unsigned DisableModuleHash : 1;



unsigned ImplicitModuleMaps : 1;








unsigned ModuleMapFileHomeIsCwd : 1;



unsigned EnablePrebuiltImplicitModules : 1;








unsigned ModuleCachePruneInterval = 7 * 24 * 60 * 60;








unsigned ModuleCachePruneAfter = 31 * 24 * 60 * 60;





uint64_t BuildSessionTimestamp = 0;



llvm::SmallSetVector<llvm::CachedHashString, 16> ModulesIgnoreMacros;


std::vector<std::string> VFSOverlayFiles;


unsigned UseBuiltinIncludes : 1;


unsigned UseStandardSystemIncludes : 1;


unsigned UseStandardCXXIncludes : 1;


unsigned UseLibcxx : 1;


unsigned Verbose : 1;




unsigned ModulesValidateOncePerBuildSession : 1;


unsigned ModulesValidateSystemHeaders : 1;



unsigned ValidateASTInputFilesContent : 1;


unsigned UseDebugInfo : 1;

unsigned ModulesValidateDiagnosticOptions : 1;

unsigned ModulesHashContent : 1;






unsigned ModulesStrictContextHash : 1;

HeaderSearchOptions(StringRef _Sysroot = "/")
: Sysroot(_Sysroot), ModuleFormat("raw"), DisableModuleHash(false),
ImplicitModuleMaps(false), ModuleMapFileHomeIsCwd(false),
EnablePrebuiltImplicitModules(false), UseBuiltinIncludes(true),
UseStandardSystemIncludes(true), UseStandardCXXIncludes(true),
UseLibcxx(false), Verbose(false),
ModulesValidateOncePerBuildSession(false),
ModulesValidateSystemHeaders(false),
ValidateASTInputFilesContent(false), UseDebugInfo(false),
ModulesValidateDiagnosticOptions(true), ModulesHashContent(false),
ModulesStrictContextHash(false) {}


void AddPath(StringRef Path, frontend::IncludeDirGroup Group,
bool IsFramework, bool IgnoreSysRoot) {
UserEntries.emplace_back(Path, Group, IsFramework, IgnoreSysRoot);
}




void AddSystemHeaderPrefix(StringRef Prefix, bool IsSystemHeader) {
SystemHeaderPrefixes.emplace_back(Prefix, IsSystemHeader);
}

void AddVFSOverlayFile(StringRef Name) {
VFSOverlayFiles.push_back(std::string(Name));
}

void AddPrebuiltModulePath(StringRef Name) {
PrebuiltModulePaths.push_back(std::string(Name));
}
};

inline llvm::hash_code hash_value(const HeaderSearchOptions::Entry &E) {
return llvm::hash_combine(E.Path, E.Group, E.IsFramework, E.IgnoreSysRoot);
}

inline llvm::hash_code
hash_value(const HeaderSearchOptions::SystemHeaderPrefix &SHP) {
return llvm::hash_combine(SHP.Prefix, SHP.IsSystemHeader);
}

}

#endif
