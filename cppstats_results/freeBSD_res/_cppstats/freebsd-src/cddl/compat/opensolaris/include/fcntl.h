#include_next <fcntl.h>
#if !defined(open64)
#define open64(...) open(__VA_ARGS__)
#endif
#define openat64(...) openat(__VA_ARGS__)
