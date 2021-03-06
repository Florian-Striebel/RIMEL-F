












#if !defined(LLVM_CLANG_BASIC_OBJCRUNTIME_H)
#define LLVM_CLANG_BASIC_OBJCRUNTIME_H

#include "clang/Basic/LLVM.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/Triple.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/VersionTuple.h"
#include <string>

namespace clang {


class ObjCRuntime {
public:

enum Kind {



MacOSX,




FragileMacOSX,




iOS,



WatchOS,



GCC,


GNUstep,


ObjFW
};

private:
Kind TheKind = MacOSX;
VersionTuple Version;

public:

ObjCRuntime() = default;
ObjCRuntime(Kind kind, const VersionTuple &version)
: TheKind(kind), Version(version) {}

void set(Kind kind, VersionTuple version) {
TheKind = kind;
Version = version;
}

Kind getKind() const { return TheKind; }
const VersionTuple &getVersion() const { return Version; }



bool isNonFragile() const {
switch (getKind()) {
case FragileMacOSX: return false;
case GCC: return false;
case MacOSX: return true;
case GNUstep: return true;
case ObjFW: return true;
case iOS: return true;
case WatchOS: return true;
}
llvm_unreachable("bad kind");
}



bool isFragile() const { return !isNonFragile(); }


bool isLegacyDispatchDefaultForArch(llvm::Triple::ArchType Arch) {


if (getKind() == GNUstep && getVersion() >= VersionTuple(1, 6)) {
if (Arch == llvm::Triple::arm ||
Arch == llvm::Triple::x86 ||
Arch == llvm::Triple::x86_64)
return false;
}
else if ((getKind() == MacOSX) && isNonFragile() &&
(getVersion() >= VersionTuple(10, 0)) &&
(getVersion() < VersionTuple(10, 6)))
return Arch != llvm::Triple::x86_64;


return true;
}


bool isGNUFamily() const {
switch (getKind()) {
case FragileMacOSX:
case MacOSX:
case iOS:
case WatchOS:
return false;
case GCC:
case GNUstep:
case ObjFW:
return true;
}
llvm_unreachable("bad kind");
}


bool isNeXTFamily() const {


return !isGNUFamily();
}


bool allowsARC() const {
switch (getKind()) {
case FragileMacOSX:

return getVersion() >= VersionTuple(10, 7);
case MacOSX: return true;
case iOS: return true;
case WatchOS: return true;
case GCC: return false;
case GNUstep: return true;
case ObjFW: return true;
}
llvm_unreachable("bad kind");
}






bool hasNativeARC() const {
switch (getKind()) {
case FragileMacOSX: return getVersion() >= VersionTuple(10, 7);
case MacOSX: return getVersion() >= VersionTuple(10, 7);
case iOS: return getVersion() >= VersionTuple(5);
case WatchOS: return true;

case GCC: return false;
case GNUstep: return getVersion() >= VersionTuple(1, 6);
case ObjFW: return true;
}
llvm_unreachable("bad kind");
}


















bool shouldUseARCFunctionsForRetainRelease() const {
switch (getKind()) {
case FragileMacOSX:
return false;
case MacOSX:
return getVersion() >= VersionTuple(10, 10);
case iOS:
return getVersion() >= VersionTuple(8);
case WatchOS:
return true;
case GCC:
return false;
case GNUstep:
return false;
case ObjFW:
return false;
}
llvm_unreachable("bad kind");
}

















bool shouldUseRuntimeFunctionsForAlloc() const {
switch (getKind()) {
case FragileMacOSX:
return false;
case MacOSX:
return getVersion() >= VersionTuple(10, 10);
case iOS:
return getVersion() >= VersionTuple(8);
case WatchOS:
return true;

case GCC:
return false;
case GNUstep:
return false;
case ObjFW:
return false;
}
llvm_unreachable("bad kind");
}




bool shouldUseRuntimeFunctionForCombinedAllocInit() const {
switch (getKind()) {
case MacOSX:
return getVersion() >= VersionTuple(10, 14, 4);
case iOS:
return getVersion() >= VersionTuple(12, 2);
case WatchOS:
return getVersion() >= VersionTuple(5, 2);
default:
return false;
}
}


bool hasOptimizedSetter() const {
switch (getKind()) {
case MacOSX:
return getVersion() >= VersionTuple(10, 8);
case iOS:
return (getVersion() >= VersionTuple(6));
case WatchOS:
return true;
case GNUstep:
return getVersion() >= VersionTuple(1, 7);
default:
return false;
}
}


bool allowsWeak() const {
return hasNativeWeak();
}



bool hasNativeWeak() const {


return hasNativeARC();
}




bool hasSubscripting() const {
switch (getKind()) {
case FragileMacOSX: return false;
case MacOSX: return getVersion() >= VersionTuple(10, 11);
case iOS: return getVersion() >= VersionTuple(9);
case WatchOS: return true;




case GCC: return true;
case GNUstep: return true;
case ObjFW: return true;
}
llvm_unreachable("bad kind");
}


bool allowsSizeofAlignof() const {
return isFragile();
}





bool allowsPointerArithmetic() const {
switch (getKind()) {
case FragileMacOSX:
case GCC:
return true;
case MacOSX:
case iOS:
case WatchOS:
case GNUstep:
case ObjFW:
return false;
}
llvm_unreachable("bad kind");
}


bool isSubscriptPointerArithmetic() const {
return allowsPointerArithmetic();
}





bool hasTerminate() const {
switch (getKind()) {
case FragileMacOSX: return getVersion() >= VersionTuple(10, 8);
case MacOSX: return getVersion() >= VersionTuple(10, 8);
case iOS: return getVersion() >= VersionTuple(5);
case WatchOS: return true;
case GCC: return false;
case GNUstep: return false;
case ObjFW: return false;
}
llvm_unreachable("bad kind");
}


bool hasWeakClassImport() const {
switch (getKind()) {
case MacOSX: return true;
case iOS: return true;
case WatchOS: return true;
case FragileMacOSX: return false;
case GCC: return true;
case GNUstep: return true;
case ObjFW: return true;
}
llvm_unreachable("bad kind");
}


bool hasUnwindExceptions() const {
switch (getKind()) {
case MacOSX: return true;
case iOS: return true;
case WatchOS: return true;
case FragileMacOSX: return false;
case GCC: return true;
case GNUstep: return true;
case ObjFW: return true;
}
llvm_unreachable("bad kind");
}

bool hasAtomicCopyHelper() const {
switch (getKind()) {
case FragileMacOSX:
case MacOSX:
case iOS:
case WatchOS:
return true;
case GNUstep:
return getVersion() >= VersionTuple(1, 7);
default: return false;
}
}


bool hasARCUnsafeClaimAutoreleasedReturnValue() const {
switch (getKind()) {
case MacOSX:
case FragileMacOSX:
return getVersion() >= VersionTuple(10, 11);
case iOS:
return getVersion() >= VersionTuple(9);
case WatchOS:
return getVersion() >= VersionTuple(2);
case GNUstep:
return false;
default:
return false;
}
}


bool hasEmptyCollections() const {
switch (getKind()) {
default:
return false;
case MacOSX:
return getVersion() >= VersionTuple(10, 11);
case iOS:
return getVersion() >= VersionTuple(9);
case WatchOS:
return getVersion() >= VersionTuple(2);
}
}



bool allowsClassStubs() const {
switch (getKind()) {
case FragileMacOSX:
case GCC:
case GNUstep:
case ObjFW:
return false;
case MacOSX:
case iOS:
case WatchOS:
return true;
}
llvm_unreachable("bad kind");
}


bool allowsDirectDispatch() const {
switch (getKind()) {
case FragileMacOSX: return false;
case MacOSX: return true;
case iOS: return true;
case WatchOS: return true;
case GCC: return false;
case GNUstep: return false;
case ObjFW: return false;
}
llvm_unreachable("bad kind");
}





bool tryParse(StringRef input);

std::string getAsString() const;

friend bool operator==(const ObjCRuntime &left, const ObjCRuntime &right) {
return left.getKind() == right.getKind() &&
left.getVersion() == right.getVersion();
}

friend bool operator!=(const ObjCRuntime &left, const ObjCRuntime &right) {
return !(left == right);
}

friend llvm::hash_code hash_value(const ObjCRuntime &OCR) {
return llvm::hash_combine(OCR.getKind(), OCR.getVersion());
}
};

raw_ostream &operator<<(raw_ostream &out, const ObjCRuntime &value);

}

#endif
