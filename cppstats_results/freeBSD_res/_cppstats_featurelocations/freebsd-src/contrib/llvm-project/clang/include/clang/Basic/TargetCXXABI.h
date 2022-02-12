













#if !defined(LLVM_CLANG_BASIC_TARGETCXXABI_H)
#define LLVM_CLANG_BASIC_TARGETCXXABI_H

#include <map>

#include "clang/Basic/LLVM.h"
#include "llvm/ADT/StringMap.h"
#include "llvm/ADT/Triple.h"
#include "llvm/Support/ErrorHandling.h"

namespace clang {


class TargetCXXABI {
public:

enum Kind {
#define CXXABI(Name, Str) Name,
#include "TargetCXXABI.def"
};

private:



Kind TheKind;

static const auto &getABIMap() {
static llvm::StringMap<Kind> ABIMap = {
#define CXXABI(Name, Str) {Str, Name},
#include "TargetCXXABI.def"
};
return ABIMap;
}

static const auto &getSpellingMap() {
static std::map<Kind, std::string> SpellingMap = {
#define CXXABI(Name, Str) {Name, Str},
#include "TargetCXXABI.def"
};
return SpellingMap;
}

public:
static Kind getKind(StringRef Name) { return getABIMap().lookup(Name); }
static const auto &getSpelling(Kind ABIKind) {
return getSpellingMap().find(ABIKind)->second;
}
static bool isABI(StringRef Name) {
return getABIMap().find(Name) != getABIMap().end();
}



static bool usesRelativeVTables(const llvm::Triple &T) {
return T.isOSFuchsia();
}


TargetCXXABI() : TheKind(GenericItanium) {}

TargetCXXABI(Kind kind) : TheKind(kind) {}

void set(Kind kind) {
TheKind = kind;
}

Kind getKind() const { return TheKind; }






static bool isSupportedCXXABI(const llvm::Triple &T, Kind Kind) {
switch (Kind) {
case GenericARM:
return T.isARM() || T.isAArch64();

case iOS:
case WatchOS:
case AppleARM64:
return T.isOSDarwin();

case Fuchsia:
return T.isOSFuchsia();

case GenericAArch64:
return T.isAArch64();

case GenericMIPS:
return T.isMIPS();

case WebAssembly:
return T.isWasm();

case XL:
return T.isOSAIX();

case GenericItanium:
return true;

case Microsoft:
return T.isKnownWindowsMSVCEnvironment();
}
llvm_unreachable("invalid CXXABI kind");
};


bool isItaniumFamily() const {
switch (getKind()) {
#define CXXABI(Name, Str)
#define ITANIUM_CXXABI(Name, Str) case Name:
#include "TargetCXXABI.def"
return true;

default:
return false;
}
llvm_unreachable("bad ABI kind");
}


bool isMicrosoft() const {
switch (getKind()) {
#define CXXABI(Name, Str)
#define MICROSOFT_CXXABI(Name, Str) case Name:
#include "TargetCXXABI.def"
return true;

default:
return false;
}
llvm_unreachable("bad ABI kind");
}










bool areMemberFunctionsAligned() const {
switch (getKind()) {
case WebAssembly:

return false;
case AppleARM64:
case Fuchsia:
case GenericARM:
case GenericAArch64:
case GenericMIPS:



case GenericItanium:
case iOS:
case WatchOS:
case Microsoft:
case XL:
return true;
}
llvm_unreachable("bad ABI kind");
}








bool areArgsDestroyedLeftToRightInCallee() const {
return isMicrosoft();
}



bool hasConstructorVariants() const {
return isItaniumFamily();
}


bool hasPrimaryVBases() const {
return isItaniumFamily();
}




bool hasKeyFunctions() const {
return isItaniumFamily();
}





























bool canKeyFunctionBeInline() const {
switch (getKind()) {
case AppleARM64:
case Fuchsia:
case GenericARM:
case WebAssembly:
case WatchOS:
return false;

case GenericAArch64:
case GenericItanium:
case iOS:
case Microsoft:
case GenericMIPS:
case XL:
return true;
}
llvm_unreachable("bad ABI kind");
}










enum TailPaddingUseRules {


AlwaysUseTailPadding,



UseTailPaddingUnlessPOD03,



UseTailPaddingUnlessPOD11
};
TailPaddingUseRules getTailPaddingUseRules() const {
switch (getKind()) {



case GenericItanium:
case GenericAArch64:
case GenericARM:
case iOS:
case GenericMIPS:
case XL:
return UseTailPaddingUnlessPOD03;



case AppleARM64:
case Fuchsia:
case WebAssembly:
case WatchOS:
return UseTailPaddingUnlessPOD11;



case Microsoft:
return AlwaysUseTailPadding;
}
llvm_unreachable("bad ABI kind");
}

friend bool operator==(const TargetCXXABI &left, const TargetCXXABI &right) {
return left.getKind() == right.getKind();
}

friend bool operator!=(const TargetCXXABI &left, const TargetCXXABI &right) {
return !(left == right);
}
};

}

#endif
