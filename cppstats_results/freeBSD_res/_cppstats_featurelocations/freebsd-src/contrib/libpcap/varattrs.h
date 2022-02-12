

































#if !defined(varattrs_h)
#define varattrs_h

#include <pcap/compiler-tests.h>






#if __has_attribute(unused) || PCAP_IS_AT_LEAST_GNUC_VERSION(2,0)





#define _U_ __attribute__((unused))
#else



#define _U_
#endif

#endif
