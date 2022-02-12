







#if !defined(_LIBCPP___ALGORITHM_COMP_REF_TYPE_H)
#define _LIBCPP___ALGORITHM_COMP_REF_TYPE_H

#include <__config>
#include <type_traits>

#if !defined(_LIBCPP_HAS_NO_PRAGMA_SYSTEM_HEADER)
#pragma GCC system_header
#endif

_LIBCPP_PUSH_MACROS
#include <__undef_macros>

_LIBCPP_BEGIN_NAMESPACE_STD

#if defined(_LIBCPP_DEBUG)

template <class _Compare>
struct __debug_less
{
_Compare &__comp_;
_LIBCPP_CONSTEXPR_AFTER_CXX17
__debug_less(_Compare& __c) : __comp_(__c) {}

template <class _Tp, class _Up>
_LIBCPP_CONSTEXPR_AFTER_CXX17
bool operator()(const _Tp& __x, const _Up& __y)
{
bool __r = __comp_(__x, __y);
if (__r)
__do_compare_assert(0, __y, __x);
return __r;
}

template <class _Tp, class _Up>
_LIBCPP_CONSTEXPR_AFTER_CXX17
bool operator()(_Tp& __x, _Up& __y)
{
bool __r = __comp_(__x, __y);
if (__r)
__do_compare_assert(0, __y, __x);
return __r;
}

template <class _LHS, class _RHS>
_LIBCPP_CONSTEXPR_AFTER_CXX17
inline _LIBCPP_INLINE_VISIBILITY
decltype((void)declval<_Compare&>()(
declval<_LHS &>(), declval<_RHS &>()))
__do_compare_assert(int, _LHS & __l, _RHS & __r) {
_LIBCPP_ASSERT(!__comp_(__l, __r),
"Comparator does not induce a strict weak ordering");
}

template <class _LHS, class _RHS>
_LIBCPP_CONSTEXPR_AFTER_CXX17
inline _LIBCPP_INLINE_VISIBILITY
void __do_compare_assert(long, _LHS &, _RHS &) {}
};

#endif

template <class _Comp>
struct __comp_ref_type {


#if !defined(_LIBCPP_DEBUG)
typedef typename add_lvalue_reference<_Comp>::type type;
#else
typedef __debug_less<_Comp> type;
#endif
};


_LIBCPP_END_NAMESPACE_STD

_LIBCPP_POP_MACROS

#endif
