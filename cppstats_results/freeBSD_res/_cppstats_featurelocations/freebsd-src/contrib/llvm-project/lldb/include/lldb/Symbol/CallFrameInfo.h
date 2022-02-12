







#if !defined(LLDB_SYMBOL_CALLFRAMEINFO_H)
#define LLDB_SYMBOL_CALLFRAMEINFO_H

#include "lldb/Core/Address.h"

namespace lldb_private {

class CallFrameInfo {
public:
virtual ~CallFrameInfo() = default;

virtual bool GetAddressRange(Address addr, AddressRange &range) = 0;

virtual bool GetUnwindPlan(const Address &addr, UnwindPlan &unwind_plan) = 0;
virtual bool GetUnwindPlan(const AddressRange &range, UnwindPlan &unwind_plan) = 0;
};

}

#endif
