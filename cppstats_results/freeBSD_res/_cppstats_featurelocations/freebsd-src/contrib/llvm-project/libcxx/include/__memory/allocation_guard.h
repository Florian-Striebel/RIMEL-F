








#if !defined(_LIBCPP___MEMORY_ALLOCATION_GUARD_H)
#define _LIBCPP___MEMORY_ALLOCATION_GUARD_H

#include <__config>
#include <__memory/allocator_traits.h>
#include <cstddef>
#include <utility>

#if !defined(_LIBCPP_HAS_NO_PRAGMA_SYSTEM_HEADER)
#pragma GCC system_header
#endif

_LIBCPP_PUSH_MACROS
#include <__undef_macros>


_LIBCPP_BEGIN_NAMESPACE_STD




















template<class _Alloc>
struct __allocation_guard {
using _Pointer = typename allocator_traits<_Alloc>::pointer;
using _Size = typename allocator_traits<_Alloc>::size_type;

template<class _AllocT>
_LIBCPP_HIDE_FROM_ABI
explicit __allocation_guard(_AllocT __alloc, _Size __n)
: __alloc_(_VSTD::move(__alloc))
, __n_(__n)
, __ptr_(allocator_traits<_Alloc>::allocate(__alloc_, __n_))
{ }

_LIBCPP_HIDE_FROM_ABI
~__allocation_guard() _NOEXCEPT {
if (__ptr_ != nullptr) {
allocator_traits<_Alloc>::deallocate(__alloc_, __ptr_, __n_);
}
}

_LIBCPP_HIDE_FROM_ABI
_Pointer __release_ptr() _NOEXCEPT {
_Pointer __tmp = __ptr_;
__ptr_ = nullptr;
return __tmp;
}

_LIBCPP_HIDE_FROM_ABI
_Pointer __get() const _NOEXCEPT {
return __ptr_;
}

private:
_Alloc __alloc_;
_Size __n_;
_Pointer __ptr_;
};

_LIBCPP_END_NAMESPACE_STD

_LIBCPP_POP_MACROS

#endif
