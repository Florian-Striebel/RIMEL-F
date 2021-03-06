












#if !defined(LLVM_CLANG_BASIC_ALLDIAGNOSTICS_H)
#define LLVM_CLANG_BASIC_ALLDIAGNOSTICS_H

#include "clang/Basic/DiagnosticAST.h"
#include "clang/Basic/DiagnosticAnalysis.h"
#include "clang/Basic/DiagnosticComment.h"
#include "clang/Basic/DiagnosticCrossTU.h"
#include "clang/Basic/DiagnosticDriver.h"
#include "clang/Basic/DiagnosticFrontend.h"
#include "clang/Basic/DiagnosticLex.h"
#include "clang/Basic/DiagnosticParse.h"
#include "clang/Basic/DiagnosticSema.h"
#include "clang/Basic/DiagnosticSerialization.h"
#include "clang/Basic/DiagnosticRefactoring.h"

namespace clang {
template <size_t SizeOfStr, typename FieldType>
class StringSizerHelper {
static_assert(SizeOfStr <= FieldType(~0U), "Field too small!");
public:
enum { Size = SizeOfStr };
};
}

#define STR_SIZE(str, fieldTy) clang::StringSizerHelper<sizeof(str)-1, fieldTy>::Size


#endif
