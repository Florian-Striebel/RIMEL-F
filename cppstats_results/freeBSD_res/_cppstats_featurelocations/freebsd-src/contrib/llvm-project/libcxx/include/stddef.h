








#if defined(__need_ptrdiff_t) || defined(__need_size_t) || defined(__need_wchar_t) || defined(__need_NULL) || defined(__need_wint_t)


#if !defined(_LIBCPP_HAS_NO_PRAGMA_SYSTEM_HEADER)
#pragma GCC system_header
#endif

#include_next <stddef.h>

#elif !defined(_LIBCPP_STDDEF_H)
#define _LIBCPP_STDDEF_H


















#include <__config>

#if !defined(_LIBCPP_HAS_NO_PRAGMA_SYSTEM_HEADER)
#pragma GCC system_header
#endif

#include_next <stddef.h>

#if defined(__cplusplus)

extern "C++" {
#include <__nullptr>
using std::nullptr_t;
}

#endif

#endif
