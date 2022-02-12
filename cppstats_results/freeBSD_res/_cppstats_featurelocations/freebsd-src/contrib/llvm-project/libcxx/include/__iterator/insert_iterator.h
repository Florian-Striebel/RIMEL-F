








#if !defined(_LIBCPP___ITERATOR_INSERT_ITERATOR_H)
#define _LIBCPP___ITERATOR_INSERT_ITERATOR_H

#include <__config>
#include <__iterator/iterator.h>
#include <__iterator/iterator_traits.h>
#include <__memory/addressof.h>
#include <__utility/move.h>
#include <cstddef>

#if !defined(_LIBCPP_HAS_NO_PRAGMA_SYSTEM_HEADER)
#pragma GCC system_header
#endif

_LIBCPP_PUSH_MACROS
#include <__undef_macros>

_LIBCPP_BEGIN_NAMESPACE_STD

_LIBCPP_SUPPRESS_DEPRECATED_PUSH
template <class _Container>
class _LIBCPP_TEMPLATE_VIS insert_iterator
#if _LIBCPP_STD_VER <= 14 || !defined(_LIBCPP_ABI_NO_ITERATOR_BASES)
: public iterator<output_iterator_tag, void, void, void, void>
#endif
{
_LIBCPP_SUPPRESS_DEPRECATED_POP
protected:
_Container* container;
typename _Container::iterator iter;
public:
typedef output_iterator_tag iterator_category;
typedef void value_type;
#if _LIBCPP_STD_VER > 17
typedef ptrdiff_t difference_type;
#else
typedef void difference_type;
#endif
typedef void pointer;
typedef void reference;
typedef _Container container_type;

_LIBCPP_INLINE_VISIBILITY _LIBCPP_CONSTEXPR_AFTER_CXX17 insert_iterator(_Container& __x, typename _Container::iterator __i)
: container(_VSTD::addressof(__x)), iter(__i) {}
_LIBCPP_INLINE_VISIBILITY _LIBCPP_CONSTEXPR_AFTER_CXX17 insert_iterator& operator=(const typename _Container::value_type& __value_)
{iter = container->insert(iter, __value_); ++iter; return *this;}
#if !defined(_LIBCPP_CXX03_LANG)
_LIBCPP_INLINE_VISIBILITY _LIBCPP_CONSTEXPR_AFTER_CXX17 insert_iterator& operator=(typename _Container::value_type&& __value_)
{iter = container->insert(iter, _VSTD::move(__value_)); ++iter; return *this;}
#endif
_LIBCPP_INLINE_VISIBILITY _LIBCPP_CONSTEXPR_AFTER_CXX17 insert_iterator& operator*() {return *this;}
_LIBCPP_INLINE_VISIBILITY _LIBCPP_CONSTEXPR_AFTER_CXX17 insert_iterator& operator++() {return *this;}
_LIBCPP_INLINE_VISIBILITY _LIBCPP_CONSTEXPR_AFTER_CXX17 insert_iterator& operator++(int) {return *this;}
};

template <class _Container>
inline _LIBCPP_INLINE_VISIBILITY _LIBCPP_CONSTEXPR_AFTER_CXX17
insert_iterator<_Container>
inserter(_Container& __x, typename _Container::iterator __i)
{
return insert_iterator<_Container>(__x, __i);
}

_LIBCPP_END_NAMESPACE_STD

_LIBCPP_POP_MACROS

#endif