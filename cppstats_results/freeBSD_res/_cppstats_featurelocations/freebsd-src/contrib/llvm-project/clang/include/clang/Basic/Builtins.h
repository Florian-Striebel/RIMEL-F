













#if !defined(LLVM_CLANG_BASIC_BUILTINS_H)
#define LLVM_CLANG_BASIC_BUILTINS_H

#include "llvm/ADT/ArrayRef.h"
#include <cstring>



#undef alloca

namespace clang {
class TargetInfo;
class IdentifierTable;
class LangOptions;

enum LanguageID {
GNU_LANG = 0x1,
C_LANG = 0x2,
CXX_LANG = 0x4,
OBJC_LANG = 0x8,
MS_LANG = 0x10,
OCLC20_LANG = 0x20,
OCLC1X_LANG = 0x40,
OMP_LANG = 0x80,
CUDA_LANG = 0x100,
COR_LANG = 0x200,
ALL_LANGUAGES = C_LANG | CXX_LANG | OBJC_LANG,
ALL_GNU_LANGUAGES = ALL_LANGUAGES | GNU_LANG,
ALL_MS_LANGUAGES = ALL_LANGUAGES | MS_LANG,
ALL_OCLC_LANGUAGES = OCLC1X_LANG | OCLC20_LANG
};

namespace Builtin {
enum ID {
NotBuiltin = 0,
#define BUILTIN(ID, TYPE, ATTRS) BI##ID,
#include "clang/Basic/Builtins.def"
FirstTSBuiltin
};

struct Info {
const char *Name, *Type, *Attributes, *HeaderName;
LanguageID Langs;
const char *Features;
};







class Context {
llvm::ArrayRef<Info> TSRecords;
llvm::ArrayRef<Info> AuxTSRecords;

public:
Context() {}



void InitializeTarget(const TargetInfo &Target, const TargetInfo *AuxTarget);




void initializeBuiltins(IdentifierTable &Table, const LangOptions& LangOpts);



const char *getName(unsigned ID) const {
return getRecord(ID).Name;
}


const char *getTypeString(unsigned ID) const {
return getRecord(ID).Type;
}


bool isTSBuiltin(unsigned ID) const {
return ID >= Builtin::FirstTSBuiltin;
}


bool isPure(unsigned ID) const {
return strchr(getRecord(ID).Attributes, 'U') != nullptr;
}



bool isConst(unsigned ID) const {
return strchr(getRecord(ID).Attributes, 'c') != nullptr;
}


bool isNoThrow(unsigned ID) const {
return strchr(getRecord(ID).Attributes, 'n') != nullptr;
}


bool isNoReturn(unsigned ID) const {
return strchr(getRecord(ID).Attributes, 'r') != nullptr;
}


bool isReturnsTwice(unsigned ID) const {
return strchr(getRecord(ID).Attributes, 'j') != nullptr;
}



bool isUnevaluated(unsigned ID) const {
return strchr(getRecord(ID).Attributes, 'u') != nullptr;
}



bool isLibFunction(unsigned ID) const {
return strchr(getRecord(ID).Attributes, 'F') != nullptr;
}




bool isPredefinedLibFunction(unsigned ID) const {
return strchr(getRecord(ID).Attributes, 'f') != nullptr;
}




bool isHeaderDependentFunction(unsigned ID) const {
return strchr(getRecord(ID).Attributes, 'h') != nullptr;
}




bool isPredefinedRuntimeFunction(unsigned ID) const {
return strchr(getRecord(ID).Attributes, 'i') != nullptr;
}


bool hasCustomTypechecking(unsigned ID) const {
return strchr(getRecord(ID).Attributes, 't') != nullptr;
}



bool allowTypeMismatch(unsigned ID) const {
return strchr(getRecord(ID).Attributes, 'T') != nullptr ||
hasCustomTypechecking(ID);
}



bool hasPtrArgsOrResult(unsigned ID) const {
return strchr(getRecord(ID).Type, '*') != nullptr;
}



bool hasReferenceArgsOrResult(unsigned ID) const {
return strchr(getRecord(ID).Type, '&') != nullptr ||
strchr(getRecord(ID).Type, 'A') != nullptr;
}



const char *getHeaderName(unsigned ID) const {
return getRecord(ID).HeaderName;
}




bool isPrintfLike(unsigned ID, unsigned &FormatIdx, bool &HasVAListArg);




bool isScanfLike(unsigned ID, unsigned &FormatIdx, bool &HasVAListArg);




bool performsCallback(unsigned ID,
llvm::SmallVectorImpl<int> &Encoding) const;





bool isConstWithoutErrno(unsigned ID) const {
return strchr(getRecord(ID).Attributes, 'e') != nullptr;
}

const char *getRequiredFeatures(unsigned ID) const {
return getRecord(ID).Features;
}

unsigned getRequiredVectorWidth(unsigned ID) const;


bool isAuxBuiltinID(unsigned ID) const {
return ID >= (Builtin::FirstTSBuiltin + TSRecords.size());
}



unsigned getAuxBuiltinID(unsigned ID) const { return ID - TSRecords.size(); }



static bool isBuiltinFunc(llvm::StringRef Name);



bool canBeRedeclared(unsigned ID) const;

private:
const Info &getRecord(unsigned ID) const;


bool builtinIsSupported(const Builtin::Info &BuiltinInfo,
const LangOptions &LangOpts);


bool isLike(unsigned ID, unsigned &FormatIdx, bool &HasVAListArg,
const char *Fmt) const;
};

}


enum BuiltinTemplateKind : int {

BTK__make_integer_seq,


BTK__type_pack_element
};

}
#endif
