











#if !defined(SANITIZER_STOPTHEWORLD_H)
#define SANITIZER_STOPTHEWORLD_H

#include "sanitizer_internal_defs.h"
#include "sanitizer_common.h"

namespace __sanitizer {

enum PtraceRegistersStatus {
REGISTERS_UNAVAILABLE_FATAL = -1,
REGISTERS_UNAVAILABLE = 0,
REGISTERS_AVAILABLE = 1
};



class SuspendedThreadsList {
public:
SuspendedThreadsList() = default;



virtual PtraceRegistersStatus GetRegistersAndSP(
uptr index, InternalMmapVector<uptr> *buffer, uptr *sp) const {
UNIMPLEMENTED();
}

virtual uptr ThreadCount() const { UNIMPLEMENTED(); }
virtual tid_t GetThreadID(uptr index) const { UNIMPLEMENTED(); }

protected:
~SuspendedThreadsList() {}

private:

SuspendedThreadsList(const SuspendedThreadsList &) = delete;
void operator=(const SuspendedThreadsList &) = delete;
};

typedef void (*StopTheWorldCallback)(
const SuspendedThreadsList &suspended_threads_list,
void *argument);






void StopTheWorld(StopTheWorldCallback callback, void *argument);

}

#endif
