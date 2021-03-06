

































#if !defined(_diag_control_h)
#define _diag_control_h

#include "pcap/compiler-tests.h"

#if !defined(_MSC_VER)






#define PCAP_DO_PRAGMA(x) _Pragma (#x)
#endif




#if defined(_MSC_VER)







#define DIAG_OFF_FLEX __pragma(warning(push)) __pragma(warning(disable:4127)) __pragma(warning(disable:4242)) __pragma(warning(disable:4244)) __pragma(warning(disable:4702))





#define DIAG_ON_FLEX __pragma(warning(pop))
#elif PCAP_IS_AT_LEAST_CLANG_VERSION(2,8)









#define DIAG_OFF_FLEX PCAP_DO_PRAGMA(clang diagnostic push) PCAP_DO_PRAGMA(clang diagnostic ignored "-Wsign-compare") PCAP_DO_PRAGMA(clang diagnostic ignored "-Wdocumentation") PCAP_DO_PRAGMA(clang diagnostic ignored "-Wmissing-noreturn") PCAP_DO_PRAGMA(clang diagnostic ignored "-Wunused-parameter") PCAP_DO_PRAGMA(clang diagnostic ignored "-Wunreachable-code")






#define DIAG_ON_FLEX PCAP_DO_PRAGMA(clang diagnostic pop)

#elif PCAP_IS_AT_LEAST_GNUC_VERSION(4,6)





#define DIAG_OFF_FLEX PCAP_DO_PRAGMA(GCC diagnostic push) PCAP_DO_PRAGMA(GCC diagnostic ignored "-Wsign-compare") PCAP_DO_PRAGMA(GCC diagnostic ignored "-Wunused-parameter") PCAP_DO_PRAGMA(GCC diagnostic ignored "-Wunreachable-code")




#define DIAG_ON_FLEX PCAP_DO_PRAGMA(GCC diagnostic pop)

#else





#define DIAG_OFF_FLEX
#define DIAG_ON_FLEX
#endif

#if defined(YYBYACC)
















#if defined(_MSC_VER)




#define DIAG_OFF_BISON_BYACC __pragma(warning(push)) __pragma(warning(disable:4702))


#define DIAG_ON_BISON_BYACC __pragma(warning(pop))
#elif PCAP_IS_AT_LEAST_CLANG_VERSION(2,8)




#define DIAG_OFF_BISON_BYACC PCAP_DO_PRAGMA(clang diagnostic push) PCAP_DO_PRAGMA(clang diagnostic ignored "-Wshadow") PCAP_DO_PRAGMA(clang diagnostic ignored "-Wunreachable-code")



#define DIAG_ON_BISON_BYACC PCAP_DO_PRAGMA(clang diagnostic pop)

#elif PCAP_IS_AT_LEAST_GNUC_VERSION(4,6)





#define DIAG_OFF_BISON_BYACC PCAP_DO_PRAGMA(GCC diagnostic push) PCAP_DO_PRAGMA(GCC diagnostic ignored "-Wshadow") PCAP_DO_PRAGMA(GCC diagnostic ignored "-Wunreachable-code")



#define DIAG_ON_BISON_BYACC PCAP_DO_PRAGMA(GCC diagnostic pop)

#else




#define DIAG_OFF_BISON_BYACC
#define DIAG_ON_BISON_BYACC
#endif
#else






#if defined(_MSC_VER)






#define DIAG_OFF_BISON_BYACC __pragma(warning(push)) __pragma(warning(disable:4127)) __pragma(warning(disable:4242)) __pragma(warning(disable:4244)) __pragma(warning(disable:4702))





#define DIAG_ON_BISON_BYACC __pragma(warning(pop))
#elif PCAP_IS_AT_LEAST_CLANG_VERSION(2,8)




#define DIAG_OFF_BISON_BYACC PCAP_DO_PRAGMA(clang diagnostic push) PCAP_DO_PRAGMA(clang diagnostic ignored "-Wunreachable-code")


#define DIAG_ON_BISON_BYACC PCAP_DO_PRAGMA(clang diagnostic pop)

#elif PCAP_IS_AT_LEAST_GNUC_VERSION(4,6)





#define DIAG_OFF_BISON_BYACC PCAP_DO_PRAGMA(GCC diagnostic push) PCAP_DO_PRAGMA(GCC diagnostic ignored "-Wunreachable-code")


#define DIAG_ON_BISON_BYACC PCAP_DO_PRAGMA(GCC diagnostic pop)

#else




#define DIAG_OFF_BISON_BYACC
#define DIAG_ON_BISON_BYACC
#endif
#endif

#endif
