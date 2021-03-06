








#if !defined(_MM_MALLOC_H_INCLUDED)
#define _MM_MALLOC_H_INCLUDED

#if defined(__linux__) && defined(__ppc64__)

#include <stdlib.h>



#if !defined(__cplusplus)
extern int posix_memalign (void **, size_t, size_t);
#else
extern "C" int posix_memalign (void **, size_t, size_t) throw ();
#endif

static __inline void *
_mm_malloc (size_t size, size_t alignment)
{

size_t vec_align = sizeof (__vector float);
void *ptr;

if (alignment < vec_align)
alignment = vec_align;
if (posix_memalign (&ptr, alignment, size) == 0)
return ptr;
else
return NULL;
}

static __inline void
_mm_free (void * ptr)
{
free (ptr);
}

#else
#include_next <mm_malloc.h>
#endif

#endif
