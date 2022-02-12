







#if !defined(LLD_ELF_CONFIG_H)
#define LLD_ELF_CONFIG_H

#include "lld/Common/ErrorHandler.h"
#include "llvm/ADT/CachedHashString.h"
#include "llvm/ADT/MapVector.h"
#include "llvm/ADT/SetVector.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/StringSet.h"
#include "llvm/BinaryFormat/ELF.h"
#include "llvm/Support/CachePruning.h"
#include "llvm/Support/CodeGen.h"
#include "llvm/Support/Endian.h"
#include "llvm/Support/GlobPattern.h"
#include "llvm/Support/PrettyStackTrace.h"
#include <atomic>
#include <vector>

namespace lld {
namespace elf {

class InputFile;
class InputSectionBase;

enum ELFKind {
ELFNoneKind,
ELF32LEKind,
ELF32BEKind,
ELF64LEKind,
ELF64BEKind
};



enum class BsymbolicKind { None, NonWeakFunctions, Functions, All };


enum class BuildIdKind { None, Fast, Md5, Sha1, Hexstring, Uuid };


enum class DiscardPolicy { Default, All, Locals, None };


enum class ICFLevel { None, Safe, All };


enum class StripPolicy { None, All, Debug };


enum class UnresolvedPolicy { ReportError, Warn, Ignore };


enum class OrphanHandlingPolicy { Place, Warn, Error };


enum class SortSectionPolicy { Default, None, Alignment, Name, Priority };


enum class Target2Policy { Abs, Rel, GotRel };


enum class ARMVFPArgKind { Default, Base, VFP, ToolChain };


enum class SeparateSegmentKind { None, Code, Loadable };


enum class GnuStackKind { None, Exec, NoExec };

struct SymbolVersion {
llvm::StringRef name;
bool isExternCpp;
bool hasWildcard;
};



struct VersionDefinition {
llvm::StringRef name;
uint16_t id;
std::vector<SymbolVersion> nonLocalPatterns;
std::vector<SymbolVersion> localPatterns;
};





struct Configuration {
uint8_t osabi = 0;
uint32_t andFeatures = 0;
llvm::CachePruningPolicy thinLTOCachePolicy;
llvm::SetVector<llvm::CachedHashString> dependencyFiles;
llvm::StringMap<uint64_t> sectionStartMap;
llvm::StringRef bfdname;
llvm::StringRef chroot;
llvm::StringRef dependencyFile;
llvm::StringRef dwoDir;
llvm::StringRef dynamicLinker;
llvm::StringRef entry;
llvm::StringRef emulation;
llvm::StringRef fini;
llvm::StringRef init;
llvm::StringRef ltoAAPipeline;
llvm::StringRef ltoCSProfileFile;
llvm::StringRef ltoNewPmPasses;
llvm::StringRef ltoObjPath;
llvm::StringRef ltoSampleProfile;
llvm::StringRef mapFile;
llvm::StringRef outputFile;
llvm::StringRef optRemarksFilename;
llvm::Optional<uint64_t> optRemarksHotnessThreshold = 0;
llvm::StringRef optRemarksPasses;
llvm::StringRef optRemarksFormat;
llvm::StringRef progName;
llvm::StringRef printArchiveStats;
llvm::StringRef printSymbolOrder;
llvm::StringRef soName;
llvm::StringRef sysroot;
llvm::StringRef thinLTOCacheDir;
llvm::StringRef thinLTOIndexOnlyArg;
llvm::StringRef ltoBasicBlockSections;
std::pair<llvm::StringRef, llvm::StringRef> thinLTOObjectSuffixReplace;
std::pair<llvm::StringRef, llvm::StringRef> thinLTOPrefixReplace;
std::string rpath;
std::vector<VersionDefinition> versionDefinitions;
std::vector<llvm::StringRef> auxiliaryList;
std::vector<llvm::StringRef> filterList;
std::vector<llvm::StringRef> searchPaths;
std::vector<llvm::StringRef> symbolOrderingFile;
std::vector<llvm::StringRef> thinLTOModulesToCompile;
std::vector<llvm::StringRef> undefined;
std::vector<SymbolVersion> dynamicList;
std::vector<uint8_t> buildIdVector;
llvm::MapVector<std::pair<const InputSectionBase *, const InputSectionBase *>,
uint64_t>
callGraphProfile;
bool allowMultipleDefinition;
bool androidPackDynRelocs;
bool armHasBlx = false;
bool armHasMovtMovw = false;
bool armJ1J2BranchEncoding = false;
bool asNeeded = false;
BsymbolicKind bsymbolic = BsymbolicKind::None;
bool callGraphProfileSort;
bool checkSections;
bool checkDynamicRelocs;
bool compressDebugSections;
bool cref;
std::vector<std::pair<llvm::GlobPattern, uint64_t>> deadRelocInNonAlloc;
bool defineCommon;
bool demangle = true;
bool dependentLibraries;
bool disableVerify;
bool ehFrameHdr;
bool emitLLVM;
bool emitRelocs;
bool enableNewDtags;
bool executeOnly;
bool exportDynamic;
bool fixCortexA53Errata843419;
bool fixCortexA8;
bool formatBinary = false;
bool fortranCommon;
bool gcSections;
bool gdbIndex;
bool gnuHash = false;
bool gnuUnique;
bool hasDynSymTab;
bool ignoreDataAddressEquality;
bool ignoreFunctionAddressEquality;
bool ltoCSProfileGenerate;
bool ltoDebugPassManager;
bool ltoEmitAsm;
bool ltoNewPassManager;
bool ltoPseudoProbeForProfiling;
bool ltoUniqueBasicBlockSectionNames;
bool ltoWholeProgramVisibility;
bool mergeArmExidx;
bool mipsN32Abi = false;
bool mmapOutputFile;
bool nmagic;
bool noDynamicLinker = false;
bool noinhibitExec;
bool nostdlib;
bool oFormatBinary;
bool omagic;
bool optEB = false;
bool optEL = false;
bool optimizeBBJumps;
bool optRemarksWithHotness;
bool picThunk;
bool pie;
bool printGcSections;
bool printIcfSections;
bool relocatable;
bool relrPackDynRelocs;
bool saveTemps;
std::vector<std::pair<llvm::GlobPattern, uint32_t>> shuffleSections;
bool singleRoRx;
bool shared;
bool symbolic;
bool isStatic = false;
bool sysvHash = false;
bool target1Rel;
bool trace;
bool thinLTOEmitImportsFiles;
bool thinLTOIndexOnly;
bool timeTraceEnabled;
bool tocOptimize;
bool pcRelOptimize;
bool undefinedVersion;
bool unique;
bool useAndroidRelrTags = false;
bool warnBackrefs;
std::vector<llvm::GlobPattern> warnBackrefsExclude;
bool warnCommon;
bool warnMissingEntry;
bool warnSymbolOrdering;
bool writeAddends;
bool zCombreloc;
bool zCopyreloc;
bool zForceBti;
bool zForceIbt;
bool zGlobal;
bool zHazardplt;
bool zIfuncNoplt;
bool zInitfirst;
bool zInterpose;
bool zKeepTextSectionPrefix;
bool zNodefaultlib;
bool zNodelete;
bool zNodlopen;
bool zNow;
bool zOrigin;
bool zPacPlt;
bool zRelro;
bool zRodynamic;
bool zShstk;
bool zStartStopGC;
uint8_t zStartStopVisibility;
bool zText;
bool zRetpolineplt;
bool zWxneeded;
DiscardPolicy discard;
GnuStackKind zGnustack;
ICFLevel icf;
OrphanHandlingPolicy orphanHandling;
SortSectionPolicy sortSection;
StripPolicy strip;
UnresolvedPolicy unresolvedSymbols;
UnresolvedPolicy unresolvedSymbolsInShlib;
Target2Policy target2;
bool Power10Stub;
ARMVFPArgKind armVFPArgs = ARMVFPArgKind::Default;
BuildIdKind buildId = BuildIdKind::None;
SeparateSegmentKind zSeparate;
ELFKind ekind = ELFNoneKind;
uint16_t emachine = llvm::ELF::EM_NONE;
llvm::Optional<uint64_t> imageBase;
uint64_t commonPageSize;
uint64_t maxPageSize;
uint64_t mipsGotSize;
uint64_t zStackSize;
unsigned ltoPartitions;
unsigned ltoo;
unsigned optimize;
StringRef thinLTOJobs;
unsigned timeTraceGranularity;
int32_t splitStackAdjustSize;






bool copyRelocs;


bool is64;


bool isLE;


llvm::support::endianness endianness;













bool isMips64EL;













std::atomic<bool> hasStaticTlsModel{false};


uint32_t eflags = 0;















bool isRela;


bool isPic;


int wordsize;
};


extern Configuration *config;



static inline ArrayRef<VersionDefinition> namedVersionDefs() {
return llvm::makeArrayRef(config->versionDefinitions).slice(2);
}

static inline void errorOrWarn(const Twine &msg) {
if (!config->noinhibitExec)
error(msg);
else
warn(msg);
}

static inline void internalLinkerError(StringRef loc, const Twine &msg) {
errorOrWarn(loc + "internal linker error: " + msg + "\n" +
llvm::getBugReportMsg());
}

}
}

#endif
