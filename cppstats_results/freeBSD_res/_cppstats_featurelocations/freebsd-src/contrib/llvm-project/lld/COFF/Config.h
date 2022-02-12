







#if !defined(LLD_COFF_CONFIG_H)
#define LLD_COFF_CONFIG_H

#include "llvm/ADT/MapVector.h"
#include "llvm/ADT/StringMap.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Object/COFF.h"
#include "llvm/Support/CachePruning.h"
#include <cstdint>
#include <map>
#include <set>
#include <string>

namespace lld {
namespace coff {

using llvm::COFF::IMAGE_FILE_MACHINE_UNKNOWN;
using llvm::COFF::WindowsSubsystem;
using llvm::StringRef;
class DefinedAbsolute;
class DefinedRelative;
class StringChunk;
class Symbol;
class InputFile;
class SectionChunk;


static const auto AMD64 = llvm::COFF::IMAGE_FILE_MACHINE_AMD64;
static const auto ARM64 = llvm::COFF::IMAGE_FILE_MACHINE_ARM64;
static const auto ARMNT = llvm::COFF::IMAGE_FILE_MACHINE_ARMNT;
static const auto I386 = llvm::COFF::IMAGE_FILE_MACHINE_I386;


struct Export {
StringRef name;
StringRef extName;
Symbol *sym = nullptr;
uint16_t ordinal = 0;
bool noname = false;
bool data = false;
bool isPrivate = false;
bool constant = false;




StringRef forwardTo;
StringChunk *forwardChunk = nullptr;


bool directives = false;
StringRef symbolName;
StringRef exportName;

bool operator==(const Export &e) {
return (name == e.name && extName == e.extName &&
ordinal == e.ordinal && noname == e.noname &&
data == e.data && isPrivate == e.isPrivate);
}
};

enum class DebugType {
None = 0x0,
CV = 0x1,
PData = 0x2,
Fixup = 0x4,
};

enum GuardCFLevel {
Off = 0x0,
CF = 0x1,
LongJmp = 0x2,
EHCont = 0x4,
All = 0x7
};

enum class ICFLevel {
None,
Safe,
All,

};


struct Configuration {
enum ManifestKind { SideBySide, Embed, No };
bool is64() { return machine == AMD64 || machine == ARM64; }

llvm::COFF::MachineTypes machine = IMAGE_FILE_MACHINE_UNKNOWN;
size_t wordsize;
bool verbose = false;
WindowsSubsystem subsystem = llvm::COFF::IMAGE_SUBSYSTEM_UNKNOWN;
Symbol *entry = nullptr;
bool noEntry = false;
std::string outputFile;
std::string importName;
bool demangle = true;
bool doGC = true;
ICFLevel doICF = ICFLevel::None;
bool tailMerge;
bool relocatable = true;
bool forceMultiple = false;
bool forceMultipleRes = false;
bool forceUnresolved = false;
bool debug = false;
bool debugDwarf = false;
bool debugGHashes = false;
bool debugSymtab = false;
bool driver = false;
bool driverUponly = false;
bool driverWdm = false;
bool showTiming = false;
bool showSummary = false;
unsigned debugTypes = static_cast<unsigned>(DebugType::None);
std::vector<std::string> natvisFiles;
llvm::StringMap<std::string> namedStreams;
llvm::SmallString<128> pdbAltPath;
llvm::SmallString<128> pdbPath;
llvm::SmallString<128> pdbSourcePath;
std::vector<llvm::StringRef> argv;


std::vector<Symbol *> gcroot;

std::set<std::string> noDefaultLibs;
bool noDefaultLibAll = false;


bool dll = false;
StringRef implib;
std::vector<Export> exports;
bool hadExplicitExports;
std::set<std::string> delayLoads;
std::map<std::string, int> dllOrder;
Symbol *delayLoadHelper = nullptr;

bool saveTemps = false;


int guardCF = GuardCFLevel::Off;


bool safeSEH = false;
Symbol *sehTable = nullptr;
Symbol *sehCount = nullptr;
bool noSEH = false;


unsigned ltoo = 2;


std::string thinLTOJobs;

unsigned ltoPartitions = 1;


StringRef ltoCache;

llvm::CachePruningPolicy ltoCachePolicy;


bool ltoNewPassManager = false;

bool ltoDebugPassManager = false;


std::map<StringRef, StringRef> merge;


std::map<StringRef, uint32_t> section;


ManifestKind manifest = No;
int manifestID = 1;
StringRef manifestDependency;
bool manifestUAC = true;
std::vector<std::string> manifestInput;
StringRef manifestLevel = "'asInvoker'";
StringRef manifestUIAccess = "'false'";
StringRef manifestFile;


std::map<std::string, int> alignComm;


std::map<StringRef, std::pair<StringRef, InputFile *>> mustMatch;


std::map<StringRef, StringRef> alternateNames;


llvm::StringMap<int> order;


std::string lldmapFile;


std::string mapFile;


llvm::StringRef thinLTOIndexOnlyArg;


std::pair<llvm::StringRef, llvm::StringRef> thinLTOPrefixReplace;


std::pair<llvm::StringRef, llvm::StringRef> thinLTOObjectSuffixReplace;


llvm::StringRef ltoObjPath;


bool ltoCSProfileGenerate = false;


llvm::StringRef ltoCSProfileFile;


llvm::MapVector<std::pair<const SectionChunk *, const SectionChunk *>,
uint64_t>
callGraphProfile;
bool callGraphProfileSort = false;


StringRef printSymbolOrder;

uint64_t align = 4096;
uint64_t imageBase = -1;
uint64_t fileAlign = 512;
uint64_t stackReserve = 1024 * 1024;
uint64_t stackCommit = 4096;
uint64_t heapReserve = 1024 * 1024;
uint64_t heapCommit = 4096;
uint32_t majorImageVersion = 0;
uint32_t minorImageVersion = 0;


uint32_t majorOSVersion = 6;
uint32_t minorOSVersion = 0;
uint32_t majorSubsystemVersion = 6;
uint32_t minorSubsystemVersion = 0;
uint32_t timestamp = 0;
uint32_t functionPadMin = 0;
bool dynamicBase = true;
bool allowBind = true;
bool cetCompat = false;
bool nxCompat = true;
bool allowIsolation = true;
bool terminalServerAware = true;
bool largeAddressAware = false;
bool highEntropyVA = false;
bool appContainer = false;
bool mingw = false;
bool warnMissingOrderSymbol = true;
bool warnLocallyDefinedImported = true;
bool warnDebugInfoUnusable = true;
bool warnLongSectionNames = true;
bool warnStdcallFixup = true;
bool incremental = true;
bool integrityCheck = false;
bool killAt = false;
bool repro = false;
bool swaprunCD = false;
bool swaprunNet = false;
bool thinLTOEmitImportsFiles;
bool thinLTOIndexOnly;
bool autoImport = false;
bool pseudoRelocs = false;
bool stdcallFixup = false;
};

extern Configuration *config;

}
}

#endif
