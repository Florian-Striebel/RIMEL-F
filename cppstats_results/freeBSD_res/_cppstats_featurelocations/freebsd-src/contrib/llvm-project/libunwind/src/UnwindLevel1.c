

















#define _LIBUNWIND_UNWIND_LEVEL1_EXTERNAL_LINKAGE 1

#include <inttypes.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "config.h"
#include "libunwind.h"
#include "libunwind_ext.h"
#include "unwind.h"

#if !defined(_LIBUNWIND_ARM_EHABI) && !defined(__USING_SJLJ_EXCEPTIONS__)

#if !defined(_LIBUNWIND_SUPPORT_SEH_UNWIND)

static _Unwind_Reason_Code
unwind_phase1(unw_context_t *uc, unw_cursor_t *cursor, _Unwind_Exception *exception_object) {
__unw_init_local(cursor, uc);


while (true) {


int stepResult = __unw_step(cursor);
if (stepResult == 0) {
_LIBUNWIND_TRACE_UNWINDING(
"unwind_phase1(ex_ojb=%p): __unw_step() reached "
"bottom => _URC_END_OF_STACK",
(void *)exception_object);
return _URC_END_OF_STACK;
} else if (stepResult < 0) {
_LIBUNWIND_TRACE_UNWINDING(
"unwind_phase1(ex_ojb=%p): __unw_step failed => "
"_URC_FATAL_PHASE1_ERROR",
(void *)exception_object);
return _URC_FATAL_PHASE1_ERROR;
}


unw_proc_info_t frameInfo;
unw_word_t sp;
if (__unw_get_proc_info(cursor, &frameInfo) != UNW_ESUCCESS) {
_LIBUNWIND_TRACE_UNWINDING(
"unwind_phase1(ex_ojb=%p): __unw_get_proc_info "
"failed => _URC_FATAL_PHASE1_ERROR",
(void *)exception_object);
return _URC_FATAL_PHASE1_ERROR;
}


if (_LIBUNWIND_TRACING_UNWINDING) {
char functionBuf[512];
const char *functionName = functionBuf;
unw_word_t offset;
if ((__unw_get_proc_name(cursor, functionBuf, sizeof(functionBuf),
&offset) != UNW_ESUCCESS) ||
(frameInfo.start_ip + offset > frameInfo.end_ip))
functionName = ".anonymous.";
unw_word_t pc;
__unw_get_reg(cursor, UNW_REG_IP, &pc);
_LIBUNWIND_TRACE_UNWINDING(
"unwind_phase1(ex_ojb=%p): pc=0x%" PRIxPTR ", start_ip=0x%" PRIxPTR
", func=%s, lsda=0x%" PRIxPTR ", personality=0x%" PRIxPTR "",
(void *)exception_object, pc, frameInfo.start_ip, functionName,
frameInfo.lsda, frameInfo.handler);
}



if (frameInfo.handler != 0) {
_Unwind_Personality_Fn p =
(_Unwind_Personality_Fn)(uintptr_t)(frameInfo.handler);
_LIBUNWIND_TRACE_UNWINDING(
"unwind_phase1(ex_ojb=%p): calling personality function %p",
(void *)exception_object, (void *)(uintptr_t)p);
_Unwind_Reason_Code personalityResult =
(*p)(1, _UA_SEARCH_PHASE, exception_object->exception_class,
exception_object, (struct _Unwind_Context *)(cursor));
switch (personalityResult) {
case _URC_HANDLER_FOUND:


__unw_get_reg(cursor, UNW_REG_SP, &sp);
exception_object->private_2 = (uintptr_t)sp;
_LIBUNWIND_TRACE_UNWINDING(
"unwind_phase1(ex_ojb=%p): _URC_HANDLER_FOUND",
(void *)exception_object);
return _URC_NO_REASON;

case _URC_CONTINUE_UNWIND:
_LIBUNWIND_TRACE_UNWINDING(
"unwind_phase1(ex_ojb=%p): _URC_CONTINUE_UNWIND",
(void *)exception_object);

break;

default:

_LIBUNWIND_TRACE_UNWINDING(
"unwind_phase1(ex_ojb=%p): _URC_FATAL_PHASE1_ERROR",
(void *)exception_object);
return _URC_FATAL_PHASE1_ERROR;
}
}
}
return _URC_NO_REASON;
}


