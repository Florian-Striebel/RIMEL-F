












#if SANITIZER_MAC

#if !defined(INCLUDED_FROM_INTERCEPTION_LIB)
#error "interception_mac.h should be included from interception.h only"
#endif

#if !defined(INTERCEPTION_MAC_H)
#define INTERCEPTION_MAC_H

#define INTERCEPT_FUNCTION_MAC(func)
#define INTERCEPT_FUNCTION_VER_MAC(func, symver)

#endif
#endif
