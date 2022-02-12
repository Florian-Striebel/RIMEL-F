












#if !defined(LLVM_CLANG_BASIC_LANGOPTIONS_H)
#define LLVM_CLANG_BASIC_LANGOPTIONS_H

#include "clang/Basic/CommentOptions.h"
#include "clang/Basic/LLVM.h"
#include "clang/Basic/LangStandard.h"
#include "clang/Basic/ObjCRuntime.h"
#include "clang/Basic/Sanitizers.h"
#include "clang/Basic/TargetCXXABI.h"
#include "clang/Basic/Visibility.h"
#include "llvm/ADT/FloatingPointMode.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/Triple.h"
#include <string>
#include <vector>

namespace clang {



class LangOptionsBase {
friend class CompilerInvocation;

public:

#define LANGOPT(Name, Bits, Default, Description) unsigned Name : Bits;
#define ENUM_LANGOPT(Name, Type, Bits, Default, Description)
#include "clang/Basic/LangOptions.def"

protected:


#define LANGOPT(Name, Bits, Default, Description)
#define ENUM_LANGOPT(Name, Type, Bits, Default, Description) unsigned Name : Bits;

#include "clang/Basic/LangOptions.def"
};



enum class MSVtorDispMode { Never, ForVBaseOverride, ForVFTable };



class LangOptions : public LangOptionsBase {
public:
using Visibility = clang::Visibility;
using RoundingMode = llvm::RoundingMode;

enum GCMode { NonGC, GCOnly, HybridGC };
enum StackProtectorMode { SSPOff, SSPOn, SSPStrong, SSPReq };




enum class TrivialAutoVarInitKind { Uninitialized, Zero, Pattern };

enum SignedOverflowBehaviorTy {

SOB_Undefined,


SOB_Defined,


SOB_Trapping
};


enum CompilingModuleKind {

CMK_None,


CMK_ModuleMap,


CMK_HeaderModule,


CMK_ModuleInterface,
};

enum PragmaMSPointersToMembersKind {
PPTMK_BestCase,
PPTMK_FullGeneralitySingleInheritance,
PPTMK_FullGeneralityMultipleInheritance,
PPTMK_FullGeneralityVirtualInheritance
};

using MSVtorDispMode = clang::MSVtorDispMode;

enum DefaultCallingConvention {
DCC_None,
DCC_CDecl,
DCC_FastCall,
DCC_StdCall,
DCC_VectorCall,
DCC_RegCall
};

enum AddrSpaceMapMangling { ASMM_Target, ASMM_On, ASMM_Off };


enum MSVCMajorVersion {
MSVC2010 = 1600,
MSVC2012 = 1700,
MSVC2013 = 1800,
MSVC2015 = 1900,
MSVC2017 = 1910,
MSVC2017_5 = 1912,
MSVC2017_7 = 1914,
MSVC2019 = 1920,
MSVC2019_8 = 1928,
};

enum SYCLMajorVersion {
SYCL_None,
SYCL_2017,
SYCL_2020,


SYCL_Default = SYCL_2020
};


enum class ClangABI {



Ver3_8,




Ver4,






Ver6,





Ver7,





Ver9,







Ver11,




Ver12,



Latest
};

enum class CoreFoundationABI {

Unspecified,

Standalone,

ObjectiveC,

Swift,

Swift5_0,

Swift4_2,

Swift4_1,
};

enum FPModeKind {

FPM_Off,


FPM_On,


FPM_Fast,


FPM_FastHonorPragmas
};


static constexpr unsigned FPR_ToNearest =
static_cast<unsigned>(llvm::RoundingMode::NearestTiesToEven);


enum FPExceptionModeKind {

FPE_Ignore,

FPE_MayTrap,

FPE_Strict
};


enum class ExceptionHandlingKind { None, SjLj, WinEH, DwarfCFI, Wasm };

enum class LaxVectorConversionKind {

None,


Integer,


All,
};

enum class AltivecSrcCompatKind {


Mixed,

GCC,

XL,

Default = Mixed,
};

enum class SignReturnAddressScopeKind {

None,

NonLeaf,

All
};

enum class SignReturnAddressKeyKind {

AKey,

BKey
};

enum class ThreadModelKind {

POSIX,

Single
};

enum class ExtendArgsKind {


ExtendTo32,
ExtendTo64
};

public:

LangStandard::Kind LangStd;


SanitizerSet Sanitize;

bool SanitizeCoverage = false;



std::vector<std::string> NoSanitizeFiles;





std::vector<std::string> XRayAlwaysInstrumentFiles;





std::vector<std::string> XRayNeverInstrumentFiles;




std::vector<std::string> XRayAttrListFiles;



std::vector<std::string> ProfileListFiles;

clang::ObjCRuntime ObjCRuntime;

CoreFoundationABI CFRuntime = CoreFoundationABI::Unspecified;

std::string ObjCConstantStringClass;





std::string OverflowHandler;


std::string ModuleName;






std::string CurrentModule;





std::vector<std::string> ModuleFeatures;


CommentOptions CommentOpts;


std::vector<std::string> NoBuiltinFuncs;


std::map<std::string, std::string, std::greater<std::string>> MacroPrefixMap;



std::vector<llvm::Triple> OMPTargetTriples;



std::string OMPHostIRFile;





std::string CUID;



llvm::Optional<TargetCXXABI::Kind> CXXABI;



bool IsHeaderFile = false;

LangOptions();


#define LANGOPT(Name, Bits, Default, Description)
#define ENUM_LANGOPT(Name, Type, Bits, Default, Description) Type get##Name() const { return static_cast<Type>(Name); } void set##Name(Type Value) { Name = static_cast<unsigned>(Value); }


#include "clang/Basic/LangOptions.def"


bool isCompilingModule() const {
return getCompilingModule() != CMK_None;
}


bool trackLocalOwningModule() const {
return isCompilingModule() || ModulesLocalVisibility;
}

bool isSignedOverflowDefined() const {
return getSignedOverflowBehavior() == SOB_Defined;
}

bool isSubscriptPointerArithmetic() const {
return ObjCRuntime.isSubscriptPointerArithmetic() &&
!ObjCSubscriptingLegacyRuntime;
}

bool isCompatibleWithMSVC(MSVCMajorVersion MajorVersion) const {
return MSCompatibilityVersion >= MajorVersion * 100000U;
}



void resetNonModularOptions();



bool isNoBuiltinFunc(StringRef Name) const;


bool allowsNonTrivialObjCLifetimeQualifiers() const {
return ObjCAutoRefCount || ObjCWeak;
}

bool assumeFunctionsAreConvergent() const {
return ConvergentFunctions;
}


VersionTuple getOpenCLVersionTuple() const;


bool hasSignReturnAddress() const {
return getSignReturnAddressScope() != SignReturnAddressScopeKind::None;
}


bool isSignReturnAddressWithAKey() const {
return getSignReturnAddressKey() == SignReturnAddressKeyKind::AKey;
}


bool isSignReturnAddressScopeAll() const {
return getSignReturnAddressScope() == SignReturnAddressScopeKind::All;
}

bool hasSjLjExceptions() const {
return getExceptionHandling() == ExceptionHandlingKind::SjLj;
}

bool hasSEHExceptions() const {
return getExceptionHandling() == ExceptionHandlingKind::WinEH;
}

bool hasDWARFExceptions() const {
return getExceptionHandling() == ExceptionHandlingKind::DwarfCFI;
}

bool hasWasmExceptions() const {
return getExceptionHandling() == ExceptionHandlingKind::Wasm;
}

bool isSYCL() const { return SYCLIsDevice || SYCLIsHost; }


void remapPathPrefix(SmallString<256> &Path) const;
};


class FPOptionsOverride;
class FPOptions {
public:

using storage_type = uint16_t;

using RoundingMode = llvm::RoundingMode;

static constexpr unsigned StorageBitSize = 8 * sizeof(storage_type);



static constexpr storage_type FirstShift = 0, FirstWidth = 0;
#define OPTION(NAME, TYPE, WIDTH, PREVIOUS) static constexpr storage_type NAME##Shift = PREVIOUS##Shift + PREVIOUS##Width; static constexpr storage_type NAME##Width = WIDTH; static constexpr storage_type NAME##Mask = ((1 << NAME##Width) - 1) << NAME##Shift;





#include "clang/Basic/FPOptions.def"

static constexpr storage_type TotalWidth = 0
#define OPTION(NAME, TYPE, WIDTH, PREVIOUS) +WIDTH
#include "clang/Basic/FPOptions.def"
;
static_assert(TotalWidth <= StorageBitSize, "Too short type for FPOptions");

private:
storage_type Value;

public:
FPOptions() : Value(0) {
setFPContractMode(LangOptions::FPM_Off);
setRoundingMode(static_cast<RoundingMode>(LangOptions::FPR_ToNearest));
setFPExceptionMode(LangOptions::FPE_Ignore);
}
explicit FPOptions(const LangOptions &LO) {
Value = 0;



auto LangOptContractMode = LO.getDefaultFPContractMode();
if (LangOptContractMode == LangOptions::FPM_FastHonorPragmas)
LangOptContractMode = LangOptions::FPM_Fast;
setFPContractMode(LangOptContractMode);
setRoundingMode(LO.getFPRoundingMode());
setFPExceptionMode(LO.getFPExceptionMode());
setAllowFPReassociate(LO.AllowFPReassoc);
setNoHonorNaNs(LO.NoHonorNaNs);
setNoHonorInfs(LO.NoHonorInfs);
setNoSignedZero(LO.NoSignedZero);
setAllowReciprocal(LO.AllowRecip);
setAllowApproxFunc(LO.ApproxFunc);
if (getFPContractMode() == LangOptions::FPM_On &&
getRoundingMode() == llvm::RoundingMode::Dynamic &&
getFPExceptionMode() == LangOptions::FPE_Strict)


setAllowFEnvAccess(true);
else
setAllowFEnvAccess(LangOptions::FPM_Off);
}

bool allowFPContractWithinStatement() const {
return getFPContractMode() == LangOptions::FPM_On;
}
void setAllowFPContractWithinStatement() {
setFPContractMode(LangOptions::FPM_On);
}

bool allowFPContractAcrossStatement() const {
return getFPContractMode() == LangOptions::FPM_Fast;
}
void setAllowFPContractAcrossStatement() {
setFPContractMode(LangOptions::FPM_Fast);
}

bool isFPConstrained() const {
return getRoundingMode() != llvm::RoundingMode::NearestTiesToEven ||
getFPExceptionMode() != LangOptions::FPE_Ignore ||
getAllowFEnvAccess();
}

bool operator==(FPOptions other) const { return Value == other.Value; }



static FPOptions defaultWithoutTrailingStorage(const LangOptions &LO);

storage_type getAsOpaqueInt() const { return Value; }
static FPOptions getFromOpaqueInt(storage_type Value) {
FPOptions Opts;
Opts.Value = Value;
return Opts;
}


#define OPTION(NAME, TYPE, WIDTH, PREVIOUS) TYPE get##NAME() const { return static_cast<TYPE>((Value & NAME##Mask) >> NAME##Shift); } void set##NAME(TYPE value) { Value = (Value & ~NAME##Mask) | (storage_type(value) << NAME##Shift); }






#include "clang/Basic/FPOptions.def"
LLVM_DUMP_METHOD void dump();
};














class FPOptionsOverride {
FPOptions Options = FPOptions::getFromOpaqueInt(0);
FPOptions::storage_type OverrideMask = 0;

public:
using RoundingMode = llvm::RoundingMode;



using storage_type = uint32_t;
static_assert(sizeof(storage_type) >= 2 * sizeof(FPOptions::storage_type),
"Too short type for FPOptionsOverride");



static constexpr storage_type OverrideMaskBits =
(static_cast<storage_type>(1) << FPOptions::StorageBitSize) - 1;

FPOptionsOverride() {}
FPOptionsOverride(const LangOptions &LO)
: Options(LO), OverrideMask(OverrideMaskBits) {}
FPOptionsOverride(FPOptions FPO)
: Options(FPO), OverrideMask(OverrideMaskBits) {}

bool requiresTrailingStorage() const { return OverrideMask != 0; }

void setAllowFPContractWithinStatement() {
setFPContractModeOverride(LangOptions::FPM_On);
}

void setAllowFPContractAcrossStatement() {
setFPContractModeOverride(LangOptions::FPM_Fast);
}

void setDisallowFPContract() {
setFPContractModeOverride(LangOptions::FPM_Off);
}

void setFPPreciseEnabled(bool Value) {
setAllowFPReassociateOverride(!Value);
setNoHonorNaNsOverride(!Value);
setNoHonorInfsOverride(!Value);
setNoSignedZeroOverride(!Value);
setAllowReciprocalOverride(!Value);
setAllowApproxFuncOverride(!Value);
if (Value)

setAllowFPContractWithinStatement();
else

setAllowFPContractAcrossStatement();
}

storage_type getAsOpaqueInt() const {
return (static_cast<storage_type>(Options.getAsOpaqueInt())
<< FPOptions::StorageBitSize) |
OverrideMask;
}
static FPOptionsOverride getFromOpaqueInt(storage_type I) {
FPOptionsOverride Opts;
Opts.OverrideMask = I & OverrideMaskBits;
Opts.Options = FPOptions::getFromOpaqueInt(I >> FPOptions::StorageBitSize);
return Opts;
}

FPOptions applyOverrides(FPOptions Base) {
FPOptions Result =
FPOptions::getFromOpaqueInt((Base.getAsOpaqueInt() & ~OverrideMask) |
(Options.getAsOpaqueInt() & OverrideMask));
return Result;
}

FPOptions applyOverrides(const LangOptions &LO) {
return applyOverrides(FPOptions(LO));
}

bool operator==(FPOptionsOverride other) const {
return Options == other.Options && OverrideMask == other.OverrideMask;
}
bool operator!=(FPOptionsOverride other) const { return !(*this == other); }

#define OPTION(NAME, TYPE, WIDTH, PREVIOUS) bool has##NAME##Override() const { return OverrideMask & FPOptions::NAME##Mask; } TYPE get##NAME##Override() const { assert(has##NAME##Override()); return Options.get##NAME(); } void clear##NAME##Override() { Options.set##NAME(TYPE(0)); OverrideMask &= ~FPOptions::NAME##Mask; } void set##NAME##Override(TYPE value) { Options.set##NAME(value); OverrideMask |= FPOptions::NAME##Mask; }

















#include "clang/Basic/FPOptions.def"
LLVM_DUMP_METHOD void dump();
};


enum TranslationUnitKind {

TU_Complete,



TU_Prefix,


TU_Module,



TU_Incremental
};

}

#endif
