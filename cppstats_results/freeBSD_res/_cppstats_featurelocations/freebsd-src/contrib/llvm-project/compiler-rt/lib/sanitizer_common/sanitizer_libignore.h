














#if !defined(SANITIZER_LIBIGNORE_H)
#define SANITIZER_LIBIGNORE_H

#include "sanitizer_internal_defs.h"
#include "sanitizer_common.h"
#include "sanitizer_atomic.h"
#include "sanitizer_mutex.h"

namespace __sanitizer {

class LibIgnore {
public:
explicit LibIgnore(LinkerInitialized);


void AddIgnoredLibrary(const char *name_templ);
void IgnoreNoninstrumentedModules(bool enable) {
track_instrumented_libs_ = enable;
}


void OnLibraryLoaded(const char *name);


void OnLibraryUnloaded();





bool IsIgnored(uptr pc, bool *pc_in_ignored_lib) const;


bool IsPcInstrumented(uptr pc) const;

private:
struct Lib {
char *templ;
char *name;
char *real_name;
bool loaded;
};

struct LibCodeRange {
uptr begin;
uptr end;
};

inline bool IsInRange(uptr pc, const LibCodeRange &range) const {
return (pc >= range.begin && pc < range.end);
}

static const uptr kMaxIgnoredRanges = 128;
static const uptr kMaxInstrumentedRanges = 1024;
static const uptr kMaxLibs = 1024;


atomic_uintptr_t ignored_ranges_count_;
LibCodeRange ignored_code_ranges_[kMaxIgnoredRanges];

atomic_uintptr_t instrumented_ranges_count_;
LibCodeRange instrumented_code_ranges_[kMaxInstrumentedRanges];


BlockingMutex mutex_;
uptr count_;
Lib libs_[kMaxLibs];
bool track_instrumented_libs_;


LibIgnore(const LibIgnore&);
void operator = (const LibIgnore&);
};

inline bool LibIgnore::IsIgnored(uptr pc, bool *pc_in_ignored_lib) const {
const uptr n = atomic_load(&ignored_ranges_count_, memory_order_acquire);
for (uptr i = 0; i < n; i++) {
if (IsInRange(pc, ignored_code_ranges_[i])) {
*pc_in_ignored_lib = true;
return true;
}
}
*pc_in_ignored_lib = false;
if (track_instrumented_libs_ && !IsPcInstrumented(pc))
return true;
return false;
}

inline bool LibIgnore::IsPcInstrumented(uptr pc) const {
const uptr n = atomic_load(&instrumented_ranges_count_, memory_order_acquire);
for (uptr i = 0; i < n; i++) {
if (IsInRange(pc, instrumented_code_ranges_[i]))
return true;
}
return false;
}

}

#endif
