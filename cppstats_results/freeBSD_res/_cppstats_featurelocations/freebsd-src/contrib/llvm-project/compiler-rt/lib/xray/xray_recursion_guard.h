










#if !defined(XRAY_XRAY_RECURSION_GUARD_H)
#define XRAY_XRAY_RECURSION_GUARD_H

#include "sanitizer_common/sanitizer_atomic.h"

namespace __xray {















class RecursionGuard {
atomic_uint8_t &Running;
const bool Valid;

public:
explicit inline RecursionGuard(atomic_uint8_t &R)
: Running(R), Valid(!atomic_exchange(&R, 1, memory_order_acq_rel)) {}

inline RecursionGuard(const RecursionGuard &) = delete;
inline RecursionGuard(RecursionGuard &&) = delete;
inline RecursionGuard &operator=(const RecursionGuard &) = delete;
inline RecursionGuard &operator=(RecursionGuard &&) = delete;

explicit inline operator bool() const { return Valid; }

inline ~RecursionGuard() noexcept {
if (Valid)
atomic_store(&Running, 0, memory_order_release);
}
};

}

#endif
