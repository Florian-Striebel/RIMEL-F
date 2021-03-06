












#if !defined(LLVM_CLANG_LEX_MODULEMAP_H)
#define LLVM_CLANG_LEX_MODULEMAP_H

#include "clang/Basic/IdentifierTable.h"
#include "clang/Basic/LangOptions.h"
#include "clang/Basic/Module.h"
#include "clang/Basic/SourceLocation.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/PointerIntPair.h"
#include "llvm/ADT/SmallPtrSet.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringMap.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/StringSet.h"
#include "llvm/ADT/TinyPtrVector.h"
#include "llvm/ADT/Twine.h"
#include <ctime>
#include <memory>
#include <string>
#include <utility>

namespace clang {

class DiagnosticsEngine;
class DirectoryEntry;
class FileEntry;
class FileManager;
class HeaderSearch;
class SourceManager;



class ModuleMapCallbacks {
virtual void anchor();

public:
virtual ~ModuleMapCallbacks() = default;







virtual void moduleMapFileRead(SourceLocation FileStart,
const FileEntry &File, bool IsSystem) {}




virtual void moduleMapAddHeader(StringRef Filename) {}





virtual void moduleMapAddUmbrellaHeader(FileManager *FileMgr,
const FileEntry *Header) {}
};

class ModuleMap {
SourceManager &SourceMgr;
DiagnosticsEngine &Diags;
const LangOptions &LangOpts;
const TargetInfo *Target;
HeaderSearch &HeaderInfo;

llvm::SmallVector<std::unique_ptr<ModuleMapCallbacks>, 1> Callbacks;



const DirectoryEntry *BuiltinIncludeDir = nullptr;




LangOptions MMapLangOpts;



Module *SourceModule = nullptr;



llvm::SmallVector<std::unique_ptr<Module>, 8> PendingSubmodules;


llvm::StringMap<Module *> Modules;



llvm::DenseMap<const IdentifierInfo *, Module *> CachedModuleLoads;


llvm::SmallVector<Module*, 2> ShadowModules;


unsigned NumCreatedModules = 0;



llvm::StringMap<llvm::StringSet<>> PendingLinkAsModule;

public:


void resolveLinkAsDependencies(Module *Mod);



void addLinkAsDependency(Module *Mod);


enum ModuleHeaderRole {

NormalHeader = 0x0,


PrivateHeader = 0x1,



TextualHeader = 0x2,







};


static ModuleHeaderRole headerKindToRole(Module::HeaderKind Kind);


static Module::HeaderKind headerRoleToKind(ModuleHeaderRole Role);



class KnownHeader {
llvm::PointerIntPair<Module *, 2, ModuleHeaderRole> Storage;

public:
KnownHeader() : Storage(nullptr, NormalHeader) {}
KnownHeader(Module *M, ModuleHeaderRole Role) : Storage(M, Role) {}

friend bool operator==(const KnownHeader &A, const KnownHeader &B) {
return A.Storage == B.Storage;
}
friend bool operator!=(const KnownHeader &A, const KnownHeader &B) {
return A.Storage != B.Storage;
}


Module *getModule() const { return Storage.getPointer(); }


ModuleHeaderRole getRole() const { return Storage.getInt(); }


bool isAvailable() const {
return getModule()->isAvailable();
}


bool isAccessibleFrom(Module *M) const {
return !(getRole() & PrivateHeader) ||
(M && M->getTopLevelModule() == getModule()->getTopLevelModule());
}



explicit operator bool() const {
return Storage.getPointer() != nullptr;
}
};

using AdditionalModMapsSet = llvm::SmallPtrSet<const FileEntry *, 1>;

private:
friend class ModuleMapParser;

using HeadersMap =
llvm::DenseMap<const FileEntry *, SmallVector<KnownHeader, 1>>;



HeadersMap Headers;


mutable llvm::DenseMap<off_t, llvm::TinyPtrVector<Module*>> LazyHeadersBySize;


mutable llvm::DenseMap<time_t, llvm::TinyPtrVector<Module*>>
LazyHeadersByModTime;







llvm::DenseMap<const DirectoryEntry *, Module *> UmbrellaDirs;






unsigned CurrentModuleScopeID = 0;

llvm::DenseMap<Module *, unsigned> ModuleScopeIDs;


struct Attributes {

unsigned IsSystem : 1;


unsigned IsExternC : 1;


unsigned IsExhaustive : 1;



unsigned NoUndeclaredIncludes : 1;

Attributes()
: IsSystem(false), IsExternC(false), IsExhaustive(false),
NoUndeclaredIncludes(false) {}
};


struct InferredDirectory {

unsigned InferModules : 1;


Attributes Attrs;



const FileEntry *ModuleMapFile;



SmallVector<std::string, 2> ExcludedModules;

InferredDirectory() : InferModules(false) {}
};



llvm::DenseMap<const DirectoryEntry *, InferredDirectory> InferredDirectories;



llvm::DenseMap<const Module *, const FileEntry *> InferredModuleAllowedBy;

llvm::DenseMap<const Module *, AdditionalModMapsSet> AdditionalModMaps;



llvm::DenseMap<const FileEntry *, bool> ParsedModuleMap;













Module::ExportDecl
resolveExport(Module *Mod, const Module::UnresolvedExportDecl &Unresolved,
bool Complain) const;












Module *resolveModuleId(const ModuleId &Id, Module *Mod, bool Complain) const;








void addUnresolvedHeader(Module *Mod,
Module::UnresolvedHeaderDirective Header,
bool &NeedsFramework);










Optional<FileEntryRef>
findHeader(Module *M, const Module::UnresolvedHeaderDirective &Header,
SmallVectorImpl<char> &RelativePathName, bool &NeedsFramework);







void resolveHeader(Module *M, const Module::UnresolvedHeaderDirective &Header,
bool &NeedsFramework);




bool resolveAsBuiltinHeader(Module *M,
const Module::UnresolvedHeaderDirective &Header);






HeadersMap::iterator findKnownHeader(const FileEntry *File);







KnownHeader findHeaderInUmbrellaDirs(const FileEntry *File,
SmallVectorImpl<const DirectoryEntry *> &IntermediateDirs);



KnownHeader findOrCreateModuleForHeaderInUmbrellaDir(const FileEntry *File);



bool isHeaderInUmbrellaDirs(const FileEntry *File) {
SmallVector<const DirectoryEntry *, 2> IntermediateDirs;
return static_cast<bool>(findHeaderInUmbrellaDirs(File, IntermediateDirs));
}

Module *inferFrameworkModule(const DirectoryEntry *FrameworkDir,
Attributes Attrs, Module *Parent);

public:











ModuleMap(SourceManager &SourceMgr, DiagnosticsEngine &Diags,
const LangOptions &LangOpts, const TargetInfo *Target,
HeaderSearch &HeaderInfo);


~ModuleMap();


void setTarget(const TargetInfo &Target);



void setBuiltinIncludeDir(const DirectoryEntry *Dir) {
BuiltinIncludeDir = Dir;
}


const DirectoryEntry *getBuiltinDir() const {
return BuiltinIncludeDir;
}


static bool isBuiltinHeader(StringRef FileName);
bool isBuiltinHeader(const FileEntry *File);


void addModuleMapCallbacks(std::unique_ptr<ModuleMapCallbacks> Callback) {
Callbacks.push_back(std::move(Callback));
}















KnownHeader findModuleForHeader(const FileEntry *File,
bool AllowTextual = false);








ArrayRef<KnownHeader> findAllModulesForHeader(const FileEntry *File);



ArrayRef<KnownHeader>
findResolvedModulesForHeader(const FileEntry *File) const;





void resolveHeaderDirectives(const FileEntry *File) const;


void resolveHeaderDirectives(Module *Mod) const;















void diagnoseHeaderInclusion(Module *RequestingModule,
bool RequestingModuleIsModuleInterface,
SourceLocation FilenameLoc, StringRef Filename,
const FileEntry *File);



bool isHeaderInUnavailableModule(const FileEntry *Header) const;



bool isHeaderUnavailableInModule(const FileEntry *Header,
const Module *RequestingModule) const;






Module *findModule(StringRef Name) const;










Module *lookupModuleUnqualified(StringRef Name, Module *Context) const;










Module *lookupModuleQualified(StringRef Name, Module *Context) const;















std::pair<Module *, bool> findOrCreateModule(StringRef Name, Module *Parent,
bool IsFramework,
bool IsExplicit);






Module *createGlobalModuleFragmentForModuleUnit(SourceLocation Loc);


Module *createPrivateModuleFragmentForInterfaceUnit(Module *Parent,
SourceLocation Loc);








Module *createModuleForInterfaceUnit(SourceLocation Loc, StringRef Name,
Module *GlobalModule);


Module *createHeaderModule(StringRef Name, ArrayRef<Module::Header> Headers);



Module *inferFrameworkModule(const DirectoryEntry *FrameworkDir,
bool IsSystem, Module *Parent);



Module *createShadowedModule(StringRef Name, bool IsFramework,
Module *ShadowingModule);






void finishModuleDeclarationScope() { CurrentModuleScopeID += 1; }

bool mayShadowNewModule(Module *ExistingModule) {
assert(!ExistingModule->Parent && "expected top-level module");
assert(ModuleScopeIDs.count(ExistingModule) && "unknown module");
return ModuleScopeIDs[ExistingModule] < CurrentModuleScopeID;
}








const FileEntry *getContainingModuleMapFile(const Module *Module) const;










const FileEntry *getModuleMapFileForUniquing(const Module *M) const;

void setInferredModuleAllowedBy(Module *M, const FileEntry *ModMap);





AdditionalModMapsSet *getAdditionalModuleMapFiles(const Module *M) {
auto I = AdditionalModMaps.find(M);
if (I == AdditionalModMaps.end())
return nullptr;
return &I->second;
}

void addAdditionalModuleMapFile(const Module *M, const FileEntry *ModuleMap);









bool resolveExports(Module *Mod, bool Complain);









bool resolveUses(Module *Mod, bool Complain);









bool resolveConflicts(Module *Mod, bool Complain);



void setUmbrellaHeader(Module *Mod, const FileEntry *UmbrellaHeader,
const Twine &NameAsWritten,
const Twine &PathRelativeToRootModuleDirectory);



void setUmbrellaDir(Module *Mod, const DirectoryEntry *UmbrellaDir,
const Twine &NameAsWritten,
const Twine &PathRelativeToRootModuleDirectory);



void addHeader(Module *Mod, Module::Header Header,
ModuleHeaderRole Role, bool Imported = false);


void excludeHeader(Module *Mod, Module::Header Header);





















bool parseModuleMapFile(const FileEntry *File, bool IsSystem,
const DirectoryEntry *HomeDir,
FileID ID = FileID(), unsigned *Offset = nullptr,
SourceLocation ExternModuleLoc = SourceLocation());


void dump();

using module_iterator = llvm::StringMap<Module *>::const_iterator;

module_iterator module_begin() const { return Modules.begin(); }
module_iterator module_end() const { return Modules.end(); }
llvm::iterator_range<module_iterator> modules() const {
return {module_begin(), module_end()};
}


void cacheModuleLoad(const IdentifierInfo &II, Module *M) {
CachedModuleLoads[&II] = M;
}


llvm::Optional<Module *> getCachedModuleLoad(const IdentifierInfo &II) {
auto I = CachedModuleLoads.find(&II);
if (I == CachedModuleLoads.end())
return None;
return I->second;
}
};

}

#endif
