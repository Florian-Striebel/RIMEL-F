







#if !defined(LLDB_CORE_DUMPREGISTERVALUE_H)
#define LLDB_CORE_DUMPREGISTERVALUE_H

#include "lldb/lldb-enumerations.h"
#include <cstdint>

namespace lldb_private {

class RegisterValue;
struct RegisterInfo;
class Stream;



bool DumpRegisterValue(const RegisterValue &reg_val, Stream *s,
const RegisterInfo *reg_info, bool prefix_with_name,
bool prefix_with_alt_name, lldb::Format format,
uint32_t reg_name_right_align_at = 0);

}

#endif
