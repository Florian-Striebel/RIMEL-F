












#if !defined(LLVM_CLANG_LEX_MODULELOADER_H)
#define LLVM_CLANG_LEX_MODULELOADER_H

#include "clang/Basic/LLVM.h"
#include "clang/Basic/Module.h"
#include "clang/Basic/SourceLocation.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/PointerIntPair.h"
#include "llvm/ADT/StringRef.h"
#include <utility>

namespace clang {

class GlobalModuleIndex;
class IdentifierInfo;



using ModuleIdPath = ArrayRef<std::pair<IdentifierInfo *, SourceLocation>>;


class ModuleLoadResult {
public:
enum LoadResultKind {

Normal,




MissingExpected,


ConfigMismatch,
};
llvm::PointerIntPair<Module *, 2, LoadResultKind> Storage;

ModuleLoadResult() = default;
ModuleLoadResult(Module *M) : Storage(M, Normal) {}
ModuleLoadResult(LoadResultKind Kind) : Storage(nullptr, Kind) {}

operator Module *() const { return Storage.getPointer(); }



bool isNormal() const { return Storage.getInt() == Normal; }





bool isMissingExpected() const { return Storage.getInt() == MissingExpected; }



bool isConfigMismatch() const { return Storage.getInt() == ConfigMismatch; }
};






class ModuleLoader {

bool BuildingModule;

public:
explicit ModuleLoader(bool BuildingModule = false)
: BuildingModule(BuildingModule) {}

virtual ~ModuleLoader();


bool buildingModule() const {
return BuildingModule;
}


void setBuildingModule(bool BuildingModuleFlag) {
BuildingModule = BuildingModuleFlag;
}





















virtual ModuleLoadResult loadModule(SourceLocation ImportLoc,
ModuleIdPath Path,
Module::NameVisibilityKind Visibility,
bool IsInclusionDirective) = 0;








virtual void createModuleFromSource(SourceLocation Loc, StringRef ModuleName,
StringRef Source) = 0;


virtual void makeModuleVisible(Module *Mod,
Module::NameVisibilityKind Visibility,
SourceLocation ImportLoc) = 0;











virtual GlobalModuleIndex *loadGlobalModuleIndex(
SourceLocation TriggerLoc) = 0;





virtual bool lookupMissingImports(StringRef Name,
SourceLocation TriggerLoc) = 0;

bool HadFatalFailure = false;
};


class TrivialModuleLoader : public ModuleLoader {
public:
ModuleLoadResult loadModule(SourceLocation ImportLoc, ModuleIdPath Path,
Module::NameVisibilityKind Visibility,
bool IsInclusionDirective) override {
return {};
}

void createModuleFromSource(SourceLocation ImportLoc, StringRef ModuleName,
StringRef Source) override {}

void makeModuleVisible(Module *Mod, Module::NameVisibilityKind Visibility,
SourceLocation ImportLoc) override {}

GlobalModuleIndex *loadGlobalModuleIndex(SourceLocation TriggerLoc) override {
return nullptr;
}

bool lookupMissingImports(StringRef Name,
SourceLocation TriggerLoc) override {
return false;
}
};

}

#endif
