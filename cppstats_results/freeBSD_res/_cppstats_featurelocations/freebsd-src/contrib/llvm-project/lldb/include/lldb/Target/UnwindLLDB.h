







#if !defined(LLDB_TARGET_UNWINDLLDB_H)
#define LLDB_TARGET_UNWINDLLDB_H

#include <vector>

#include "lldb/Symbol/FuncUnwinders.h"
#include "lldb/Symbol/SymbolContext.h"
#include "lldb/Symbol/UnwindPlan.h"
#include "lldb/Target/RegisterContext.h"
#include "lldb/Target/Unwind.h"
#include "lldb/Utility/ConstString.h"
#include "lldb/lldb-public.h"

namespace lldb_private {

class RegisterContextUnwind;

class UnwindLLDB : public lldb_private::Unwind {
public:
UnwindLLDB(lldb_private::Thread &thread);

~UnwindLLDB() override = default;

enum RegisterSearchResult {
eRegisterFound = 0,
eRegisterNotFound,
eRegisterIsVolatile
};

protected:
friend class lldb_private::RegisterContextUnwind;

struct RegisterLocation {
enum RegisterLocationTypes {
eRegisterNotSaved = 0,

eRegisterSavedAtMemoryLocation,

eRegisterInRegister,

eRegisterSavedAtHostMemoryLocation,

eRegisterValueInferred,

eRegisterInLiveRegisterContext

};
int type;
union {
lldb::addr_t target_memory_location;
uint32_t
register_number;
void *host_memory_location;
uint64_t inferred_value;

} location;
};

void DoClear() override {
m_frames.clear();
m_candidate_frame.reset();
m_unwind_complete = false;
}

uint32_t DoGetFrameCount() override;

bool DoGetFrameInfoAtIndex(uint32_t frame_idx, lldb::addr_t &cfa,
lldb::addr_t &start_pc,
bool &behaves_like_zeroth_frame) override;

lldb::RegisterContextSP
DoCreateRegisterContextForFrame(lldb_private::StackFrame *frame) override;

typedef std::shared_ptr<RegisterContextUnwind> RegisterContextLLDBSP;





RegisterContextLLDBSP GetRegisterContextForFrameNum(uint32_t frame_num);



bool SearchForSavedLocationForRegister(
uint32_t lldb_regnum, lldb_private::UnwindLLDB::RegisterLocation &regloc,
uint32_t starting_frame_num, bool pc_register);











const std::vector<ConstString> &GetUserSpecifiedTrapHandlerFunctionNames() {
return m_user_supplied_trap_handler_functions;
}

private:
struct Cursor {
lldb::addr_t start_pc =
LLDB_INVALID_ADDRESS;

lldb::addr_t cfa = LLDB_INVALID_ADDRESS;

lldb_private::SymbolContext sctx;

RegisterContextLLDBSP
reg_ctx_lldb_sp;

Cursor() : sctx(), reg_ctx_lldb_sp() {}

private:
Cursor(const Cursor &) = delete;
const Cursor &operator=(const Cursor &) = delete;
};

typedef std::shared_ptr<Cursor> CursorSP;
std::vector<CursorSP> m_frames;
CursorSP m_candidate_frame;
bool m_unwind_complete;





std::vector<ConstString> m_user_supplied_trap_handler_functions;





void UpdateUnwindPlanForFirstFrameIfInvalid(ABI *abi);

CursorSP GetOneMoreFrame(ABI *abi);

bool AddOneMoreFrame(ABI *abi);

bool AddFirstFrame();


UnwindLLDB(const UnwindLLDB &) = delete;
const UnwindLLDB &operator=(const UnwindLLDB &) = delete;
};

}

#endif
