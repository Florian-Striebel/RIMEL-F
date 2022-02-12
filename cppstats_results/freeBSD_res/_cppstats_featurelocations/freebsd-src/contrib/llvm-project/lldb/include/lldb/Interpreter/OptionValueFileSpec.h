







#if !defined(LLDB_INTERPRETER_OPTIONVALUEFILESPEC_H)
#define LLDB_INTERPRETER_OPTIONVALUEFILESPEC_H

#include "lldb/Interpreter/CommandCompletions.h"
#include "lldb/Interpreter/OptionValue.h"

#include "lldb/Utility/FileSpec.h"
#include "llvm/Support/Chrono.h"

namespace lldb_private {

class OptionValueFileSpec : public Cloneable<OptionValueFileSpec, OptionValue> {
public:
OptionValueFileSpec(bool resolve = true);

OptionValueFileSpec(const FileSpec &value, bool resolve = true);

OptionValueFileSpec(const FileSpec &current_value,
const FileSpec &default_value, bool resolve = true);

~OptionValueFileSpec() override = default;



OptionValue::Type GetType() const override { return eTypeFileSpec; }

void DumpValue(const ExecutionContext *exe_ctx, Stream &strm,
uint32_t dump_mask) override;

Status
SetValueFromString(llvm::StringRef value,
VarSetOperationType op = eVarSetOperationAssign) override;

void Clear() override {
m_current_value = m_default_value;
m_value_was_set = false;
m_data_sp.reset();
m_data_mod_time = llvm::sys::TimePoint<>();
}

void AutoComplete(CommandInterpreter &interpreter,
CompletionRequest &request) override;



FileSpec &GetCurrentValue() { return m_current_value; }

const FileSpec &GetCurrentValue() const { return m_current_value; }

const FileSpec &GetDefaultValue() const { return m_default_value; }

void SetCurrentValue(const FileSpec &value, bool set_value_was_set) {
m_current_value = value;
if (set_value_was_set)
m_value_was_set = true;
m_data_sp.reset();
}

void SetDefaultValue(const FileSpec &value) { m_default_value = value; }

const lldb::DataBufferSP &GetFileContents();

void SetCompletionMask(uint32_t mask) { m_completion_mask = mask; }

protected:
FileSpec m_current_value;
FileSpec m_default_value;
lldb::DataBufferSP m_data_sp;
llvm::sys::TimePoint<> m_data_mod_time;
uint32_t m_completion_mask = CommandCompletions::eDiskFileCompletion;
bool m_resolve;
};

}

#endif
