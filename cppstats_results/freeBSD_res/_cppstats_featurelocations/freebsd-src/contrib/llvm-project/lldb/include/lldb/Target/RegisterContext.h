







#if !defined(LLDB_TARGET_REGISTERCONTEXT_H)
#define LLDB_TARGET_REGISTERCONTEXT_H

#include "lldb/Target/ExecutionContextScope.h"
#include "lldb/lldb-private.h"

namespace lldb_private {

class RegisterContext : public std::enable_shared_from_this<RegisterContext>,
public ExecutionContextScope {
public:

RegisterContext(Thread &thread, uint32_t concrete_frame_idx);

~RegisterContext() override;

void InvalidateIfNeeded(bool force);


virtual void InvalidateAllRegisters() = 0;

virtual size_t GetRegisterCount() = 0;

virtual const RegisterInfo *GetRegisterInfoAtIndex(size_t reg) = 0;


uint32_t UpdateDynamicRegisterSize(const lldb_private::ArchSpec &arch,
RegisterInfo *reg_info);

virtual size_t GetRegisterSetCount() = 0;

virtual const RegisterSet *GetRegisterSet(size_t reg_set) = 0;

virtual lldb::ByteOrder GetByteOrder();

virtual bool ReadRegister(const RegisterInfo *reg_info,
RegisterValue &reg_value) = 0;

virtual bool WriteRegister(const RegisterInfo *reg_info,
const RegisterValue &reg_value) = 0;

virtual bool ReadAllRegisterValues(lldb::DataBufferSP &data_sp) {
return false;
}

virtual bool WriteAllRegisterValues(const lldb::DataBufferSP &data_sp) {
return false;
}











virtual bool
ReadAllRegisterValues(lldb_private::RegisterCheckpoint &reg_checkpoint);

virtual bool WriteAllRegisterValues(
const lldb_private::RegisterCheckpoint &reg_checkpoint);

bool CopyFromRegisterContext(lldb::RegisterContextSP context);


































virtual uint32_t ConvertRegisterKindToRegisterNumber(lldb::RegisterKind kind,
uint32_t num);


virtual uint32_t NumSupportedHardwareBreakpoints();

virtual uint32_t SetHardwareBreakpoint(lldb::addr_t addr, size_t size);

virtual bool ClearHardwareBreakpoint(uint32_t hw_idx);

virtual uint32_t NumSupportedHardwareWatchpoints();

virtual uint32_t SetHardwareWatchpoint(lldb::addr_t addr, size_t size,
bool read, bool write);

virtual bool ClearHardwareWatchpoint(uint32_t hw_index);

virtual bool HardwareSingleStep(bool enable);

virtual Status
ReadRegisterValueFromMemory(const lldb_private::RegisterInfo *reg_info,
lldb::addr_t src_addr, uint32_t src_len,
RegisterValue &reg_value);

virtual Status
WriteRegisterValueToMemory(const lldb_private::RegisterInfo *reg_info,
lldb::addr_t dst_addr, uint32_t dst_len,
const RegisterValue &reg_value);


virtual lldb::tid_t GetThreadID() const;

virtual Thread &GetThread() { return m_thread; }

const RegisterInfo *GetRegisterInfoByName(llvm::StringRef reg_name,
uint32_t start_idx = 0);

const RegisterInfo *GetRegisterInfo(lldb::RegisterKind reg_kind,
uint32_t reg_num);

uint64_t GetPC(uint64_t fail_value = LLDB_INVALID_ADDRESS);
























bool GetPCForSymbolication(Address &address);

bool SetPC(uint64_t pc);

bool SetPC(Address addr);

uint64_t GetSP(uint64_t fail_value = LLDB_INVALID_ADDRESS);

bool SetSP(uint64_t sp);

uint64_t GetFP(uint64_t fail_value = LLDB_INVALID_ADDRESS);

bool SetFP(uint64_t fp);

const char *GetRegisterName(uint32_t reg);

uint64_t GetReturnAddress(uint64_t fail_value = LLDB_INVALID_ADDRESS);

uint64_t GetFlags(uint64_t fail_value = 0);

uint64_t ReadRegisterAsUnsigned(uint32_t reg, uint64_t fail_value);

uint64_t ReadRegisterAsUnsigned(const RegisterInfo *reg_info,
uint64_t fail_value);

bool WriteRegisterFromUnsigned(uint32_t reg, uint64_t uval);

bool WriteRegisterFromUnsigned(const RegisterInfo *reg_info, uint64_t uval);

bool ConvertBetweenRegisterKinds(lldb::RegisterKind source_rk,
uint32_t source_regnum,
lldb::RegisterKind target_rk,
uint32_t &target_regnum);


lldb::TargetSP CalculateTarget() override;

lldb::ProcessSP CalculateProcess() override;

lldb::ThreadSP CalculateThread() override;

lldb::StackFrameSP CalculateStackFrame() override;

void CalculateExecutionContext(ExecutionContext &exe_ctx) override;

uint32_t GetStopID() const { return m_stop_id; }

void SetStopID(uint32_t stop_id) { m_stop_id = stop_id; }

protected:









virtual bool BehavesLikeZerothFrame() const {
return m_concrete_frame_idx == 0;
}


Thread &m_thread;
uint32_t m_concrete_frame_idx;

uint32_t m_stop_id;
private:

RegisterContext(const RegisterContext &) = delete;
const RegisterContext &operator=(const RegisterContext &) = delete;
};

}

#endif
