










#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"
#include "libunwind_ext.h"
#include "libunwind.h"
#include "Unwind-EHABI.h"
#include "unwind.h"

#if defined(_LIBUNWIND_BUILD_ZERO_COST_APIS)

#if defined(_LIBUNWIND_SUPPORT_SEH_UNWIND)
#define private_1 private_[0]
#endif


_LIBUNWIND_EXPORT _Unwind_Reason_Code
_Unwind_Resume_or_Rethrow(_Unwind_Exception *exception_object) {
#if defined(_LIBUNWIND_ARM_EHABI)
_LIBUNWIND_TRACE_API("_Unwind_Resume_or_Rethrow(ex_obj=%p), private_1=%ld",
(void *)exception_object,
(long)exception_object->unwinder_cache.reserved1);
#else
_LIBUNWIND_TRACE_API("_Unwind_Resume_or_Rethrow(ex_obj=%p), private_1=%" PRIdPTR,
(void *)exception_object,
(intptr_t)exception_object->private_1);
#endif

#if defined(_LIBUNWIND_ARM_EHABI)


return _Unwind_RaiseException(exception_object);
#else



if (exception_object->private_1 == 0) {
return _Unwind_RaiseException(exception_object);


}



_Unwind_Resume(exception_object);
_LIBUNWIND_ABORT("_Unwind_Resume_or_Rethrow() called _Unwind_RaiseException()"
" which unexpectedly returned");
#endif
}




_LIBUNWIND_EXPORT uintptr_t
_Unwind_GetDataRelBase(struct _Unwind_Context *context) {
(void)context;
_LIBUNWIND_TRACE_API("_Unwind_GetDataRelBase(context=%p)", (void *)context);
_LIBUNWIND_ABORT("_Unwind_GetDataRelBase() not implemented");
}




_LIBUNWIND_EXPORT uintptr_t
_Unwind_GetTextRelBase(struct _Unwind_Context *context) {
(void)context;
_LIBUNWIND_TRACE_API("_Unwind_GetTextRelBase(context=%p)", (void *)context);
_LIBUNWIND_ABORT("_Unwind_GetTextRelBase() not implemented");
}




_LIBUNWIND_EXPORT void *_Unwind_FindEnclosingFunction(void *pc) {
_LIBUNWIND_TRACE_API("_Unwind_FindEnclosingFunction(pc=%p)", pc);


unw_cursor_t cursor;
unw_context_t uc;
unw_proc_info_t info;
__unw_getcontext(&uc);
__unw_init_local(&cursor, &uc);
__unw_set_reg(&cursor, UNW_REG_IP, (unw_word_t)(intptr_t)pc);
if (__unw_get_proc_info(&cursor, &info) == UNW_ESUCCESS)
return (void *)(intptr_t) info.start_ip;
else
return NULL;
}



_LIBUNWIND_EXPORT _Unwind_Reason_Code
_Unwind_Backtrace(_Unwind_Trace_Fn callback, void *ref) {
unw_cursor_t cursor;
unw_context_t uc;
__unw_getcontext(&uc);
__unw_init_local(&cursor, &uc);

_LIBUNWIND_TRACE_API("_Unwind_Backtrace(callback=%p)",
(void *)(uintptr_t)callback);

#if defined(_LIBUNWIND_ARM_EHABI)

_Unwind_Exception ex;
memset(&ex, '\0', sizeof(ex));
ex.exception_class = 0x434C4E47554E5700;
#endif


while (true) {
_Unwind_Reason_Code result;

#if !defined(_LIBUNWIND_ARM_EHABI)


if (__unw_step(&cursor) <= 0) {
_LIBUNWIND_TRACE_UNWINDING(" _backtrace: ended because cursor reached "
"bottom of stack, returning %d",
_URC_END_OF_STACK);
return _URC_END_OF_STACK;
}
#else

unw_proc_info_t frameInfo;
if (__unw_get_proc_info(&cursor, &frameInfo) != UNW_ESUCCESS) {
return _URC_END_OF_STACK;
}


const uint32_t* unwindInfo = (uint32_t *) frameInfo.unwind_info;
ex.pr_cache.fnstart = frameInfo.start_ip;
ex.pr_cache.ehtp = (_Unwind_EHT_Header *) unwindInfo;
ex.pr_cache.additional= frameInfo.flags;

struct _Unwind_Context *context = (struct _Unwind_Context *)&cursor;

_Unwind_Personality_Fn handler = (_Unwind_Personality_Fn)frameInfo.handler;
if (handler == NULL) {
return _URC_END_OF_STACK;
}
if (handler(_US_VIRTUAL_UNWIND_FRAME | _US_FORCE_UNWIND, &ex, context) !=
_URC_CONTINUE_UNWIND) {
return _URC_END_OF_STACK;
}
#endif


if (_LIBUNWIND_TRACING_UNWINDING) {
char functionName[512];
unw_proc_info_t frame;
unw_word_t offset;
__unw_get_proc_name(&cursor, functionName, 512, &offset);
__unw_get_proc_info(&cursor, &frame);
_LIBUNWIND_TRACE_UNWINDING(
" _backtrace: start_ip=0x%" PRIxPTR ", func=%s, lsda=0x%" PRIxPTR ", context=%p",
frame.start_ip, functionName, frame.lsda,
(void *)&cursor);
}


result = (*callback)((struct _Unwind_Context *)(&cursor), ref);
if (result != _URC_NO_REASON) {
_LIBUNWIND_TRACE_UNWINDING(
" _backtrace: ended because callback returned %d", result);
return result;
}
}
}
#if defined(__arm__)

