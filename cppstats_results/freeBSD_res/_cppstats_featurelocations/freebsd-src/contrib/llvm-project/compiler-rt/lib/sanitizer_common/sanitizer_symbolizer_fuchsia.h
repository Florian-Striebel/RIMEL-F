











#if !defined(SANITIZER_SYMBOLIZER_FUCHSIA_H)
#define SANITIZER_SYMBOLIZER_FUCHSIA_H

#include "sanitizer_internal_defs.h"

namespace __sanitizer {





constexpr const char *kFormatDemangle = "{{{symbol:%s}}}";
constexpr uptr kFormatDemangleMax = 1024;


constexpr const char *kFormatFunction = "{{{pc:%p}}}";
constexpr uptr kFormatFunctionMax = 64;


constexpr const char *kFormatData = "{{{data:%p}}}";


constexpr const char *kFormatFrame = "{{{bt:%u:%p}}}";


#define FORMAT_DUMPFILE "{{{dumpfile:%s:%s}}}"

}

#endif
