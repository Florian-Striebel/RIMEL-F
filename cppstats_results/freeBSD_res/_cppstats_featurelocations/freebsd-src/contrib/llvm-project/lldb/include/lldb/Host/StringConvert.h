







#if !defined(LLDB_HOST_STRINGCONVERT_H)
#define LLDB_HOST_STRINGCONVERT_H

#include <cstdint>

namespace lldb_private {

namespace StringConvert {




int32_t ToSInt32(const char *s, int32_t fail_value = 0, int base = 0,
bool *success_ptr = nullptr);

uint32_t ToUInt32(const char *s, uint32_t fail_value = 0, int base = 0,
bool *success_ptr = nullptr);

int64_t ToSInt64(const char *s, int64_t fail_value = 0, int base = 0,
bool *success_ptr = nullptr);

uint64_t ToUInt64(const char *s, uint64_t fail_value = 0, int base = 0,
bool *success_ptr = nullptr);

double ToDouble(const char *s, double fail_value = 0.0,
bool *success_ptr = nullptr);
}
}

#endif
