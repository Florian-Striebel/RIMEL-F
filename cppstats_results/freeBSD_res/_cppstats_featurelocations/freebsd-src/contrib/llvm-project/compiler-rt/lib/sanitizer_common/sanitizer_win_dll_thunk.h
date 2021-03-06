











#if !defined(SANITIZER_WIN_DLL_THUNK_H)
#define SANITIZER_WIN_DLL_THUNK_H
#include "sanitizer_internal_defs.h"

namespace __sanitizer {
uptr dllThunkGetRealAddrOrDie(const char *name);

int dllThunkIntercept(const char* main_function, uptr dll_function);

int dllThunkInterceptWhenPossible(const char* main_function,
const char* default_function, uptr dll_function);
}

extern "C" int __dll_thunk_init();



#define INTERCEPT_OR_DIE(main_function, dll_function) static int intercept_##dll_function() { return __sanitizer::dllThunkIntercept(main_function, (__sanitizer::uptr) dll_function); } __pragma(section(".DLLTH$M", long, read)) __declspec(allocate(".DLLTH$M")) int (*__dll_thunk_##dll_function)() = intercept_##dll_function;










#define INTERCEPT_WHEN_POSSIBLE(main_function, default_function, dll_function) static int intercept_##dll_function() { return __sanitizer::dllThunkInterceptWhenPossible(main_function, default_function, (__sanitizer::uptr)dll_function); } __pragma(section(".DLLTH$M", long, read)) __declspec(allocate(".DLLTH$M")) int (*__dll_thunk_##dll_function)() = intercept_##dll_function;











#define INTERCEPT_SANITIZER_FUNCTION(name) extern "C" __declspec(noinline) void name() { volatile int prevent_icf = (__LINE__ << 8) ^ __COUNTER__; static const char function_name[] = #name; for (const char* ptr = &function_name[0]; *ptr; ++ptr) prevent_icf ^= *ptr; (void)prevent_icf; __debugbreak(); } INTERCEPT_OR_DIE(#name, name)














#define INTERCEPT_SANITIZER_WEAK_FUNCTION(name) extern "C" __declspec(noinline) void name() { volatile int prevent_icf = (__LINE__ << 8) ^ __COUNTER__; static const char function_name[] = #name; for (const char* ptr = &function_name[0]; *ptr; ++ptr) prevent_icf ^= *ptr; (void)prevent_icf; __debugbreak(); } INTERCEPT_WHEN_POSSIBLE(#name, STRINGIFY(WEAK_EXPORT_NAME(name)), name)
















#define INTERCEPT_LIBRARY_FUNCTION(name) extern "C" void name(); INTERCEPT_OR_DIE(WRAPPER_NAME(name), name)





#define INTERCEPT_WRAP_V_V(name) extern "C" void name() { typedef decltype(name) *fntype; static fntype fn = (fntype)__sanitizer::dllThunkGetRealAddrOrDie(#name); fn(); } INTERCEPT_OR_DIE(#name, name);







#define INTERCEPT_WRAP_V_W(name) extern "C" void name(void *arg) { typedef decltype(name) *fntype; static fntype fn = (fntype)__sanitizer::dllThunkGetRealAddrOrDie(#name); fn(arg); } INTERCEPT_OR_DIE(#name, name);







#define INTERCEPT_WRAP_V_WW(name) extern "C" void name(void *arg1, void *arg2) { typedef decltype(name) *fntype; static fntype fn = (fntype)__sanitizer::dllThunkGetRealAddrOrDie(#name); fn(arg1, arg2); } INTERCEPT_OR_DIE(#name, name);







#define INTERCEPT_WRAP_V_WWW(name) extern "C" void name(void *arg1, void *arg2, void *arg3) { typedef decltype(name) *fntype; static fntype fn = (fntype)__sanitizer::dllThunkGetRealAddrOrDie(#name); fn(arg1, arg2, arg3); } INTERCEPT_OR_DIE(#name, name);







#define INTERCEPT_WRAP_W_V(name) extern "C" void *name() { typedef decltype(name) *fntype; static fntype fn = (fntype)__sanitizer::dllThunkGetRealAddrOrDie(#name); return fn(); } INTERCEPT_OR_DIE(#name, name);







#define INTERCEPT_WRAP_W_W(name) extern "C" void *name(void *arg) { typedef decltype(name) *fntype; static fntype fn = (fntype)__sanitizer::dllThunkGetRealAddrOrDie(#name); return fn(arg); } INTERCEPT_OR_DIE(#name, name);







#define INTERCEPT_WRAP_W_WW(name) extern "C" void *name(void *arg1, void *arg2) { typedef decltype(name) *fntype; static fntype fn = (fntype)__sanitizer::dllThunkGetRealAddrOrDie(#name); return fn(arg1, arg2); } INTERCEPT_OR_DIE(#name, name);







#define INTERCEPT_WRAP_W_WWW(name) extern "C" void *name(void *arg1, void *arg2, void *arg3) { typedef decltype(name) *fntype; static fntype fn = (fntype)__sanitizer::dllThunkGetRealAddrOrDie(#name); return fn(arg1, arg2, arg3); } INTERCEPT_OR_DIE(#name, name);







#define INTERCEPT_WRAP_W_WWWW(name) extern "C" void *name(void *arg1, void *arg2, void *arg3, void *arg4) { typedef decltype(name) *fntype; static fntype fn = (fntype)__sanitizer::dllThunkGetRealAddrOrDie(#name); return fn(arg1, arg2, arg3, arg4); } INTERCEPT_OR_DIE(#name, name);







#define INTERCEPT_WRAP_W_WWWWW(name) extern "C" void *name(void *arg1, void *arg2, void *arg3, void *arg4, void *arg5) { typedef decltype(name) *fntype; static fntype fn = (fntype)__sanitizer::dllThunkGetRealAddrOrDie(#name); return fn(arg1, arg2, arg3, arg4, arg5); } INTERCEPT_OR_DIE(#name, name);








#define INTERCEPT_WRAP_W_WWWWWW(name) extern "C" void *name(void *arg1, void *arg2, void *arg3, void *arg4, void *arg5, void *arg6) { typedef decltype(name) *fntype; static fntype fn = (fntype)__sanitizer::dllThunkGetRealAddrOrDie(#name); return fn(arg1, arg2, arg3, arg4, arg5, arg6); } INTERCEPT_OR_DIE(#name, name);








#endif
