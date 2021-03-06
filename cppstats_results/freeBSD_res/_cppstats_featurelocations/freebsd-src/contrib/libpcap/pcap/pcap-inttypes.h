





























#if !defined(pcap_pcap_inttypes_h)
#define pcap_pcap_inttypes_h





#if defined(_MSC_VER)



#if _MSC_VER >= 1800



#include <inttypes.h>
#else



typedef unsigned char uint8_t;
typedef signed char int8_t;
typedef unsigned short uint16_t;
typedef signed short int16_t;
typedef unsigned int uint32_t;
typedef signed int int32_t;
#if defined(_MSC_EXTENSIONS)
typedef unsigned _int64 uint64_t;
typedef _int64 int64_t;
#else
typedef unsigned long long uint64_t;
typedef long long int64_t;
#endif
#endif













#if !defined(PRId64)
#if defined(_MSC_EXTENSIONS)
#define PRId64 "I64d"
#else
#define PRId64 "lld"
#endif
#endif

#if !defined(PRIo64)
#if defined(_MSC_EXTENSIONS)
#define PRIo64 "I64o"
#else
#define PRIo64 "llo"
#endif
#endif

#if !defined(PRIx64)
#if defined(_MSC_EXTENSIONS)
#define PRIx64 "I64x"
#else
#define PRIx64 "llx"
#endif
#endif

#if !defined(PRIu64)
#if defined(_MSC_EXTENSIONS)
#define PRIu64 "I64u"
#else
#define PRIu64 "llu"
#endif
#endif





#define PRIsize "Iu"
#elif defined(__MINGW32__) || !defined(_WIN32)




#include <inttypes.h>




#define PRIsize "zu"
#endif

#endif
