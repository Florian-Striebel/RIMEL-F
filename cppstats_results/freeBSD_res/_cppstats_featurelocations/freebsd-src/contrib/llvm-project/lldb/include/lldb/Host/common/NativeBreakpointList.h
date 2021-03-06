







#if !defined(LLDB_HOST_COMMON_NATIVEBREAKPOINTLIST_H)
#define LLDB_HOST_COMMON_NATIVEBREAKPOINTLIST_H

#include "lldb/lldb-private-forward.h"
#include "lldb/lldb-types.h"
#include <map>

namespace lldb_private {

struct HardwareBreakpoint {
lldb::addr_t m_addr;
size_t m_size;
};

using HardwareBreakpointMap = std::map<lldb::addr_t, HardwareBreakpoint>;
}

#endif
