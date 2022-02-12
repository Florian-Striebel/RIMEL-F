










#include <unwind.h>

#include <inttypes.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include "config.h"









#if defined(_LIBUNWIND_BUILD_SJLJ_APIS)

struct _Unwind_FunctionContext {

struct _Unwind_FunctionContext *prev;

#if defined(__ve__)





uint64_t resumeLocation;


uint64_t resumeParameters[4];
#else

uint32_t resumeLocation;


uint32_t resumeParameters[4];
#endif


_Unwind_Personality_Fn personality;
uintptr_t lsda;



void *jbuf[];
};

#if defined(_LIBUNWIND_HAS_NO_THREADS)
#define _LIBUNWIND_THREAD_LOCAL
#else
#if __STDC_VERSION__ >= 201112L
#define _LIBUNWIND_THREAD_LOCAL _Thread_local
#elif defined(_MSC_VER)
#define _LIBUNWIND_THREAD_LOCAL __declspec(thread)
#elif defined(__GNUC__) || defined(__clang__)
#define _LIBUNWIND_THREAD_LOCAL __thread
#else
#error Unable to create thread local storage
#endif
#endif


#if !defined(FOR_DYLD)

#if defined(__APPLE__)
#include <System/pthread_machdep.h>
#else
static _LIBUNWIND_THREAD_LOCAL struct _Unwind_FunctionContext *stack = NULL;
#endif

static struct _Unwind_FunctionContext *__Unwind_SjLj_GetTopOfFunctionStack() {
#if defined(__APPLE__)
return _pthread_getspecific_direct(__PTK_LIBC_DYLD_Unwind_SjLj_Key);
#else
return stack;
#endif
}

static void
__Unwind_SjLj_SetTopOfFunctionStack(struct _Unwind_FunctionContext *fc) {
#if defined(__APPLE__)
_pthread_setspecific_direct(__PTK_LIBC_DYLD_Unwind_SjLj_Key, fc);
#else
stack = fc;
#endif
}

#endif



_LIBUNWIND_EXPORT void
_Unwind_SjLj_Register(struct _Unwind_FunctionContext *fc) {
fc->prev = __Unwind_SjLj_GetTopOfFunctionStack();
__Unwind_SjLj_SetTopOfFunctionStack(fc);
}



_LIBUNWIND_EXPORT void
_Unwind_SjLj_Unregister(struct _Unwind_FunctionContext *fc) {
__Unwind_SjLj_SetTopOfFunctionStack(fc->prev);
}


