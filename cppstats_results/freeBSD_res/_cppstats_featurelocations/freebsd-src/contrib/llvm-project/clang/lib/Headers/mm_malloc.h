








#if !defined(__MM_MALLOC_H)
#define __MM_MALLOC_H

#include <stdlib.h>

#if defined(_WIN32)
#include <malloc.h>
#else
#if !defined(__cplusplus)
extern int posix_memalign(void **__memptr, size_t __alignment, size_t __size);
#else




extern "C" int posix_memalign(void **__memptr, size_t __alignment, size_t __size);
#endif
#endif

#if !(defined(_WIN32) && defined(_mm_malloc))
static __inline__ void *__attribute__((__always_inline__, __nodebug__,
__malloc__))
_mm_malloc(size_t __size, size_t __align)
{
if (__align == 1) {
return malloc(__size);
}

if (!(__align & (__align - 1)) && __align < sizeof(void *))
__align = sizeof(void *);

void *__mallocedMemory;
#if defined(__MINGW32__)
__mallocedMemory = __mingw_aligned_malloc(__size, __align);
#elif defined(_WIN32)
__mallocedMemory = _aligned_malloc(__size, __align);
#else
if (posix_memalign(&__mallocedMemory, __align, __size))
return 0;
#endif

return __mallocedMemory;
}

static __inline__ void __attribute__((__always_inline__, __nodebug__))
_mm_free(void *__p)
{
#if defined(__MINGW32__)
__mingw_aligned_free(__p);
#elif defined(_WIN32)
_aligned_free(__p);
#else
free(__p);
#endif
}
#endif

#endif
