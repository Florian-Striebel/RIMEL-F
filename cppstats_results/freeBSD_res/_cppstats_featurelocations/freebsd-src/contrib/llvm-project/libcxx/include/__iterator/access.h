








#if !defined(_LIBCPP___ITERATOR_ACCESS_H)
#define _LIBCPP___ITERATOR_ACCESS_H

#include <__config>
#include <cstddef>

#if !defined(_LIBCPP_HAS_NO_PRAGMA_SYSTEM_HEADER)
#pragma GCC system_header
#endif

_LIBCPP_PUSH_MACROS
#include <__undef_macros>

_LIBCPP_BEGIN_NAMESPACE_STD

template <class _Tp, size_t _Np>
_LIBCPP_INLINE_VISIBILITY _LIBCPP_CONSTEXPR_AFTER_CXX11
_Tp*
begin(_Tp (&__array)[_Np])
{
return __array;
}

template <class _Tp, size_t _Np>
_LIBCPP_INLINE_VISIBILITY _LIBCPP_CONSTEXPR_AFTER_CXX11
_Tp*
end(_Tp (&__array)[_Np])
{
return __array + _Np;
}

#if !defined(_LIBCPP_CXX03_LANG)

template <class _Cp>
_LIBCPP_INLINE_VISIBILITY _LIBCPP_CONSTEXPR_AFTER_CXX14
auto
begin(_Cp& __c) -> decltype(__c.begin())
{
return __c.begin();
}

template <class _Cp>
_LIBCPP_INLINE_VISIBILITY _LIBCPP_CONSTEXPR_AFTER_CXX14
auto
begin(const _Cp& __c) -> decltype(__c.begin())
{
return __c.begin();
}

template <class _Cp>
_LIBCPP_INLINE_VISIBILITY _LIBCPP_CONSTEXPR_AFTER_CXX14
auto
end(_Cp& __c) -> decltype(__c.end())
{
return __c.end();
}

template <class _Cp>
_LIBCPP_INLINE_VISIBILITY _LIBCPP_CONSTEXPR_AFTER_CXX14
auto
end(const _Cp& __c) -> decltype(__c.end())
{
return __c.end();
}

#if _LIBCPP_STD_VER > 11

template <class _Cp>
_LIBCPP_INLINE_VISIBILITY _LIBCPP_CONSTEXPR_AFTER_CXX11
auto cbegin(const _Cp& __c) -> decltype(_VSTD::begin(__c))
{
return _VSTD::begin(__c);
}

template <class _Cp>
_LIBCPP_INLINE_VISIBILITY _LIBCPP_CONSTEXPR_AFTER_CXX11
auto cend(const _Cp& __c) -> decltype(_VSTD::end(__c))
{
return _VSTD::end(__c);
}

#endif


#else

template <class _Cp>
_LIBCPP_INLINE_VISIBILITY
typename _Cp::iterator
begin(_Cp& __c)
{
return __c.begin();
}

template <class _Cp>
_LIBCPP_INLINE_VISIBILITY
typename _Cp::const_iterator
begin(const _Cp& __c)
{
return __c.begin();
}

template <class _Cp>
_LIBCPP_INLINE_VISIBILITY
typename _Cp::iterator
end(_Cp& __c)
{
return __c.end();
}

template <class _Cp>
_LIBCPP_INLINE_VISIBILITY
typename _Cp::const_iterator
end(const _Cp& __c)
{
return __c.end();
}

#endif

_LIBCPP_END_NAMESPACE_STD

_LIBCPP_POP_MACROS

#endif
