












#if !defined(LSAN_FUCHSIA_H)
#define LSAN_FUCHSIA_H

#include "lsan_thread.h"
#include "sanitizer_common/sanitizer_platform.h"

#if !SANITIZER_FUCHSIA
#error "lsan_fuchsia.h is used only on Fuchsia systems (SANITIZER_FUCHSIA)"
#endif

namespace __lsan {

class ThreadContext final : public ThreadContextLsanBase {
public:
explicit ThreadContext(int tid);
void OnCreated(void *arg) override;
void OnStarted(void *arg) override;
};

}

#endif
