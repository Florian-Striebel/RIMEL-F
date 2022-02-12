








#if !defined(_LIBCPP___MEMORY_RAW_STORAGE_ITERATOR_H)
#define _LIBCPP___MEMORY_RAW_STORAGE_ITERATOR_H

#include <__config>
#include <__memory/addressof.h>
#include <cstddef>
#include <iterator>
#include <utility>

#if !defined(_LIBCPP_HAS_NO_PRAGMA_SYSTEM_HEADER)
#pragma GCC system_header
#endif

_LIBCPP_PUSH_MACROS
#include <__undef_macros>

_LIBCPP_BEGIN_NAMESPACE_STD

#if _LIBCPP_STD_VER <= 17 || defined(_LIBCPP_ENABLE_CXX20_REMOVED_RAW_STORAGE_ITERATOR)

_LIBCPP_SUPPRESS_DEPRECATED_PUSH
template <class _OutputIterator, class _Tp>
class _LIBCPP_TEMPLATE_VIS _LIBCPP_DEPRECATED_IN_CXX17 raw_storage_iterator
#if _LIBCPP_STD_VER <= 14 || !defined(_LIBCPP_ABI_NO_ITERATOR_BASES)
: public iterator<output_iterator_tag, void, void, void, void>
#endif
{
_LIBCPP_SUPPRESS_DEPRECATED_POP
private:
_OutputIterator __x_;
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

_LIBCPP_INLINE_VISIBILITY explicit raw_storage_iterator(_OutputIterator __x) : __x_(__x) {}
_LIBCPP_INLINE_VISIBILITY raw_storage_iterator& operator*() {return *this;}
_LIBCPP_INLINE_VISIBILITY raw_storage_iterator& operator=(const _Tp& __element)
{::new ((void*)_VSTD::addressof(*__x_)) _Tp(__element); return *this;}
#if _LIBCPP_STD_VER >= 14
_LIBCPP_INLINE_VISIBILITY raw_storage_iterator& operator=(_Tp&& __element)
{::new ((void*)_VSTD::addressof(*__x_)) _Tp(_VSTD::move(__element)); return *this;}
#endif
_LIBCPP_INLINE_VISIBILITY raw_storage_iterator& operator++() {++__x_; return *this;}
_LIBCPP_INLINE_VISIBILITY raw_storage_iterator operator++(int)
{raw_storage_iterator __t(*this); ++__x_; return __t;}
#if _LIBCPP_STD_VER >= 14
_LIBCPP_INLINE_VISIBILITY _OutputIterator base() const { return __x_; }
#endif
};

#endif

_LIBCPP_END_NAMESPACE_STD

_LIBCPP_POP_MACROS

#endif