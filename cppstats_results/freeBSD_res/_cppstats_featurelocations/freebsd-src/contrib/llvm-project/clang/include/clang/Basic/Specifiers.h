













#if !defined(LLVM_CLANG_BASIC_SPECIFIERS_H)
#define LLVM_CLANG_BASIC_SPECIFIERS_H

#include "llvm/ADT/StringRef.h"
#include "llvm/Support/DataTypes.h"
#include "llvm/Support/ErrorHandling.h"

namespace clang {


enum class ExplicitSpecKind : unsigned {
ResolvedFalse,
ResolvedTrue,
Unresolved,
};


enum class ConstexprSpecKind { Unspecified, Constexpr, Consteval, Constinit };


enum class TypeSpecifierWidth { Unspecified, Short, Long, LongLong };


enum class TypeSpecifierSign { Unspecified, Signed, Unsigned };

enum class TypeSpecifiersPipe { Unspecified, Pipe };


enum TypeSpecifierType {
TST_unspecified,
TST_void,
TST_char,
TST_wchar,
TST_char8,
TST_char16,
TST_char32,
TST_int,
TST_int128,
TST_extint,
TST_half,
TST_Float16,
TST_Accum,
TST_Fract,
TST_BFloat16,
TST_float,
TST_double,
TST_float128,
TST_bool,
TST_decimal32,
TST_decimal64,
TST_decimal128,
TST_enum,
TST_union,
TST_struct,
TST_class,
TST_interface,
TST_typename,
TST_typeofType,
TST_typeofExpr,
TST_decltype,
TST_underlyingType,
TST_auto,
TST_decltype_auto,
TST_auto_type,
TST_unknown_anytype,
TST_atomic,
#define GENERIC_IMAGE_TYPE(ImgType, Id) TST_##ImgType##_t,
#include "clang/Basic/OpenCLImageTypes.def"
TST_error
};



struct WrittenBuiltinSpecs {
static_assert(TST_error < 1 << 6, "Type bitfield not wide enough for TST");
unsigned Type : 6;
unsigned Sign : 2;
unsigned Width : 2;
unsigned ModeAttr : 1;
};



enum AccessSpecifier {
AS_public,
AS_protected,
AS_private,
AS_none
};



enum ExprValueKind {


VK_PRValue,



VK_LValue,




VK_XValue
};



enum ExprObjectKind {

OK_Ordinary,


OK_BitField,


OK_VectorComponent,



OK_ObjCProperty,




OK_ObjCSubscript,


OK_MatrixComponent
};


enum NonOdrUseReason {

NOUR_None = 0,

NOUR_Unevaluated,


NOUR_Constant,


NOUR_Discarded,
};



enum TemplateSpecializationKind {


TSK_Undeclared = 0,


TSK_ImplicitInstantiation,



TSK_ExplicitSpecialization,



TSK_ExplicitInstantiationDeclaration,



TSK_ExplicitInstantiationDefinition
};




inline bool isTemplateInstantiation(TemplateSpecializationKind Kind) {
return Kind != TSK_Undeclared && Kind != TSK_ExplicitSpecialization;
}




inline bool isTemplateExplicitInstantiationOrSpecialization(
TemplateSpecializationKind Kind) {
switch (Kind) {
case TSK_ExplicitSpecialization:
case TSK_ExplicitInstantiationDeclaration:
case TSK_ExplicitInstantiationDefinition:
return true;

case TSK_Undeclared:
case TSK_ImplicitInstantiation:
return false;
}
llvm_unreachable("bad template specialization kind");
}


enum ThreadStorageClassSpecifier {
TSCS_unspecified,

TSCS___thread,


TSCS_thread_local,


TSCS__Thread_local
};


enum StorageClass {

SC_None,
SC_Extern,
SC_Static,
SC_PrivateExtern,


SC_Auto,
SC_Register
};


inline bool isLegalForFunction(StorageClass SC) {
return SC <= SC_PrivateExtern;
}


inline bool isLegalForVariable(StorageClass SC) {
return true;
}


enum InClassInitStyle {
ICIS_NoInit,
ICIS_CopyInit,
ICIS_ListInit
};


enum CallingConv {
CC_C,
CC_X86StdCall,
CC_X86FastCall,
CC_X86ThisCall,
CC_X86VectorCall,
CC_X86Pascal,
CC_Win64,
CC_X86_64SysV,
CC_X86RegCall,
CC_AAPCS,
CC_AAPCS_VFP,
CC_IntelOclBicc,
CC_SpirFunction,
CC_OpenCLKernel,
CC_Swift,
CC_SwiftAsync,
CC_PreserveMost,
CC_PreserveAll,
CC_AArch64VectorCall,
};



inline bool supportsVariadicCall(CallingConv CC) {
switch (CC) {
case CC_X86StdCall:
case CC_X86FastCall:
case CC_X86ThisCall:
case CC_X86RegCall:
case CC_X86Pascal:
case CC_X86VectorCall:
case CC_SpirFunction:
case CC_OpenCLKernel:
case CC_Swift:
case CC_SwiftAsync:
return false;
default:
return true;
}
}


enum StorageDuration {
SD_FullExpression,
SD_Automatic,
SD_Thread,
SD_Static,
SD_Dynamic
};


enum class NullabilityKind : uint8_t {

NonNull = 0,

Nullable,




Unspecified,




NullableResult,
};



inline bool hasWeakerNullability(NullabilityKind L, NullabilityKind R) {
return uint8_t(L) > uint8_t(R);
}


llvm::StringRef getNullabilitySpelling(NullabilityKind kind,
bool isContextSensitive = false);


enum class ParameterABI {

Ordinary,



SwiftIndirectResult,




SwiftErrorResult,




SwiftContext,




SwiftAsyncContext,
};



enum class MSInheritanceModel {
Single = 0,
Multiple = 1,
Virtual = 2,
Unspecified = 3,
};

llvm::StringRef getParameterABISpelling(ParameterABI kind);

inline llvm::StringRef getAccessSpelling(AccessSpecifier AS) {
switch (AS) {
case AccessSpecifier::AS_public:
return "public";
case AccessSpecifier::AS_protected:
return "protected";
case AccessSpecifier::AS_private:
return "private";
case AccessSpecifier::AS_none:
return {};
}
llvm_unreachable("Unknown AccessSpecifier");
}
}

#endif
