







#if !defined(LLDB_TARGET_REGISTERCONTEXTUNWIND_H)
#define LLDB_TARGET_REGISTERCONTEXTUNWIND_H

#include <vector>

#include "lldb/Symbol/SymbolContext.h"
#include "lldb/Symbol/UnwindPlan.h"
#include "lldb/Target/RegisterContext.h"
#include "lldb/Target/RegisterNumber.h"
#include "lldb/Target/UnwindLLDB.h"
#include "lldb/lldb-private.h"

namespace lldb_private {

class UnwindLLDB;

class RegisterContextUnwind : public lldb_private::RegisterContext {
public:
typedef std::shared_ptr<RegisterContextUnwind> SharedPtr;

RegisterContextUnwind(lldb_private::Thread &thread,
const SharedPtr &next_frame,
lldb_private::SymbolContext &sym_ctx,
uint32_t frame_number,
lldb_private::UnwindLLDB &unwind_lldb);

~RegisterContextUnwind() override = default;

void InvalidateAllRegisters() override;

size_t GetRegisterCount() override;

const lldb_private::RegisterInfo *GetRegisterInfoAtIndex(size_t reg) override;

size_t GetRegisterSetCount() override;

const lldb_private::RegisterSet *GetRegisterSet(size_t reg_set) override;

bool ReadRegister(const lldb_private::RegisterInfo *reg_info,
lldb_private::RegisterValue &value) override;

bool WriteRegister(const lldb_private::RegisterInfo *reg_info,
const lldb_private::RegisterValue &value) override;

bool ReadAllRegisterValues(lldb::DataBufferSP &data_sp) override;

bool WriteAllRegisterValues(const lldb::DataBufferSP &data_sp) override;

uint32_t ConvertRegisterKindToRegisterNumber(lldb::RegisterKind kind,
uint32_t num) override;

bool IsValid() const;

bool IsTrapHandlerFrame() const;

bool GetCFA(lldb::addr_t &cfa);

bool GetStartPC(lldb::addr_t &start_pc);

bool ReadPC(lldb::addr_t &start_pc);




bool BehavesLikeZerothFrame() const override;

private:
enum FrameType {
eNormalFrame,
eTrapHandlerFrame,
eDebuggerFrame,

eSkipFrame,

eNotAValidFrame

};


friend class UnwindLLDB;



bool CheckIfLoopingStack();



bool IsFrameZero() const;

void InitializeZerothFrame();

void InitializeNonZerothFrame();

SharedPtr GetNextFrame() const;

SharedPtr GetPrevFrame() const;











bool IsSkipFrame() const;








bool IsTrapHandlerSymbol(lldb_private::Process *process,
const lldb_private::SymbolContext &m_sym_ctx) const;



void PropagateTrapHandlerFlagFromUnwindPlan(lldb::UnwindPlanSP unwind_plan);



















lldb_private::UnwindLLDB::RegisterSearchResult
SavedLocationForRegister(uint32_t lldb_regnum,
lldb_private::UnwindLLDB::RegisterLocation &regloc);

bool ReadRegisterValueFromRegisterLocation(
lldb_private::UnwindLLDB::RegisterLocation regloc,
const lldb_private::RegisterInfo *reg_info,
lldb_private::RegisterValue &value);

bool WriteRegisterValueToRegisterLocation(
lldb_private::UnwindLLDB::RegisterLocation regloc,
const lldb_private::RegisterInfo *reg_info,
const lldb_private::RegisterValue &value);











bool TryFallbackUnwindPlan();








bool ForceSwitchToFallbackUnwindPlan();




bool ReadGPRValue(lldb::RegisterKind register_kind, uint32_t regnum,
lldb::addr_t &value);

bool ReadGPRValue(const RegisterNumber &reg_num, lldb::addr_t &value);


bool ReadFrameAddress(lldb::RegisterKind register_kind,
UnwindPlan::Row::FAValue &fa, lldb::addr_t &address);

lldb::UnwindPlanSP GetFastUnwindPlanForFrame();

lldb::UnwindPlanSP GetFullUnwindPlanForFrame();

void UnwindLogMsg(const char *fmt, ...) __attribute__((format(printf, 2, 3)));

void UnwindLogMsgVerbose(const char *fmt, ...)
__attribute__((format(printf, 2, 3)));

bool IsUnwindPlanValidForCurrentPC(lldb::UnwindPlanSP unwind_plan_sp,
int &valid_pc_offset);

lldb::addr_t GetReturnAddressHint(int32_t plan_offset);

lldb_private::Thread &m_thread;







lldb::UnwindPlanSP m_fast_unwind_plan_sp;
lldb::UnwindPlanSP m_full_unwind_plan_sp;
lldb::UnwindPlanSP m_fallback_unwind_plan_sp;

bool m_all_registers_available;

int m_frame_type;

lldb::addr_t m_cfa;
lldb::addr_t m_afa;
lldb_private::Address m_start_pc;
lldb_private::Address m_current_pc;

int m_current_offset;









int m_current_offset_backed_up_one;


bool m_behaves_like_zeroth_frame;

lldb_private::SymbolContext &m_sym_ctx;
bool m_sym_ctx_valid;


uint32_t m_frame_number;

std::map<uint32_t, lldb_private::UnwindLLDB::RegisterLocation>
m_registers;

lldb_private::UnwindLLDB &m_parent_unwind;


RegisterContextUnwind(const RegisterContextUnwind &) = delete;
const RegisterContextUnwind &
operator=(const RegisterContextUnwind &) = delete;
};

}

#endif
