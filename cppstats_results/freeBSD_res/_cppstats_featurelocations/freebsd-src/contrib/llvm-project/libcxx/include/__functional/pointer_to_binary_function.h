








#if !defined(_LIBCPP___FUNCTIONAL_POINTER_TO_BINARY_FUNCTION_H)
#define _LIBCPP___FUNCTIONAL_POINTER_TO_BINARY_FUNCTION_H

#include <__config>
#include <__functional/binary_function.h>

#if !defined(_LIBCPP_HAS_NO_PRAGMA_SYSTEM_HEADER)
#pragma GCC system_header
#endif

_LIBCPP_BEGIN_NAMESPACE_STD

#if _LIBCPP_STD_VER <= 14 || defined(_LIBCPP_ENABLE_CXX17_REMOVED_BINDERS)

template <class _Arg1, class _Arg2, class _Result>
class _LIBCPP_TEMPLATE_VIS _LIBCPP_DEPRECATED_IN_CXX11 pointer_to_binary_function
: public binary_function<_Arg1, _Arg2, _Result>
{
_Result (*__f_)(_Arg1, _Arg2);
public:
_LIBCPP_INLINE_VISIBILITY explicit pointer_to_binary_function(_Result (*__f)(_Arg1, _Arg2))
: __f_(__f) {}
_LIBCPP_INLINE_VISIBILITY _Result operator()(_Arg1 __x, _Arg2 __y) const
{return __f_(__x, __y);}
};

template <class _Arg1, class _Arg2, class _Result>
_LIBCPP_DEPRECATED_IN_CXX11 inline _LIBCPP_INLINE_VISIBILITY
pointer_to_binary_function<_Arg1,_Arg2,_Result>
ptr_fun(_Result (*__f)(_Arg1,_Arg2))
{return pointer_to_binary_function<_Arg1,_Arg2,_Result>(__f);}

#endif

_LIBCPP_END_NAMESPACE_STD

#endif
