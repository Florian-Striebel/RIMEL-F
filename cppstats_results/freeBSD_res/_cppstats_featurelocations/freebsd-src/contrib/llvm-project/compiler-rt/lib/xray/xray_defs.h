










#if !defined(XRAY_XRAY_DEFS_H)
#define XRAY_XRAY_DEFS_H

#if XRAY_SUPPORTED
#define XRAY_NEVER_INSTRUMENT __attribute__((xray_never_instrument))
#else
#define XRAY_NEVER_INSTRUMENT
#endif

#if SANITIZER_NETBSD


#define XRAY_TLS_ALIGNAS(x)
#define XRAY_HAS_TLS_ALIGNAS 0
#else
#define XRAY_TLS_ALIGNAS(x) alignas(x)
#define XRAY_HAS_TLS_ALIGNAS 1
#endif

#endif
