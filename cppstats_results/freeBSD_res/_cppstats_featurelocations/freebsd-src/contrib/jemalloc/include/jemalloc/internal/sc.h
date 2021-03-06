#if !defined(JEMALLOC_INTERNAL_SC_H)
#define JEMALLOC_INTERNAL_SC_H

#include "jemalloc/internal/jemalloc_internal_types.h"






































































































































































#define SC_LG_NGROUP 2
#define SC_LG_TINY_MIN 3

#if SC_LG_TINY_MIN == 0

#error "Unsupported LG_TINY_MIN"
#endif





#define SC_NGROUP (1ULL << SC_LG_NGROUP)
#define SC_PTR_BITS ((1ULL << LG_SIZEOF_PTR) * 8)
#define SC_NTINY (LG_QUANTUM - SC_LG_TINY_MIN)
#define SC_LG_TINY_MAXCLASS (LG_QUANTUM > SC_LG_TINY_MIN ? LG_QUANTUM - 1 : -1)
#define SC_NPSEUDO SC_NGROUP
#define SC_LG_FIRST_REGULAR_BASE (LG_QUANTUM + SC_LG_NGROUP)






#define SC_LG_BASE_MAX (SC_PTR_BITS - 2)
#define SC_NREGULAR (SC_NGROUP * (SC_LG_BASE_MAX - SC_LG_FIRST_REGULAR_BASE + 1) - 1)

#define SC_NSIZES (SC_NTINY + SC_NPSEUDO + SC_NREGULAR)


#define SC_NPSIZES ( SC_NSIZES - (LG_PAGE - 1 - SC_LG_FIRST_REGULAR_BASE) * SC_NGROUP - SC_NPSEUDO - SC_NTINY - (SC_LG_NGROUP * SC_NGROUP))



























#define SC_NBINS ( SC_NTINY + SC_NPSEUDO + SC_NGROUP * (LG_PAGE + SC_LG_NGROUP - SC_LG_FIRST_REGULAR_BASE) - 1)











#if (SC_NBINS > 256)
#error "Too many small size classes"
#endif


#define SC_LOOKUP_MAXCLASS ((size_t)1 << 12)


#define SC_SMALL_MAX_BASE ((size_t)1 << (LG_PAGE + SC_LG_NGROUP - 1))
#define SC_SMALL_MAX_DELTA ((size_t)1 << (LG_PAGE - 1))


#define SC_SMALL_MAXCLASS (SC_SMALL_MAX_BASE + (SC_NGROUP - 1) * SC_SMALL_MAX_DELTA)



#define SC_LARGE_MINCLASS ((size_t)1ULL << (LG_PAGE + SC_LG_NGROUP))
#define SC_LG_LARGE_MINCLASS (LG_PAGE + SC_LG_NGROUP)


#define SC_MAX_BASE ((size_t)1 << (SC_PTR_BITS - 2))
#define SC_MAX_DELTA ((size_t)1 << (SC_PTR_BITS - 2 - SC_LG_NGROUP))


#define SC_LARGE_MAXCLASS (SC_MAX_BASE + (SC_NGROUP - 1) * SC_MAX_DELTA)

typedef struct sc_s sc_t;
struct sc_s {

int index;

int lg_base;

int lg_delta;

int ndelta;




bool psz;



bool bin;

int pgs;

int lg_delta_lookup;
};

typedef struct sc_data_s sc_data_t;
struct sc_data_s {

unsigned ntiny;

int nlbins;

int nbins;

int nsizes;

int lg_ceil_nsizes;

unsigned npsizes;

int lg_tiny_maxclass;

size_t lookup_maxclass;

size_t small_maxclass;

int lg_large_minclass;

size_t large_minclass;

size_t large_maxclass;

bool initialized;

sc_t sc[SC_NSIZES];
};

void sc_data_init(sc_data_t *data);




void sc_data_update_slab_size(sc_data_t *data, size_t begin, size_t end,
int pgs);
void sc_boot(sc_data_t *data);

#endif
