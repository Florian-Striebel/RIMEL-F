








#if !defined(_LIBCPP___RANGES_COPYABLE_BOX_H)
#define _LIBCPP___RANGES_COPYABLE_BOX_H

#include <__config>
#include <__memory/addressof.h>
#include <__memory/construct_at.h>
#include <__utility/move.h>
#include <concepts>
#include <optional>
#include <type_traits>

#if !defined(_LIBCPP_HAS_NO_PRAGMA_SYSTEM_HEADER)
#pragma GCC system_header
#endif

_LIBCPP_PUSH_MACROS
#include <__undef_macros>

_LIBCPP_BEGIN_NAMESPACE_STD

#if !defined(_LIBCPP_HAS_NO_RANGES)









template<class _Tp>
concept __copy_constructible_object = copy_constructible<_Tp> && is_object_v<_Tp>;

namespace ranges {

template<__copy_constructible_object _Tp>
class __copyable_box {
[[no_unique_address]] optional<_Tp> __val_;

public:
template<class ..._Args>
requires is_constructible_v<_Tp, _Args...>
_LIBCPP_HIDE_FROM_ABI
constexpr explicit __copyable_box(in_place_t, _Args&& ...__args)
noexcept(is_nothrow_constructible_v<_Tp, _Args...>)
: __val_(in_place, _VSTD::forward<_Args>(__args)...)
{ }

_LIBCPP_HIDE_FROM_ABI
constexpr __copyable_box() noexcept(is_nothrow_default_constructible_v<_Tp>)
requires default_initializable<_Tp>
: __val_(in_place)
{ }

_LIBCPP_HIDE_FROM_ABI __copyable_box(__copyable_box const&) = default;
_LIBCPP_HIDE_FROM_ABI __copyable_box(__copyable_box&&) = default;

_LIBCPP_HIDE_FROM_ABI
constexpr __copyable_box& operator=(__copyable_box const& __other)
noexcept(is_nothrow_copy_constructible_v<_Tp>)
{
if (this != _VSTD::addressof(__other)) {
if (__other.__has_value()) __val_.emplace(*__other);
else __val_.reset();
}
return *this;
}

_LIBCPP_HIDE_FROM_ABI
__copyable_box& operator=(__copyable_box&&) requires movable<_Tp> = default;

_LIBCPP_HIDE_FROM_ABI
constexpr __copyable_box& operator=(__copyable_box&& __other)
noexcept(is_nothrow_move_constructible_v<_Tp>)
{
if (this != _VSTD::addressof(__other)) {
if (__other.__has_value()) __val_.emplace(_VSTD::move(*__other));
else __val_.reset();
}
return *this;
}

_LIBCPP_HIDE_FROM_ABI constexpr _Tp const& operator*() const noexcept { return *__val_; }
_LIBCPP_HIDE_FROM_ABI constexpr _Tp& operator*() noexcept { return *__val_; }
_LIBCPP_HIDE_FROM_ABI constexpr bool __has_value() const noexcept { return __val_.has_value(); }
};













template<class _Tp>
concept __doesnt_need_empty_state_for_copy = copyable<_Tp> || is_nothrow_copy_constructible_v<_Tp>;

template<class _Tp>
concept __doesnt_need_empty_state_for_move = movable<_Tp> || is_nothrow_move_constructible_v<_Tp>;

template<__copy_constructible_object _Tp>
requires __doesnt_need_empty_state_for_copy<_Tp> && __doesnt_need_empty_state_for_move<_Tp>
class __copyable_box<_Tp> {
[[no_unique_address]] _Tp __val_;

public:
template<class ..._Args>
requires is_constructible_v<_Tp, _Args...>
_LIBCPP_HIDE_FROM_ABI
constexpr explicit __copyable_box(in_place_t, _Args&& ...__args)
noexcept(is_nothrow_constructible_v<_Tp, _Args...>)
: __val_(_VSTD::forward<_Args>(__args)...)
{ }

_LIBCPP_HIDE_FROM_ABI
constexpr __copyable_box() noexcept(is_nothrow_default_constructible_v<_Tp>)
requires default_initializable<_Tp>
: __val_()
{ }

_LIBCPP_HIDE_FROM_ABI __copyable_box(__copyable_box const&) = default;
_LIBCPP_HIDE_FROM_ABI __copyable_box(__copyable_box&&) = default;


_LIBCPP_HIDE_FROM_ABI __copyable_box& operator=(__copyable_box const&) requires copyable<_Tp> = default;
_LIBCPP_HIDE_FROM_ABI __copyable_box& operator=(__copyable_box&&) requires movable<_Tp> = default;


_LIBCPP_HIDE_FROM_ABI
constexpr __copyable_box& operator=(__copyable_box const& __other) noexcept {
static_assert(is_nothrow_copy_constructible_v<_Tp>);
if (this != _VSTD::addressof(__other)) {
_VSTD::destroy_at(_VSTD::addressof(__val_));
_VSTD::construct_at(_VSTD::addressof(__val_), __other.__val_);
}
return *this;
}

_LIBCPP_HIDE_FROM_ABI
constexpr __copyable_box& operator=(__copyable_box&& __other) noexcept {
static_assert(is_nothrow_move_constructible_v<_Tp>);
if (this != _VSTD::addressof(__other)) {
_VSTD::destroy_at(_VSTD::addressof(__val_));
_VSTD::construct_at(_VSTD::addressof(__val_), _VSTD::move(__other.__val_));
}
return *this;
}

_LIBCPP_HIDE_FROM_ABI constexpr _Tp const& operator*() const noexcept { return __val_; }
_LIBCPP_HIDE_FROM_ABI constexpr _Tp& operator*() noexcept { return __val_; }
_LIBCPP_HIDE_FROM_ABI constexpr bool __has_value() const noexcept { return true; }
};
}

#endif

_LIBCPP_END_NAMESPACE_STD

_LIBCPP_POP_MACROS

#endif
