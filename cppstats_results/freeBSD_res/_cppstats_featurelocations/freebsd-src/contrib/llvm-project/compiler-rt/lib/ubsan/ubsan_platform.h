










#if !defined(UBSAN_PLATFORM_H)
#define UBSAN_PLATFORM_H


#if defined(__linux__) || defined(__FreeBSD__) || defined(__APPLE__) || defined(__NetBSD__) || defined(__DragonFly__) || (defined(__sun__) && defined(__svr4__)) || defined(_WIN32) || defined(__Fuchsia__)



#define CAN_SANITIZE_UB 1
#else
#define CAN_SANITIZE_UB 0
#endif

#endif
