







#if !defined(LLDB_HOST_COMMON_NATIVETHREADPROTOCOL_H)
#define LLDB_HOST_COMMON_NATIVETHREADPROTOCOL_H

#include <memory>

#include "lldb/Host/Debug.h"
#include "lldb/lldb-private-forward.h"
#include "lldb/lldb-types.h"

namespace lldb_private {

class NativeThreadProtocol {
public:
NativeThreadProtocol(NativeProcessProtocol &process, lldb::tid_t tid);

virtual ~NativeThreadProtocol() = default;

virtual std::string GetName() = 0;

virtual lldb::StateType GetState() = 0;

virtual NativeRegisterContext &GetRegisterContext() = 0;

virtual bool GetStopReason(ThreadStopInfo &stop_info,
std::string &description) = 0;

lldb::tid_t GetID() const { return m_tid; }

NativeProcessProtocol &GetProcess() { return m_process; }


virtual Status SetWatchpoint(lldb::addr_t addr, size_t size,
uint32_t watch_flags, bool hardware) = 0;

virtual Status RemoveWatchpoint(lldb::addr_t addr) = 0;


virtual Status SetHardwareBreakpoint(lldb::addr_t addr, size_t size) = 0;

virtual Status RemoveHardwareBreakpoint(lldb::addr_t addr) = 0;

protected:
NativeProcessProtocol &m_process;
lldb::tid_t m_tid;
};
}

#endif
