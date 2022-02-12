










#if !defined(LLVM_CLANG_LIB_BASIC_TARGETS_OSTARGETS_H)
#define LLVM_CLANG_LIB_BASIC_TARGETS_OSTARGETS_H

#include "Targets.h"

namespace clang {
namespace targets {

template <typename TgtInfo>
class LLVM_LIBRARY_VISIBILITY OSTargetInfo : public TgtInfo {
protected:
virtual void getOSDefines(const LangOptions &Opts, const llvm::Triple &Triple,
MacroBuilder &Builder) const = 0;

public:
OSTargetInfo(const llvm::Triple &Triple, const TargetOptions &Opts)
: TgtInfo(Triple, Opts) {}

void getTargetDefines(const LangOptions &Opts,
MacroBuilder &Builder) const override {
TgtInfo::getTargetDefines(Opts, Builder);
getOSDefines(Opts, TgtInfo::getTriple(), Builder);
}
};


template <typename Target>
class LLVM_LIBRARY_VISIBILITY CloudABITargetInfo : public OSTargetInfo<Target> {
protected:
void getOSDefines(const LangOptions &Opts, const llvm::Triple &Triple,
MacroBuilder &Builder) const override {
Builder.defineMacro("__CloudABI__");
Builder.defineMacro("__ELF__");


Builder.defineMacro("__STDC_ISO_10646__", "201206L");
Builder.defineMacro("__STDC_UTF_16__");
Builder.defineMacro("__STDC_UTF_32__");
}

public:
CloudABITargetInfo(const llvm::Triple &Triple, const TargetOptions &Opts)
: OSTargetInfo<Target>(Triple, Opts) {}
};


template <typename Target>
class LLVM_LIBRARY_VISIBILITY AnanasTargetInfo : public OSTargetInfo<Target> {
protected:
void getOSDefines(const LangOptions &Opts, const llvm::Triple &Triple,
MacroBuilder &Builder) const override {

Builder.defineMacro("__Ananas__");
Builder.defineMacro("__ELF__");
}

public:
AnanasTargetInfo(const llvm::Triple &Triple, const TargetOptions &Opts)
: OSTargetInfo<Target>(Triple, Opts) {}
};

void getDarwinDefines(MacroBuilder &Builder, const LangOptions &Opts,
const llvm::Triple &Triple, StringRef &PlatformName,
VersionTuple &PlatformMinVersion);

template <typename Target>
class LLVM_LIBRARY_VISIBILITY DarwinTargetInfo : public OSTargetInfo<Target> {
protected:
void getOSDefines(const LangOptions &Opts, const llvm::Triple &Triple,
MacroBuilder &Builder) const override {
getDarwinDefines(Builder, Opts, Triple, this->PlatformName,
this->PlatformMinVersion);
}

public:
DarwinTargetInfo(const llvm::Triple &Triple, const TargetOptions &Opts)
: OSTargetInfo<Target>(Triple, Opts) {


this->TLSSupported = false;

if (Triple.isMacOSX())
this->TLSSupported = !Triple.isMacOSXVersionLT(10, 7);
else if (Triple.isiOS()) {


if (Triple.isArch64Bit())
this->TLSSupported = !Triple.isOSVersionLT(8);
else if (Triple.isArch32Bit()) {
if (!Triple.isSimulatorEnvironment())
this->TLSSupported = !Triple.isOSVersionLT(9);
else
this->TLSSupported = !Triple.isOSVersionLT(10);
}
} else if (Triple.isWatchOS()) {
if (!Triple.isSimulatorEnvironment())
this->TLSSupported = !Triple.isOSVersionLT(2);
else
this->TLSSupported = !Triple.isOSVersionLT(3);
}

this->MCountName = "\01mcount";
}

const char *getStaticInitSectionSpecifier() const override {

return "__TEXT,__StaticInit,regular,pure_instructions";
}




bool hasProtectedVisibility() const override { return false; }

unsigned getExnObjectAlignment() const override {



llvm::VersionTuple MinVersion;
const llvm::Triple &T = this->getTriple();


switch (T.getOS()) {
case llvm::Triple::Darwin:
case llvm::Triple::MacOSX:
MinVersion = llvm::VersionTuple(10U, 14U);
break;
case llvm::Triple::IOS:
case llvm::Triple::TvOS:
MinVersion = llvm::VersionTuple(12U);
break;
case llvm::Triple::WatchOS:
MinVersion = llvm::VersionTuple(5U);
break;
default:

return 64;
}

unsigned Major, Minor, Micro;
T.getOSVersion(Major, Minor, Micro);
if (llvm::VersionTuple(Major, Minor, Micro) < MinVersion)
return 64;
return OSTargetInfo<Target>::getExnObjectAlignment();
}

TargetInfo::IntType getLeastIntTypeByWidth(unsigned BitWidth,
bool IsSigned) const final {

return BitWidth == 64
? (IsSigned ? TargetInfo::SignedLongLong
: TargetInfo::UnsignedLongLong)
: TargetInfo::getLeastIntTypeByWidth(BitWidth, IsSigned);
}
};


template <typename Target>
class LLVM_LIBRARY_VISIBILITY DragonFlyBSDTargetInfo
: public OSTargetInfo<Target> {
protected:
void getOSDefines(const LangOptions &Opts, const llvm::Triple &Triple,
MacroBuilder &Builder) const override {

Builder.defineMacro("__DragonFly__");
Builder.defineMacro("__DragonFly_cc_version", "100001");
Builder.defineMacro("__ELF__");
Builder.defineMacro("__KPRINTF_ATTRIBUTE__");
Builder.defineMacro("__tune_i386__");
DefineStd(Builder, "unix", Opts);
}

public:
DragonFlyBSDTargetInfo(const llvm::Triple &Triple, const TargetOptions &Opts)
: OSTargetInfo<Target>(Triple, Opts) {
switch (Triple.getArch()) {
default:
case llvm::Triple::x86:
case llvm::Triple::x86_64:
this->MCountName = ".mcount";
break;
}
}
};

#if !defined(FREEBSD_CC_VERSION)
#define FREEBSD_CC_VERSION 0U
#endif


template <typename Target>
class LLVM_LIBRARY_VISIBILITY FreeBSDTargetInfo : public OSTargetInfo<Target> {
protected:
void getOSDefines(const LangOptions &Opts, const llvm::Triple &Triple,
MacroBuilder &Builder) const override {


unsigned Release = Triple.getOSMajorVersion();
if (Release == 0U)
Release = 8U;
unsigned CCVersion = FREEBSD_CC_VERSION;
if (CCVersion == 0U)
CCVersion = Release * 100000U + 1U;

Builder.defineMacro("__FreeBSD__", Twine(Release));
Builder.defineMacro("__FreeBSD_cc_version", Twine(CCVersion));
Builder.defineMacro("__KPRINTF_ATTRIBUTE__");
DefineStd(Builder, "unix", Opts);
Builder.defineMacro("__ELF__");










Builder.defineMacro("__STDC_MB_MIGHT_NEQ_WC__", "1");
}

public:
FreeBSDTargetInfo(const llvm::Triple &Triple, const TargetOptions &Opts)
: OSTargetInfo<Target>(Triple, Opts) {
switch (Triple.getArch()) {
default:
case llvm::Triple::x86:
case llvm::Triple::x86_64:
this->MCountName = ".mcount";
break;
case llvm::Triple::mips:
case llvm::Triple::mipsel:
case llvm::Triple::ppc:
case llvm::Triple::ppcle:
case llvm::Triple::ppc64:
case llvm::Triple::ppc64le:
this->MCountName = "_mcount";
break;
case llvm::Triple::arm:
this->MCountName = "__mcount";
break;
case llvm::Triple::riscv32:
case llvm::Triple::riscv64:
break;
}
}
};


template <typename Target>
class LLVM_LIBRARY_VISIBILITY KFreeBSDTargetInfo : public OSTargetInfo<Target> {
protected:
void getOSDefines(const LangOptions &Opts, const llvm::Triple &Triple,
MacroBuilder &Builder) const override {


DefineStd(Builder, "unix", Opts);
Builder.defineMacro("__FreeBSD_kernel__");
Builder.defineMacro("__GLIBC__");
Builder.defineMacro("__ELF__");
if (Opts.POSIXThreads)
Builder.defineMacro("_REENTRANT");
if (Opts.CPlusPlus)
Builder.defineMacro("_GNU_SOURCE");
}

public:
KFreeBSDTargetInfo(const llvm::Triple &Triple, const TargetOptions &Opts)
: OSTargetInfo<Target>(Triple, Opts) {}
};


template <typename Target>
class LLVM_LIBRARY_VISIBILITY HaikuTargetInfo : public OSTargetInfo<Target> {
protected:
void getOSDefines(const LangOptions &Opts, const llvm::Triple &Triple,
MacroBuilder &Builder) const override {

Builder.defineMacro("__HAIKU__");
Builder.defineMacro("__ELF__");
DefineStd(Builder, "unix", Opts);
if (this->HasFloat128)
Builder.defineMacro("__FLOAT128__");
}

public:
HaikuTargetInfo(const llvm::Triple &Triple, const TargetOptions &Opts)
: OSTargetInfo<Target>(Triple, Opts) {
this->SizeType = TargetInfo::UnsignedLong;
this->IntPtrType = TargetInfo::SignedLong;
this->PtrDiffType = TargetInfo::SignedLong;
this->ProcessIDType = TargetInfo::SignedLong;
this->TLSSupported = false;
switch (Triple.getArch()) {
default:
break;
case llvm::Triple::x86:
case llvm::Triple::x86_64:
this->HasFloat128 = true;
break;
}
}
};


template <typename Target>
class LLVM_LIBRARY_VISIBILITY HurdTargetInfo : public OSTargetInfo<Target> {
protected:
void getOSDefines(const LangOptions &Opts, const llvm::Triple &Triple,
MacroBuilder &Builder) const override {

DefineStd(Builder, "unix", Opts);
Builder.defineMacro("__GNU__");
Builder.defineMacro("__gnu_hurd__");
Builder.defineMacro("__MACH__");
Builder.defineMacro("__GLIBC__");
Builder.defineMacro("__ELF__");
if (Opts.POSIXThreads)
Builder.defineMacro("_REENTRANT");
if (Opts.CPlusPlus)
Builder.defineMacro("_GNU_SOURCE");
}
public:
HurdTargetInfo(const llvm::Triple &Triple, const TargetOptions &Opts)
: OSTargetInfo<Target>(Triple, Opts) {}
};


template <typename Target>
class LLVM_LIBRARY_VISIBILITY MinixTargetInfo : public OSTargetInfo<Target> {
protected:
void getOSDefines(const LangOptions &Opts, const llvm::Triple &Triple,
MacroBuilder &Builder) const override {


Builder.defineMacro("__minix", "3");
Builder.defineMacro("_EM_WSIZE", "4");
Builder.defineMacro("_EM_PSIZE", "4");
Builder.defineMacro("_EM_SSIZE", "2");
Builder.defineMacro("_EM_LSIZE", "4");
Builder.defineMacro("_EM_FSIZE", "4");
Builder.defineMacro("_EM_DSIZE", "8");
Builder.defineMacro("__ELF__");
DefineStd(Builder, "unix", Opts);
}

public:
MinixTargetInfo(const llvm::Triple &Triple, const TargetOptions &Opts)
: OSTargetInfo<Target>(Triple, Opts) {}
};


template <typename Target>
class LLVM_LIBRARY_VISIBILITY LinuxTargetInfo : public OSTargetInfo<Target> {
protected:
void getOSDefines(const LangOptions &Opts, const llvm::Triple &Triple,
MacroBuilder &Builder) const override {

DefineStd(Builder, "unix", Opts);
DefineStd(Builder, "linux", Opts);
Builder.defineMacro("__ELF__");
if (Triple.isAndroid()) {
Builder.defineMacro("__ANDROID__", "1");
unsigned Maj, Min, Rev;
Triple.getEnvironmentVersion(Maj, Min, Rev);
this->PlatformName = "android";
this->PlatformMinVersion = VersionTuple(Maj, Min, Rev);
if (Maj) {
Builder.defineMacro("__ANDROID_MIN_SDK_VERSION__", Twine(Maj));


Builder.defineMacro("__ANDROID_API__", "__ANDROID_MIN_SDK_VERSION__");
}
} else {
Builder.defineMacro("__gnu_linux__");
}
if (Opts.POSIXThreads)
Builder.defineMacro("_REENTRANT");
if (Opts.CPlusPlus)
Builder.defineMacro("_GNU_SOURCE");
if (this->HasFloat128)
Builder.defineMacro("__FLOAT128__");
}

public:
LinuxTargetInfo(const llvm::Triple &Triple, const TargetOptions &Opts)
: OSTargetInfo<Target>(Triple, Opts) {
this->WIntType = TargetInfo::UnsignedInt;

switch (Triple.getArch()) {
default:
break;
case llvm::Triple::mips:
case llvm::Triple::mipsel:
case llvm::Triple::mips64:
case llvm::Triple::mips64el:
case llvm::Triple::ppc:
case llvm::Triple::ppcle:
case llvm::Triple::ppc64:
case llvm::Triple::ppc64le:
this->MCountName = "_mcount";
break;
case llvm::Triple::x86:
case llvm::Triple::x86_64:
this->HasFloat128 = true;
break;
}
}

const char *getStaticInitSectionSpecifier() const override {
return ".text.startup";
}
};


template <typename Target>
class LLVM_LIBRARY_VISIBILITY NetBSDTargetInfo : public OSTargetInfo<Target> {
protected:
void getOSDefines(const LangOptions &Opts, const llvm::Triple &Triple,
MacroBuilder &Builder) const override {

Builder.defineMacro("__NetBSD__");
Builder.defineMacro("__unix__");
Builder.defineMacro("__ELF__");
if (Opts.POSIXThreads)
Builder.defineMacro("_REENTRANT");
}

public:
NetBSDTargetInfo(const llvm::Triple &Triple, const TargetOptions &Opts)
: OSTargetInfo<Target>(Triple, Opts) {
this->MCountName = "__mcount";
}
};


template <typename Target>
class LLVM_LIBRARY_VISIBILITY OpenBSDTargetInfo : public OSTargetInfo<Target> {
protected:
void getOSDefines(const LangOptions &Opts, const llvm::Triple &Triple,
MacroBuilder &Builder) const override {


Builder.defineMacro("__OpenBSD__");
DefineStd(Builder, "unix", Opts);
Builder.defineMacro("__ELF__");
if (Opts.POSIXThreads)
Builder.defineMacro("_REENTRANT");
if (this->HasFloat128)
Builder.defineMacro("__FLOAT128__");

if (Opts.C11) {
Builder.defineMacro("__STDC_NO_ATOMICS__");
Builder.defineMacro("__STDC_NO_THREADS__");
}
}

public:
OpenBSDTargetInfo(const llvm::Triple &Triple, const TargetOptions &Opts)
: OSTargetInfo<Target>(Triple, Opts) {
this->WCharType = this->WIntType = this->SignedInt;
this->IntMaxType = TargetInfo::SignedLongLong;
this->Int64Type = TargetInfo::SignedLongLong;
switch (Triple.getArch()) {
case llvm::Triple::x86:
case llvm::Triple::x86_64:
this->HasFloat128 = true;
LLVM_FALLTHROUGH;
default:
this->MCountName = "__mcount";
break;
case llvm::Triple::mips64:
case llvm::Triple::mips64el:
case llvm::Triple::ppc:
case llvm::Triple::ppc64:
case llvm::Triple::ppc64le:
case llvm::Triple::sparcv9:
this->MCountName = "_mcount";
break;
case llvm::Triple::riscv32:
case llvm::Triple::riscv64:
break;
}
}
};


template <typename Target>
class LLVM_LIBRARY_VISIBILITY PSPTargetInfo : public OSTargetInfo<Target> {
protected:
void getOSDefines(const LangOptions &Opts, const llvm::Triple &Triple,
MacroBuilder &Builder) const override {

Builder.defineMacro("PSP");
Builder.defineMacro("_PSP");
Builder.defineMacro("__psp__");
Builder.defineMacro("__ELF__");
}

public:
PSPTargetInfo(const llvm::Triple &Triple) : OSTargetInfo<Target>(Triple) {}
};


template <typename Target>
class LLVM_LIBRARY_VISIBILITY PS3PPUTargetInfo : public OSTargetInfo<Target> {
protected:
void getOSDefines(const LangOptions &Opts, const llvm::Triple &Triple,
MacroBuilder &Builder) const override {

Builder.defineMacro("__PPC__");
Builder.defineMacro("__PPU__");
Builder.defineMacro("__CELLOS_LV2__");
Builder.defineMacro("__ELF__");
Builder.defineMacro("__LP32__");
Builder.defineMacro("_ARCH_PPC64");
Builder.defineMacro("__powerpc64__");
}

public:
PS3PPUTargetInfo(const llvm::Triple &Triple, const TargetOptions &Opts)
: OSTargetInfo<Target>(Triple, Opts) {
this->LongWidth = this->LongAlign = 32;
this->PointerWidth = this->PointerAlign = 32;
this->IntMaxType = TargetInfo::SignedLongLong;
this->Int64Type = TargetInfo::SignedLongLong;
this->SizeType = TargetInfo::UnsignedInt;
this->resetDataLayout("E-m:e-p:32:32-i64:64-n32:64");
}
};

template <typename Target>
class LLVM_LIBRARY_VISIBILITY PS4OSTargetInfo : public OSTargetInfo<Target> {
protected:
void getOSDefines(const LangOptions &Opts, const llvm::Triple &Triple,
MacroBuilder &Builder) const override {
Builder.defineMacro("__FreeBSD__", "9");
Builder.defineMacro("__FreeBSD_cc_version", "900001");
Builder.defineMacro("__KPRINTF_ATTRIBUTE__");
DefineStd(Builder, "unix", Opts);
Builder.defineMacro("__ELF__");
Builder.defineMacro("__SCE__");
Builder.defineMacro("__ORBIS__");
}

public:
PS4OSTargetInfo(const llvm::Triple &Triple, const TargetOptions &Opts)
: OSTargetInfo<Target>(Triple, Opts) {
this->WCharType = TargetInfo::UnsignedShort;


this->MaxTLSAlign = 256;



this->UseExplicitBitFieldAlignment = false;

switch (Triple.getArch()) {
default:
case llvm::Triple::x86_64:
this->MCountName = ".mcount";
this->NewAlign = 256;
break;
}
}
TargetInfo::CallingConvCheckResult
checkCallingConvention(CallingConv CC) const override {
return (CC == CC_C) ? TargetInfo::CCCR_OK : TargetInfo::CCCR_Error;
}
};


template <typename Target>
class LLVM_LIBRARY_VISIBILITY RTEMSTargetInfo : public OSTargetInfo<Target> {
protected:
void getOSDefines(const LangOptions &Opts, const llvm::Triple &Triple,
MacroBuilder &Builder) const override {


Builder.defineMacro("__rtems__");
Builder.defineMacro("__ELF__");
if (Opts.CPlusPlus)
Builder.defineMacro("_GNU_SOURCE");
}

public:
RTEMSTargetInfo(const llvm::Triple &Triple, const TargetOptions &Opts)
: OSTargetInfo<Target>(Triple, Opts) {
switch (Triple.getArch()) {
default:
case llvm::Triple::x86:

break;
case llvm::Triple::mips:
case llvm::Triple::mipsel:
case llvm::Triple::ppc:
case llvm::Triple::ppc64:
case llvm::Triple::ppc64le:

break;
case llvm::Triple::arm:

break;
}
}
};


template <typename Target>
class LLVM_LIBRARY_VISIBILITY SolarisTargetInfo : public OSTargetInfo<Target> {
protected:
void getOSDefines(const LangOptions &Opts, const llvm::Triple &Triple,
MacroBuilder &Builder) const override {
DefineStd(Builder, "sun", Opts);
DefineStd(Builder, "unix", Opts);
Builder.defineMacro("__ELF__");
Builder.defineMacro("__svr4__");
Builder.defineMacro("__SVR4");




if (Opts.C99)
Builder.defineMacro("_XOPEN_SOURCE", "600");
else
Builder.defineMacro("_XOPEN_SOURCE", "500");
if (Opts.CPlusPlus) {
Builder.defineMacro("__C99FEATURES__");
Builder.defineMacro("_FILE_OFFSET_BITS", "64");
}

Builder.defineMacro("_LARGEFILE_SOURCE");
Builder.defineMacro("_LARGEFILE64_SOURCE");
Builder.defineMacro("__EXTENSIONS__");
if (Opts.POSIXThreads)
Builder.defineMacro("_REENTRANT");
if (this->HasFloat128)
Builder.defineMacro("__FLOAT128__");
}

public:
SolarisTargetInfo(const llvm::Triple &Triple, const TargetOptions &Opts)
: OSTargetInfo<Target>(Triple, Opts) {
if (this->PointerWidth == 64) {
this->WCharType = this->WIntType = this->SignedInt;
} else {
this->WCharType = this->WIntType = this->SignedLong;
}
switch (Triple.getArch()) {
default:
break;
case llvm::Triple::x86:
case llvm::Triple::x86_64:
this->HasFloat128 = true;
break;
}
}
};


template <typename Target>
class AIXTargetInfo : public OSTargetInfo<Target> {
protected:
void getOSDefines(const LangOptions &Opts, const llvm::Triple &Triple,
MacroBuilder &Builder) const override {
DefineStd(Builder, "unix", Opts);
Builder.defineMacro("_IBMR2");
Builder.defineMacro("_POWER");

Builder.defineMacro("_AIX");
Builder.defineMacro("__TOS_AIX__");

if (Opts.C11) {
Builder.defineMacro("__STDC_NO_ATOMICS__");
Builder.defineMacro("__STDC_NO_THREADS__");
}

if (Opts.EnableAIXExtendedAltivecABI)
Builder.defineMacro("__EXTABI__");

unsigned Major, Minor, Micro;
Triple.getOSVersion(Major, Minor, Micro);



std::pair<int, int> OsVersion = {Major, Minor};
if (OsVersion >= std::make_pair(3, 2)) Builder.defineMacro("_AIX32");
if (OsVersion >= std::make_pair(4, 1)) Builder.defineMacro("_AIX41");
if (OsVersion >= std::make_pair(4, 3)) Builder.defineMacro("_AIX43");
if (OsVersion >= std::make_pair(5, 0)) Builder.defineMacro("_AIX50");
if (OsVersion >= std::make_pair(5, 1)) Builder.defineMacro("_AIX51");
if (OsVersion >= std::make_pair(5, 2)) Builder.defineMacro("_AIX52");
if (OsVersion >= std::make_pair(5, 3)) Builder.defineMacro("_AIX53");
if (OsVersion >= std::make_pair(6, 1)) Builder.defineMacro("_AIX61");
if (OsVersion >= std::make_pair(7, 1)) Builder.defineMacro("_AIX71");
if (OsVersion >= std::make_pair(7, 2)) Builder.defineMacro("_AIX72");
if (OsVersion >= std::make_pair(7, 3)) Builder.defineMacro("_AIX73");


Builder.defineMacro("_LONG_LONG");

if (Opts.POSIXThreads) {
Builder.defineMacro("_THREAD_SAFE");
}

if (this->PointerWidth == 64) {
Builder.defineMacro("__64BIT__");
}



if (Opts.CPlusPlus && Opts.WChar) {
Builder.defineMacro("_WCHAR_T");
}
}

public:
AIXTargetInfo(const llvm::Triple &Triple, const TargetOptions &Opts)
: OSTargetInfo<Target>(Triple, Opts) {
this->TheCXXABI.set(TargetCXXABI::XL);

if (this->PointerWidth == 64) {
this->WCharType = this->UnsignedInt;
} else {
this->WCharType = this->UnsignedShort;
}
this->UseZeroLengthBitfieldAlignment = true;
}


unsigned getFloatEvalMethod() const override { return 1; }
bool hasInt128Type() const override { return false; }

bool defaultsToAIXPowerAlignment() const override { return true; }
};


template <typename Target>
class LLVM_LIBRARY_VISIBILITY ZOSTargetInfo : public OSTargetInfo<Target> {
protected:
void getOSDefines(const LangOptions &Opts, const llvm::Triple &Triple,
MacroBuilder &Builder) const override {

Builder.defineMacro("_LONG_LONG");
Builder.defineMacro("_OPEN_DEFAULT");

Builder.defineMacro("_UNIX03_WITHDRAWN");
Builder.defineMacro("__370__");
Builder.defineMacro("__BFP__");

Builder.defineMacro("__BOOL__");
Builder.defineMacro("__LONGNAME__");
Builder.defineMacro("__MVS__");
Builder.defineMacro("__THW_370__");
Builder.defineMacro("__THW_BIG_ENDIAN__");
Builder.defineMacro("__TOS_390__");
Builder.defineMacro("__TOS_MVS__");
Builder.defineMacro("__XPLINK__");

if (this->PointerWidth == 64)
Builder.defineMacro("__64BIT__");

if (Opts.CPlusPlus) {
Builder.defineMacro("__DLL__");

Builder.defineMacro("_XOPEN_SOURCE", "600");
}

if (Opts.GNUMode) {
Builder.defineMacro("_MI_BUILTIN");
Builder.defineMacro("_EXT");
}

if (Opts.CPlusPlus && Opts.WChar) {


Builder.defineMacro("__wchar_t");
}

this->PlatformName = llvm::Triple::getOSTypeName(Triple.getOS());
}

public:
ZOSTargetInfo(const llvm::Triple &Triple, const TargetOptions &Opts)
: OSTargetInfo<Target>(Triple, Opts) {
this->WCharType = TargetInfo::UnsignedInt;
this->MaxAlignedAttribute = 128;
this->UseBitFieldTypeAlignment = false;
this->UseZeroLengthBitfieldAlignment = true;
this->UseLeadingZeroLengthBitfield = false;
this->ZeroLengthBitfieldBoundary = 32;
this->DefaultAlignForAttributeAligned = 128;
}
};

void addWindowsDefines(const llvm::Triple &Triple, const LangOptions &Opts,
MacroBuilder &Builder);


template <typename Target>
class LLVM_LIBRARY_VISIBILITY WindowsTargetInfo : public OSTargetInfo<Target> {
protected:
void getOSDefines(const LangOptions &Opts, const llvm::Triple &Triple,
MacroBuilder &Builder) const override {
addWindowsDefines(Triple, Opts, Builder);
}

public:
WindowsTargetInfo(const llvm::Triple &Triple, const TargetOptions &Opts)
: OSTargetInfo<Target>(Triple, Opts) {
this->WCharType = TargetInfo::UnsignedShort;
this->WIntType = TargetInfo::UnsignedShort;
}
};

template <typename Target>
class LLVM_LIBRARY_VISIBILITY NaClTargetInfo : public OSTargetInfo<Target> {
protected:
void getOSDefines(const LangOptions &Opts, const llvm::Triple &Triple,
MacroBuilder &Builder) const override {
if (Opts.POSIXThreads)
Builder.defineMacro("_REENTRANT");
if (Opts.CPlusPlus)
Builder.defineMacro("_GNU_SOURCE");

DefineStd(Builder, "unix", Opts);
Builder.defineMacro("__ELF__");
Builder.defineMacro("__native_client__");
}

public:
NaClTargetInfo(const llvm::Triple &Triple, const TargetOptions &Opts)
: OSTargetInfo<Target>(Triple, Opts) {
this->LongAlign = 32;
this->LongWidth = 32;
this->PointerAlign = 32;
this->PointerWidth = 32;
this->IntMaxType = TargetInfo::SignedLongLong;
this->Int64Type = TargetInfo::SignedLongLong;
this->DoubleAlign = 64;
this->LongDoubleWidth = 64;
this->LongDoubleAlign = 64;
this->LongLongWidth = 64;
this->LongLongAlign = 64;
this->SizeType = TargetInfo::UnsignedInt;
this->PtrDiffType = TargetInfo::SignedInt;
this->IntPtrType = TargetInfo::SignedInt;

this->LongDoubleFormat = &llvm::APFloat::IEEEdouble();
if (Triple.getArch() == llvm::Triple::arm) {

} else if (Triple.getArch() == llvm::Triple::x86) {
this->resetDataLayout("e-m:e-p:32:32-p270:32:32-p271:32:32-p272:64:64-"
"i64:64-n8:16:32-S128");
} else if (Triple.getArch() == llvm::Triple::x86_64) {
this->resetDataLayout("e-m:e-p:32:32-p270:32:32-p271:32:32-p272:64:64-"
"i64:64-n8:16:32:64-S128");
} else if (Triple.getArch() == llvm::Triple::mipsel) {

} else {
assert(Triple.getArch() == llvm::Triple::le32);
this->resetDataLayout("e-p:32:32-i64:64");
}
}
};


template <typename Target>
class LLVM_LIBRARY_VISIBILITY FuchsiaTargetInfo : public OSTargetInfo<Target> {
protected:
void getOSDefines(const LangOptions &Opts, const llvm::Triple &Triple,
MacroBuilder &Builder) const override {
Builder.defineMacro("__Fuchsia__");
Builder.defineMacro("__ELF__");
if (Opts.POSIXThreads)
Builder.defineMacro("_REENTRANT");

if (Opts.CPlusPlus)
Builder.defineMacro("_GNU_SOURCE");
}

public:
FuchsiaTargetInfo(const llvm::Triple &Triple, const TargetOptions &Opts)
: OSTargetInfo<Target>(Triple, Opts) {
this->MCountName = "__mcount";
this->TheCXXABI.set(TargetCXXABI::Fuchsia);
}
};


template <typename Target>
class LLVM_LIBRARY_VISIBILITY WebAssemblyOSTargetInfo
: public OSTargetInfo<Target> {
protected:
void getOSDefines(const LangOptions &Opts, const llvm::Triple &Triple,
MacroBuilder &Builder) const override {

if (Opts.POSIXThreads)
Builder.defineMacro("_REENTRANT");

if (Opts.CPlusPlus)
Builder.defineMacro("_GNU_SOURCE");

Builder.defineMacro("__FLOAT128__");
}

public:
explicit WebAssemblyOSTargetInfo(const llvm::Triple &Triple,
const TargetOptions &Opts)
: OSTargetInfo<Target>(Triple, Opts) {
this->MCountName = "__mcount";
this->TheCXXABI.set(TargetCXXABI::WebAssembly);
this->HasFloat128 = true;
}
};


template <typename Target>
class LLVM_LIBRARY_VISIBILITY WASITargetInfo
: public WebAssemblyOSTargetInfo<Target> {
void getOSDefines(const LangOptions &Opts, const llvm::Triple &Triple,
MacroBuilder &Builder) const final {
WebAssemblyOSTargetInfo<Target>::getOSDefines(Opts, Triple, Builder);
Builder.defineMacro("__wasi__");
}

public:
explicit WASITargetInfo(const llvm::Triple &Triple, const TargetOptions &Opts)
: WebAssemblyOSTargetInfo<Target>(Triple, Opts) {}
};


template <typename Target>
class LLVM_LIBRARY_VISIBILITY EmscriptenTargetInfo
: public WebAssemblyOSTargetInfo<Target> {
void getOSDefines(const LangOptions &Opts, const llvm::Triple &Triple,
MacroBuilder &Builder) const final {
WebAssemblyOSTargetInfo<Target>::getOSDefines(Opts, Triple, Builder);
Builder.defineMacro("__EMSCRIPTEN__");
if (Opts.POSIXThreads)
Builder.defineMacro("__EMSCRIPTEN_PTHREADS__");
}

public:
explicit EmscriptenTargetInfo(const llvm::Triple &Triple,
const TargetOptions &Opts)
: WebAssemblyOSTargetInfo<Target>(Triple, Opts) {





this->LongDoubleAlign = 64;
}
};

}
}
#endif
