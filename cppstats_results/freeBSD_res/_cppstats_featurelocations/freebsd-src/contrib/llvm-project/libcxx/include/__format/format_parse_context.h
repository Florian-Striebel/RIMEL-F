








#if !defined(_LIBCPP___FORMAT_FORMAT_PARSE_CONTEXT_H)
#define _LIBCPP___FORMAT_FORMAT_PARSE_CONTEXT_H

#include <__config>
#include <__format/format_error.h>
#include <string_view>

#if !defined(_LIBCPP_HAS_NO_PRAGMA_SYSTEM_HEADER)
#pragma GCC system_header
#endif

_LIBCPP_PUSH_MACROS
#include <__undef_macros>

_LIBCPP_BEGIN_NAMESPACE_STD

#if _LIBCPP_STD_VER > 17





#if !defined(_LIBCPP_HAS_NO_CONCEPTS) && !defined(_LIBCPP_HAS_NO_BUILTIN_IS_CONSTANT_EVALUATED)


template <class _CharT>
class _LIBCPP_TEMPLATE_VIS _LIBCPP_AVAILABILITY_FORMAT basic_format_parse_context {
public:
using char_type = _CharT;
using const_iterator = typename basic_string_view<_CharT>::const_iterator;
using iterator = const_iterator;

_LIBCPP_HIDE_FROM_ABI
constexpr explicit basic_format_parse_context(basic_string_view<_CharT> __fmt,
size_t __num_args = 0) noexcept
: __begin_(__fmt.begin()),
__end_(__fmt.end()),
__indexing_(__unknown),
__next_arg_id_(0),
__num_args_(__num_args) {}

basic_format_parse_context(const basic_format_parse_context&) = delete;
basic_format_parse_context&
operator=(const basic_format_parse_context&) = delete;

_LIBCPP_HIDE_FROM_ABI constexpr const_iterator begin() const noexcept {
return __begin_;
}
_LIBCPP_HIDE_FROM_ABI constexpr const_iterator end() const noexcept {
return __end_;
}
_LIBCPP_HIDE_FROM_ABI constexpr void advance_to(const_iterator __it) {
__begin_ = __it;
}

_LIBCPP_HIDE_FROM_ABI constexpr size_t next_arg_id() {
if (__indexing_ == __manual)
__throw_format_error("Using automatic argument numbering in manual "
"argument numbering mode");

if (__indexing_ == __unknown)
__indexing_ = __automatic;
return __next_arg_id_++;
}
_LIBCPP_HIDE_FROM_ABI constexpr void check_arg_id(size_t __id) {
if (__indexing_ == __automatic)
__throw_format_error("Using manual argument numbering in automatic "
"argument numbering mode");

if (__indexing_ == __unknown)
__indexing_ = __manual;








if (is_constant_evaluated() && __id >= __num_args_)
__throw_format_error("Argument index outside the valid range");
}

private:
iterator __begin_;
iterator __end_;
enum _Indexing { __unknown, __manual, __automatic };
_Indexing __indexing_;
size_t __next_arg_id_;
size_t __num_args_;
};

using format_parse_context = basic_format_parse_context<char>;
using wformat_parse_context = basic_format_parse_context<wchar_t>;

#endif

#endif

_LIBCPP_END_NAMESPACE_STD

_LIBCPP_POP_MACROS

#endif
