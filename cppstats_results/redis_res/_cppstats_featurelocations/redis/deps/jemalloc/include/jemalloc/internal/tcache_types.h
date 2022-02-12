#if !defined(JEMALLOC_INTERNAL_TCACHE_TYPES_H)
#define JEMALLOC_INTERNAL_TCACHE_TYPES_H

#include "jemalloc/internal/sc.h"

typedef struct tcache_s tcache_t;
typedef struct tcaches_s tcaches_t;






#define TCACHE_STATE_DISABLED ((tcache_t *)(uintptr_t)1)
#define TCACHE_STATE_REINCARNATED ((tcache_t *)(uintptr_t)2)
#define TCACHE_STATE_PURGATORY ((tcache_t *)(uintptr_t)3)
#define TCACHE_STATE_MAX TCACHE_STATE_PURGATORY




#define TCACHE_NSLOTS_SMALL_MIN 20








#define TCACHE_NSLOTS_SMALL_MAX 200


#define TCACHE_NSLOTS_LARGE 20


#define LG_TCACHE_MAXCLASS_DEFAULT 15






#define TCACHE_GC_SWEEP 8192


#define TCACHE_GC_INCR ((TCACHE_GC_SWEEP / SC_NBINS) + ((TCACHE_GC_SWEEP / SC_NBINS == 0) ? 0 : 1))



#define TCACHE_ZERO_INITIALIZER {0}


#define TCACHE_ENABLED_ZERO_INITIALIZER false


#define TCACHES_ELM_NEED_REINIT ((tcache_t *)(uintptr_t)1)

#endif
