#include "jemalloc/internal/malloc_io.h"
#include "jemalloc/internal/util.h"





#if !defined(assert)
#define assert(e) do { if (unlikely(config_debug && !(e))) { malloc_printf( "<jemalloc>: %s:%d: Failed assertion: \"%s\"\n", __FILE__, __LINE__, #e); abort(); } } while (0)







#endif

#if !defined(not_reached)
#define not_reached() do { if (config_debug) { malloc_printf( "<jemalloc>: %s:%d: Unreachable code reached\n", __FILE__, __LINE__); abort(); } unreachable(); } while (0)








#endif

#if !defined(not_implemented)
#define not_implemented() do { if (config_debug) { malloc_printf("<jemalloc>: %s:%d: Not implemented\n", __FILE__, __LINE__); abort(); } } while (0)






#endif

#if !defined(assert_not_implemented)
#define assert_not_implemented(e) do { if (unlikely(config_debug && !(e))) { not_implemented(); } } while (0)




#endif


#if !defined(cassert)
#define cassert(c) do { if (unlikely(!(c))) { not_reached(); } } while (0)




#endif
