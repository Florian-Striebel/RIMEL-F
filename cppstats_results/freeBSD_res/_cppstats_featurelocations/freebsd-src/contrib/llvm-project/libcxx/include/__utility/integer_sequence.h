







#if !defined(_LIBCPP___UTILITY_INTEGER_SEQUENCE_H)
#define _LIBCPP___UTILITY_INTEGER_SEQUENCE_H

#include <__config>
#include <type_traits>

#if !defined(_LIBCPP_HAS_NO_PRAGMA_SYSTEM_HEADER)
#pragma GCC system_header
#endif

_LIBCPP_PUSH_MACROS
#include <__undef_macros>

_LIBCPP_BEGIN_NAMESPACE_STD

#if _LIBCPP_STD_VER > 11

template<class _Tp, _Tp... _Ip>
struct _LIBCPP_TEMPLATE_VIS integer_sequence
{
typedef _Tp value_type;
static_assert( is_integral<_Tp>::value,
"std::integer_sequence can only be instantiated with an integral type" );
static
_LIBCPP_INLINE_VISIBILITY
constexpr
size_t
size() noexcept { return sizeof...(_Ip); }
};

template<size_t... _Ip>
using index_sequence = integer_sequence<size_t, _Ip...>;

#if __has_builtin(__make_integer_seq) && !defined(_LIBCPP_TESTING_FALLBACK_MAKE_INTEGER_SEQUENCE)

template <class _Tp, _Tp _Ep>
using __make_integer_sequence _LIBCPP_NODEBUG_TYPE = __make_integer_seq<integer_sequence, _Tp, _Ep>;

#else

template<typename _Tp, _Tp _Np> using __make_integer_sequence_unchecked _LIBCPP_NODEBUG_TYPE =
typename __detail::__make<_Np>::type::template __convert<integer_sequence, _Tp>;

template <class _Tp, _Tp _Ep>
struct __make_integer_sequence_checked
{
static_assert(is_integral<_Tp>::value,
"std::make_integer_sequence can only be instantiated with an integral type" );
static_assert(0 <= _Ep, "std::make_integer_sequence must have a non-negative sequence length");


typedef _LIBCPP_NODEBUG_TYPE __make_integer_sequence_unchecked<_Tp, 0 <= _Ep ? _Ep : 0> type;
};

template <class _Tp, _Tp _Ep>
using __make_integer_sequence _LIBCPP_NODEBUG_TYPE = typename __make_integer_sequence_checked<_Tp, _Ep>::type;

#endif

template<class _Tp, _Tp _Np>
using make_integer_sequence = __make_integer_sequence<_Tp, _Np>;

template<size_t _Np>
using make_index_sequence = make_integer_sequence<size_t, _Np>;

template<class... _Tp>
using index_sequence_for = make_index_sequence<sizeof...(_Tp)>;

#endif

_LIBCPP_END_NAMESPACE_STD

_LIBCPP_POP_MACROS

#endif
