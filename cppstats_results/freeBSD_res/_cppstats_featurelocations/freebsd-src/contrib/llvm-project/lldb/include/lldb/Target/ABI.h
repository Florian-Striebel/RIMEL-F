







#if !defined(LLDB_TARGET_ABI_H)
#define LLDB_TARGET_ABI_H

#include "lldb/Core/PluginInterface.h"
#include "lldb/Symbol/UnwindPlan.h"
#include "lldb/Utility/Status.h"
#include "lldb/lldb-private.h"

#include "llvm/ADT/ArrayRef.h"
#include "llvm/MC/MCRegisterInfo.h"

namespace llvm {
class Type;
}

namespace lldb_private {

class ABI : public PluginInterface {
public:
struct CallArgument {
enum eType {
HostPointer = 0,
TargetValue,
};
eType type;
size_t size;

lldb::addr_t value;
std::unique_ptr<uint8_t[]> data_up;
};

~ABI() override;

virtual size_t GetRedZoneSize() const = 0;

virtual bool PrepareTrivialCall(lldb_private::Thread &thread, lldb::addr_t sp,
lldb::addr_t functionAddress,
lldb::addr_t returnAddress,
llvm::ArrayRef<lldb::addr_t> args) const = 0;











virtual bool PrepareTrivialCall(lldb_private::Thread &thread, lldb::addr_t sp,
lldb::addr_t functionAddress,
lldb::addr_t returnAddress,
llvm::Type &prototype,
llvm::ArrayRef<CallArgument> args) const;

virtual bool GetArgumentValues(Thread &thread, ValueList &values) const = 0;

lldb::ValueObjectSP GetReturnValueObject(Thread &thread, CompilerType &type,
bool persistent = true) const;


lldb::ValueObjectSP GetReturnValueObject(Thread &thread, llvm::Type &type,
bool persistent = true) const;


virtual Status SetReturnValueObject(lldb::StackFrameSP &frame_sp,
lldb::ValueObjectSP &new_value) = 0;

protected:



virtual lldb::ValueObjectSP
GetReturnValueObjectImpl(Thread &thread, CompilerType &ast_type) const = 0;


virtual lldb::ValueObjectSP
GetReturnValueObjectImpl(Thread &thread, llvm::Type &ir_type) const;







lldb::ProcessSP GetProcessSP() const { return m_process_wp.lock(); }

public:
virtual bool CreateFunctionEntryUnwindPlan(UnwindPlan &unwind_plan) = 0;

virtual bool CreateDefaultUnwindPlan(UnwindPlan &unwind_plan) = 0;

virtual bool RegisterIsVolatile(const RegisterInfo *reg_info) = 0;

virtual bool
GetFallbackRegisterLocation(const RegisterInfo *reg_info,
UnwindPlan::Row::RegisterLocation &unwind_regloc);







virtual bool CallFrameAddressIsValid(lldb::addr_t cfa) = 0;



virtual bool CodeAddressIsValid(lldb::addr_t pc) = 0;





virtual lldb::addr_t FixCodeAddress(lldb::addr_t pc) { return pc; }
virtual lldb::addr_t FixDataAddress(lldb::addr_t pc) { return pc; }


llvm::MCRegisterInfo &GetMCRegisterInfo() { return *m_mc_register_info_up; }

virtual void AugmentRegisterInfo(RegisterInfo &info) = 0;

virtual bool GetPointerReturnRegister(const char *&name) { return false; }

static lldb::ABISP FindPlugin(lldb::ProcessSP process_sp, const ArchSpec &arch);

protected:
ABI(lldb::ProcessSP process_sp, std::unique_ptr<llvm::MCRegisterInfo> info_up)
: m_process_wp(process_sp), m_mc_register_info_up(std::move(info_up)) {
assert(m_mc_register_info_up && "ABI must have MCRegisterInfo");
}




static std::unique_ptr<llvm::MCRegisterInfo>
MakeMCRegisterInfo(const ArchSpec &arch);

lldb::ProcessWP m_process_wp;
std::unique_ptr<llvm::MCRegisterInfo> m_mc_register_info_up;

virtual lldb::addr_t FixCodeAddress(lldb::addr_t pc, lldb::addr_t mask) {
return pc;
}

private:
ABI(const ABI &) = delete;
const ABI &operator=(const ABI &) = delete;
};

class RegInfoBasedABI : public ABI {
public:
void AugmentRegisterInfo(RegisterInfo &info) override;

protected:
using ABI::ABI;

bool GetRegisterInfoByName(llvm::StringRef name, RegisterInfo &info);

virtual const RegisterInfo *GetRegisterInfoArray(uint32_t &count) = 0;
};

class MCBasedABI : public ABI {
public:
void AugmentRegisterInfo(RegisterInfo &info) override;



static void MapRegisterName(std::string &reg, llvm::StringRef from_prefix,
llvm::StringRef to_prefix);
protected:
using ABI::ABI;


virtual std::pair<uint32_t, uint32_t> GetEHAndDWARFNums(llvm::StringRef reg);


virtual uint32_t GetGenericNum(llvm::StringRef reg) = 0;



virtual std::string GetMCName(std::string reg) { return reg; }
};

}

#endif
