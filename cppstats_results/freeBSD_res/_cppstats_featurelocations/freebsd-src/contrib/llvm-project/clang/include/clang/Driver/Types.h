







#if !defined(LLVM_CLANG_DRIVER_TYPES_H)
#define LLVM_CLANG_DRIVER_TYPES_H

#include "clang/Driver/Phases.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/Option/ArgList.h"

namespace llvm {
class StringRef;
}
namespace clang {
namespace driver {
class Driver;
namespace types {
enum ID {
TY_INVALID,
#define TYPE(NAME, ID, PP_TYPE, TEMP_SUFFIX, ...) TY_##ID,
#include "clang/Driver/Types.def"
#undef TYPE
TY_LAST
};


const char *getTypeName(ID Id);




ID getPreprocessedType(ID Id);




ID getPrecompiledType(ID Id);



const char *getTypeTempSuffix(ID Id, bool CLMode = false);


bool onlyPrecompileType(ID Id);




bool canTypeBeUserSpecified(ID Id);




bool appendSuffixForType(ID Id);




bool canLipoType(ID Id);


bool isAcceptedByClang(ID Id);


bool isCXX(ID Id);


bool isLLVMIR(ID Id);


bool isCuda(ID Id);


bool isHIP(ID Id);


bool isObjC(ID Id);


bool isOpenCL(ID Id);


bool isFortran(ID Id);




bool isSrcFile(ID Id);



ID lookupTypeForExtension(llvm::StringRef Ext);



ID lookupTypeForTypeSpecifier(const char *Name);



llvm::SmallVector<phases::ID, phases::MaxNumberOfPhases>
getCompilationPhases(ID Id, phases::ID LastPhase = phases::LastPhase);
llvm::SmallVector<phases::ID, phases::MaxNumberOfPhases>
getCompilationPhases(const clang::driver::Driver &Driver,
llvm::opt::DerivedArgList &DAL, ID Id);



ID lookupCXXTypeForCType(ID Id);



ID lookupHeaderTypeForSourceType(ID Id);

}
}
}

#endif
