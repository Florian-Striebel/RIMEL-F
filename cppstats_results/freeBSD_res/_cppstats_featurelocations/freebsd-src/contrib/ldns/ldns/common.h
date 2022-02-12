













#if !defined(LDNS_COMMON_H)
#define LDNS_COMMON_H





#define LDNS_BUILD_CONFIG_HAVE_SSL 1
#define LDNS_BUILD_CONFIG_HAVE_INTTYPES_H 1
#define LDNS_BUILD_CONFIG_HAVE_ATTR_FORMAT 1
#define LDNS_BUILD_CONFIG_HAVE_ATTR_UNUSED 1
#define LDNS_BUILD_CONFIG_HAVE_SOCKLEN_T 1
#define LDNS_BUILD_CONFIG_USE_DANE 1
#define LDNS_BUILD_CONFIG_HAVE_B32_PTON 0
#define LDNS_BUILD_CONFIG_HAVE_B32_NTOP 0










#if !defined(__bool_true_false_are_defined)
#if defined(HAVE_STDBOOL_H)
#include <stdbool.h>
#else
#if !defined(HAVE__BOOL)
#if defined(__cplusplus)
typedef bool _Bool;
#else
#define _Bool signed char
#endif
#endif
#define bool _Bool
#define false 0
#define true 1
#define __bool_true_false_are_defined 1
#endif
#endif


#if LDNS_BUILD_CONFIG_HAVE_ATTR_FORMAT
#define ATTR_FORMAT(archetype, string_index, first_to_check) __attribute__ ((format (archetype, string_index, first_to_check)))

#else
#define ATTR_FORMAT(archetype, string_index, first_to_check)
#endif

#if defined(__cplusplus)
#define ATTR_UNUSED(x)
#elif LDNS_BUILD_CONFIG_HAVE_ATTR_UNUSED
#define ATTR_UNUSED(x) x __attribute__((unused))
#else
#define ATTR_UNUSED(x) x
#endif

#if !LDNS_BUILD_CONFIG_HAVE_SOCKLEN_T
typedef int socklen_t;
#endif

#endif