__sym_compat(_Unwind_Backtrace, _Unwind_Backtrace, GCC_3.3);
#endif



_LIBUNWIND_EXPORT const void *_Unwind_Find_FDE(const void *pc,
struct dwarf_eh_bases *bases) {


unw_cursor_t cursor;
unw_context_t uc;
unw_proc_info_t info;
__unw_getcontext(&uc);
__unw_init_local(&cursor, &uc);
__unw_set_reg(&cursor, UNW_REG_IP, (unw_word_t)(intptr_t)pc);
__unw_get_proc_info(&cursor, &info);
bases->tbase = (uintptr_t)info.extra;
bases->dbase = 0;
bases->func = (uintptr_t)info.start_ip;
_LIBUNWIND_TRACE_API("_Unwind_Find_FDE(pc=%p) => %p", pc,
(void *)(intptr_t) info.unwind_info);
return (void *)(intptr_t) info.unwind_info;
}



_LIBUNWIND_EXPORT uintptr_t _Unwind_GetCFA(struct _Unwind_Context *context) {
unw_cursor_t *cursor = (unw_cursor_t *)context;
unw_word_t result;
__unw_get_reg(cursor, UNW_REG_SP, &result);
_LIBUNWIND_TRACE_API("_Unwind_GetCFA(context=%p) => 0x%" PRIxPTR,
(void *)context, result);
return (uintptr_t)result;
}





_LIBUNWIND_EXPORT uintptr_t _Unwind_GetIPInfo(struct _Unwind_Context *context,
int *ipBefore) {
_LIBUNWIND_TRACE_API("_Unwind_GetIPInfo(context=%p)", (void *)context);
int isSignalFrame = __unw_is_signal_frame((unw_cursor_t *)context);



if (isSignalFrame <= 0)
*ipBefore = 0;
else
*ipBefore = 1;
return _Unwind_GetIP(context);
}

#if defined(_LIBUNWIND_SUPPORT_DWARF_UNWIND)

#if defined(__FreeBSD__)



static void processFDE(const char *addr, bool isDeregister) {
uint64_t length;
while ((length = *((const uint32_t *)addr)) != 0) {
const char *p = addr + 4;
if (length == 0xffffffff) {
length = *((const uint64_t *)p);
p += 8;
}
uint32_t offset = *((const uint32_t *)p);
if (offset != 0) {
if (isDeregister)
__unw_remove_dynamic_fde((unw_word_t)(uintptr_t)addr);
else
__unw_add_dynamic_fde((unw_word_t)(uintptr_t)addr);
}
addr = p + length;
}
}




_LIBUNWIND_EXPORT void __register_frame(const void *addr) {
_LIBUNWIND_TRACE_API("__register_frame(%p)", addr);
processFDE(addr, false);
}



_LIBUNWIND_EXPORT void __deregister_frame(const void *addr) {
_LIBUNWIND_TRACE_API("__deregister_frame(%p)", addr);
processFDE(addr, true);
}

#else





_LIBUNWIND_EXPORT void __register_frame(const void *fde) {
_LIBUNWIND_TRACE_API("__register_frame(%p)", fde);
__unw_add_dynamic_fde((unw_word_t)(uintptr_t)fde);
}





_LIBUNWIND_EXPORT void __deregister_frame(const void *fde) {
_LIBUNWIND_TRACE_API("__deregister_frame(%p)", fde);
__unw_remove_dynamic_fde((unw_word_t)(uintptr_t)fde);
}

#endif









#if defined(_LIBUNWIND_SUPPORT_FRAME_APIS)
_LIBUNWIND_EXPORT void __register_frame_info_bases(const void *fde, void *ob,
void *tb, void *db) {
(void)fde;
(void)ob;
(void)tb;
(void)db;
_LIBUNWIND_TRACE_API("__register_frame_info_bases(%p,%p, %p, %p)",
fde, ob, tb, db);

}

_LIBUNWIND_EXPORT void __register_frame_info(const void *fde, void *ob) {
(void)fde;
(void)ob;
_LIBUNWIND_TRACE_API("__register_frame_info(%p, %p)", fde, ob);

}

_LIBUNWIND_EXPORT void __register_frame_info_table_bases(const void *fde,
void *ob, void *tb,
void *db) {
(void)fde;
(void)ob;
(void)tb;
(void)db;
_LIBUNWIND_TRACE_API("__register_frame_info_table_bases"
"(%p,%p, %p, %p)", fde, ob, tb, db);

}

_LIBUNWIND_EXPORT void __register_frame_info_table(const void *fde, void *ob) {
(void)fde;
(void)ob;
_LIBUNWIND_TRACE_API("__register_frame_info_table(%p, %p)", fde, ob);

}

_LIBUNWIND_EXPORT void __register_frame_table(const void *fde) {
(void)fde;
_LIBUNWIND_TRACE_API("__register_frame_table(%p)", fde);

}

_LIBUNWIND_EXPORT void *__deregister_frame_info(const void *fde) {
(void)fde;
_LIBUNWIND_TRACE_API("__deregister_frame_info(%p)", fde);

return NULL;
}

_LIBUNWIND_EXPORT void *__deregister_frame_info_bases(const void *fde) {
(void)fde;
_LIBUNWIND_TRACE_API("__deregister_frame_info_bases(%p)", fde);

return NULL;
}
#endif

#endif

#endif
