







#if !defined(LLDB_HOST_COMMON_NATIVEREGISTERCONTEXT_H)
#define LLDB_HOST_COMMON_NATIVEREGISTERCONTEXT_H

#include "lldb/Host/common/NativeWatchpointList.h"
#include "lldb/lldb-private.h"

namespace lldb_private {

class NativeThreadProtocol;

enum class ExpeditedRegs { Minimal, Full };

class NativeRegisterContext
: public std::enable_shared_from_this<NativeRegisterContext> {
public:

NativeRegisterContext(NativeThreadProtocol &thread);

virtual ~NativeRegisterContext();








virtual uint32_t GetRegisterCount() const = 0;

virtual uint32_t GetUserRegisterCount() const = 0;

virtual const RegisterInfo *GetRegisterInfoAtIndex(uint32_t reg) const = 0;

const char *GetRegisterSetNameForRegisterAtIndex(uint32_t reg_index) const;

virtual uint32_t GetRegisterSetCount() const = 0;

virtual const RegisterSet *GetRegisterSet(uint32_t set_index) const = 0;

virtual Status ReadRegister(const RegisterInfo *reg_info,
RegisterValue &reg_value) = 0;

virtual Status WriteRegister(const RegisterInfo *reg_info,
const RegisterValue &reg_value) = 0;

virtual Status ReadAllRegisterValues(lldb::DataBufferSP &data_sp) = 0;

virtual Status WriteAllRegisterValues(const lldb::DataBufferSP &data_sp) = 0;

uint32_t ConvertRegisterKindToRegisterNumber(uint32_t kind,
uint32_t num) const;


virtual uint32_t NumSupportedHardwareBreakpoints();

virtual uint32_t SetHardwareBreakpoint(lldb::addr_t addr, size_t size);

virtual bool ClearHardwareBreakpoint(uint32_t hw_idx);

virtual Status ClearAllHardwareBreakpoints();

virtual Status GetHardwareBreakHitIndex(uint32_t &bp_index,
lldb::addr_t trap_addr);

virtual uint32_t NumSupportedHardwareWatchpoints();

virtual uint32_t SetHardwareWatchpoint(lldb::addr_t addr, size_t size,
uint32_t watch_flags);

virtual bool ClearHardwareWatchpoint(uint32_t hw_index);

virtual Status ClearWatchpointHit(uint32_t hw_index);

virtual Status ClearAllHardwareWatchpoints();

virtual Status IsWatchpointHit(uint32_t wp_index, bool &is_hit);

virtual Status GetWatchpointHitIndex(uint32_t &wp_index,
lldb::addr_t trap_addr);

virtual Status IsWatchpointVacant(uint32_t wp_index, bool &is_vacant);

virtual lldb::addr_t GetWatchpointAddress(uint32_t wp_index);










virtual lldb::addr_t GetWatchpointHitAddress(uint32_t wp_index);

virtual bool HardwareSingleStep(bool enable);

virtual Status
ReadRegisterValueFromMemory(const lldb_private::RegisterInfo *reg_info,
lldb::addr_t src_addr, size_t src_len,
RegisterValue &reg_value);

virtual Status
WriteRegisterValueToMemory(const lldb_private::RegisterInfo *reg_info,
lldb::addr_t dst_addr, size_t dst_len,
const RegisterValue &reg_value);


virtual lldb::tid_t GetThreadID() const;

virtual NativeThreadProtocol &GetThread() { return m_thread; }

virtual std::vector<uint32_t>
GetExpeditedRegisters(ExpeditedRegs expType) const;

virtual bool RegisterOffsetIsDynamic() const { return false; }

const RegisterInfo *GetRegisterInfoByName(llvm::StringRef reg_name,
uint32_t start_idx = 0);

const RegisterInfo *GetRegisterInfo(uint32_t reg_kind, uint32_t reg_num);

lldb::addr_t GetPC(lldb::addr_t fail_value = LLDB_INVALID_ADDRESS);

virtual lldb::addr_t
GetPCfromBreakpointLocation(lldb::addr_t fail_value = LLDB_INVALID_ADDRESS);

Status SetPC(lldb::addr_t pc);

lldb::addr_t GetSP(lldb::addr_t fail_value = LLDB_INVALID_ADDRESS);

Status SetSP(lldb::addr_t sp);

lldb::addr_t GetFP(lldb::addr_t fail_value = LLDB_INVALID_ADDRESS);

Status SetFP(lldb::addr_t fp);

const char *GetRegisterName(uint32_t reg);

lldb::addr_t GetReturnAddress(lldb::addr_t fail_value = LLDB_INVALID_ADDRESS);

lldb::addr_t GetFlags(lldb::addr_t fail_value = 0);

lldb::addr_t ReadRegisterAsUnsigned(uint32_t reg, lldb::addr_t fail_value);

lldb::addr_t ReadRegisterAsUnsigned(const RegisterInfo *reg_info,
lldb::addr_t fail_value);

Status WriteRegisterFromUnsigned(uint32_t reg, uint64_t uval);

Status WriteRegisterFromUnsigned(const RegisterInfo *reg_info, uint64_t uval);













protected:

NativeThreadProtocol
&m_thread;



private:

NativeRegisterContext(const NativeRegisterContext &) = delete;
const NativeRegisterContext &
operator=(const NativeRegisterContext &) = delete;
};

}

#endif
