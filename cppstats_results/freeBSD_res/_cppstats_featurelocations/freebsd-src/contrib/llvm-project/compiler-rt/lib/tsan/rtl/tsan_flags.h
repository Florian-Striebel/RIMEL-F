











#if !defined(TSAN_FLAGS_H)
#define TSAN_FLAGS_H

#include "sanitizer_common/sanitizer_flags.h"
#include "sanitizer_common/sanitizer_deadlock_detector_interface.h"

namespace __tsan {

struct Flags : DDFlags {
#define TSAN_FLAG(Type, Name, DefaultValue, Description) Type Name;
#include "tsan_flags.inc"
#undef TSAN_FLAG

void SetDefaults();
void ParseFromString(const char *str);
};

void InitializeFlags(Flags *flags, const char *env,
const char *env_option_name = nullptr);
}

#endif
