







#if !defined(LLDB_BREAKPOINT_STOPPOINTSITE_H)
#define LLDB_BREAKPOINT_STOPPOINTSITE_H

#include "lldb/Breakpoint/StoppointHitCounter.h"
#include "lldb/Utility/UserID.h"
#include "lldb/lldb-private.h"

namespace lldb_private {

class StoppointSite {
public:
StoppointSite(lldb::break_id_t bid, lldb::addr_t m_addr, bool hardware);

StoppointSite(lldb::break_id_t bid, lldb::addr_t m_addr,
uint32_t byte_size, bool hardware);

virtual ~StoppointSite() = default;

virtual lldb::addr_t GetLoadAddress() const { return m_addr; }

virtual void SetLoadAddress(lldb::addr_t addr) { m_addr = addr; }

uint32_t GetByteSize() const { return m_byte_size; }

uint32_t GetHitCount() const { return m_hit_counter.GetValue(); }

void ResetHitCount() { m_hit_counter.Reset(); }

bool HardwareRequired() const { return m_is_hardware_required; }

virtual bool IsHardware() const = 0;

uint32_t GetHardwareIndex() const { return m_hardware_index; }

void SetHardwareIndex(uint32_t index) { m_hardware_index = index; }

virtual bool ShouldStop(StoppointCallbackContext* context) = 0;

virtual void Dump(Stream* stream) const = 0;

lldb::break_id_t GetID() const { return m_id; }

protected:

lldb::break_id_t m_id;


lldb::addr_t m_addr;



bool m_is_hardware_required;


uint32_t m_hardware_index;




uint32_t m_byte_size;


StoppointHitCounter m_hit_counter;

private:
StoppointSite(const StoppointSite &) = delete;
const StoppointSite &operator=(const StoppointSite &) = delete;
StoppointSite() = delete;
};

}

#endif