static _Unwind_Reason_Code
unwind_phase2(unw_context_t *uc, unw_cursor_t *cursor, _Unwind_Exception *exception_object) {
__unw_init_local(cursor, uc);

_LIBUNWIND_TRACE_UNWINDING("unwind_phase2(ex_ojb=%p)",
(void *)exception_object);


while (true) {



int stepResult = __unw_step(cursor);
if (stepResult == 0) {
_LIBUNWIND_TRACE_UNWINDING(
"unwind_phase2(ex_ojb=%p): __unw_step() reached "
"bottom => _URC_END_OF_STACK",
(void *)exception_object);
return _URC_END_OF_STACK;
} else if (stepResult < 0) {
_LIBUNWIND_TRACE_UNWINDING(
"unwind_phase2(ex_ojb=%p): __unw_step failed => "
"_URC_FATAL_PHASE1_ERROR",
(void *)exception_object);
return _URC_FATAL_PHASE2_ERROR;
}


unw_word_t sp;
unw_proc_info_t frameInfo;
__unw_get_reg(cursor, UNW_REG_SP, &sp);
if (__unw_get_proc_info(cursor, &frameInfo) != UNW_ESUCCESS) {
_LIBUNWIND_TRACE_UNWINDING(
"unwind_phase2(ex_ojb=%p): __unw_get_proc_info "
"failed => _URC_FATAL_PHASE1_ERROR",
(void *)exception_object);
return _URC_FATAL_PHASE2_ERROR;
}


if (_LIBUNWIND_TRACING_UNWINDING) {
char functionBuf[512];
const char *functionName = functionBuf;
unw_word_t offset;
if ((__unw_get_proc_name(cursor, functionBuf, sizeof(functionBuf),
&offset) != UNW_ESUCCESS) ||
(frameInfo.start_ip + offset > frameInfo.end_ip))
functionName = ".anonymous.";
_LIBUNWIND_TRACE_UNWINDING("unwind_phase2(ex_ojb=%p): start_ip=0x%" PRIxPTR
", func=%s, sp=0x%" PRIxPTR ", lsda=0x%" PRIxPTR
", personality=0x%" PRIxPTR,
(void *)exception_object, frameInfo.start_ip,
functionName, sp, frameInfo.lsda,
frameInfo.handler);
}


if (frameInfo.handler != 0) {
_Unwind_Personality_Fn p =
(_Unwind_Personality_Fn)(uintptr_t)(frameInfo.handler);
_Unwind_Action action = _UA_CLEANUP_PHASE;
if (sp == exception_object->private_2) {

action = (_Unwind_Action)(_UA_CLEANUP_PHASE | _UA_HANDLER_FRAME);
}
_Unwind_Reason_Code personalityResult =
(*p)(1, action, exception_object->exception_class, exception_object,
(struct _Unwind_Context *)(cursor));
switch (personalityResult) {
case _URC_CONTINUE_UNWIND:

_LIBUNWIND_TRACE_UNWINDING(
"unwind_phase2(ex_ojb=%p): _URC_CONTINUE_UNWIND",
(void *)exception_object);
if (sp == exception_object->private_2) {

_LIBUNWIND_ABORT("during phase1 personality function said it would "
"stop here, but now in phase2 it did not stop here");
}
break;
case _URC_INSTALL_CONTEXT:
_LIBUNWIND_TRACE_UNWINDING(
"unwind_phase2(ex_ojb=%p): _URC_INSTALL_CONTEXT",
(void *)exception_object);


if (_LIBUNWIND_TRACING_UNWINDING) {
unw_word_t pc;
__unw_get_reg(cursor, UNW_REG_IP, &pc);
__unw_get_reg(cursor, UNW_REG_SP, &sp);
_LIBUNWIND_TRACE_UNWINDING("unwind_phase2(ex_ojb=%p): re-entering "
"user code with ip=0x%" PRIxPTR
", sp=0x%" PRIxPTR,
(void *)exception_object, pc, sp);
}
__unw_resume(cursor);

return _URC_FATAL_PHASE2_ERROR;
default:

_LIBUNWIND_DEBUG_LOG("personality function returned unknown result %d",
personalityResult);
return _URC_FATAL_PHASE2_ERROR;
}
}
}



return _URC_FATAL_PHASE2_ERROR;
}

