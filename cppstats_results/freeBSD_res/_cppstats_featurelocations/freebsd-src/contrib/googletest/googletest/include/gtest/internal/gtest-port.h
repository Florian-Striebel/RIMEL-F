









































#if !defined(GTEST_INCLUDE_GTEST_INTERNAL_GTEST_PORT_H_)
#define GTEST_INCLUDE_GTEST_INTERNAL_GTEST_PORT_H_





























































































































































































































#include <ctype.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#if !defined(_WIN32_WCE)
#include <sys/types.h>
#include <sys/stat.h>
#endif

#if defined __APPLE__
#include <AvailabilityMacros.h>
#include <TargetConditionals.h>
#endif



#include <string>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <utility>
#include <vector>

#include "gtest/internal/gtest-port-arch.h"
#include "gtest/internal/custom/gtest-port.h"

#if !defined(GTEST_DEV_EMAIL_)
#define GTEST_DEV_EMAIL_ "googletestframework@@googlegroups.com"
#define GTEST_FLAG_PREFIX_ "gtest_"
#define GTEST_FLAG_PREFIX_DASH_ "gtest-"
#define GTEST_FLAG_PREFIX_UPPER_ "GTEST_"
#define GTEST_NAME_ "Google Test"
#define GTEST_PROJECT_URL_ "https://github.com/google/googletest/"
#endif

#if !defined(GTEST_INIT_GOOGLE_TEST_NAME_)
#define GTEST_INIT_GOOGLE_TEST_NAME_ "testing::InitGoogleTest"
#endif


#if defined(__GNUC__)

#define GTEST_GCC_VER_ (__GNUC__*10000 + __GNUC_MINOR__*100 + __GNUC_PATCHLEVEL__)

#endif






#if _MSC_VER >= 1400
#define GTEST_DISABLE_MSC_WARNINGS_PUSH_(warnings) __pragma(warning(push)) __pragma(warning(disable: warnings))


#define GTEST_DISABLE_MSC_WARNINGS_POP_() __pragma(warning(pop))

#else

#define GTEST_DISABLE_MSC_WARNINGS_PUSH_(warnings)
#define GTEST_DISABLE_MSC_WARNINGS_POP_()
#endif



#if defined(__clang__)
#define GTEST_DISABLE_MSC_DEPRECATED_PUSH_() _Pragma("clang diagnostic push") _Pragma("clang diagnostic ignored \"-Wdeprecated-declarations\"") _Pragma("clang diagnostic ignored \"-Wdeprecated-implementations\"")



#define GTEST_DISABLE_MSC_DEPRECATED_POP_() _Pragma("clang diagnostic pop")

#else
#define GTEST_DISABLE_MSC_DEPRECATED_PUSH_() GTEST_DISABLE_MSC_WARNINGS_PUSH_(4996)

#define GTEST_DISABLE_MSC_DEPRECATED_POP_() GTEST_DISABLE_MSC_WARNINGS_POP_()

#endif

#if !defined(GTEST_LANG_CXX11)




#if __GXX_EXPERIMENTAL_CXX0X__ || __cplusplus >= 201103L || _MSC_VER >= 1900

#define GTEST_LANG_CXX11 1
#else
#define GTEST_LANG_CXX11 0
#endif
#endif










#if GTEST_LANG_CXX11 && (!defined(__GLIBCXX__) || ( __GLIBCXX__ >= 20110325ul && __GLIBCXX__ != 20110416ul && __GLIBCXX__ != 20120313ul && __GLIBCXX__ != 20110428ul && __GLIBCXX__ != 20120702ul))







#define GTEST_STDLIB_CXX11 1
#endif


#if GTEST_STDLIB_CXX11
#define GTEST_HAS_STD_BEGIN_AND_END_ 1
#define GTEST_HAS_STD_FORWARD_LIST_ 1
#if !defined(_MSC_VER) || (_MSC_FULL_VER >= 190023824)

#define GTEST_HAS_STD_FUNCTION_ 1
#endif
#define GTEST_HAS_STD_INITIALIZER_LIST_ 1
#define GTEST_HAS_STD_MOVE_ 1
#define GTEST_HAS_STD_UNIQUE_PTR_ 1
#define GTEST_HAS_STD_SHARED_PTR_ 1
#define GTEST_HAS_UNORDERED_MAP_ 1
#define GTEST_HAS_UNORDERED_SET_ 1
#endif



#if GTEST_LANG_CXX11
#define GTEST_HAS_STD_TUPLE_ 1
#if defined(__clang__)


#if defined(__has_include) && !__has_include(<tuple>)
#undef GTEST_HAS_STD_TUPLE_
#endif
#elif defined(_MSC_VER)

#if defined(_CPPLIB_VER) && _CPPLIB_VER < 520
#undef GTEST_HAS_STD_TUPLE_
#endif
#elif defined(__GLIBCXX__)



#if __GNUC__ < 4 || (__GNUC__ == 4 && __GNUC_MINOR__ < 2)
#undef GTEST_HAS_STD_TUPLE_
#endif
#endif
#endif




#if GTEST_OS_WINDOWS
#if !GTEST_OS_WINDOWS_MOBILE
#include <direct.h>
#include <io.h>
#endif

#if GTEST_OS_WINDOWS_MINGW && !defined(__MINGW64_VERSION_MAJOR)


typedef struct _CRITICAL_SECTION GTEST_CRITICAL_SECTION;
#else



typedef struct _RTL_CRITICAL_SECTION GTEST_CRITICAL_SECTION;
#endif
#else



#include <unistd.h>
#include <strings.h>
#endif

#if GTEST_OS_LINUX_ANDROID

#include <android/api-level.h>
#endif


#if !defined(GTEST_HAS_POSIX_RE)
#if GTEST_OS_LINUX_ANDROID

#define GTEST_HAS_POSIX_RE (__ANDROID_API__ >= 9)
#else
#define GTEST_HAS_POSIX_RE (!GTEST_OS_WINDOWS)
#endif
#endif

#if GTEST_USES_PCRE


#elif GTEST_HAS_POSIX_RE





#include <regex.h>

#define GTEST_USES_POSIX_RE 1

#elif GTEST_OS_WINDOWS



#define GTEST_USES_SIMPLE_RE 1

#else



#define GTEST_USES_SIMPLE_RE 1

#endif

#if !defined(GTEST_HAS_EXCEPTIONS)


#if defined(_MSC_VER) && defined(_CPPUNWIND)

#define GTEST_HAS_EXCEPTIONS 1
#elif defined(__BORLANDC__)



#if !defined(_HAS_EXCEPTIONS)
#define _HAS_EXCEPTIONS 1
#endif
#define GTEST_HAS_EXCEPTIONS _HAS_EXCEPTIONS
#elif defined(__clang__)







