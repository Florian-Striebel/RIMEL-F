#undef assert
#undef _assert
#if defined(NDEBUG)
#define assert(e) ((void)0)
#define _assert(e) ((void)0)
#else
#define _assert(e) assert(e)
#define assert(e) ((e) ? (void)0 : __assert(#e, __FILE__, __LINE__))
#endif
#if !defined(_ASSERT_H_)
#define _ASSERT_H_
#include <stdio.h>
#include <stdlib.h>
#if defined(__cplusplus)
extern "C" {
#endif
static __inline void
__assert(const char *expr, const char *file, int line)
{
(void)fprintf(stderr, "Assertion failed: (%s), file %s, line %d.\n",
expr, file, line);
abort();
}
#if defined(__cplusplus)
}
#endif
#endif