static _Unwind_Reason_Code
unwind_phase2_forced(unw_context_t *uc, unw_cursor_t *cursor,
_Unwind_Exception *exception_object,
_Unwind_Stop_Fn stop, void *stop_parameter) {
__unw_init_local(cursor, uc);


while (__unw_step(cursor) > 0) {


unw_proc_info_t frameInfo;
if (__unw_get_proc_info(cursor, &frameInfo) != UNW_ESUCCESS) {
_LIBUNWIND_TRACE_UNWINDING("unwind_phase2_forced(ex_ojb=%p): __unw_step "
"failed => _URC_END_OF_STACK",
(void *)exception_object);
return _URC_FATAL_PHASE2_ERROR;
}


if (_LIBUNWIND_TRACING_UNWINDING) {
char functionBuf[512];
const char *functionName = functionBuf;
unw_word_t offset;
if ((__unw_get_proc_name(cursor, functionBuf, sizeof(functionBuf),
&offset) != UNW_ESUCCESS) ||
(frameInfo.start_ip + offset > frameInfo.end_ip))
functionName = ".anonymous.";
_LIBUNWIND_TRACE_UNWINDING(
"unwind_phase2_forced(ex_ojb=%p): start_ip=0x%" PRIxPTR
", func=%s, lsda=0x%" PRIxPTR ", personality=0x%" PRIxPTR,
(void *)exception_object, frameInfo.start_ip, functionName,
frameInfo.lsda, frameInfo.handler);
}


_Unwind_Action action =
(_Unwind_Action)(_UA_FORCE_UNWIND | _UA_CLEANUP_PHASE);
_Unwind_Reason_Code stopResult =
(*stop)(1, action, exception_object->exception_class, exception_object,
(struct _Unwind_Context *)(cursor), stop_parameter);
_LIBUNWIND_TRACE_UNWINDING(
"unwind_phase2_forced(ex_ojb=%p): stop function returned %d",
(void *)exception_object, stopResult);
if (stopResult != _URC_NO_REASON) {
_LIBUNWIND_TRACE_UNWINDING(
"unwind_phase2_forced(ex_ojb=%p): stopped by stop function",
(void *)exception_object);
return _URC_FATAL_PHASE2_ERROR;
}


if (frameInfo.handler != 0) {
_Unwind_Personality_Fn p =
(_Unwind_Personality_Fn)(intptr_t)(frameInfo.handler);
_LIBUNWIND_TRACE_UNWINDING(
"unwind_phase2_forced(ex_ojb=%p): calling personality function %p",
(void *)exception_object, (void *)(uintptr_t)p);
_Unwind_Reason_Code personalityResult =
(*p)(1, action, exception_object->exception_class, exception_object,
(struct _Unwind_Context *)(cursor));
switch (personalityResult) {
case _URC_CONTINUE_UNWIND:
_LIBUNWIND_TRACE_UNWINDING("unwind_phase2_forced(ex_ojb=%p): "
"personality returned "
"_URC_CONTINUE_UNWIND",
(void *)exception_object);

break;
case _URC_INSTALL_CONTEXT:
_LIBUNWIND_TRACE_UNWINDING("unwind_phase2_forced(ex_ojb=%p): "
"personality returned "
"_URC_INSTALL_CONTEXT",
(void *)exception_object);

__unw_resume(cursor);
break;
default:

_LIBUNWIND_TRACE_UNWINDING("unwind_phase2_forced(ex_ojb=%p): "
"personality returned %d, "
"_URC_FATAL_PHASE2_ERROR",
(void *)exception_object, personalityResult);
return _URC_FATAL_PHASE2_ERROR;
}
}
}



_LIBUNWIND_TRACE_UNWINDING("unwind_phase2_forced(ex_ojb=%p): calling stop "
"function with _UA_END_OF_STACK",
(void *)exception_object);
_Unwind_Action lastAction =
(_Unwind_Action)(_UA_FORCE_UNWIND | _UA_CLEANUP_PHASE | _UA_END_OF_STACK);
(*stop)(1, lastAction, exception_object->exception_class, exception_object,
(struct _Unwind_Context *)(cursor), stop_parameter);



return _URC_FATAL_PHASE2_ERROR;
}



_LIBUNWIND_EXPORT _Unwind_Reason_Code
_Unwind_RaiseException(_Unwind_Exception *exception_object) {
_LIBUNWIND_TRACE_API("_Unwind_RaiseException(ex_obj=%p)",
(void *)exception_object);
unw_context_t uc;
unw_cursor_t cursor;
__unw_getcontext(&uc);



exception_object->private_1 = 0;
exception_object->private_2 = 0;


_Unwind_Reason_Code phase1 = unwind_phase1(&uc, &cursor, exception_object);
if (phase1 != _URC_NO_REASON)
return phase1;


return unwind_phase2(&uc, &cursor, exception_object);
}














_LIBUNWIND_EXPORT void
_Unwind_Resume(_Unwind_Exception *exception_object) {
_LIBUNWIND_TRACE_API("_Unwind_Resume(ex_obj=%p)", (void *)exception_object);
unw_context_t uc;
unw_cursor_t cursor;
__unw_getcontext(&uc);

if (exception_object->private_1 != 0)
unwind_phase2_forced(&uc, &cursor, exception_object,
(_Unwind_Stop_Fn) exception_object->private_1,
(void *)exception_object->private_2);
else
unwind_phase2(&uc, &cursor, exception_object);


_LIBUNWIND_ABORT("_Unwind_Resume() can't return");
}






