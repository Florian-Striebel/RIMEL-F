








#if !defined(LLVM_CLANG_TOOLING_DEPENDENCY_SCANNING_MODULE_DEP_COLLECTOR_H)
#define LLVM_CLANG_TOOLING_DEPENDENCY_SCANNING_MODULE_DEP_COLLECTOR_H

#include "clang/Basic/LLVM.h"
#include "clang/Basic/SourceManager.h"
#include "clang/Frontend/CompilerInvocation.h"
#include "clang/Frontend/Utils.h"
#include "clang/Lex/HeaderSearch.h"
#include "clang/Lex/PPCallbacks.h"
#include "clang/Serialization/ASTReader.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/StringSet.h"
#include "llvm/Support/raw_ostream.h"
#include <string>
#include <unordered_map>

namespace clang {
namespace tooling {
namespace dependencies {

class DependencyConsumer;


struct PrebuiltModuleDep {
std::string ModuleName;
std::string PCMFile;
std::string ModuleMapFile;

explicit PrebuiltModuleDep(const Module *M)
: ModuleName(M->getTopLevelModuleName()),
PCMFile(M->getASTFile()->getName()),
ModuleMapFile(M->PresumedModuleMapFile) {}
};


struct ModuleID {


std::string ModuleName;







std::string ContextHash;

bool operator==(const ModuleID &Other) const {
return ModuleName == Other.ModuleName && ContextHash == Other.ContextHash;
}
};

struct ModuleIDHasher {
std::size_t operator()(const ModuleID &MID) const {
return llvm::hash_combine(MID.ModuleName, MID.ContextHash);
}
};

struct ModuleDeps {

ModuleID ID;


bool IsSystem;





std::string ClangModuleMapFile;


std::string ImplicitModulePCMPath;



llvm::StringSet<> FileDeps;



std::vector<PrebuiltModuleDep> PrebuiltModuleDeps;






std::vector<ModuleID> ClangModuleDeps;



bool ImportedByMainFile = false;


CompilerInvocation Invocation;











std::vector<std::string> getCanonicalCommandLine(
std::function<StringRef(ModuleID)> LookupPCMPath,
std::function<const ModuleDeps &(ModuleID)> LookupModuleDeps) const;




std::vector<std::string> getCanonicalCommandLineWithoutModulePaths() const;
};

namespace detail {


void collectPCMAndModuleMapPaths(
llvm::ArrayRef<ModuleID> Modules,
std::function<StringRef(ModuleID)> LookupPCMPath,
std::function<const ModuleDeps &(ModuleID)> LookupModuleDeps,
std::vector<std::string> &PCMPaths, std::vector<std::string> &ModMapPaths);
}

class ModuleDepCollector;





class ModuleDepCollectorPP final : public PPCallbacks {
public:
ModuleDepCollectorPP(CompilerInstance &I, ModuleDepCollector &MDC)
: Instance(I), MDC(MDC) {}

void FileChanged(SourceLocation Loc, FileChangeReason Reason,
SrcMgr::CharacteristicKind FileType,
FileID PrevFID) override;
void InclusionDirective(SourceLocation HashLoc, const Token &IncludeTok,
StringRef FileName, bool IsAngled,
CharSourceRange FilenameRange, const FileEntry *File,
StringRef SearchPath, StringRef RelativePath,
const Module *Imported,
SrcMgr::CharacteristicKind FileType) override;
void moduleImport(SourceLocation ImportLoc, ModuleIdPath Path,
const Module *Imported) override;

void EndOfMainFile() override;

private:

CompilerInstance &Instance;

ModuleDepCollector &MDC;

llvm::DenseSet<const Module *> DirectModularDeps;

llvm::DenseSet<const Module *> DirectPrebuiltModularDeps;

void handleImport(const Module *Imported);



void addDirectPrebuiltModuleDeps(const Module *M, ModuleDeps &MD);




ModuleID handleTopLevelModule(const Module *M);
void addAllSubmoduleDeps(const Module *M, ModuleDeps &MD,
llvm::DenseSet<const Module *> &AddedModules);
void addModuleDep(const Module *M, ModuleDeps &MD,
llvm::DenseSet<const Module *> &AddedModules);
};



class ModuleDepCollector final : public DependencyCollector {
public:
ModuleDepCollector(std::unique_ptr<DependencyOutputOptions> Opts,
CompilerInstance &I, DependencyConsumer &C,
CompilerInvocation &&OriginalCI);

void attachToPreprocessor(Preprocessor &PP) override;
void attachToASTReader(ASTReader &R) override;

private:
friend ModuleDepCollectorPP;


CompilerInstance &Instance;

DependencyConsumer &Consumer;

std::string MainFile;

std::string ContextHash;


std::vector<std::string> FileDeps;

std::unordered_map<const Module *, ModuleDeps> ModularDeps;

std::unique_ptr<DependencyOutputOptions> Opts;

CompilerInvocation OriginalInvocation;


bool isPrebuiltModule(const Module *M);




CompilerInvocation
makeInvocationForModuleBuildWithoutPaths(const ModuleDeps &Deps) const;
};

}
}
}

#endif
