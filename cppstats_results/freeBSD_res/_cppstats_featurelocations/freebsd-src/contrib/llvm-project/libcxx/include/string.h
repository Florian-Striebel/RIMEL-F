








#if !defined(_LIBCPP_STRING_H)
#define _LIBCPP_STRING_H










































#include <__config>

#if !defined(_LIBCPP_HAS_NO_PRAGMA_SYSTEM_HEADER)
#pragma GCC system_header
#endif

#include_next <string.h>




#if defined(__CORRECT_ISO_CPP_STRING_H_PROTO) || defined(_LIBCPP_MSVCRT) || defined(__sun__) || defined(_STRING_H_CPLUSPLUS_98_CONFORMANCE_)

#define _LIBCPP_STRING_H_HAS_CONST_OVERLOADS
#endif

#if defined(__cplusplus) && !defined(_LIBCPP_STRING_H_HAS_CONST_OVERLOADS) && defined(_LIBCPP_PREFERRED_OVERLOAD)
extern "C++" {
inline _LIBCPP_INLINE_VISIBILITY
char* __libcpp_strchr(const char* __s, int __c) {return (char*)strchr(__s, __c);}
inline _LIBCPP_INLINE_VISIBILITY _LIBCPP_PREFERRED_OVERLOAD
const char* strchr(const char* __s, int __c) {return __libcpp_strchr(__s, __c);}
inline _LIBCPP_INLINE_VISIBILITY _LIBCPP_PREFERRED_OVERLOAD
char* strchr( char* __s, int __c) {return __libcpp_strchr(__s, __c);}

inline _LIBCPP_INLINE_VISIBILITY
char* __libcpp_strpbrk(const char* __s1, const char* __s2) {return (char*)strpbrk(__s1, __s2);}
inline _LIBCPP_INLINE_VISIBILITY _LIBCPP_PREFERRED_OVERLOAD
const char* strpbrk(const char* __s1, const char* __s2) {return __libcpp_strpbrk(__s1, __s2);}
inline _LIBCPP_INLINE_VISIBILITY _LIBCPP_PREFERRED_OVERLOAD
char* strpbrk( char* __s1, const char* __s2) {return __libcpp_strpbrk(__s1, __s2);}

inline _LIBCPP_INLINE_VISIBILITY
char* __libcpp_strrchr(const char* __s, int __c) {return (char*)strrchr(__s, __c);}
inline _LIBCPP_INLINE_VISIBILITY _LIBCPP_PREFERRED_OVERLOAD
const char* strrchr(const char* __s, int __c) {return __libcpp_strrchr(__s, __c);}
inline _LIBCPP_INLINE_VISIBILITY _LIBCPP_PREFERRED_OVERLOAD
char* strrchr( char* __s, int __c) {return __libcpp_strrchr(__s, __c);}

inline _LIBCPP_INLINE_VISIBILITY
void* __libcpp_memchr(const void* __s, int __c, size_t __n) {return (void*)memchr(__s, __c, __n);}
inline _LIBCPP_INLINE_VISIBILITY _LIBCPP_PREFERRED_OVERLOAD
const void* memchr(const void* __s, int __c, size_t __n) {return __libcpp_memchr(__s, __c, __n);}
inline _LIBCPP_INLINE_VISIBILITY _LIBCPP_PREFERRED_OVERLOAD
void* memchr( void* __s, int __c, size_t __n) {return __libcpp_memchr(__s, __c, __n);}

inline _LIBCPP_INLINE_VISIBILITY
char* __libcpp_strstr(const char* __s1, const char* __s2) {return (char*)strstr(__s1, __s2);}
inline _LIBCPP_INLINE_VISIBILITY _LIBCPP_PREFERRED_OVERLOAD
const char* strstr(const char* __s1, const char* __s2) {return __libcpp_strstr(__s1, __s2);}
inline _LIBCPP_INLINE_VISIBILITY _LIBCPP_PREFERRED_OVERLOAD
char* strstr( char* __s1, const char* __s2) {return __libcpp_strstr(__s1, __s2);}
}
#endif

#endif
