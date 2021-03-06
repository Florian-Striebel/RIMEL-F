#include "fmacros.h"
#include <stdint.h>
#include <unistd.h>
typedef uint64_t monotime;
extern monotime (*getMonotonicUs)(void);
const char * monotonicInit();
static inline void elapsedStart(monotime *start_time) {
*start_time = getMonotonicUs();
}
static inline uint64_t elapsedUs(monotime start_time) {
return getMonotonicUs() - start_time;
}
static inline uint64_t elapsedMs(monotime start_time) {
return elapsedUs(start_time) / 1000;
}
