








#if !defined(__S390INTRIN_H)
#define __S390INTRIN_H

#if !defined(__s390__)
#error "<s390intrin.h> is for s390 only"
#endif

#if defined(__HTM__)
#include <htmintrin.h>
#endif

#if defined(__VEC__)
#include <vecintrin.h>
#endif

#endif