#define GTEST_HAS_EXCEPTIONS (__EXCEPTIONS && __has_feature(cxx_exceptions))
#elif defined(__GNUC__) && __EXCEPTIONS

#define GTEST_HAS_EXCEPTIONS 1
#elif defined(__SUNPRO_CC)



#define GTEST_HAS_EXCEPTIONS 1
#elif defined(__IBMCPP__) && __EXCEPTIONS

#define GTEST_HAS_EXCEPTIONS 1
#elif defined(__HP_aCC)


#define GTEST_HAS_EXCEPTIONS 1
#else


#define GTEST_HAS_EXCEPTIONS 0
#endif
#endif

#if !defined(GTEST_HAS_STD_STRING)


#define GTEST_HAS_STD_STRING 1
#elif !GTEST_HAS_STD_STRING

#error "::std::string isn't available."
#endif

#if !defined(GTEST_HAS_GLOBAL_STRING)
#define GTEST_HAS_GLOBAL_STRING 0
#endif

#if !defined(GTEST_HAS_STD_WSTRING)








#define GTEST_HAS_STD_WSTRING (!(GTEST_OS_LINUX_ANDROID || GTEST_OS_CYGWIN || GTEST_OS_SOLARIS))


#endif

#if !defined(GTEST_HAS_GLOBAL_WSTRING)


#define GTEST_HAS_GLOBAL_WSTRING (GTEST_HAS_STD_WSTRING && GTEST_HAS_GLOBAL_STRING)

#endif


#if !defined(GTEST_HAS_RTTI)



#if defined(_MSC_VER)

#if defined(_CPPRTTI)
#define GTEST_HAS_RTTI 1
#else
#define GTEST_HAS_RTTI 0
#endif


#elif defined(__GNUC__) && (GTEST_GCC_VER_ >= 40302)

#if defined(__GXX_RTTI)




#if GTEST_OS_LINUX_ANDROID && defined(_STLPORT_MAJOR) && !defined(__EXCEPTIONS)

#define GTEST_HAS_RTTI 0
#else
#define GTEST_HAS_RTTI 1
#endif
#else
#define GTEST_HAS_RTTI 0
#endif




#elif defined(__clang__)

#define GTEST_HAS_RTTI __has_feature(cxx_rtti)



#elif defined(__IBMCPP__) && (__IBMCPP__ >= 900)

#if defined(__RTTI_ALL__)
#define GTEST_HAS_RTTI 1
#else
#define GTEST_HAS_RTTI 0
#endif

#else


#define GTEST_HAS_RTTI 1

#endif

#endif



#if GTEST_HAS_RTTI
#include <typeinfo>
#endif


#if !defined(GTEST_HAS_PTHREAD)





#define GTEST_HAS_PTHREAD (GTEST_OS_LINUX || GTEST_OS_MAC || GTEST_OS_HPUX || GTEST_OS_QNX || GTEST_OS_FREEBSD || GTEST_OS_NACL || GTEST_OS_NETBSD || GTEST_OS_FUCHSIA)


#endif

#if GTEST_HAS_PTHREAD


#include <pthread.h>


#include <time.h>
#endif



#if !defined(GTEST_HAS_HASH_MAP_)
#if defined(_MSC_VER) && (_MSC_VER < 1900)
#define GTEST_HAS_HASH_MAP_ 1
#define GTEST_HAS_HASH_SET_ 1
#endif
#endif




#if !defined(GTEST_HAS_TR1_TUPLE)
#if GTEST_OS_LINUX_ANDROID && defined(_STLPORT_MAJOR)

#define GTEST_HAS_TR1_TUPLE 0
#elif defined(_MSC_VER) && (_MSC_VER >= 1910)



#define GTEST_HAS_TR1_TUPLE 0
#elif GTEST_LANG_CXX11 && defined(_LIBCPP_VERSION)

#define GTEST_HAS_TR1_TUPLE 0
#else

#define GTEST_HAS_TR1_TUPLE 1
#endif
#endif



#if !defined(GTEST_USE_OWN_TR1_TUPLE)

#if GTEST_OS_SYMBIAN
#define GTEST_USE_OWN_TR1_TUPLE 1
#else












#if (defined(__GNUC__) && !defined(__CUDACC__) && (GTEST_GCC_VER_ >= 40000) && !GTEST_OS_QNX && !defined(_LIBCPP_VERSION)) || (_MSC_VER >= 1600 && _MSC_VER < 1900)


#define GTEST_ENV_HAS_TR1_TUPLE_ 1
#endif




#if GTEST_LANG_CXX11 && (!defined(__GLIBCXX__) || __GLIBCXX__ > 20110325)
#define GTEST_ENV_HAS_STD_TUPLE_ 1
#endif

#if GTEST_ENV_HAS_TR1_TUPLE_ || GTEST_ENV_HAS_STD_TUPLE_
#define GTEST_USE_OWN_TR1_TUPLE 0
#else
#define GTEST_USE_OWN_TR1_TUPLE 1
#endif
#endif
#endif



#if GTEST_HAS_STD_TUPLE_
#include <tuple>
#define GTEST_TUPLE_NAMESPACE_ ::std
#endif



#if GTEST_HAS_TR1_TUPLE
#if !defined(GTEST_TUPLE_NAMESPACE_)
#define GTEST_TUPLE_NAMESPACE_ ::std::tr1
#endif

#if GTEST_USE_OWN_TR1_TUPLE
#include "gtest/internal/gtest-tuple.h"
#elif GTEST_OS_SYMBIAN






#if defined(BOOST_HAS_TR1_TUPLE)
#undef BOOST_HAS_TR1_TUPLE
#endif



#define BOOST_TR1_DETAIL_CONFIG_HPP_INCLUDED
#include <tuple>

#elif defined(__GNUC__) && (GTEST_GCC_VER_ >= 40000)



#if !GTEST_HAS_RTTI && GTEST_GCC_VER_ < 40302





#define _TR1_FUNCTIONAL 1
#include <tr1/tuple>
#undef _TR1_FUNCTIONAL

#else
#include <tr1/tuple>
#endif


#elif _MSC_VER >= 1600
#include <tuple>

#else
#include <tr1/tuple>
#endif

#endif





#if !defined(GTEST_HAS_CLONE)


#if GTEST_OS_LINUX && !defined(__ia64__)
#if GTEST_OS_LINUX_ANDROID


#if defined(__LP64__) || (defined(__arm__) && __ANDROID_API__ >= 9) || (defined(__mips__) && __ANDROID_API__ >= 12) || (defined(__i386__) && __ANDROID_API__ >= 17)



#define GTEST_HAS_CLONE 1
#else
#define GTEST_HAS_CLONE 0
#endif
#else
#define GTEST_HAS_CLONE 1
#endif
#else
#define GTEST_HAS_CLONE 0
#endif

