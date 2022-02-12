















#if !defined(ORC_RT_C_API_H)
#define ORC_RT_C_API_H

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#if defined(__clang__)
#define ORC_RT_C_STRICT_PROTOTYPES_BEGIN _Pragma("clang diagnostic push") _Pragma("clang diagnostic error \"-Wstrict-prototypes\"")


#define ORC_RT_C_STRICT_PROTOTYPES_END _Pragma("clang diagnostic pop")
#else
#define ORC_RT_C_STRICT_PROTOTYPES_BEGIN
#define ORC_RT_C_STRICT_PROTOTYPES_END
#endif


#if defined(__cplusplus)
#define ORC_RT_C_EXTERN_C_BEGIN extern "C" { ORC_RT_C_STRICT_PROTOTYPES_BEGIN


#define ORC_RT_C_EXTERN_C_END ORC_RT_C_STRICT_PROTOTYPES_END }


#else
#define ORC_RT_C_EXTERN_C_BEGIN ORC_RT_C_STRICT_PROTOTYPES_BEGIN
#define ORC_RT_C_EXTERN_C_END ORC_RT_C_STRICT_PROTOTYPES_END
#endif

ORC_RT_C_EXTERN_C_BEGIN

typedef union {
char *ValuePtr;
char Value[sizeof(ValuePtr)];
} __orc_rt_CWrapperFunctionResultDataUnion;


















typedef struct {
__orc_rt_CWrapperFunctionResultDataUnion Data;
size_t Size;
} __orc_rt_CWrapperFunctionResult;

typedef struct __orc_rt_CSharedOpaqueJITProcessControl
*__orc_rt_SharedJITProcessControlRef;




static inline void
__orc_rt_CWrapperFunctionResultInit(__orc_rt_CWrapperFunctionResult *R) {
R->Size = 0;
R->Data.ValuePtr = 0;
}





static inline char *
__orc_rt_CWrapperFunctionResultAllocate(__orc_rt_CWrapperFunctionResult *R,
size_t Size) {
R->Size = Size;
if (Size <= sizeof(R->Data.Value))
return R->Data.Value;

R->Data.ValuePtr = (char *)malloc(Size);
return R->Data.ValuePtr;
}




static inline __orc_rt_CWrapperFunctionResult
__orc_rt_CreateCWrapperFunctionResultFromRange(const char *Data, size_t Size) {
__orc_rt_CWrapperFunctionResult R;
R.Size = Size;
if (R.Size > sizeof(R.Data.Value)) {
char *Tmp = (char *)malloc(Size);
memcpy(Tmp, Data, Size);
R.Data.ValuePtr = Tmp;
} else
memcpy(R.Data.Value, Data, Size);
return R;
}








static inline __orc_rt_CWrapperFunctionResult
__orc_rt_CreateCWrapperFunctionResultFromString(const char *Source) {
return __orc_rt_CreateCWrapperFunctionResultFromRange(Source,
strlen(Source) + 1);
}








static inline __orc_rt_CWrapperFunctionResult
__orc_rt_CreateCWrapperFunctionResultFromOutOfBandError(const char *ErrMsg) {
__orc_rt_CWrapperFunctionResult R;
R.Size = 0;
char *Tmp = (char *)malloc(strlen(ErrMsg) + 1);
strcpy(Tmp, ErrMsg);
R.Data.ValuePtr = Tmp;
return R;
}





static inline void
__orc_rt_DisposeCWrapperFunctionResult(__orc_rt_CWrapperFunctionResult *R) {
if (R->Size > sizeof(R->Data.Value) ||
(R->Size == 0 && R->Data.ValuePtr))
free(R->Data.ValuePtr);
}





static inline const char *
__orc_rt_CWrapperFunctionResultData(const __orc_rt_CWrapperFunctionResult *R) {
assert((R->Size != 0 || R->Data.ValuePtr == nullptr) &&
"Cannot get data for out-of-band error value");
return R->Size > sizeof(R->Data.Value) ? R->Data.ValuePtr : R->Data.Value;
}






static inline size_t
__orc_rt_CWrapperFunctionResultSize(const __orc_rt_CWrapperFunctionResult *R) {
assert((R->Size != 0 || R->Data.ValuePtr == nullptr) &&
"Cannot get size for out-of-band error value");
return R->Size;
}





static inline size_t
__orc_rt_CWrapperFunctionResultEmpty(const __orc_rt_CWrapperFunctionResult *R) {
return R->Size == 0 && R->Data.ValuePtr == 0;
}








static inline const char *__orc_rt_CWrapperFunctionResultGetOutOfBandError(
const __orc_rt_CWrapperFunctionResult *R) {
return R->Size == 0 ? R->Data.ValuePtr : 0;
}

ORC_RT_C_EXTERN_C_END

#endif
