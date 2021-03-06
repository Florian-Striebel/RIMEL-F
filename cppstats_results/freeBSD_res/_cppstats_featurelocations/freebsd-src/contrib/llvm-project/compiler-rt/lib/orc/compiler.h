













#if !defined(ORC_RT_COMPILER_H)
#define ORC_RT_COMPILER_H

#define ORC_RT_INTERFACE extern "C" __attribute__((visibility("default")))
#define ORC_RT_HIDDEN __attribute__((visibility("hidden")))

#if !defined(__has_builtin)
#define __has_builtin(x) 0
#endif



#if !defined(ORC_RT_HAS_CPP_ATTRIBUTE)
#if defined(__cplusplus) && defined(__has_cpp_attribute)
#define ORC_RT_HAS_CPP_ATTRIBUTE(x) __has_cpp_attribute(x)
#else
#define ORC_RT_HAS_CPP_ATTRIBUTE(x) 0
#endif
#endif


#if defined(__cplusplus) && __cplusplus > 201402L && ORC_RT_HAS_CPP_ATTRIBUTE(nodiscard)

#define ORC_RT_NODISCARD [[nodiscard]]
#elif ORC_RT_HAS_CPP_ATTRIBUTE(clang::warn_unused_result)
#define ORC_RT_NODISCARD [[clang::warn_unused_result]]




#elif defined(__GNUC__) && ORC_RT_HAS_CPP_ATTRIBUTE(nodiscard)
#define ORC_RT_NODISCARD [[nodiscard]]
#else
#define ORC_RT_NODISCARD
#endif

#if __has_builtin(__builtin_expect)
#define ORC_RT_LIKELY(EXPR) __builtin_expect((bool)(EXPR), true)
#define ORC_RT_UNLIKELY(EXPR) __builtin_expect((bool)(EXPR), false)
#else
#define ORC_RT_LIKELY(EXPR) (EXPR)
#define ORC_RT_UNLIKELY(EXPR) (EXPR)
#endif

#if defined(__APPLE__)
#define ORC_RT_WEAK_IMPORT __attribute__((weak_import))
#else
#define ORC_RT_WEAK_IMPORT __attribute__((weak))
#endif

#endif
