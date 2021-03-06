













#if !defined(LLVM_CLANG_BASIC_MODULE_H)
#define LLVM_CLANG_BASIC_MODULE_H

#include "clang/Basic/DirectoryEntry.h"
#include "clang/Basic/FileEntry.h"
#include "clang/Basic/SourceLocation.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/DenseSet.h"
#include "llvm/ADT/Optional.h"
#include "llvm/ADT/PointerIntPair.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/ADT/SetVector.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringMap.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/iterator_range.h"
#include <array>
#include <cassert>
#include <cstdint>
#include <ctime>
#include <iterator>
#include <string>
#include <utility>
#include <vector>

namespace llvm {

class raw_ostream;

}

namespace clang {

class FileManager;
class LangOptions;
class TargetInfo;


using ModuleId = SmallVector<std::pair<std::string, SourceLocation>, 2>;


struct ASTFileSignature : std::array<uint8_t, 20> {
using BaseT = std::array<uint8_t, 20>;

static constexpr size_t size = std::tuple_size<BaseT>::value;

ASTFileSignature(BaseT S = {{0}}) : BaseT(std::move(S)) {}

explicit operator bool() const { return *this != BaseT({{0}}); }


uint64_t truncatedValue() const {
uint64_t Value = 0;
static_assert(sizeof(*this) >= sizeof(uint64_t), "No need to truncate.");
for (unsigned I = 0; I < sizeof(uint64_t); ++I)
Value |= static_cast<uint64_t>((*this)[I]) << (I * 8);
return Value;
}

static ASTFileSignature create(StringRef Bytes) {
return create(Bytes.bytes_begin(), Bytes.bytes_end());
}

static ASTFileSignature createDISentinel() {
ASTFileSignature Sentinel;
Sentinel.fill(0xFF);
return Sentinel;
}

template <typename InputIt>
static ASTFileSignature create(InputIt First, InputIt Last) {
assert(std::distance(First, Last) == size &&
"Wrong amount of bytes to create an ASTFileSignature");

ASTFileSignature Signature;
std::copy(First, Last, Signature.begin());
return Signature;
}
};


class Module {
public:

std::string Name;


SourceLocation DefinitionLoc;

enum ModuleKind {


ModuleMapModule,


ModuleInterfaceUnit,


GlobalModuleFragment,


PrivateModuleFragment,
};


ModuleKind Kind = ModuleMapModule;



Module *Parent;




const DirectoryEntry *Directory = nullptr;



std::string PresumedModuleMapFile;


llvm::PointerUnion<const FileEntry *, const DirectoryEntry *> Umbrella;


ASTFileSignature Signature;


std::string UmbrellaAsWritten;


std::string UmbrellaRelativeToRootModuleDirectory;



std::string ExportAsModule;


bool isModulePurview() const {
return Kind == ModuleInterfaceUnit || Kind == PrivateModuleFragment;
}

private:

std::vector<Module *> SubModules;



llvm::StringMap<unsigned> SubModuleIndex;



Optional<FileEntryRef> ASTFile;


llvm::SmallSetVector<const FileEntry *, 2> TopHeaders;


std::vector<std::string> TopHeaderNames;


mutable llvm::DenseSet<const Module*> VisibleModulesCache;


unsigned VisibilityID;

public:
enum HeaderKind {
HK_Normal,
HK_Textual,
HK_Private,
HK_PrivateTextual,
HK_Excluded
};
static const int NumHeaderKinds = HK_Excluded + 1;



struct Header {
std::string NameAsWritten;
std::string PathRelativeToRootModuleDirectory;
const FileEntry *Entry;

explicit operator bool() { return Entry; }
};



struct DirectoryName {
std::string NameAsWritten;
std::string PathRelativeToRootModuleDirectory;
const DirectoryEntry *Entry;

explicit operator bool() { return Entry; }
};


SmallVector<Header, 2> Headers[5];



struct UnresolvedHeaderDirective {
HeaderKind Kind = HK_Normal;
SourceLocation FileNameLoc;
std::string FileName;
bool IsUmbrella = false;
bool HasBuiltinHeader = false;
Optional<off_t> Size;
Optional<time_t> ModTime;
};



SmallVector<UnresolvedHeaderDirective, 1> UnresolvedHeaders;



SmallVector<UnresolvedHeaderDirective, 1> MissingHeaders;



using Requirement = std::pair<std::string, bool>;





SmallVector<Requirement, 2> Requirements;


Module *ShadowingModule = nullptr;




unsigned IsUnimportable : 1;


unsigned HasIncompatibleModuleFile : 1;





unsigned IsAvailable : 1;


unsigned IsFromModuleFile : 1;


unsigned IsFramework : 1;


unsigned IsExplicit : 1;



unsigned IsSystem : 1;




unsigned IsExternC : 1;


unsigned IsInferred : 1;





unsigned InferSubmodules : 1;



unsigned InferExplicitSubmodules : 1;



unsigned InferExportWildcard : 1;






unsigned ConfigMacrosExhaustive : 1;



unsigned NoUndeclaredIncludes : 1;



unsigned ModuleMapIsPrivate : 1;



enum NameVisibilityKind {

Hidden,

AllVisible
};


NameVisibilityKind NameVisibility;


SourceLocation InferredSubmoduleLoc;



llvm::SmallSetVector<Module *, 2> Imports;





using ExportDecl = llvm::PointerIntPair<Module *, 1, bool>;


SmallVector<ExportDecl, 2> Exports;



struct UnresolvedExportDecl {

SourceLocation ExportLoc;


ModuleId Id;




bool Wildcard;
};


SmallVector<UnresolvedExportDecl, 2> UnresolvedExports;


SmallVector<Module *, 2> DirectUses;


SmallVector<ModuleId, 2> UnresolvedDirectUses;



struct LinkLibrary {
LinkLibrary() = default;
LinkLibrary(const std::string &Library, bool IsFramework)
: Library(Library), IsFramework(IsFramework) {}





std::string Library;


bool IsFramework = false;
};



llvm::SmallVector<LinkLibrary, 2> LinkLibraries;



bool UseExportAsModuleLinkName = false;



std::vector<std::string> ConfigMacros;


struct UnresolvedConflict {

ModuleId Id;


std::string Message;
};



std::vector<UnresolvedConflict> UnresolvedConflicts;


struct Conflict {

Module *Other;


std::string Message;
};


std::vector<Conflict> Conflicts;


Module(StringRef Name, SourceLocation DefinitionLoc, Module *Parent,
bool IsFramework, bool IsExplicit, unsigned VisibilityID);

~Module();


bool isUnimportable() const { return IsUnimportable; }














bool isUnimportable(const LangOptions &LangOpts, const TargetInfo &Target,
Requirement &Req, Module *&ShadowingModule) const;



bool isAvailable() const { return IsAvailable; }


















bool isAvailable(const LangOptions &LangOpts,
const TargetInfo &Target,
Requirement &Req,
UnresolvedHeaderDirective &MissingHeader,
Module *&ShadowingModule) const;


bool isSubModule() const { return Parent != nullptr; }







bool isSubModuleOf(const Module *Other) const;




bool isPartOfFramework() const {
for (const Module *Mod = this; Mod; Mod = Mod->Parent)
if (Mod->IsFramework)
return true;

return false;
}



bool isSubFramework() const {
return IsFramework && Parent && Parent->isPartOfFramework();
}



void setParent(Module *M) {
assert(!Parent);
Parent = M;
Parent->SubModuleIndex[Name] = Parent->SubModules.size();
Parent->SubModules.push_back(this);
}





std::string getFullModuleName(bool AllowStringLiterals = false) const;





bool fullModuleNameIs(ArrayRef<StringRef> nameParts) const;



Module *getTopLevelModule() {
return const_cast<Module *>(
const_cast<const Module *>(this)->getTopLevelModule());
}



const Module *getTopLevelModule() const;


StringRef getTopLevelModuleName() const {
return getTopLevelModule()->Name;
}


OptionalFileEntryRefDegradesToFileEntryPtr getASTFile() const {
return getTopLevelModule()->ASTFile;
}


void setASTFile(Optional<FileEntryRef> File) {
assert((!File || !getASTFile() || getASTFile() == File) &&
"file path changed");
getTopLevelModule()->ASTFile = File;
}



DirectoryName getUmbrellaDir() const;



Header getUmbrellaHeader() const {
if (auto *FE = Umbrella.dyn_cast<const FileEntry *>())
return Header{UmbrellaAsWritten, UmbrellaRelativeToRootModuleDirectory,
FE};
return Header{};
}



bool hasUmbrellaDir() const {
return Umbrella && Umbrella.is<const DirectoryEntry *>();
}


void addTopHeader(const FileEntry *File);


void addTopHeaderFilename(StringRef Filename) {
TopHeaderNames.push_back(std::string(Filename));
}


ArrayRef<const FileEntry *> getTopHeaders(FileManager &FileMgr);



bool directlyUses(const Module *Requested) const;















void addRequirement(StringRef Feature, bool RequiredState,
const LangOptions &LangOpts,
const TargetInfo &Target);


void markUnavailable(bool Unimportable);




Module *findSubmodule(StringRef Name) const;
Module *findOrInferSubmodule(StringRef Name);







bool isModuleVisible(const Module *M) const {
if (VisibleModulesCache.empty())
buildVisibleModulesCache();
return VisibleModulesCache.count(M);
}

unsigned getVisibilityID() const { return VisibilityID; }

using submodule_iterator = std::vector<Module *>::iterator;
using submodule_const_iterator = std::vector<Module *>::const_iterator;

submodule_iterator submodule_begin() { return SubModules.begin(); }
submodule_const_iterator submodule_begin() const {return SubModules.begin();}
submodule_iterator submodule_end() { return SubModules.end(); }
submodule_const_iterator submodule_end() const { return SubModules.end(); }

llvm::iterator_range<submodule_iterator> submodules() {
return llvm::make_range(submodule_begin(), submodule_end());
}
llvm::iterator_range<submodule_const_iterator> submodules() const {
return llvm::make_range(submodule_begin(), submodule_end());
}





void getExportedModules(SmallVectorImpl<Module *> &Exported) const;

static StringRef getModuleInputBufferName() {
return "<module-includes>";
}


void print(raw_ostream &OS, unsigned Indent = 0, bool Dump = false) const;


void dump() const;

private:
void buildVisibleModulesCache() const;
};


class VisibleModuleSet {
public:
VisibleModuleSet() = default;
VisibleModuleSet(VisibleModuleSet &&O)
: ImportLocs(std::move(O.ImportLocs)), Generation(O.Generation ? 1 : 0) {
O.ImportLocs.clear();
++O.Generation;
}



VisibleModuleSet &operator=(VisibleModuleSet &&O) {
ImportLocs = std::move(O.ImportLocs);
O.ImportLocs.clear();
++O.Generation;
++Generation;
return *this;
}



unsigned getGeneration() const { return Generation; }


bool isVisible(const Module *M) const {
return getImportLoc(M).isValid();
}


SourceLocation getImportLoc(const Module *M) const {
return M->getVisibilityID() < ImportLocs.size()
? ImportLocs[M->getVisibilityID()]
: SourceLocation();
}



using VisibleCallback = llvm::function_ref<void(Module *M)>;




using ConflictCallback =
llvm::function_ref<void(ArrayRef<Module *> Path, Module *Conflict,
StringRef Message)>;


void setVisible(Module *M, SourceLocation Loc,
VisibleCallback Vis = [](Module *) {},
ConflictCallback Cb = [](ArrayRef<Module *>, Module *,
StringRef) {});

private:


std::vector<SourceLocation> ImportLocs;


unsigned Generation = 0;
};




class ASTSourceDescriptor {
StringRef PCHModuleName;
StringRef Path;
StringRef ASTFile;
ASTFileSignature Signature;
Module *ClangModule = nullptr;

public:
ASTSourceDescriptor() = default;
ASTSourceDescriptor(StringRef Name, StringRef Path, StringRef ASTFile,
ASTFileSignature Signature)
: PCHModuleName(std::move(Name)), Path(std::move(Path)),
ASTFile(std::move(ASTFile)), Signature(Signature) {}
ASTSourceDescriptor(Module &M);

std::string getModuleName() const;
StringRef getPath() const { return Path; }
StringRef getASTFile() const { return ASTFile; }
ASTFileSignature getSignature() const { return Signature; }
Module *getModuleOrNull() const { return ClangModule; }
};


}

#endif
