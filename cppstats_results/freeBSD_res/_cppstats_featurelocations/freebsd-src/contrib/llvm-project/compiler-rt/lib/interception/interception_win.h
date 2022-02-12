












#if SANITIZER_WINDOWS

#if !defined(INCLUDED_FROM_INTERCEPTION_LIB)
#error "interception_win.h should be included from interception library only"
#endif

#if !defined(INTERCEPTION_WIN_H)
#define INTERCEPTION_WIN_H

namespace __interception {




bool OverrideFunction(uptr old_func, uptr new_func, uptr *orig_old_func = 0);


bool OverrideFunction(const char *name, uptr new_func, uptr *orig_old_func = 0);


uptr InternalGetProcAddress(void *module, const char *func_name);




bool OverrideImportedFunction(const char *module_to_patch,
const char *imported_module,
const char *function_name, uptr new_function,
uptr *orig_old_func);

#if !SANITIZER_WINDOWS64

bool OverrideFunctionWithDetour(
uptr old_func, uptr new_func, uptr *orig_old_func);
#endif


bool OverrideFunctionWithRedirectJump(
uptr old_func, uptr new_func, uptr *orig_old_func);
bool OverrideFunctionWithHotPatch(
uptr old_func, uptr new_func, uptr *orig_old_func);
bool OverrideFunctionWithTrampoline(
uptr old_func, uptr new_func, uptr *orig_old_func);


void TestOnlyReleaseTrampolineRegions();

}

#if defined(INTERCEPTION_DYNAMIC_CRT)
#define INTERCEPT_FUNCTION_WIN(func) ::__interception::OverrideFunction(#func, (::__interception::uptr)WRAP(func), (::__interception::uptr *)&REAL(func))



#else
#define INTERCEPT_FUNCTION_WIN(func) ::__interception::OverrideFunction((::__interception::uptr)func, (::__interception::uptr)WRAP(func), (::__interception::uptr *)&REAL(func))



#endif

#define INTERCEPT_FUNCTION_VER_WIN(func, symver) INTERCEPT_FUNCTION_WIN(func)

#define INTERCEPT_FUNCTION_DLLIMPORT(user_dll, provider_dll, func) ::__interception::OverrideImportedFunction( user_dll, provider_dll, #func, (::__interception::uptr)WRAP(func), (::__interception::uptr *)&REAL(func))




#endif
#endif
