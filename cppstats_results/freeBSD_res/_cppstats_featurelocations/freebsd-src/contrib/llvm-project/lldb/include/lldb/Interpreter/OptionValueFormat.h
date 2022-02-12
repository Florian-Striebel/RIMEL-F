







#if !defined(LLDB_INTERPRETER_OPTIONVALUEFORMAT_H)
#define LLDB_INTERPRETER_OPTIONVALUEFORMAT_H

#include "lldb/Interpreter/OptionValue.h"

namespace lldb_private {

class OptionValueFormat
: public Cloneable<OptionValueFormat, OptionValue> {
public:
OptionValueFormat(lldb::Format value)
: m_current_value(value), m_default_value(value) {}

OptionValueFormat(lldb::Format current_value, lldb::Format default_value)
: m_current_value(current_value), m_default_value(default_value) {}

~OptionValueFormat() override = default;



OptionValue::Type GetType() const override { return eTypeFormat; }

void DumpValue(const ExecutionContext *exe_ctx, Stream &strm,
uint32_t dump_mask) override;

Status
SetValueFromString(llvm::StringRef value,
VarSetOperationType op = eVarSetOperationAssign) override;

void Clear() override {
m_current_value = m_default_value;
m_value_was_set = false;
}



lldb::Format GetCurrentValue() const { return m_current_value; }

lldb::Format GetDefaultValue() const { return m_default_value; }

void SetCurrentValue(lldb::Format value) { m_current_value = value; }

void SetDefaultValue(lldb::Format value) { m_default_value = value; }

protected:
lldb::Format m_current_value;
lldb::Format m_default_value;
};

}

#endif
