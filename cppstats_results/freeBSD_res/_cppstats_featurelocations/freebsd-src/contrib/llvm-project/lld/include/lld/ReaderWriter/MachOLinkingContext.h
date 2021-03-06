







#if !defined(LLD_READER_WRITER_MACHO_LINKING_CONTEXT_H)
#define LLD_READER_WRITER_MACHO_LINKING_CONTEXT_H

#include "lld/Core/LinkingContext.h"
#include "lld/Core/Reader.h"
#include "lld/Core/Writer.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/ADT/StringMap.h"
#include "llvm/ADT/StringSet.h"
#include "llvm/BinaryFormat/MachO.h"
#include "llvm/Support/ErrorHandling.h"
#include <set>

using llvm::MachO::HeaderFileType;

namespace lld {

namespace mach_o {
class ArchHandler;
class MachODylibFile;
class MachOFile;
class SectCreateFile;
}

class MachOLinkingContext : public LinkingContext {
public:
MachOLinkingContext();
~MachOLinkingContext() override;

enum Arch {
arch_unknown,
arch_ppc,
arch_x86,
arch_x86_64,
arch_armv6,
arch_armv7,
arch_armv7s,
arch_arm64,
};

enum class OS {
unknown,
macOSX,
iOS,
iOS_simulator
};

enum class ExportMode {
globals,
exported,
unexported
};

enum class DebugInfoMode {
addDebugMap,
noDebugMap
};

enum class UndefinedMode {
error,
warning,
suppress,
dynamicLookup
};

enum ObjCConstraint {
objc_unknown = 0,
objc_supports_gc = 2,
objc_gc_only = 4,


objc_retainReleaseForSimulator = 32,
objc_retainRelease
};




void configure(HeaderFileType type, Arch arch, OS os, uint32_t minOSVersion,
bool exportDynamicSymbols);

void addPasses(PassManager &pm) override;
bool validateImpl() override;
std::string demangle(StringRef symbolName) const override;

void createImplicitFiles(std::vector<std::unique_ptr<File>> &) override;



template <class T, class... Args>
typename std::enable_if<!std::is_array<T>::value, T *>::type
make_file(Args &&... args) const {
auto file = std::unique_ptr<T>(new T(std::forward<Args>(args)...));
auto *filePtr = file.get();
auto *ctx = const_cast<MachOLinkingContext *>(this);
ctx->getNodes().push_back(std::make_unique<FileNode>(std::move(file)));
return filePtr;
}

uint32_t getCPUType() const;
uint32_t getCPUSubType() const;

bool addEntryPointLoadCommand() const;
bool addUnixThreadLoadCommand() const;
bool outputTypeHasEntry() const;
bool is64Bit() const;

virtual uint64_t pageZeroSize() const { return _pageZeroSize; }
virtual uint64_t pageSize() const { return _pageSize; }

mach_o::ArchHandler &archHandler() const;

HeaderFileType outputMachOType() const { return _outputMachOType; }

Arch arch() const { return _arch; }
StringRef archName() const { return nameFromArch(_arch); }
OS os() const { return _os; }

ExportMode exportMode() const { return _exportMode; }
void setExportMode(ExportMode mode) { _exportMode = mode; }
void addExportSymbol(StringRef sym);
bool exportRestrictMode() const { return _exportMode != ExportMode::globals; }
bool exportSymbolNamed(StringRef sym) const;

DebugInfoMode debugInfoMode() const { return _debugInfoMode; }
void setDebugInfoMode(DebugInfoMode mode) {
_debugInfoMode = mode;
}

void appendOrderedSymbol(StringRef symbol, StringRef filename);

bool keepPrivateExterns() const { return _keepPrivateExterns; }
void setKeepPrivateExterns(bool v) { _keepPrivateExterns = v; }
bool demangleSymbols() const { return _demangle; }
void setDemangleSymbols(bool d) { _demangle = d; }
bool mergeObjCCategories() const { return _mergeObjCCategories; }
void setMergeObjCCategories(bool v) { _mergeObjCCategories = v; }


std::error_code createDependencyFile(StringRef path);
void addInputFileDependency(StringRef path) const;
void addInputFileNotFound(StringRef path) const;
void addOutputFileDependency(StringRef path) const;

bool minOS(StringRef mac, StringRef iOS) const;
void setDoNothing(bool value) { _doNothing = value; }
bool doNothing() const { return _doNothing; }
bool printAtoms() const { return _printAtoms; }
bool testingFileUsage() const { return _testingFileUsage; }
const StringRefVector &searchDirs() const { return _searchDirs; }
const StringRefVector &frameworkDirs() const { return _frameworkDirs; }
void setSysLibRoots(const StringRefVector &paths);
const StringRefVector &sysLibRoots() const { return _syslibRoots; }
bool PIE() const { return _pie; }
void setPIE(bool pie) { _pie = pie; }
bool generateVersionLoadCommand() const {
return _generateVersionLoadCommand;
}
void setGenerateVersionLoadCommand(bool v) {
_generateVersionLoadCommand = v;
}

bool generateFunctionStartsLoadCommand() const {
return _generateFunctionStartsLoadCommand;
}
void setGenerateFunctionStartsLoadCommand(bool v) {
_generateFunctionStartsLoadCommand = v;
}

bool generateDataInCodeLoadCommand() const {
return _generateDataInCodeLoadCommand;
}
void setGenerateDataInCodeLoadCommand(bool v) {
_generateDataInCodeLoadCommand = v;
}

uint64_t stackSize() const { return _stackSize; }
void setStackSize(uint64_t stackSize) { _stackSize = stackSize; }

uint64_t baseAddress() const { return _baseAddress; }
void setBaseAddress(uint64_t baseAddress) { _baseAddress = baseAddress; }

ObjCConstraint objcConstraint() const { return _objcConstraint; }

uint32_t osMinVersion() const { return _osMinVersion; }

uint32_t sdkVersion() const { return _sdkVersion; }
void setSdkVersion(uint64_t v) { _sdkVersion = v; }

uint64_t sourceVersion() const { return _sourceVersion; }
void setSourceVersion(uint64_t v) { _sourceVersion = v; }

uint32_t swiftVersion() const { return _swiftVersion; }






bool pathExists(StringRef path) const;


bool fileExists(StringRef path) const;







void addModifiedSearchDir(StringRef libPath, bool isSystemPath = false);







llvm::Optional<StringRef> searchDirForLibrary(StringRef path,
StringRef libName) const;



llvm::Optional<StringRef> searchLibrary(StringRef libName) const;



void addFrameworkSearchDir(StringRef fwPath, bool isSystemPath = false);



llvm::Optional<StringRef> findPathForFramework(StringRef fwName) const;








uint32_t compatibilityVersion() const { return _compatibilityVersion; }






uint32_t currentVersion() const { return _currentVersion; }






StringRef installName() const { return _installName; }






bool deadStrippableDylib() const { return _deadStrippableDylib; }











bool useFlatNamespace() const { return _flatNamespace; }











UndefinedMode undefinedMode() const { return _undefinedMode; }







StringRef bundleLoader() const { return _bundleLoader; }

void setCompatibilityVersion(uint32_t vers) { _compatibilityVersion = vers; }
void setCurrentVersion(uint32_t vers) { _currentVersion = vers; }
void setInstallName(StringRef name) { _installName = name; }
void setDeadStrippableDylib(bool deadStrippable) {
_deadStrippableDylib = deadStrippable;
}
void setUseFlatNamespace(bool flatNamespace) {
_flatNamespace = flatNamespace;
}

void setUndefinedMode(UndefinedMode undefinedMode) {
_undefinedMode = undefinedMode;
}

void setBundleLoader(StringRef loader) { _bundleLoader = loader; }
void setPrintAtoms(bool value=true) { _printAtoms = value; }
void setTestingFileUsage(bool value = true) {
_testingFileUsage = value;
}
void addExistingPathForDebug(StringRef path) {
_existingPaths.insert(path);
}

void addRpath(StringRef rpath);
const StringRefVector &rpaths() const { return _rpaths; }


void addSectionAlignment(StringRef seg, StringRef sect, uint16_t align);


void addSectCreateSection(StringRef seg, StringRef sect,
std::unique_ptr<MemoryBuffer> content);


bool sectionAligned(StringRef seg, StringRef sect, uint16_t &align) const;

StringRef dyldPath() const { return "/usr/lib/dyld"; }


bool needsStubsPass() const;


bool needsGOTPass() const;


bool needsTLVPass() const;


bool needsCompactUnwindPass() const;


bool needsShimPass() const;


bool needsObjCPass() const;


StringRef binderSymbolName() const;


void registerDylib(mach_o::MachODylibFile *dylib, bool upward) const;



ErrorOr<std::unique_ptr<MemoryBuffer>> getMemoryBuffer(StringRef path);




mach_o::MachODylibFile* findIndirectDylib(StringRef path);

uint32_t dylibCurrentVersion(StringRef installName) const;

uint32_t dylibCompatVersion(StringRef installName) const;

ArrayRef<mach_o::MachODylibFile*> allDylibs() const {
return _allDylibs;
}


StringRef copy(StringRef str) { return str.copy(_allocator); }



bool sliceFromFatFile(MemoryBufferRef mb, uint32_t &offset, uint32_t &size);


bool isUpwardDylib(StringRef installName) const;

static bool isThinObjectFile(StringRef path, Arch &arch);
static Arch archFromCpuType(uint32_t cputype, uint32_t cpusubtype);
static Arch archFromName(StringRef archName);
static StringRef nameFromArch(Arch arch);
static uint32_t cpuTypeFromArch(Arch arch);
static uint32_t cpuSubtypeFromArch(Arch arch);
static bool is64Bit(Arch arch);
static bool isHostEndian(Arch arch);
static bool isBigEndian(Arch arch);



static bool parsePackedVersion(StringRef str, uint32_t &result);



static bool parsePackedVersion(StringRef str, uint64_t &result);

void finalizeInputFiles() override;

llvm::Error handleLoadedFile(File &file) override;

bool customAtomOrderer(const DefinedAtom *left, const DefinedAtom *right,
bool &leftBeforeRight) const;




File* flatNamespaceFile() const { return _flatNamespaceFile; }

private:
Writer &writer() const override;
mach_o::MachODylibFile* loadIndirectDylib(StringRef path);
struct ArchInfo {
StringRef archName;
MachOLinkingContext::Arch arch;
bool littleEndian;
uint32_t cputype;
uint32_t cpusubtype;
};

struct SectionAlign {
StringRef segmentName;
StringRef sectionName;
uint16_t align;
};

struct OrderFileNode {
StringRef fileFilter;
unsigned order;
};

static bool findOrderOrdinal(const std::vector<OrderFileNode> &nodes,
const DefinedAtom *atom, unsigned &ordinal);

static ArchInfo _s_archInfos[];

std::set<StringRef> _existingPaths;
StringRefVector _searchDirs;
StringRefVector _syslibRoots;
StringRefVector _frameworkDirs;
HeaderFileType _outputMachOType = llvm::MachO::MH_EXECUTE;
bool _outputMachOTypeStatic = false;
bool _doNothing = false;
bool _pie = false;
Arch _arch = arch_unknown;
OS _os = OS::macOSX;
uint32_t _osMinVersion = 0;
uint32_t _sdkVersion = 0;
uint64_t _sourceVersion = 0;
uint64_t _pageZeroSize = 0;
uint64_t _pageSize = 4096;
uint64_t _baseAddress = 0;
uint64_t _stackSize = 0;
uint32_t _compatibilityVersion = 0;
uint32_t _currentVersion = 0;
ObjCConstraint _objcConstraint = objc_unknown;
uint32_t _swiftVersion = 0;
StringRef _installName;
StringRefVector _rpaths;
bool _flatNamespace = false;
UndefinedMode _undefinedMode = UndefinedMode::error;
bool _deadStrippableDylib = false;
bool _printAtoms = false;
bool _testingFileUsage = false;
bool _keepPrivateExterns = false;
bool _demangle = false;
bool _mergeObjCCategories = true;
bool _generateVersionLoadCommand = false;
bool _generateFunctionStartsLoadCommand = false;
bool _generateDataInCodeLoadCommand = false;
StringRef _bundleLoader;
mutable std::unique_ptr<mach_o::ArchHandler> _archHandler;
mutable std::unique_ptr<Writer> _writer;
std::vector<SectionAlign> _sectAligns;
mutable llvm::StringMap<mach_o::MachODylibFile*> _pathToDylibMap;
mutable std::vector<mach_o::MachODylibFile*> _allDylibs;
mutable std::set<mach_o::MachODylibFile*> _upwardDylibs;
mutable std::vector<std::unique_ptr<File>> _indirectDylibs;
mutable std::mutex _dylibsMutex;
ExportMode _exportMode = ExportMode::globals;
llvm::StringSet<> _exportedSymbols;
DebugInfoMode _debugInfoMode = DebugInfoMode::addDebugMap;
std::unique_ptr<llvm::raw_fd_ostream> _dependencyInfo;
llvm::StringMap<std::vector<OrderFileNode>> _orderFiles;
unsigned _orderFileEntries = 0;
File *_flatNamespaceFile = nullptr;
mach_o::SectCreateFile *_sectCreateFile = nullptr;
};

}

#endif