static _Unwind_Reason_Code
unwind_phase1(struct _Unwind_Exception *exception_object) {
_Unwind_FunctionContext_t c = __Unwind_SjLj_GetTopOfFunctionStack();
_LIBUNWIND_TRACE_UNWINDING("unwind_phase1: initial function-context=%p",
(void *)c);


for (bool handlerNotFound = true; handlerNotFound; c = c->prev) {


if (c == NULL) {
_LIBUNWIND_TRACE_UNWINDING("unwind_phase1(ex_ojb=%p): reached "
"bottom => _URC_END_OF_STACK",
(void *)exception_object);
return _URC_END_OF_STACK;
}

_LIBUNWIND_TRACE_UNWINDING("unwind_phase1: function-context=%p", (void *)c);


if (c->personality != NULL) {
_LIBUNWIND_TRACE_UNWINDING("unwind_phase1(ex_ojb=%p): calling "
"personality function %p",
(void *)exception_object,
(void *)c->personality);
_Unwind_Reason_Code personalityResult = (*c->personality)(
1, _UA_SEARCH_PHASE, exception_object->exception_class,
exception_object, (struct _Unwind_Context *)c);
switch (personalityResult) {
case _URC_HANDLER_FOUND:


handlerNotFound = false;
exception_object->private_2 = (uintptr_t) c;
_LIBUNWIND_TRACE_UNWINDING("unwind_phase1(ex_ojb=%p): "
"_URC_HANDLER_FOUND",
(void *)exception_object);
return _URC_NO_REASON;

case _URC_CONTINUE_UNWIND:
_LIBUNWIND_TRACE_UNWINDING("unwind_phase1(ex_ojb=%p): "
"_URC_CONTINUE_UNWIND",
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
unwind_phase2(struct _Unwind_Exception *exception_object) {
_LIBUNWIND_TRACE_UNWINDING("unwind_phase2(ex_ojb=%p)",
(void *)exception_object);


_Unwind_FunctionContext_t c = __Unwind_SjLj_GetTopOfFunctionStack();
while (true) {
_LIBUNWIND_TRACE_UNWINDING("unwind_phase2s(ex_ojb=%p): context=%p",
(void *)exception_object, (void *)c);


if (c == NULL) {
_LIBUNWIND_TRACE_UNWINDING(
"unwind_phase2(ex_ojb=%p): __unw_step() reached "
"bottom => _URC_END_OF_STACK",
(void *)exception_object);
return _URC_END_OF_STACK;
}


if (c->personality != NULL) {
_Unwind_Action action = _UA_CLEANUP_PHASE;
if ((uintptr_t) c == exception_object->private_2)
action = (_Unwind_Action)(
_UA_CLEANUP_PHASE |
_UA_HANDLER_FRAME);

_Unwind_Reason_Code personalityResult =
(*c->personality)(1, action, exception_object->exception_class,
exception_object, (struct _Unwind_Context *)c);
switch (personalityResult) {
case _URC_CONTINUE_UNWIND:

_LIBUNWIND_TRACE_UNWINDING(
"unwind_phase2(ex_ojb=%p): _URC_CONTINUE_UNWIND",
(void *)exception_object);
if ((uintptr_t) c == exception_object->private_2) {

_LIBUNWIND_ABORT("during phase1 personality function said it would "
"stop here, but now if phase2 it did not stop here");
}
break;
case _URC_INSTALL_CONTEXT:
_LIBUNWIND_TRACE_UNWINDING("unwind_phase2(ex_ojb=%p): "
"_URC_INSTALL_CONTEXT, will resume at "
"landing pad %p",
(void *)exception_object, c->jbuf[1]);


__Unwind_SjLj_SetTopOfFunctionStack(c);
__builtin_longjmp(c->jbuf, 1);

return _URC_FATAL_PHASE2_ERROR;
default:

_LIBUNWIND_DEBUG_LOG("personality function returned unknown result %d",
personalityResult);
return _URC_FATAL_PHASE2_ERROR;
}
}
c = c->prev;
}



return _URC_FATAL_PHASE2_ERROR;
}


static _Unwind_Reason_Code
unwind_phase2_forced(struct _Unwind_Exception *exception_object,
_Unwind_Stop_Fn stop, void *stop_parameter) {

_Unwind_FunctionContext_t c = __Unwind_SjLj_GetTopOfFunctionStack();
while (true) {


if (c == NULL) {
_LIBUNWIND_TRACE_UNWINDING(
"unwind_phase2(ex_ojb=%p): __unw_step() reached "
"bottom => _URC_END_OF_STACK",
(void *)exception_object);
return _URC_END_OF_STACK;
}


_Unwind_Action action =
(_Unwind_Action)(_UA_FORCE_UNWIND | _UA_CLEANUP_PHASE);
_Unwind_Reason_Code stopResult =
(*stop)(1, action, exception_object->exception_class, exception_object,
(struct _Unwind_Context *)c, stop_parameter);
_LIBUNWIND_TRACE_UNWINDING("unwind_phase2_forced(ex_ojb=%p): "
"stop function returned %d",
(void *)exception_object, stopResult);
if (stopResult != _URC_NO_REASON) {
_LIBUNWIND_TRACE_UNWINDING("unwind_phase2_forced(ex_ojb=%p): "
"stopped by stop function",
(void *)exception_object);
return _URC_FATAL_PHASE2_ERROR;
}


if (c->personality != NULL) {
_Unwind_Personality_Fn p = (_Unwind_Personality_Fn)c->personality;
_LIBUNWIND_TRACE_UNWINDING("unwind_phase2_forced(ex_ojb=%p): "
"calling personality function %p",
(void *)exception_object, (void *)p);
_Unwind_Reason_Code personalityResult =
(*p)(1, action, exception_object->exception_class, exception_object,
(struct _Unwind_Context *)c);
switch (personalityResult) {
case _URC_CONTINUE_UNWIND:
_LIBUNWIND_TRACE_UNWINDING("unwind_phase2_forced(ex_ojb=%p): "
"personality returned _URC_CONTINUE_UNWIND",
(void *)exception_object);

break;
case _URC_INSTALL_CONTEXT:
_LIBUNWIND_TRACE_UNWINDING("unwind_phase2_forced(ex_ojb=%p): "
"personality returned _URC_INSTALL_CONTEXT",
(void *)exception_object);

__Unwind_SjLj_SetTopOfFunctionStack(c);
__builtin_longjmp(c->jbuf, 1);
break;
default:

_LIBUNWIND_TRACE_UNWINDING("unwind_phase2_forced(ex_ojb=%p): "
"personality returned %d, "
"_URC_FATAL_PHASE2_ERROR",
(void *)exception_object, personalityResult);
return _URC_FATAL_PHASE2_ERROR;
}
}
c = c->prev;
}



_LIBUNWIND_TRACE_UNWINDING("unwind_phase2_forced(ex_ojb=%p): calling stop "
"function with _UA_END_OF_STACK",
(void *)exception_object);
_Unwind_Action lastAction =
(_Unwind_Action)(_UA_FORCE_UNWIND | _UA_CLEANUP_PHASE | _UA_END_OF_STACK);
(*stop)(1, lastAction, exception_object->exception_class, exception_object,
(struct _Unwind_Context *)c, stop_parameter);



return _URC_FATAL_PHASE2_ERROR;
}



_LIBUNWIND_EXPORT _Unwind_Reason_Code
_Unwind_SjLj_RaiseException(struct _Unwind_Exception *exception_object) {
_LIBUNWIND_TRACE_API("_Unwind_SjLj_RaiseException(ex_obj=%p)",
(void *)exception_object);



exception_object->private_1 = 0;
exception_object->private_2 = 0;


_Unwind_Reason_Code phase1 = unwind_phase1(exception_object);
if (phase1 != _URC_NO_REASON)
return phase1;


return unwind_phase2(exception_object);
}













_LIBUNWIND_EXPORT void
_Unwind_SjLj_Resume(struct _Unwind_Exception *exception_object) {
_LIBUNWIND_TRACE_API("_Unwind_SjLj_Resume(ex_obj=%p)",
(void *)exception_object);

if (exception_object->private_1 != 0)
unwind_phase2_forced(exception_object,
(_Unwind_Stop_Fn) exception_object->private_1,
(void *)exception_object->private_2);
else
unwind_phase2(exception_object);


_LIBUNWIND_ABORT("_Unwind_SjLj_Resume() can't return");
}



_LIBUNWIND_EXPORT _Unwind_Reason_Code
_Unwind_SjLj_Resume_or_Rethrow(struct _Unwind_Exception *exception_object) {
_LIBUNWIND_TRACE_API("__Unwind_SjLj_Resume_or_Rethrow(ex_obj=%p), "
"private_1=%" PRIuPTR,
(void *)exception_object, exception_object->private_1);



if (exception_object->private_1 == 0) {
return _Unwind_SjLj_RaiseException(exception_object);


}



_Unwind_SjLj_Resume(exception_object);
_LIBUNWIND_ABORT("__Unwind_SjLj_Resume_or_Rethrow() called "
"_Unwind_SjLj_Resume() which unexpectedly returned");
}



_LIBUNWIND_EXPORT uintptr_t
_Unwind_GetLanguageSpecificData(struct _Unwind_Context *context) {
_Unwind_FunctionContext_t ufc = (_Unwind_FunctionContext_t) context;
_LIBUNWIND_TRACE_API("_Unwind_GetLanguageSpecificData(context=%p) "
"=> 0x%" PRIuPTR,
(void *)context, ufc->lsda);
return ufc->lsda;
}



_LIBUNWIND_EXPORT uintptr_t _Unwind_GetGR(struct _Unwind_Context *context,
int index) {
_LIBUNWIND_TRACE_API("_Unwind_GetGR(context=%p, reg=%d)", (void *)context,
index);
_Unwind_FunctionContext_t ufc = (_Unwind_FunctionContext_t) context;
return ufc->resumeParameters[index];
}



_LIBUNWIND_EXPORT void _Unwind_SetGR(struct _Unwind_Context *context, int index,
uintptr_t new_value) {
_LIBUNWIND_TRACE_API("_Unwind_SetGR(context=%p, reg=%d, value=0x%" PRIuPTR
")",
(void *)context, index, new_value);
_Unwind_FunctionContext_t ufc = (_Unwind_FunctionContext_t) context;
ufc->resumeParameters[index] = new_value;
}



_LIBUNWIND_EXPORT uintptr_t _Unwind_GetIP(struct _Unwind_Context *context) {
_Unwind_FunctionContext_t ufc = (_Unwind_FunctionContext_t) context;
_LIBUNWIND_TRACE_API("_Unwind_GetIP(context=%p) => 0x%" PRIu32,
(void *)context, ufc->resumeLocation + 1);
return ufc->resumeLocation + 1;
}





_LIBUNWIND_EXPORT uintptr_t _Unwind_GetIPInfo(struct _Unwind_Context *context,
int *ipBefore) {
_Unwind_FunctionContext_t ufc = (_Unwind_FunctionContext_t) context;
*ipBefore = 0;
_LIBUNWIND_TRACE_API("_Unwind_GetIPInfo(context=%p, %p) => 0x%" PRIu32,
(void *)context, (void *)ipBefore,
ufc->resumeLocation + 1);
return ufc->resumeLocation + 1;
}



_LIBUNWIND_EXPORT void _Unwind_SetIP(struct _Unwind_Context *context,
uintptr_t new_value) {
_LIBUNWIND_TRACE_API("_Unwind_SetIP(context=%p, value=0x%" PRIuPTR ")",
(void *)context, new_value);
_Unwind_FunctionContext_t ufc = (_Unwind_FunctionContext_t) context;
ufc->resumeLocation = new_value - 1;
}




_LIBUNWIND_EXPORT uintptr_t
_Unwind_GetRegionStart(struct _Unwind_Context *context) {

(void)context;
_LIBUNWIND_TRACE_API("_Unwind_GetRegionStart(context=%p)", (void *)context);
return 0;
}




_LIBUNWIND_EXPORT void
_Unwind_DeleteException(struct _Unwind_Exception *exception_object) {
_LIBUNWIND_TRACE_API("_Unwind_DeleteException(ex_obj=%p)",
(void *)exception_object);
if (exception_object->exception_cleanup != NULL)
(*exception_object->exception_cleanup)(_URC_FOREIGN_EXCEPTION_CAUGHT,
exception_object);
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



_LIBUNWIND_EXPORT uintptr_t _Unwind_GetCFA(struct _Unwind_Context *context) {
_LIBUNWIND_TRACE_API("_Unwind_GetCFA(context=%p)", (void *)context);
if (context != NULL) {
_Unwind_FunctionContext_t ufc = (_Unwind_FunctionContext_t) context;


return (uintptr_t) ufc->jbuf[2];
}
return 0;
}

#endif
