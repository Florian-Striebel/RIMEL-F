

































#if !defined(lib_pcap_funcattrs_h)
#define lib_pcap_funcattrs_h

#include <pcap/compiler-tests.h>
















#if defined(_WIN32)






























#if defined(pcap_EXPORTS)




#define PCAP_API_DEF __declspec(dllexport)
#elif defined(PCAP_DLL)




#define PCAP_API_DEF __declspec(dllimport)
#else






#define PCAP_API_DEF
#endif
#elif defined(MSDOS)

#define PCAP_API_DEF
#else
#if defined(pcap_EXPORTS)






#if PCAP_IS_AT_LEAST_GNUC_VERSION(3,4) || PCAP_IS_AT_LEAST_XL_C_VERSION(12,0)






#define PCAP_API_DEF __attribute__((visibility("default")))
#elif PCAP_IS_AT_LEAST_SUNC_VERSION(5,5)





#define PCAP_API_DEF __global
#else



#define PCAP_API_DEF
#endif
#else



#define PCAP_API_DEF
#endif
#endif

#define PCAP_API PCAP_API_DEF extern













#if __has_attribute(noreturn) || PCAP_IS_AT_LEAST_GNUC_VERSION(2,5) || PCAP_IS_AT_LEAST_SUNC_VERSION(5,9) || PCAP_IS_AT_LEAST_XL_C_VERSION(10,1) || PCAP_IS_AT_LEAST_HP_C_VERSION(6,10)











#define PCAP_NORETURN __attribute((noreturn))
#define PCAP_NORETURN_DEF __attribute((noreturn))
#elif defined(_MSC_VER)



#define PCAP_NORETURN __declspec(noreturn)
#define PCAP_NORETURN_DEF
#else
#define PCAP_NORETURN
#define PCAP_NORETURN_DEF
#endif







#if __has_attribute(__format__) || PCAP_IS_AT_LEAST_GNUC_VERSION(2,3) || PCAP_IS_AT_LEAST_XL_C_VERSION(10,1) || PCAP_IS_AT_LEAST_HP_C_VERSION(6,10)









#define PCAP_PRINTFLIKE(x,y) __attribute__((__format__(__printf__,x,y)))
#else
#define PCAP_PRINTFLIKE(x,y)
#endif










#if __has_attribute(deprecated) || PCAP_IS_AT_LEAST_GNUC_VERSION(4,5) || PCAP_IS_AT_LEAST_SUNC_VERSION(5,13)










#define PCAP_DEPRECATED(func, msg) __attribute__((deprecated(msg)))
#elif PCAP_IS_AT_LEAST_GNUC_VERSION(3,1)






#define PCAP_DEPRECATED(func, msg) __attribute__((deprecated))
#elif (defined(_MSC_VER) && (_MSC_VER >= 1500)) && !defined(BUILDING_PCAP)







#define PCAP_DEPRECATED(func, msg) __pragma(deprecated(func))
#else
#define PCAP_DEPRECATED(func, msg)
#endif




#if defined(_MSC_VER)
#include <sal.h>
#if _MSC_VER > 1400
#define PCAP_FORMAT_STRING(p) _Printf_format_string_ p
#else
#define PCAP_FORMAT_STRING(p) __format_string p
#endif
#else
#define PCAP_FORMAT_STRING(p) p
#endif

#endif
