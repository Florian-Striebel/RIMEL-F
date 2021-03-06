







#if !defined(LLVM_CLANG_BASIC_LANGSTANDARD_H)
#define LLVM_CLANG_BASIC_LANGSTANDARD_H

#include "clang/Basic/LLVM.h"
#include "llvm/ADT/StringRef.h"

namespace clang {



enum class Language : uint8_t {
Unknown,


Asm,



LLVM_IR,


C,
CXX,
ObjC,
ObjCXX,
OpenCL,
OpenCLCXX,
CUDA,
RenderScript,
HIP,

};

enum LangFeatures {
LineComment = (1 << 0),
C99 = (1 << 1),
C11 = (1 << 2),
C17 = (1 << 3),
C2x = (1 << 4),
CPlusPlus = (1 << 5),
CPlusPlus11 = (1 << 6),
CPlusPlus14 = (1 << 7),
CPlusPlus17 = (1 << 8),
CPlusPlus20 = (1 << 9),
CPlusPlus2b = (1 << 10),
Digraphs = (1 << 11),
GNUMode = (1 << 12),
HexFloat = (1 << 13),
ImplicitInt = (1 << 14),
OpenCL = (1 << 15)
};



struct LangStandard {
enum Kind {
#define LANGSTANDARD(id, name, lang, desc, features) lang_##id,

#include "clang/Basic/LangStandards.def"
lang_unspecified
};

const char *ShortName;
const char *Description;
unsigned Flags;
clang::Language Language;

public:

const char *getName() const { return ShortName; }


const char *getDescription() const { return Description; }


clang::Language getLanguage() const { return Language; }


bool hasLineComments() const { return Flags & LineComment; }


bool isC99() const { return Flags & C99; }


bool isC11() const { return Flags & C11; }


bool isC17() const { return Flags & C17; }


bool isC2x() const { return Flags & C2x; }


bool isCPlusPlus() const { return Flags & CPlusPlus; }


bool isCPlusPlus11() const { return Flags & CPlusPlus11; }


bool isCPlusPlus14() const { return Flags & CPlusPlus14; }


bool isCPlusPlus17() const { return Flags & CPlusPlus17; }


bool isCPlusPlus20() const { return Flags & CPlusPlus20; }


bool isCPlusPlus2b() const { return Flags & CPlusPlus2b; }


bool hasDigraphs() const { return Flags & Digraphs; }


bool isGNUMode() const { return Flags & GNUMode; }


bool hasHexFloats() const { return Flags & HexFloat; }


bool hasImplicitInt() const { return Flags & ImplicitInt; }


bool isOpenCL() const { return Flags & OpenCL; }

static Kind getLangKind(StringRef Name);
static const LangStandard &getLangStandardForKind(Kind K);
static const LangStandard *getLangStandardForName(StringRef Name);
};

}

#endif
