#if !defined(__CAPSICUM_RIGHTS_H__)
#define __CAPSICUM_RIGHTS_H__

#if defined(__cplusplus)
extern "C" {
#endif

#if defined(__FreeBSD__)
#include <sys/param.h>
#if __FreeBSD_version >= 1100014 || (__FreeBSD_version >= 1001511 && __FreeBSD_version < 1100000)

#include <sys/capsicum.h>
#else
#include <sys/capability.h>
#endif
#endif

#if defined(__linux__)
#include <linux/capsicum.h>
#endif

#if defined(__cplusplus)
}
#endif

#if !defined(CAP_RIGHTS_VERSION)





#include <stdarg.h>
#include <stdbool.h>





#define cap_rights_init(rights, ...) _cap_rights_init((rights), __VA_ARGS__, 0ULL)
#define cap_rights_set(rights, ...) _cap_rights_set((rights), __VA_ARGS__, 0ULL)
#define cap_rights_clear(rights, ...) _cap_rights_clear((rights), __VA_ARGS__, 0ULL)
#define cap_rights_is_set(rights, ...) _cap_rights_is_set((rights), __VA_ARGS__, 0ULL)

inline cap_rights_t* _cap_rights_init(cap_rights_t *rights, ...) {
va_list ap;
cap_rights_t right;
*rights = 0;
va_start(ap, rights);
while (true) {
right = va_arg(ap, cap_rights_t);
*rights |= right;
if (right == 0) break;
}
va_end(ap);
return rights;
}

inline cap_rights_t* _cap_rights_set(cap_rights_t *rights, ...) {
va_list ap;
cap_rights_t right;
va_start(ap, rights);
while (true) {
right = va_arg(ap, cap_rights_t);
*rights |= right;
if (right == 0) break;
}
va_end(ap);
return rights;
}

inline cap_rights_t* _cap_rights_clear(cap_rights_t *rights, ...) {
va_list ap;
cap_rights_t right;
va_start(ap, rights);
while (true) {
right = va_arg(ap, cap_rights_t);
*rights &= ~right;
if (right == 0) break;
}
va_end(ap);
return rights;
}

inline bool _cap_rights_is_set(const cap_rights_t *rights, ...) {
va_list ap;
cap_rights_t right;
cap_rights_t accumulated = 0;
va_start(ap, rights);
while (true) {
right = va_arg(ap, cap_rights_t);
accumulated |= right;
if (right == 0) break;
}
va_end(ap);
return (accumulated & *rights) == accumulated;
}

inline bool _cap_rights_is_valid(const cap_rights_t *rights) {
return true;
}

inline cap_rights_t* cap_rights_merge(cap_rights_t *dst, const cap_rights_t *src) {
*dst |= *src;
return dst;
}

inline cap_rights_t* cap_rights_remove(cap_rights_t *dst, const cap_rights_t *src) {
*dst &= ~(*src);
return dst;
}

inline bool cap_rights_contains(const cap_rights_t *big, const cap_rights_t *little) {
return ((*big) & (*little)) == (*little);
}

#endif

#endif
