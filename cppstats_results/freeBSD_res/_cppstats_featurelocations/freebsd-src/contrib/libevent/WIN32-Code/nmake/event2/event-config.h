








#if !defined(EVENT_CONFIG_H__)
#define EVENT_CONFIG_H__














#define EVENT_DNS_USE_FTIME_FOR_ID_ 1





























#define EVENT__HAVE_FCNTL_H 1


#define EVENT__HAVE_GETADDRINFO 1


#define EVENT__HAVE_GETNAMEINFO 1


#define EVENT__HAVE_GETPROTOBYNUMBER 1


#define EVENT__HAVE_GETSERVBYNAME 1




















#define EVENT__HAVE_MEMORY_H 1















































#define EVENT__HAVE_SIGNAL 1





#define EVENT__HAVE_STDARG_H 1


#define EVENT__HAVE_STDDEF_H 1





#define EVENT__HAVE_STDLIB_H 1


#define EVENT__HAVE_STRINGS_H 1


#define EVENT__HAVE_STRING_H 1













#define EVENT__HAVE_STRUCT_ADDRINFO 1


#define EVENT__HAVE_STRUCT_IN6_ADDR 1


#define EVENT__HAVE_STRUCT_IN6_ADDR_S6_ADDR16 1


#define EVENT__HAVE_STRUCT_IN6_ADDR_S6_ADDR32 1


#define EVENT__HAVE_STRUCT_SOCKADDR_IN6 1








#define EVENT__HAVE_STRUCT_SOCKADDR_STORAGE 1



































#define EVENT__HAVE_SYS_STAT_H 1





#define EVENT__HAVE_SYS_TYPES_H 1











#define EVENT__HAVE_TIMERCLEAR 1


#define EVENT__HAVE_TIMERCMP 1


#define EVENT__HAVE_TIMERISSET 1























#define EVENT__NUMERIC_VERSION 0x02010800


#define EVENT__PACKAGE "libevent"


#define EVENT__PACKAGE_BUGREPORT ""


#define EVENT__PACKAGE_NAME ""


#define EVENT__PACKAGE_STRING ""


#define EVENT__PACKAGE_TARNAME ""


#define EVENT__PACKAGE_VERSION ""






#define EVENT__SIZEOF_INT 4


#define EVENT__SIZEOF_LONG 4


#define EVENT__SIZEOF_LONG_LONG 8


#define EVENT__SIZEOF_SHORT 2


#if defined(_WIN64)
#define EVENT__SIZEOF_SIZE_T 8
#else
#define EVENT__SIZEOF_SIZE_T 4
#endif


#if defined(_WIN64)
#define EVENT__SIZEOF_VOID_P 8
#else
#define EVENT__SIZEOF_VOID_P 4
#endif


#define EVENT__STDC_HEADERS 1


#define EVENT__TIME_WITH_SYS_TIME 1


#define EVENT__VERSION "2.1.8-stable"


#define EVENT____func__ __FUNCTION__






#if !defined(_EVENT___cplusplus)
#define EVENT__inline __inline
#endif








#define EVENT__socklen_t unsigned int


#define EVENT__ssize_t SSIZE_T

#endif
