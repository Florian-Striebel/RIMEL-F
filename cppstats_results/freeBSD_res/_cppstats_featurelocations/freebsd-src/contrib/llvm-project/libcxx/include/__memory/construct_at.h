








#if !defined(_LIBCPP___MEMORY_CONSTRUCT_AT_H)
#define _LIBCPP___MEMORY_CONSTRUCT_AT_H

#include <__config>
#include <__debug>
#include <__utility/forward.h>
#include <utility>

#if !defined(_LIBCPP_HAS_NO_PRAGMA_SYSTEM_HEADER)
#pragma GCC system_header
#endif

_LIBCPP_PUSH_MACROS
#include <__undef_macros>

_LIBCPP_BEGIN_NAMESPACE_STD



#if _LIBCPP_STD_VER > 17

template<class _Tp, class ..._Args, class = decltype(
::new (declval<void*>()) _Tp(declval<_Args>()...)
)>
_LIBCPP_INLINE_VISIBILITY
constexpr _Tp* construct_at(_Tp* __location, _Args&& ...__args) {
_LIBCPP_ASSERT(__location, "null pointer given to construct_at");
return ::new ((void*)__location) _Tp(_VSTD::forward<_Args>(__args)...);
}

#endif



#if _LIBCPP_STD_VER > 14

template <class _Tp>
inline _LIBCPP_INLINE_VISIBILITY _LIBCPP_CONSTEXPR_AFTER_CXX17
void destroy_at(_Tp* __loc) {
_LIBCPP_ASSERT(__loc, "null pointer given to destroy_at");
__loc->~_Tp();
}

#endif

_LIBCPP_END_NAMESPACE_STD

_LIBCPP_POP_MACROS

#endif