_LIBUNWIND_EXPORT _Unwind_Reason_Code
_Unwind_ForcedUnwind(_Unwind_Exception *exception_object,
_Unwind_Stop_Fn stop, void *stop_parameter) {
_LIBUNWIND_TRACE_API("_Unwind_ForcedUnwind(ex_obj=%p, stop=%p)",
(void *)exception_object, (void *)(uintptr_t)stop);
unw_context_t uc;
unw_cursor_t cursor;
__unw_getcontext(&uc);



exception_object->private_1 = (uintptr_t) stop;
exception_object->private_2 = (uintptr_t) stop_parameter;


return unwind_phase2_forced(&uc, &cursor, exception_object, stop, stop_parameter);
}



_LIBUNWIND_EXPORT uintptr_t
_Unwind_GetLanguageSpecificData(struct _Unwind_Context *context) {
unw_cursor_t *cursor = (unw_cursor_t *)context;
unw_proc_info_t frameInfo;
uintptr_t result = 0;
if (__unw_get_proc_info(cursor, &frameInfo) == UNW_ESUCCESS)
result = (uintptr_t)frameInfo.lsda;
_LIBUNWIND_TRACE_API(
"_Unwind_GetLanguageSpecificData(context=%p) => 0x%" PRIxPTR,
(void *)context, result);
if (result != 0) {
if (*((uint8_t *)result) != 0xFF)
_LIBUNWIND_DEBUG_LOG("lsda at 0x%" PRIxPTR " does not start with 0xFF",
result);
}
return result;
}




_LIBUNWIND_EXPORT uintptr_t
_Unwind_GetRegionStart(struct _Unwind_Context *context) {
unw_cursor_t *cursor = (unw_cursor_t *)context;
unw_proc_info_t frameInfo;
uintptr_t result = 0;
if (__unw_get_proc_info(cursor, &frameInfo) == UNW_ESUCCESS)
result = (uintptr_t)frameInfo.start_ip;
_LIBUNWIND_TRACE_API("_Unwind_GetRegionStart(context=%p) => 0x%" PRIxPTR,
(void *)context, result);
return result;
}

#endif



_LIBUNWIND_EXPORT void
_Unwind_DeleteException(_Unwind_Exception *exception_object) {
_LIBUNWIND_TRACE_API("_Unwind_DeleteException(ex_obj=%p)",
(void *)exception_object);
if (exception_object->exception_cleanup != NULL)
(*exception_object->exception_cleanup)(_URC_FOREIGN_EXCEPTION_CAUGHT,
exception_object);
}


_LIBUNWIND_EXPORT uintptr_t
_Unwind_GetGR(struct _Unwind_Context *context, int index) {
unw_cursor_t *cursor = (unw_cursor_t *)context;
unw_word_t result;
__unw_get_reg(cursor, index, &result);
_LIBUNWIND_TRACE_API("_Unwind_GetGR(context=%p, reg=%d) => 0x%" PRIxPTR,
(void *)context, index, result);
return (uintptr_t)result;
}


_LIBUNWIND_EXPORT void _Unwind_SetGR(struct _Unwind_Context *context, int index,
uintptr_t value) {
_LIBUNWIND_TRACE_API("_Unwind_SetGR(context=%p, reg=%d, value=0x%0" PRIxPTR
")",
(void *)context, index, value);
unw_cursor_t *cursor = (unw_cursor_t *)context;
__unw_set_reg(cursor, index, value);
}


_LIBUNWIND_EXPORT uintptr_t _Unwind_GetIP(struct _Unwind_Context *context) {
unw_cursor_t *cursor = (unw_cursor_t *)context;
unw_word_t result;
__unw_get_reg(cursor, UNW_REG_IP, &result);
_LIBUNWIND_TRACE_API("_Unwind_GetIP(context=%p) => 0x%" PRIxPTR,
(void *)context, result);
return (uintptr_t)result;
}




_LIBUNWIND_EXPORT void _Unwind_SetIP(struct _Unwind_Context *context,
uintptr_t value) {
_LIBUNWIND_TRACE_API("_Unwind_SetIP(context=%p, value=0x%0" PRIxPTR ")",
(void *)context, value);
unw_cursor_t *cursor = (unw_cursor_t *)context;
__unw_set_reg(cursor, UNW_REG_IP, value);
}

#endif
