







#if !defined(GWP_ASAN_OPTIONAL_OPTIONS_PARSER_H_)
#define GWP_ASAN_OPTIONAL_OPTIONS_PARSER_H_

#include "gwp_asan/optional/printf.h"
#include "gwp_asan/options.h"

namespace gwp_asan {
namespace options {

void initOptions(Printf_t PrintfForWarnings = nullptr);

void initOptions(const char *OptionsStr, Printf_t PrintfForWarnings = nullptr);


Options &getOptions();
}
}

extern "C" {
__attribute__((weak)) const char *__gwp_asan_default_options();
}

#endif
