







#if !defined(SANITIZER_STOPTHEWORLD_FUCHSIA_H)
#define SANITIZER_STOPTHEWORLD_FUCHSIA_H

#include "sanitizer_stoptheworld.h"

namespace __sanitizer {

class SuspendedThreadsListFuchsia final : public SuspendedThreadsList {};

}

#endif
