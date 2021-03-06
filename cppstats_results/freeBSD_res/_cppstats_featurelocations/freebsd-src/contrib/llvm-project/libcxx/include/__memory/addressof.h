








#if !defined(_LIBCPP___MEMORY_ADDRESSOF_H)
#define _LIBCPP___MEMORY_ADDRESSOF_H

#include <__config>

#if !defined(_LIBCPP_HAS_NO_PRAGMA_SYSTEM_HEADER)
#pragma GCC system_header
#endif

_LIBCPP_PUSH_MACROS
#include <__undef_macros>

_LIBCPP_BEGIN_NAMESPACE_STD

#if !defined(_LIBCPP_HAS_NO_BUILTIN_ADDRESSOF)

template <class _Tp>
inline _LIBCPP_CONSTEXPR_AFTER_CXX14
_LIBCPP_NO_CFI _LIBCPP_INLINE_VISIBILITY
_Tp*
addressof(_Tp& __x) _NOEXCEPT
{
return __builtin_addressof(__x);
}

#else

template <class _Tp>
inline _LIBCPP_NO_CFI _LIBCPP_INLINE_VISIBILITY
_Tp*
addressof(_Tp& __x) _NOEXCEPT
{
return reinterpret_cast<_Tp *>(
const_cast<char *>(&reinterpret_cast<const volatile char &>(__x)));
}

#endif

#if defined(_LIBCPP_HAS_OBJC_ARC) && !defined(_LIBCPP_PREDEFINED_OBJC_ARC_ADDRESSOF)




template <class _Tp>
inline _LIBCPP_INLINE_VISIBILITY
__strong _Tp*
addressof(__strong _Tp& __x) _NOEXCEPT
{
return &__x;
}

#if defined(_LIBCPP_HAS_OBJC_ARC_WEAK)
template <class _Tp>
inline _LIBCPP_INLINE_VISIBILITY
__weak _Tp*
addressof(__weak _Tp& __x) _NOEXCEPT
{
return &__x;
}
#endif

template <class _Tp>
inline _LIBCPP_INLINE_VISIBILITY
__autoreleasing _Tp*
addressof(__autoreleasing _Tp& __x) _NOEXCEPT
{
return &__x;
}

template <class _Tp>
inline _LIBCPP_INLINE_VISIBILITY
__unsafe_unretained _Tp*
addressof(__unsafe_unretained _Tp& __x) _NOEXCEPT
{
return &__x;
}
#endif

#if !defined(_LIBCPP_CXX03_LANG)
template <class _Tp> _Tp* addressof(const _Tp&&) noexcept = delete;
#endif

_LIBCPP_END_NAMESPACE_STD

_LIBCPP_POP_MACROS

#endif
