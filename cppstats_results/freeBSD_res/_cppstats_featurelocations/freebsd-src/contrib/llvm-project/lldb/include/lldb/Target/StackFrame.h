







#if !defined(LLDB_TARGET_STACKFRAME_H)
#define LLDB_TARGET_STACKFRAME_H

#include <memory>
#include <mutex>

#include "lldb/Utility/Flags.h"

#include "lldb/Core/ValueObjectList.h"
#include "lldb/Symbol/SymbolContext.h"
#include "lldb/Target/ExecutionContextScope.h"
#include "lldb/Target/StackID.h"
#include "lldb/Utility/Scalar.h"
#include "lldb/Utility/Status.h"
#include "lldb/Utility/StreamString.h"
#include "lldb/Utility/UserID.h"

namespace lldb_private {













class StackFrame : public ExecutionContextScope,
public std::enable_shared_from_this<StackFrame> {
public:
enum ExpressionPathOption {
eExpressionPathOptionCheckPtrVsMember = (1u << 0),
eExpressionPathOptionsNoFragileObjcIvar = (1u << 1),
eExpressionPathOptionsNoSyntheticChildren = (1u << 2),
eExpressionPathOptionsNoSyntheticArrayRange = (1u << 3),
eExpressionPathOptionsAllowDirectIVarAccess = (1u << 4),
eExpressionPathOptionsInspectAnonymousUnions = (1u << 5)
};

enum class Kind {

Regular,



History,




Artificial
};









































StackFrame(const lldb::ThreadSP &thread_sp, lldb::user_id_t frame_idx,
lldb::user_id_t concrete_frame_idx, lldb::addr_t cfa,
bool cfa_is_valid, lldb::addr_t pc, Kind frame_kind,
bool behaves_like_zeroth_frame, const SymbolContext *sc_ptr);

StackFrame(const lldb::ThreadSP &thread_sp, lldb::user_id_t frame_idx,
lldb::user_id_t concrete_frame_idx,
const lldb::RegisterContextSP &reg_context_sp, lldb::addr_t cfa,
lldb::addr_t pc, bool behaves_like_zeroth_frame,
const SymbolContext *sc_ptr);

StackFrame(const lldb::ThreadSP &thread_sp, lldb::user_id_t frame_idx,
lldb::user_id_t concrete_frame_idx,
const lldb::RegisterContextSP &reg_context_sp, lldb::addr_t cfa,
const Address &pc, bool behaves_like_zeroth_frame,
const SymbolContext *sc_ptr);

~StackFrame() override;

lldb::ThreadSP GetThread() const { return m_thread_wp.lock(); }

StackID &GetStackID();







const Address &GetFrameCodeAddress();

















Address GetFrameCodeAddressForSymbolication();











bool ChangePC(lldb::addr_t pc);















const SymbolContext &GetSymbolContext(lldb::SymbolContextItem resolve_scope);





















bool GetFrameBaseValue(Scalar &value, Status *error_ptr);











DWARFExpression *GetFrameBaseExpression(Status *error_ptr);









Block *GetFrameBlock();













lldb::RegisterContextSP GetRegisterContext();

const lldb::RegisterContextSP &GetRegisterContextSP() const {
return m_reg_context_sp;
}















VariableList *GetVariableList(bool get_file_globals);















lldb::VariableListSP
GetInScopeVariableList(bool get_file_globals,
bool must_have_valid_location = false);


























lldb::ValueObjectSP GetValueForVariableExpressionPath(
llvm::StringRef var_expr, lldb::DynamicValueType use_dynamic,
uint32_t options, lldb::VariableSP &var_sp, Status &error);






bool HasDebugInformation();






const char *Disassemble();












void DumpUsingSettingsFormat(Stream *strm, bool show_unique = false,
const char *frame_marker = nullptr);











void Dump(Stream *strm, bool show_frame_index, bool show_fullpaths);






















bool GetStatus(Stream &strm, bool show_frame_info, bool show_source,
bool show_unique = false, const char *frame_marker = nullptr);







bool IsInlined();


bool IsHistorical() const;




bool IsArtificial() const;







uint32_t GetFrameIndex() const;


void SetFrameIndex(uint32_t index) { m_frame_index = index; }









uint32_t GetConcreteFrameIndex() const { return m_concrete_frame_index; }













lldb::ValueObjectSP
GetValueObjectForFrameVariable(const lldb::VariableSP &variable_sp,
lldb::DynamicValueType use_dynamic);






lldb::LanguageType GetLanguage();



lldb::LanguageType GuessLanguage();










lldb::ValueObjectSP GuessValueForAddress(lldb::addr_t addr);













lldb::ValueObjectSP GuessValueForRegisterAndOffset(ConstString reg,
int64_t offset);












lldb::ValueObjectSP FindVariable(ConstString name);


lldb::TargetSP CalculateTarget() override;

lldb::ProcessSP CalculateProcess() override;

lldb::ThreadSP CalculateThread() override;

lldb::StackFrameSP CalculateStackFrame() override;

void CalculateExecutionContext(ExecutionContext &exe_ctx) override;

lldb::RecognizedStackFrameSP GetRecognizedFrame();

protected:
friend class StackFrameList;

void SetSymbolContextScope(SymbolContextScope *symbol_scope);

void UpdateCurrentFrameFromPreviousFrame(StackFrame &prev_frame);

void UpdatePreviousFrameFromCurrentFrame(StackFrame &curr_frame);

bool HasCachedData() const;

private:

lldb::ThreadWP m_thread_wp;
uint32_t m_frame_index;
uint32_t m_concrete_frame_index;
lldb::RegisterContextSP m_reg_context_sp;
StackID m_id;
Address m_frame_code_addr;


SymbolContext m_sc;
Flags m_flags;
Scalar m_frame_base;
Status m_frame_base_error;
bool m_cfa_is_valid;

Kind m_stack_frame_kind;





bool m_behaves_like_zeroth_frame;
lldb::VariableListSP m_variable_list_sp;
ValueObjectList m_variable_list_value_objects;


lldb::RecognizedStackFrameSP m_recognized_frame_sp;
StreamString m_disassembly;
std::recursive_mutex m_mutex;

StackFrame(const StackFrame &) = delete;
const StackFrame &operator=(const StackFrame &) = delete;
};

}

#endif
