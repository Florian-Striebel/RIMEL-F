












#if !defined(INTERCEPTION_H)
#define INTERCEPTION_H

#include "sanitizer_common/sanitizer_internal_defs.h"

#if !SANITIZER_LINUX && !SANITIZER_FREEBSD && !SANITIZER_MAC && !SANITIZER_NETBSD && !SANITIZER_WINDOWS && !SANITIZER_FUCHSIA && !SANITIZER_SOLARIS


#error "Interception doesn't work on this operating system."
#endif



typedef __sanitizer::uptr SIZE_T;
typedef __sanitizer::sptr SSIZE_T;
typedef __sanitizer::sptr PTRDIFF_T;
typedef __sanitizer::s64 INTMAX_T;
typedef __sanitizer::u64 UINTMAX_T;
typedef __sanitizer::OFF_T OFF_T;
typedef __sanitizer::OFF64_T OFF64_T;

























































#if SANITIZER_MAC
#include <sys/cdefs.h>


struct interpose_substitution {
const __sanitizer::uptr replacement;
const __sanitizer::uptr original;
};




#define INTERPOSER(func_name) __attribute__((used)) const interpose_substitution substitution_##func_name[] __attribute__((section("__DATA, __interpose"))) = { { reinterpret_cast<const uptr>(WRAP(func_name)), reinterpret_cast<const uptr>(func_name) } }









#define INTERPOSER_2(func_name, wrapper_name) __attribute__((used)) const interpose_substitution substitution_##func_name[] __attribute__((section("__DATA, __interpose"))) = { { reinterpret_cast<const uptr>(wrapper_name), reinterpret_cast<const uptr>(func_name) } }






#define WRAP(x) wrap_##x
#define WRAPPER_NAME(x) "wrap_"#x
#define INTERCEPTOR_ATTRIBUTE
#define DECLARE_WRAPPER(ret_type, func, ...)

#elif SANITIZER_WINDOWS
#define WRAP(x) __asan_wrap_##x
#define WRAPPER_NAME(x) "__asan_wrap_"#x
#define INTERCEPTOR_ATTRIBUTE __declspec(dllexport)
#define DECLARE_WRAPPER(ret_type, func, ...) extern "C" ret_type func(__VA_ARGS__);

#define DECLARE_WRAPPER_WINAPI(ret_type, func, ...) extern "C" __declspec(dllimport) ret_type __stdcall func(__VA_ARGS__);

#elif SANITIZER_FREEBSD || SANITIZER_NETBSD
#define WRAP(x) __interceptor_ ##x
#define WRAPPER_NAME(x) "__interceptor_" #x
#define INTERCEPTOR_ATTRIBUTE __attribute__((visibility("default")))



#define DECLARE_WRAPPER(ret_type, func, ...) extern "C" ret_type func(__VA_ARGS__) __attribute__((alias("__interceptor_" #func), visibility("default")));


#elif !SANITIZER_FUCHSIA
#define WRAP(x) __interceptor_ ##x
#define WRAPPER_NAME(x) "__interceptor_" #x
#define INTERCEPTOR_ATTRIBUTE __attribute__((visibility("default")))
#define DECLARE_WRAPPER(ret_type, func, ...) extern "C" ret_type func(__VA_ARGS__) __attribute__((weak, alias("__interceptor_" #func), visibility("default")));


#endif

#if SANITIZER_FUCHSIA



#include <zircon/sanitizer.h>
#define INTERCEPTOR_ATTRIBUTE __attribute__((visibility("default")))
#define REAL(x) __unsanitized_##x
#define DECLARE_REAL(ret_type, func, ...)
#elif !SANITIZER_MAC
#define PTR_TO_REAL(x) real_##x
#define REAL(x) __interception::PTR_TO_REAL(x)
#define FUNC_TYPE(x) x##_type

#define DECLARE_REAL(ret_type, func, ...) typedef ret_type (*FUNC_TYPE(func))(__VA_ARGS__); namespace __interception { extern FUNC_TYPE(func) PTR_TO_REAL(func); }




#define ASSIGN_REAL(dst, src) REAL(dst) = REAL(src)
#else
#define REAL(x) x
#define DECLARE_REAL(ret_type, func, ...) extern "C" ret_type func(__VA_ARGS__);

#define ASSIGN_REAL(x, y)
#endif

#if !SANITIZER_FUCHSIA
#define DECLARE_REAL_AND_INTERCEPTOR(ret_type, func, ...) DECLARE_REAL(ret_type, func, __VA_ARGS__) extern "C" ret_type WRAP(func)(__VA_ARGS__);




