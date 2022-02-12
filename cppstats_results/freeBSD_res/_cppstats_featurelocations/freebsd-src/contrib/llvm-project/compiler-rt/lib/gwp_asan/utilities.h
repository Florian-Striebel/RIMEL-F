







#if !defined(GWP_ASAN_UTILITIES_H_)
#define GWP_ASAN_UTILITIES_H_

#include "gwp_asan/definitions.h"

#include <stddef.h>

namespace gwp_asan {

void die(const char *Message);


GWP_ASAN_ALWAYS_INLINE void Check(bool Condition, const char *Message) {
if (Condition)
return;
die(Message);
}
}

#endif
