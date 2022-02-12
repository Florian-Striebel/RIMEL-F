












#if SANITIZER_LINUX || SANITIZER_FREEBSD || SANITIZER_NETBSD || SANITIZER_SOLARIS


#if !defined(INCLUDED_FROM_INTERCEPTION_LIB)
#error "interception_linux.h should be included from interception library only"
#endif

#if !defined(INTERCEPTION_LINUX_H)
#define INTERCEPTION_LINUX_H

namespace __interception {
bool InterceptFunction(const char *name, uptr *ptr_to_real, uptr func,
uptr wrapper);
bool InterceptFunction(const char *name, const char *ver, uptr *ptr_to_real,
uptr func, uptr wrapper);
}

#define INTERCEPT_FUNCTION_LINUX_OR_FREEBSD(func) ::__interception::InterceptFunction( #func, (::__interception::uptr *) & REAL(func), (::__interception::uptr) & (func), (::__interception::uptr) & WRAP(func))







#if SANITIZER_GLIBC || SANITIZER_FREEBSD || SANITIZER_NETBSD
#define INTERCEPT_FUNCTION_VER_LINUX_OR_FREEBSD(func, symver) ::__interception::InterceptFunction( #func, symver, (::__interception::uptr *) & REAL(func), (::__interception::uptr) & (func), (::__interception::uptr) & WRAP(func))





#else
#define INTERCEPT_FUNCTION_VER_LINUX_OR_FREEBSD(func, symver) INTERCEPT_FUNCTION_LINUX_OR_FREEBSD(func)

#endif

#endif
#endif