#endif



#if !defined(GTEST_HAS_STREAM_REDIRECTION)


#if GTEST_OS_WINDOWS_MOBILE || GTEST_OS_SYMBIAN || GTEST_OS_WINDOWS_PHONE || GTEST_OS_WINDOWS_RT

#define GTEST_HAS_STREAM_REDIRECTION 0
#else
#define GTEST_HAS_STREAM_REDIRECTION 1
#endif
#endif





#if (GTEST_OS_LINUX || GTEST_OS_CYGWIN || GTEST_OS_SOLARIS || (GTEST_OS_MAC && !GTEST_OS_IOS) || (GTEST_OS_WINDOWS_DESKTOP && _MSC_VER >= 1400) || GTEST_OS_WINDOWS_MINGW || GTEST_OS_AIX || GTEST_OS_HPUX || GTEST_OS_OPENBSD || GTEST_OS_QNX || GTEST_OS_FREEBSD || GTEST_OS_NETBSD || GTEST_OS_FUCHSIA)





#define GTEST_HAS_DEATH_TEST 1
#endif





#if defined(__GNUC__) || (_MSC_VER >= 1400) || defined(__SUNPRO_CC) || defined(__IBMCPP__) || defined(__HP_aCC)

#define GTEST_HAS_TYPED_TEST 1
#define GTEST_HAS_TYPED_TEST_P 1
#endif





#if (GTEST_HAS_TR1_TUPLE || GTEST_HAS_STD_TUPLE_) && !defined(__SUNPRO_CC)
#define GTEST_HAS_COMBINE 1
#endif


#define GTEST_WIDE_STRING_USES_UTF16_ (GTEST_OS_WINDOWS || GTEST_OS_CYGWIN || GTEST_OS_SYMBIAN || GTEST_OS_AIX)



#if GTEST_OS_LINUX
#define GTEST_CAN_STREAM_RESULTS_ 1
#endif











#if defined(__INTEL_COMPILER)
#define GTEST_AMBIGUOUS_ELSE_BLOCKER_
#else
#define GTEST_AMBIGUOUS_ELSE_BLOCKER_ switch (0) case 0: default:
#endif












#if defined(__GNUC__) && !defined(COMPILER_ICC)
#define GTEST_ATTRIBUTE_UNUSED_ __attribute__ ((unused))
#elif defined(__clang__)
#if __has_attribute(unused)
#define GTEST_ATTRIBUTE_UNUSED_ __attribute__ ((unused))
#endif
#endif
#if !defined(GTEST_ATTRIBUTE_UNUSED_)
#define GTEST_ATTRIBUTE_UNUSED_
#endif

#if GTEST_LANG_CXX11
#define GTEST_CXX11_EQUALS_DELETE_ = delete
#else
#define GTEST_CXX11_EQUALS_DELETE_
#endif


#if (defined(__GNUC__) || defined(__clang__)) && !defined(COMPILER_ICC)
#if defined(__MINGW_PRINTF_FORMAT)



#define GTEST_ATTRIBUTE_PRINTF_(string_index, first_to_check) __attribute__((__format__(__MINGW_PRINTF_FORMAT, string_index, first_to_check)))


#else
#define GTEST_ATTRIBUTE_PRINTF_(string_index, first_to_check) __attribute__((__format__(__printf__, string_index, first_to_check)))

#endif
#else
#define GTEST_ATTRIBUTE_PRINTF_(string_index, first_to_check)
#endif




#define GTEST_DISALLOW_ASSIGN_(type) void operator=(type const &) GTEST_CXX11_EQUALS_DELETE_




#define GTEST_DISALLOW_COPY_AND_ASSIGN_(type) type(type const &) GTEST_CXX11_EQUALS_DELETE_; GTEST_DISALLOW_ASSIGN_(type)








#if defined(__GNUC__) && (GTEST_GCC_VER_ >= 30400) && !defined(COMPILER_ICC)
#define GTEST_MUST_USE_RESULT_ __attribute__ ((warn_unused_result))
#else
#define GTEST_MUST_USE_RESULT_
#endif









#define GTEST_INTENTIONAL_CONST_COND_PUSH_() GTEST_DISABLE_MSC_WARNINGS_PUSH_(4127)

#define GTEST_INTENTIONAL_CONST_COND_POP_() GTEST_DISABLE_MSC_WARNINGS_POP_()





#if !defined(GTEST_HAS_SEH)


#if defined(_MSC_VER) || defined(__BORLANDC__)

#define GTEST_HAS_SEH 1
#else

#define GTEST_HAS_SEH 0
#endif

#define GTEST_IS_THREADSAFE (GTEST_HAS_MUTEX_AND_THREAD_LOCAL_ || (GTEST_OS_WINDOWS && !GTEST_OS_WINDOWS_PHONE && !GTEST_OS_WINDOWS_RT) || GTEST_HAS_PTHREAD)




#endif




#if !defined(GTEST_API_)

#if defined(_MSC_VER)
#if GTEST_LINKED_AS_SHARED_LIBRARY
#define GTEST_API_ __declspec(dllimport)
#elif GTEST_CREATE_SHARED_LIBRARY
#define GTEST_API_ __declspec(dllexport)
#endif
#elif __GNUC__ >= 4 || defined(__clang__)
#define GTEST_API_ __attribute__((visibility ("default")))
#endif

#endif

#if !defined(GTEST_API_)
#define GTEST_API_
#endif

#if !defined(GTEST_DEFAULT_DEATH_TEST_STYLE)
#define GTEST_DEFAULT_DEATH_TEST_STYLE "fast"
#endif

#if defined(__GNUC__)

#define GTEST_NO_INLINE_ __attribute__((noinline))
#else
#define GTEST_NO_INLINE_
#endif


#if !defined(GTEST_HAS_CXXABI_H_)
#if defined(__GLIBCXX__) || (defined(_LIBCPP_VERSION) && !defined(_MSC_VER))
#define GTEST_HAS_CXXABI_H_ 1
#else
#define GTEST_HAS_CXXABI_H_ 0
#endif
#endif



#if defined(__clang__)
#if __has_feature(memory_sanitizer)
#define GTEST_ATTRIBUTE_NO_SANITIZE_MEMORY_ __attribute__((no_sanitize_memory))

#else
#define GTEST_ATTRIBUTE_NO_SANITIZE_MEMORY_
#endif
#else
#define GTEST_ATTRIBUTE_NO_SANITIZE_MEMORY_
#endif


#if defined(__clang__)
#if __has_feature(address_sanitizer)
#define GTEST_ATTRIBUTE_NO_SANITIZE_ADDRESS_ __attribute__((no_sanitize_address))

