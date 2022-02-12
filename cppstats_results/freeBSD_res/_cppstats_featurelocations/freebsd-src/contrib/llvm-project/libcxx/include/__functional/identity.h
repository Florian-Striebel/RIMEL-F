








#if !defined(_LIBCPP___FUNCTIONAL_IDENTITY_H)
#define _LIBCPP___FUNCTIONAL_IDENTITY_H

#include <__config>
#include <utility>

#if !defined(_LIBCPP_HAS_NO_PRAGMA_SYSTEM_HEADER)
#pragma GCC system_header
#endif

_LIBCPP_BEGIN_NAMESPACE_STD

#if _LIBCPP_STD_VER > 17

struct identity {
template<class _Tp>
_LIBCPP_NODISCARD_EXT constexpr _Tp&& operator()(_Tp&& __t) const noexcept
{
return _VSTD::forward<_Tp>(__t);
}

using is_transparent = void;
};
#endif

_LIBCPP_END_NAMESPACE_STD

#endif
