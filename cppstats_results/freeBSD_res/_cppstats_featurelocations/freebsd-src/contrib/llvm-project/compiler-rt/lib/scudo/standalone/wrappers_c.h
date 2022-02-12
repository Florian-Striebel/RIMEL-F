







#if !defined(SCUDO_WRAPPERS_C_H_)
#define SCUDO_WRAPPERS_C_H_

#include "platform.h"
#include "stats.h"


#if SCUDO_ANDROID
typedef size_t __scudo_mallinfo_data_t;
#else
typedef int __scudo_mallinfo_data_t;
#endif

struct __scudo_mallinfo {
__scudo_mallinfo_data_t arena;
__scudo_mallinfo_data_t ordblks;
__scudo_mallinfo_data_t smblks;
__scudo_mallinfo_data_t hblks;
__scudo_mallinfo_data_t hblkhd;
__scudo_mallinfo_data_t usmblks;
__scudo_mallinfo_data_t fsmblks;
__scudo_mallinfo_data_t uordblks;
__scudo_mallinfo_data_t fordblks;
__scudo_mallinfo_data_t keepcost;
};




#if STRUCT_MALLINFO_DECLARED
#define SCUDO_MALLINFO mallinfo
#else
#define SCUDO_MALLINFO __scudo_mallinfo
#endif

#endif
