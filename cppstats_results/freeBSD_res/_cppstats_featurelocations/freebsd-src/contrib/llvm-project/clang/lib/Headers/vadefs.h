









#if !defined(_MSC_VER)
#include_next <vadefs.h>
#else

#if !defined(__clang_vadefs_h)
#define __clang_vadefs_h

#include_next <vadefs.h>


#if defined(_crt_va_start)
#undef _crt_va_start
#define _crt_va_start(ap, param) __builtin_va_start(ap, param)
#endif
#if defined(_crt_va_end)
#undef _crt_va_end
#define _crt_va_end(ap) __builtin_va_end(ap)
#endif
#if defined(_crt_va_arg)
#undef _crt_va_arg
#define _crt_va_arg(ap, type) __builtin_va_arg(ap, type)
#endif




#if defined(__crt_va_start)
#undef __crt_va_start
#define __crt_va_start(ap, param) __builtin_va_start(ap, param)
#endif
#if defined(__crt_va_end)
#undef __crt_va_end
#define __crt_va_end(ap) __builtin_va_end(ap)
#endif
#if defined(__crt_va_arg)
#undef __crt_va_arg
#define __crt_va_arg(ap, type) __builtin_va_arg(ap, type)
#endif

#endif
#endif
