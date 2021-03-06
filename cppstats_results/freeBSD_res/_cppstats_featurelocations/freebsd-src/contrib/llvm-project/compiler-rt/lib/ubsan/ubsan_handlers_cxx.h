











#if !defined(UBSAN_HANDLERS_CXX_H)
#define UBSAN_HANDLERS_CXX_H

#include "ubsan_value.h"

namespace __ubsan {

struct DynamicTypeCacheMissData {
SourceLocation Loc;
const TypeDescriptor &Type;
void *TypeInfo;
unsigned char TypeCheckKind;
};




extern "C" SANITIZER_INTERFACE_ATTRIBUTE
void __ubsan_handle_dynamic_type_cache_miss(
DynamicTypeCacheMissData *Data, ValueHandle Pointer, ValueHandle Hash);
extern "C" SANITIZER_INTERFACE_ATTRIBUTE
void __ubsan_handle_dynamic_type_cache_miss_abort(
DynamicTypeCacheMissData *Data, ValueHandle Pointer, ValueHandle Hash);

struct FunctionTypeMismatchData {
SourceLocation Loc;
const TypeDescriptor &Type;
};

extern "C" SANITIZER_INTERFACE_ATTRIBUTE void
__ubsan_handle_function_type_mismatch_v1(FunctionTypeMismatchData *Data,
ValueHandle Val,
ValueHandle calleeRTTI,
ValueHandle fnRTTI);
extern "C" SANITIZER_INTERFACE_ATTRIBUTE void
__ubsan_handle_function_type_mismatch_v1_abort(FunctionTypeMismatchData *Data,
ValueHandle Val,
ValueHandle calleeRTTI,
ValueHandle fnRTTI);
}

#endif