#define DECLARE_EXTERN_INTERCEPTOR_AND_WRAPPER(ret_type, func, ...) extern "C" ret_type WRAP(func)(__VA_ARGS__); extern "C" ret_type func(__VA_ARGS__);


#else
#define DECLARE_REAL_AND_INTERCEPTOR(ret_type, func, ...)
#define DECLARE_EXTERN_INTERCEPTOR_AND_WRAPPER(ret_type, func, ...)
#endif





#if !SANITIZER_MAC && !SANITIZER_FUCHSIA
#define DEFINE_REAL(ret_type, func, ...) typedef ret_type (*FUNC_TYPE(func))(__VA_ARGS__); namespace __interception { FUNC_TYPE(func) PTR_TO_REAL(func); }




#else
#define DEFINE_REAL(ret_type, func, ...)
#endif

#if SANITIZER_FUCHSIA




#define INTERCEPTOR(ret_type, func, ...) extern "C"[[ gnu::alias(#func), gnu::visibility("hidden") ]] ret_type __interceptor_##func(__VA_ARGS__); extern "C" INTERCEPTOR_ATTRIBUTE ret_type func(__VA_ARGS__)




#elif !SANITIZER_MAC

#define INTERCEPTOR(ret_type, func, ...) DEFINE_REAL(ret_type, func, __VA_ARGS__) DECLARE_WRAPPER(ret_type, func, __VA_ARGS__) extern "C" INTERCEPTOR_ATTRIBUTE ret_type WRAP(func)(__VA_ARGS__)







#define INTERCEPTOR_WITH_SUFFIX(ret_type, func, ...) INTERCEPTOR(ret_type, func, __VA_ARGS__)


#else

#define INTERCEPTOR_ZZZ(suffix, ret_type, func, ...) extern "C" ret_type func(__VA_ARGS__) suffix; extern "C" ret_type WRAP(func)(__VA_ARGS__); INTERPOSER(func); extern "C" INTERCEPTOR_ATTRIBUTE ret_type WRAP(func)(__VA_ARGS__)





#define INTERCEPTOR(ret_type, func, ...) INTERCEPTOR_ZZZ(, ret_type, func, __VA_ARGS__)


#define INTERCEPTOR_WITH_SUFFIX(ret_type, func, ...) INTERCEPTOR_ZZZ(__DARWIN_ALIAS_C(func), ret_type, func, __VA_ARGS__)



#define OVERRIDE_FUNCTION(overridee, overrider) INTERPOSER_2(overridee, WRAP(overrider))

#endif

#if SANITIZER_WINDOWS
#define INTERCEPTOR_WINAPI(ret_type, func, ...) typedef ret_type (__stdcall *FUNC_TYPE(func))(__VA_ARGS__); namespace __interception { FUNC_TYPE(func) PTR_TO_REAL(func); } extern "C" INTERCEPTOR_ATTRIBUTE ret_type __stdcall WRAP(func)(__VA_ARGS__)







#endif






namespace __interception {
#if defined(_WIN64)
typedef unsigned long long uptr;
#else
typedef unsigned long uptr;
#endif
}

#define INCLUDED_FROM_INTERCEPTION_LIB

#if SANITIZER_LINUX || SANITIZER_FREEBSD || SANITIZER_NETBSD || SANITIZER_SOLARIS


#include "interception_linux.h"
#define INTERCEPT_FUNCTION(func) INTERCEPT_FUNCTION_LINUX_OR_FREEBSD(func)
#define INTERCEPT_FUNCTION_VER(func, symver) INTERCEPT_FUNCTION_VER_LINUX_OR_FREEBSD(func, symver)

#elif SANITIZER_MAC
#include "interception_mac.h"
#define INTERCEPT_FUNCTION(func) INTERCEPT_FUNCTION_MAC(func)
#define INTERCEPT_FUNCTION_VER(func, symver) INTERCEPT_FUNCTION_VER_MAC(func, symver)

#elif SANITIZER_WINDOWS
#include "interception_win.h"
#define INTERCEPT_FUNCTION(func) INTERCEPT_FUNCTION_WIN(func)
#define INTERCEPT_FUNCTION_VER(func, symver) INTERCEPT_FUNCTION_VER_WIN(func, symver)

#endif

#undef INCLUDED_FROM_INTERCEPTION_LIB

#endif