#else
#define GTEST_ATTRIBUTE_NO_SANITIZE_ADDRESS_
#endif
#else
#define GTEST_ATTRIBUTE_NO_SANITIZE_ADDRESS_
#endif


#if defined(__clang__)
#if __has_feature(thread_sanitizer)
#define GTEST_ATTRIBUTE_NO_SANITIZE_THREAD_ __attribute__((no_sanitize_thread))

#else
#define GTEST_ATTRIBUTE_NO_SANITIZE_THREAD_
#endif
#else
#define GTEST_ATTRIBUTE_NO_SANITIZE_THREAD_
#endif

namespace testing {

class Message;

#if defined(GTEST_TUPLE_NAMESPACE_)



using GTEST_TUPLE_NAMESPACE_::get;
using GTEST_TUPLE_NAMESPACE_::make_tuple;
using GTEST_TUPLE_NAMESPACE_::tuple;
using GTEST_TUPLE_NAMESPACE_::tuple_size;
using GTEST_TUPLE_NAMESPACE_::tuple_element;
#endif

namespace internal {




class Secret;
















#if GTEST_LANG_CXX11
#define GTEST_COMPILE_ASSERT_(expr, msg) static_assert(expr, #msg)
#else
template <bool>
struct CompileAssert {
};

#define GTEST_COMPILE_ASSERT_(expr, msg) typedef ::testing::internal::CompileAssert<(static_cast<bool>(expr))> msg[static_cast<bool>(expr) ? 1 : -1] GTEST_ATTRIBUTE_UNUSED_


#endif















































template <typename T1, typename T2>
struct StaticAssertTypeEqHelper;

template <typename T>
struct StaticAssertTypeEqHelper<T, T> {
enum { value = true };
};


template <typename T, typename U>
struct IsSame {
enum { value = false };
};
template <typename T>
struct IsSame<T, T> {
enum { value = true };
};


#define GTEST_ARRAY_SIZE_(array) (sizeof(array) / sizeof(array[0]))

#if GTEST_HAS_GLOBAL_STRING
typedef ::string string;
#else
typedef ::std::string string;
#endif

#if GTEST_HAS_GLOBAL_WSTRING
typedef ::wstring wstring;
#elif GTEST_HAS_STD_WSTRING
typedef ::std::wstring wstring;
#endif



GTEST_API_ bool IsTrue(bool condition);





template <typename T>
class scoped_ptr {
public:
typedef T element_type;

explicit scoped_ptr(T* p = NULL) : ptr_(p) {}
~scoped_ptr() { reset(); }

T& operator*() const { return *ptr_; }
T* operator->() const { return ptr_; }
T* get() const { return ptr_; }

T* release() {
T* const ptr = ptr_;
ptr_ = NULL;
return ptr;
}

void reset(T* p = NULL) {
if (p != ptr_) {
if (IsTrue(sizeof(T) > 0)) {
delete ptr_;
}
ptr_ = p;
}
}

friend void swap(scoped_ptr& a, scoped_ptr& b) {
using std::swap;
swap(a.ptr_, b.ptr_);
}

private:
T* ptr_;

GTEST_DISALLOW_COPY_AND_ASSIGN_(scoped_ptr);
};



#if GTEST_USES_PCRE

#elif GTEST_USES_POSIX_RE || GTEST_USES_SIMPLE_RE



class GTEST_API_ RE {
public:


RE(const RE& other) { Init(other.pattern()); }


RE(const ::std::string& regex) { Init(regex.c_str()); }

#if GTEST_HAS_GLOBAL_STRING

RE(const ::string& regex) { Init(regex.c_str()); }

#endif

RE(const char* regex) { Init(regex); }
~RE();


const char* pattern() const { return pattern_; }








static bool FullMatch(const ::std::string& str, const RE& re) {
return FullMatch(str.c_str(), re);
}
static bool PartialMatch(const ::std::string& str, const RE& re) {
return PartialMatch(str.c_str(), re);
}

#if GTEST_HAS_GLOBAL_STRING

static bool FullMatch(const ::string& str, const RE& re) {
return FullMatch(str.c_str(), re);
}
static bool PartialMatch(const ::string& str, const RE& re) {
return PartialMatch(str.c_str(), re);
}

#endif

static bool FullMatch(const char* str, const RE& re);
static bool PartialMatch(const char* str, const RE& re);

private:
void Init(const char* regex);




const char* pattern_;
bool is_valid_;

#if GTEST_USES_POSIX_RE

regex_t full_regex_;
regex_t partial_regex_;

#else

const char* full_pattern_;

#endif

GTEST_DISALLOW_ASSIGN_(RE);
};

#endif



GTEST_API_ ::std::string FormatFileLocation(const char* file, int line);




GTEST_API_ ::std::string FormatCompilerIndependentFileLocation(const char* file,
int line);







enum GTestLogSeverity {
GTEST_INFO,
GTEST_WARNING,
GTEST_ERROR,
GTEST_FATAL
};




class GTEST_API_ GTestLog {
public:
GTestLog(GTestLogSeverity severity, const char* file, int line);


~GTestLog();

::std::ostream& GetStream() { return ::std::cerr; }

private:
const GTestLogSeverity severity_;

GTEST_DISALLOW_COPY_AND_ASSIGN_(GTestLog);
};

#if !defined(GTEST_LOG_)

#define GTEST_LOG_(severity) ::testing::internal::GTestLog(::testing::internal::GTEST_##severity, __FILE__, __LINE__).GetStream()



inline void LogToStderr() {}
inline void FlushInfoLog() { fflush(NULL); }

#endif

#if !defined(GTEST_CHECK_)














#define GTEST_CHECK_(condition) GTEST_AMBIGUOUS_ELSE_BLOCKER_ if (::testing::internal::IsTrue(condition)) ; else GTEST_LOG_(FATAL) << "Condition " #condition " failed. "





#endif






#define GTEST_CHECK_POSIX_SUCCESS_(posix_call) if (const int gtest_error = (posix_call)) GTEST_LOG_(FATAL) << #posix_call << "failed with error " << gtest_error







template <typename T>
struct AddReference { typedef T& type; };
template <typename T>
struct AddReference<T&> { typedef T& type; };



#define GTEST_ADD_REFERENCE_(T) typename ::testing::internal::AddReference<T>::type













template <typename T>
struct ConstRef { typedef const T& type; };
template <typename T>
struct ConstRef<T&> { typedef T& type; };


#define GTEST_REFERENCE_TO_CONST_(T) typename ::testing::internal::ConstRef<T>::type


#if GTEST_HAS_STD_MOVE_
using std::forward;
using std::move;

template <typename T>
struct RvalueRef {
typedef T&& type;
};
#else
template <typename T>
const T& move(const T& t) {
return t;
}
template <typename T>
GTEST_ADD_REFERENCE_(T) forward(GTEST_ADD_REFERENCE_(T) t) { return t; }

template <typename T>
struct RvalueRef {
typedef const T& type;
};
#endif





















template<typename To>
inline To ImplicitCast_(To x) { return x; }






















template<typename To, typename From>
inline To DownCast_(From* f) {




GTEST_INTENTIONAL_CONST_COND_PUSH_()
if (false) {
GTEST_INTENTIONAL_CONST_COND_POP_()
const To to = NULL;
::testing::internal::ImplicitCast_<From*>(to);
}

#if GTEST_HAS_RTTI

GTEST_CHECK_(f == NULL || dynamic_cast<To>(f) != NULL);
#endif
return static_cast<To>(f);
}






template <class Derived, class Base>
Derived* CheckedDowncastToActualType(Base* base) {
#if GTEST_HAS_RTTI
GTEST_CHECK_(typeid(*base) == typeid(Derived));
#endif

#if GTEST_HAS_DOWNCAST_
return ::down_cast<Derived*>(base);
#elif GTEST_HAS_RTTI
return dynamic_cast<Derived*>(base);
#else
return static_cast<Derived*>(base);
#endif
}

#if GTEST_HAS_STREAM_REDIRECTION







GTEST_API_ void CaptureStdout();
GTEST_API_ std::string GetCapturedStdout();
GTEST_API_ void CaptureStderr();
GTEST_API_ std::string GetCapturedStderr();

#endif

GTEST_API_ size_t GetFileSize(FILE* file);


GTEST_API_ std::string ReadEntireFile(FILE* file);


GTEST_API_ std::vector<std::string> GetArgvs();

#if GTEST_HAS_DEATH_TEST

std::vector<std::string> GetInjectableArgvs();

void SetInjectableArgvs(const std::vector<std::string>* new_argvs);
void SetInjectableArgvs(const std::vector<std::string>& new_argvs);
#if GTEST_HAS_GLOBAL_STRING
void SetInjectableArgvs(const std::vector< ::string>& new_argvs);
#endif
void ClearInjectableArgvs();

#endif


#if GTEST_IS_THREADSAFE
#if GTEST_HAS_PTHREAD



inline void SleepMilliseconds(int n) {
const timespec time = {
0,
n * 1000L * 1000L,
};
nanosleep(&time, NULL);
}
#endif

#if GTEST_HAS_NOTIFICATION_



#elif GTEST_HAS_PTHREAD






class Notification {
public:
Notification() : notified_(false) {
GTEST_CHECK_POSIX_SUCCESS_(pthread_mutex_init(&mutex_, NULL));
}
~Notification() {
pthread_mutex_destroy(&mutex_);
}



void Notify() {
pthread_mutex_lock(&mutex_);
notified_ = true;
pthread_mutex_unlock(&mutex_);
}



void WaitForNotification() {
for (;;) {
pthread_mutex_lock(&mutex_);
const bool notified = notified_;
pthread_mutex_unlock(&mutex_);
if (notified)
break;
SleepMilliseconds(10);
}
}

private:
pthread_mutex_t mutex_;
bool notified_;

GTEST_DISALLOW_COPY_AND_ASSIGN_(Notification);
};

#elif GTEST_OS_WINDOWS && !GTEST_OS_WINDOWS_PHONE && !GTEST_OS_WINDOWS_RT

GTEST_API_ void SleepMilliseconds(int n);



class GTEST_API_ AutoHandle {
public:





typedef void* Handle;
AutoHandle();
explicit AutoHandle(Handle handle);

~AutoHandle();

Handle Get() const;
void Reset();
void Reset(Handle handle);

private:

bool IsCloseable() const;

Handle handle_;

GTEST_DISALLOW_COPY_AND_ASSIGN_(AutoHandle);
};







class GTEST_API_ Notification {
public:
Notification();
void Notify();
void WaitForNotification();

private:
AutoHandle event_;

GTEST_DISALLOW_COPY_AND_ASSIGN_(Notification);
};
#endif




#if GTEST_HAS_PTHREAD && !GTEST_OS_WINDOWS_MINGW






class ThreadWithParamBase {
public:
virtual ~ThreadWithParamBase() {}
virtual void Run() = 0;
};







extern "C" inline void* ThreadFuncWithCLinkage(void* thread) {
static_cast<ThreadWithParamBase*>(thread)->Run();
return NULL;
}













template <typename T>
class ThreadWithParam : public ThreadWithParamBase {
public:
typedef void UserThreadFunc(T);

ThreadWithParam(UserThreadFunc* func, T param, Notification* thread_can_start)
: func_(func),
param_(param),
thread_can_start_(thread_can_start),
finished_(false) {
ThreadWithParamBase* const base = this;


GTEST_CHECK_POSIX_SUCCESS_(
pthread_create(&thread_, 0, &ThreadFuncWithCLinkage, base));
}
~ThreadWithParam() { Join(); }

void Join() {
if (!finished_) {
GTEST_CHECK_POSIX_SUCCESS_(pthread_join(thread_, 0));
finished_ = true;
}
}

virtual void Run() {
if (thread_can_start_ != NULL)
thread_can_start_->WaitForNotification();
func_(param_);
}

private:
UserThreadFunc* const func_;
const T param_;


Notification* const thread_can_start_;
bool finished_;
pthread_t thread_;

GTEST_DISALLOW_COPY_AND_ASSIGN_(ThreadWithParam);
};
#endif


#if GTEST_HAS_MUTEX_AND_THREAD_LOCAL_



#elif GTEST_OS_WINDOWS && !GTEST_OS_WINDOWS_PHONE && !GTEST_OS_WINDOWS_RT















class GTEST_API_ Mutex {
public:
enum MutexType { kStatic = 0, kDynamic = 1 };



enum StaticConstructorSelector { kStaticMutex = 0 };




explicit Mutex(StaticConstructorSelector ) {}

Mutex();
~Mutex();

void Lock();

void Unlock();



void AssertHeld();

private:

void ThreadSafeLazyInit();



unsigned int owner_thread_id_;



MutexType type_;
long critical_section_init_phase_;
GTEST_CRITICAL_SECTION* critical_section_;

GTEST_DISALLOW_COPY_AND_ASSIGN_(Mutex);
};

#define GTEST_DECLARE_STATIC_MUTEX_(mutex) extern ::testing::internal::Mutex mutex


#define GTEST_DEFINE_STATIC_MUTEX_(mutex) ::testing::internal::Mutex mutex(::testing::internal::Mutex::kStaticMutex)







class GTestMutexLock {
public:
explicit GTestMutexLock(Mutex* mutex)
: mutex_(mutex) { mutex_->Lock(); }

~GTestMutexLock() { mutex_->Unlock(); }

private:
Mutex* const mutex_;

GTEST_DISALLOW_COPY_AND_ASSIGN_(GTestMutexLock);
};

typedef GTestMutexLock MutexLock;



class ThreadLocalValueHolderBase {
public:
virtual ~ThreadLocalValueHolderBase() {}
};



class ThreadLocalBase {
public:




virtual ThreadLocalValueHolderBase* NewValueForCurrentThread() const = 0;

protected:
ThreadLocalBase() {}
virtual ~ThreadLocalBase() {}

private:
GTEST_DISALLOW_COPY_AND_ASSIGN_(ThreadLocalBase);
};




class GTEST_API_ ThreadLocalRegistry {
public:


static ThreadLocalValueHolderBase* GetValueOnCurrentThread(
const ThreadLocalBase* thread_local_instance);


static void OnThreadLocalDestroyed(
const ThreadLocalBase* thread_local_instance);
};

class GTEST_API_ ThreadWithParamBase {
public:
void Join();

protected:
class Runnable {
public:
virtual ~Runnable() {}
virtual void Run() = 0;
};

ThreadWithParamBase(Runnable *runnable, Notification* thread_can_start);
virtual ~ThreadWithParamBase();

private:
AutoHandle thread_;
};


template <typename T>
class ThreadWithParam : public ThreadWithParamBase {
public:
typedef void UserThreadFunc(T);

ThreadWithParam(UserThreadFunc* func, T param, Notification* thread_can_start)
: ThreadWithParamBase(new RunnableImpl(func, param), thread_can_start) {
}
virtual ~ThreadWithParam() {}

private:
class RunnableImpl : public Runnable {
public:
RunnableImpl(UserThreadFunc* func, T param)
: func_(func),
param_(param) {
}
virtual ~RunnableImpl() {}
virtual void Run() {
func_(param_);
}

private:
UserThreadFunc* const func_;
const T param_;

GTEST_DISALLOW_COPY_AND_ASSIGN_(RunnableImpl);
};

GTEST_DISALLOW_COPY_AND_ASSIGN_(ThreadWithParam);
};




























template <typename T>
class ThreadLocal : public ThreadLocalBase {
public:
ThreadLocal() : default_factory_(new DefaultValueHolderFactory()) {}
explicit ThreadLocal(const T& value)
: default_factory_(new InstanceValueHolderFactory(value)) {}

~ThreadLocal() { ThreadLocalRegistry::OnThreadLocalDestroyed(this); }

T* pointer() { return GetOrCreateValue(); }
const T* pointer() const { return GetOrCreateValue(); }
const T& get() const { return *pointer(); }
void set(const T& value) { *pointer() = value; }

private:


class ValueHolder : public ThreadLocalValueHolderBase {
public:
ValueHolder() : value_() {}
explicit ValueHolder(const T& value) : value_(value) {}

T* pointer() { return &value_; }

private:
T value_;
GTEST_DISALLOW_COPY_AND_ASSIGN_(ValueHolder);
};


T* GetOrCreateValue() const {
return static_cast<ValueHolder*>(
ThreadLocalRegistry::GetValueOnCurrentThread(this))->pointer();
}

virtual ThreadLocalValueHolderBase* NewValueForCurrentThread() const {
return default_factory_->MakeNewHolder();
}

class ValueHolderFactory {
public:
ValueHolderFactory() {}
virtual ~ValueHolderFactory() {}
virtual ValueHolder* MakeNewHolder() const = 0;

private:
GTEST_DISALLOW_COPY_AND_ASSIGN_(ValueHolderFactory);
};

class DefaultValueHolderFactory : public ValueHolderFactory {
public:
DefaultValueHolderFactory() {}
virtual ValueHolder* MakeNewHolder() const { return new ValueHolder(); }

private:
GTEST_DISALLOW_COPY_AND_ASSIGN_(DefaultValueHolderFactory);
};

class InstanceValueHolderFactory : public ValueHolderFactory {
public:
explicit InstanceValueHolderFactory(const T& value) : value_(value) {}
virtual ValueHolder* MakeNewHolder() const {
return new ValueHolder(value_);
}

private:
const T value_;

GTEST_DISALLOW_COPY_AND_ASSIGN_(InstanceValueHolderFactory);
};

scoped_ptr<ValueHolderFactory> default_factory_;

GTEST_DISALLOW_COPY_AND_ASSIGN_(ThreadLocal);
};

#elif GTEST_HAS_PTHREAD


class MutexBase {
public:

void Lock() {
GTEST_CHECK_POSIX_SUCCESS_(pthread_mutex_lock(&mutex_));
owner_ = pthread_self();
has_owner_ = true;
}


void Unlock() {




has_owner_ = false;
GTEST_CHECK_POSIX_SUCCESS_(pthread_mutex_unlock(&mutex_));
}



void AssertHeld() const {
GTEST_CHECK_(has_owner_ && pthread_equal(owner_, pthread_self()))
<< "The current thread is not holding the mutex @" << this;
}






public:
pthread_mutex_t mutex_;






bool has_owner_;
pthread_t owner_;
};


#define GTEST_DECLARE_STATIC_MUTEX_(mutex) extern ::testing::internal::MutexBase mutex








#define GTEST_DEFINE_STATIC_MUTEX_(mutex) ::testing::internal::MutexBase mutex = {PTHREAD_MUTEX_INITIALIZER, false, 0}




class Mutex : public MutexBase {
public:
Mutex() {
GTEST_CHECK_POSIX_SUCCESS_(pthread_mutex_init(&mutex_, NULL));
has_owner_ = false;
}
~Mutex() {
GTEST_CHECK_POSIX_SUCCESS_(pthread_mutex_destroy(&mutex_));
}

private:
GTEST_DISALLOW_COPY_AND_ASSIGN_(Mutex);
};






class GTestMutexLock {
public:
explicit GTestMutexLock(MutexBase* mutex)
: mutex_(mutex) { mutex_->Lock(); }

~GTestMutexLock() { mutex_->Unlock(); }

private:
MutexBase* const mutex_;

GTEST_DISALLOW_COPY_AND_ASSIGN_(GTestMutexLock);
};

typedef GTestMutexLock MutexLock;







class ThreadLocalValueHolderBase {
public:
virtual ~ThreadLocalValueHolderBase() {}
};



extern "C" inline void DeleteThreadLocalValue(void* value_holder) {
delete static_cast<ThreadLocalValueHolderBase*>(value_holder);
}


template <typename T>
class GTEST_API_ ThreadLocal {
public:
ThreadLocal()
: key_(CreateKey()), default_factory_(new DefaultValueHolderFactory()) {}
explicit ThreadLocal(const T& value)
: key_(CreateKey()),
default_factory_(new InstanceValueHolderFactory(value)) {}

~ThreadLocal() {

DeleteThreadLocalValue(pthread_getspecific(key_));



GTEST_CHECK_POSIX_SUCCESS_(pthread_key_delete(key_));
}

T* pointer() { return GetOrCreateValue(); }
const T* pointer() const { return GetOrCreateValue(); }
const T& get() const { return *pointer(); }
void set(const T& value) { *pointer() = value; }

private:

class ValueHolder : public ThreadLocalValueHolderBase {
public:
ValueHolder() : value_() {}
explicit ValueHolder(const T& value) : value_(value) {}

T* pointer() { return &value_; }

private:
T value_;
GTEST_DISALLOW_COPY_AND_ASSIGN_(ValueHolder);
};

static pthread_key_t CreateKey() {
pthread_key_t key;


GTEST_CHECK_POSIX_SUCCESS_(
pthread_key_create(&key, &DeleteThreadLocalValue));
return key;
}

T* GetOrCreateValue() const {
ThreadLocalValueHolderBase* const holder =
static_cast<ThreadLocalValueHolderBase*>(pthread_getspecific(key_));
if (holder != NULL) {
return CheckedDowncastToActualType<ValueHolder>(holder)->pointer();
}

ValueHolder* const new_holder = default_factory_->MakeNewHolder();
ThreadLocalValueHolderBase* const holder_base = new_holder;
GTEST_CHECK_POSIX_SUCCESS_(pthread_setspecific(key_, holder_base));
return new_holder->pointer();
}

class ValueHolderFactory {
public:
ValueHolderFactory() {}
virtual ~ValueHolderFactory() {}
virtual ValueHolder* MakeNewHolder() const = 0;

private:
GTEST_DISALLOW_COPY_AND_ASSIGN_(ValueHolderFactory);
};

class DefaultValueHolderFactory : public ValueHolderFactory {
public:
DefaultValueHolderFactory() {}
virtual ValueHolder* MakeNewHolder() const { return new ValueHolder(); }

private:
GTEST_DISALLOW_COPY_AND_ASSIGN_(DefaultValueHolderFactory);
};

class InstanceValueHolderFactory : public ValueHolderFactory {
public:
explicit InstanceValueHolderFactory(const T& value) : value_(value) {}
virtual ValueHolder* MakeNewHolder() const {
return new ValueHolder(value_);
}

private:
const T value_;

GTEST_DISALLOW_COPY_AND_ASSIGN_(InstanceValueHolderFactory);
};


const pthread_key_t key_;
scoped_ptr<ValueHolderFactory> default_factory_;

GTEST_DISALLOW_COPY_AND_ASSIGN_(ThreadLocal);
};

#endif

#else






class Mutex {
public:
Mutex() {}
void Lock() {}
void Unlock() {}
void AssertHeld() const {}
};

#define GTEST_DECLARE_STATIC_MUTEX_(mutex) extern ::testing::internal::Mutex mutex


#define GTEST_DEFINE_STATIC_MUTEX_(mutex) ::testing::internal::Mutex mutex






class GTestMutexLock {
public:
explicit GTestMutexLock(Mutex*) {}
};

typedef GTestMutexLock MutexLock;

template <typename T>
class GTEST_API_ ThreadLocal {
public:
ThreadLocal() : value_() {}
explicit ThreadLocal(const T& value) : value_(value) {}
T* pointer() { return &value_; }
const T* pointer() const { return &value_; }
const T& get() const { return value_; }
void set(const T& value) { value_ = value; }
private:
T value_;
};

#endif



GTEST_API_ size_t GetThreadCount();







#if defined(__SYMBIAN32__) || defined(__IBMCPP__) || (defined(__SUNPRO_CC) && __SUNPRO_CC < 0x5130)



#define GTEST_ELLIPSIS_NEEDS_POD_ 1
#else
#define GTEST_CAN_COMPARE_NULL 1
#endif





#if defined(__SYMBIAN32__) || defined(__IBMCPP__)
#define GTEST_NEEDS_IS_POINTER_ 1
#endif

template <bool bool_value>
struct bool_constant {
typedef bool_constant<bool_value> type;
static const bool value = bool_value;
};
template <bool bool_value> const bool bool_constant<bool_value>::value;

typedef bool_constant<false> false_type;
typedef bool_constant<true> true_type;

template <typename T, typename U>
struct is_same : public false_type {};

template <typename T>
struct is_same<T, T> : public true_type {};


template <typename T>
struct is_pointer : public false_type {};

template <typename T>
struct is_pointer<T*> : public true_type {};

template <typename Iterator>
struct IteratorTraits {
typedef typename Iterator::value_type value_type;
};


template <typename T>
struct IteratorTraits<T*> {
typedef T value_type;
};

template <typename T>
struct IteratorTraits<const T*> {
typedef T value_type;
};

#if GTEST_OS_WINDOWS
#define GTEST_PATH_SEP_ "\\"
#define GTEST_HAS_ALT_PATH_SEP_ 1

typedef __int64 BiggestInt;
#else
#define GTEST_PATH_SEP_ "/"
#define GTEST_HAS_ALT_PATH_SEP_ 0
typedef long long BiggestInt;
#endif








inline bool IsAlpha(char ch) {
return isalpha(static_cast<unsigned char>(ch)) != 0;
}
inline bool IsAlNum(char ch) {
return isalnum(static_cast<unsigned char>(ch)) != 0;
}
inline bool IsDigit(char ch) {
return isdigit(static_cast<unsigned char>(ch)) != 0;
}
inline bool IsLower(char ch) {
return islower(static_cast<unsigned char>(ch)) != 0;
}
inline bool IsSpace(char ch) {
return isspace(static_cast<unsigned char>(ch)) != 0;
}
inline bool IsUpper(char ch) {
return isupper(static_cast<unsigned char>(ch)) != 0;
}
inline bool IsXDigit(char ch) {
return isxdigit(static_cast<unsigned char>(ch)) != 0;
}
inline bool IsXDigit(wchar_t ch) {
const unsigned char low_byte = static_cast<unsigned char>(ch);
return ch == low_byte && isxdigit(low_byte) != 0;
}

inline char ToLower(char ch) {
return static_cast<char>(tolower(static_cast<unsigned char>(ch)));
}
inline char ToUpper(char ch) {
return static_cast<char>(toupper(static_cast<unsigned char>(ch)));
}

inline std::string StripTrailingSpaces(std::string str) {
std::string::iterator it = str.end();
while (it != str.begin() && IsSpace(*--it))
it = str.erase(it);
return str;
}







namespace posix {



#if GTEST_OS_WINDOWS

typedef struct _stat StatStruct;

#if defined(__BORLANDC__)
inline int IsATTY(int fd) { return isatty(fd); }
inline int StrCaseCmp(const char* s1, const char* s2) {
return stricmp(s1, s2);
}
inline char* StrDup(const char* src) { return strdup(src); }
#else
#if GTEST_OS_WINDOWS_MOBILE
inline int IsATTY(int ) { return 0; }
#else
inline int IsATTY(int fd) { return _isatty(fd); }
#endif
inline int StrCaseCmp(const char* s1, const char* s2) {
return _stricmp(s1, s2);
}
inline char* StrDup(const char* src) { return _strdup(src); }
#endif

#if GTEST_OS_WINDOWS_MOBILE
inline int FileNo(FILE* file) { return reinterpret_cast<int>(_fileno(file)); }


#else
inline int FileNo(FILE* file) { return _fileno(file); }
inline int Stat(const char* path, StatStruct* buf) { return _stat(path, buf); }
inline int RmDir(const char* dir) { return _rmdir(dir); }
inline bool IsDir(const StatStruct& st) {
return (_S_IFDIR & st.st_mode) != 0;
}
#endif

#else

typedef struct stat StatStruct;

inline int FileNo(FILE* file) { return fileno(file); }
inline int IsATTY(int fd) { return isatty(fd); }
inline int Stat(const char* path, StatStruct* buf) { return stat(path, buf); }
inline int StrCaseCmp(const char* s1, const char* s2) {
return strcasecmp(s1, s2);
}
inline char* StrDup(const char* src) { return strdup(src); }
inline int RmDir(const char* dir) { return rmdir(dir); }
inline bool IsDir(const StatStruct& st) { return S_ISDIR(st.st_mode); }

#endif



GTEST_DISABLE_MSC_DEPRECATED_PUSH_()

inline const char* StrNCpy(char* dest, const char* src, size_t n) {
return strncpy(dest, src, n);
}





#if !GTEST_OS_WINDOWS_MOBILE && !GTEST_OS_WINDOWS_PHONE && !GTEST_OS_WINDOWS_RT
inline int ChDir(const char* dir) { return chdir(dir); }
#endif
inline FILE* FOpen(const char* path, const char* mode) {
return fopen(path, mode);
}
#if !GTEST_OS_WINDOWS_MOBILE
inline FILE *FReopen(const char* path, const char* mode, FILE* stream) {
return freopen(path, mode, stream);
}
inline FILE* FDOpen(int fd, const char* mode) { return fdopen(fd, mode); }
#endif
inline int FClose(FILE* fp) { return fclose(fp); }
#if !GTEST_OS_WINDOWS_MOBILE
inline int Read(int fd, void* buf, unsigned int count) {
return static_cast<int>(read(fd, buf, count));
}
inline int Write(int fd, const void* buf, unsigned int count) {
return static_cast<int>(write(fd, buf, count));
}
inline int Close(int fd) { return close(fd); }
inline const char* StrError(int errnum) { return strerror(errnum); }
#endif
inline const char* GetEnv(const char* name) {
#if GTEST_OS_WINDOWS_MOBILE || GTEST_OS_WINDOWS_PHONE || GTEST_OS_WINDOWS_RT

static_cast<void>(name);
return NULL;
#elif defined(__BORLANDC__) || defined(__SunOS_5_8) || defined(__SunOS_5_9)


const char* const env = getenv(name);
return (env != NULL && env[0] != '\0') ? env : NULL;
#else
return getenv(name);
#endif
}

GTEST_DISABLE_MSC_DEPRECATED_POP_()

#if GTEST_OS_WINDOWS_MOBILE



void Abort();
#else
inline void Abort() { abort(); }
#endif

}






#if _MSC_VER >= 1400 && !GTEST_OS_WINDOWS_MOBILE

#define GTEST_SNPRINTF_(buffer, size, format, ...) _snprintf_s(buffer, size, size, format, __VA_ARGS__)

#elif defined(_MSC_VER)


#define GTEST_SNPRINTF_ _snprintf
#else
#define GTEST_SNPRINTF_ snprintf
#endif








const BiggestInt kMaxBiggestInt =
~(static_cast<BiggestInt>(1) << (8*sizeof(BiggestInt) - 1));



















template <size_t size>
class TypeWithSize {
public:


typedef void UInt;
};


template <>
class TypeWithSize<4> {
public:




typedef int Int;
typedef unsigned int UInt;
};


template <>
class TypeWithSize<8> {
public:
#if GTEST_OS_WINDOWS
typedef __int64 Int;
typedef unsigned __int64 UInt;
#else
typedef long long Int;
typedef unsigned long long UInt;
#endif
};


typedef TypeWithSize<4>::Int Int32;
typedef TypeWithSize<4>::UInt UInt32;
typedef TypeWithSize<8>::Int Int64;
typedef TypeWithSize<8>::UInt UInt64;
typedef TypeWithSize<8>::Int TimeInMillis;




#if !defined(GTEST_FLAG)
#define GTEST_FLAG(name) FLAGS_gtest_##name
#endif

#if !defined(GTEST_USE_OWN_FLAGFILE_FLAG_)
#define GTEST_USE_OWN_FLAGFILE_FLAG_ 1
#endif

#if !defined(GTEST_DECLARE_bool_)
#define GTEST_FLAG_SAVER_ ::testing::internal::GTestFlagSaver


#define GTEST_DECLARE_bool_(name) GTEST_API_ extern bool GTEST_FLAG(name)
#define GTEST_DECLARE_int32_(name) GTEST_API_ extern ::testing::internal::Int32 GTEST_FLAG(name)

#define GTEST_DECLARE_string_(name) GTEST_API_ extern ::std::string GTEST_FLAG(name)



#define GTEST_DEFINE_bool_(name, default_val, doc) GTEST_API_ bool GTEST_FLAG(name) = (default_val)

#define GTEST_DEFINE_int32_(name, default_val, doc) GTEST_API_ ::testing::internal::Int32 GTEST_FLAG(name) = (default_val)

#define GTEST_DEFINE_string_(name, default_val, doc) GTEST_API_ ::std::string GTEST_FLAG(name) = (default_val)


#endif


#if !defined(GTEST_EXCLUSIVE_LOCK_REQUIRED_)
#define GTEST_EXCLUSIVE_LOCK_REQUIRED_(locks)
#define GTEST_LOCK_EXCLUDED_(locks)
#endif







bool ParseInt32(const Message& src_text, const char* str, Int32* value);



bool BoolFromGTestEnv(const char* flag, bool default_val);
GTEST_API_ Int32 Int32FromGTestEnv(const char* flag, Int32 default_val);
std::string OutputFlagAlsoCheckEnvVar();
const char* StringFromGTestEnv(const char* flag, const char* default_val);

}
}

#endif