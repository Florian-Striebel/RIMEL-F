







#if !defined(LLDB_CORE_ARCHITECTURE_H)
#define LLDB_CORE_ARCHITECTURE_H

#include "lldb/Core/PluginInterface.h"
#include "lldb/Target/MemoryTagManager.h"

namespace lldb_private {

class Architecture : public PluginInterface {
public:










virtual void OverrideStopInfo(Thread &thread) const = 0;
















virtual size_t GetBytesToSkip(Symbol &func, const Address &curr_addr) const {
return 0;
}









virtual void AdjustBreakpointAddress(const Symbol &func,
Address &addr) const {}











virtual lldb::addr_t GetCallableLoadAddress(
lldb::addr_t addr, AddressClass addr_class = AddressClass::eInvalid) const {
return addr;
}












virtual lldb::addr_t GetOpcodeLoadAddress(
lldb::addr_t addr, AddressClass addr_class = AddressClass::eInvalid) const {
return addr;
}






virtual lldb::addr_t GetBreakableLoadAddress(lldb::addr_t addr,
Target &target) const {
return addr;
}








virtual const MemoryTagManager *GetMemoryTagManager() const {
return nullptr;
}
};

}

#endif
