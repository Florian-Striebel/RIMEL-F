











#if !defined(ORC_RT_COMMON_H)
#define ORC_RT_COMMON_H

#include "c_api.h"
#include "compiler.h"
#include <type_traits>



#define ORC_RT_JIT_DISPATCH_TAG(X) extern "C" char X; char X = 0;




struct __orc_rt_Opaque {};


extern "C" void __orc_rt_log_error(const char *ErrMsg);






extern "C" __orc_rt_Opaque __orc_rt_jit_dispatch_ctx ORC_RT_WEAK_IMPORT;






extern "C" __orc_rt_CWrapperFunctionResult
__orc_rt_jit_dispatch(__orc_rt_Opaque *DispatchCtx, const void *FnTag,
const char *Data, size_t Size) ORC_RT_WEAK_IMPORT;

#endif
