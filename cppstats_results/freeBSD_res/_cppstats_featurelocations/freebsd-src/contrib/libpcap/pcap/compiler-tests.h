

































#if !defined(lib_pcap_compiler_tests_h)
#define lib_pcap_compiler_tests_h








#if !defined(__has_attribute)







#define __has_attribute(x) 0
#endif



























#if ! defined(__GNUC__)
#define PCAP_IS_AT_LEAST_GNUC_VERSION(major, minor) 0
#else
#define PCAP_IS_AT_LEAST_GNUC_VERSION(major, minor) (__GNUC__ > (major) || (__GNUC__ == (major) && __GNUC_MINOR__ >= (minor)))


#endif





#if !defined(__clang__)
#define PCAP_IS_AT_LEAST_CLANG_VERSION(major, minor) 0
#else
#define PCAP_IS_AT_LEAST_CLANG_VERSION(major, minor) (__clang_major__ > (major) || (__clang_major__ == (major) && __clang_minor__ >= (minor)))


#endif



















#if ! defined(__SUNPRO_C)
#define PCAP_IS_AT_LEAST_SUNC_VERSION(major,minor) 0
#else
#define PCAP_SUNPRO_VERSION_TO_BCD(major, minor) (((minor) >= 10) ? (((major) << 12) | (((minor)/10) << 8) | (((minor)%10) << 4)) : (((major) << 8) | ((minor) << 4)))



#define PCAP_IS_AT_LEAST_SUNC_VERSION(major,minor) (__SUNPRO_C >= PCAP_SUNPRO_VERSION_TO_BCD((major), (minor)))

#endif








#if ! defined(__xlC__)
#define PCAP_IS_AT_LEAST_XL_C_VERSION(major,minor) 0
#else
#define PCAP_IS_AT_LEAST_XL_C_VERSION(major, minor) (__xlC__ >= (((major) << 8) | (minor)))

#endif












#if ! defined(__HP_aCC)
#define PCAP_IS_AT_LEAST_HP_C_VERSION(major,minor) 0
#else
#define PCAP_IS_AT_LEAST_HP_C_VERSION(major,minor) (__HP_aCC >= ((major)*10000 + (minor)*100))

#endif

#endif
