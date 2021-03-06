







#if !defined(LLDB_INTERPRETER_OPTIONVALUEFILESPECLIST_H)
#define LLDB_INTERPRETER_OPTIONVALUEFILESPECLIST_H

#include <mutex>

#include "lldb/Core/FileSpecList.h"
#include "lldb/Interpreter/OptionValue.h"

namespace lldb_private {

class OptionValueFileSpecList
: public Cloneable<OptionValueFileSpecList, OptionValue> {
public:
OptionValueFileSpecList() = default;

OptionValueFileSpecList(const OptionValueFileSpecList &other)
: Cloneable(other), m_current_value(other.GetCurrentValue()) {}

~OptionValueFileSpecList() override = default;



OptionValue::Type GetType() const override { return eTypeFileSpecList; }

void DumpValue(const ExecutionContext *exe_ctx, Stream &strm,
uint32_t dump_mask) override;

Status
SetValueFromString(llvm::StringRef value,
VarSetOperationType op = eVarSetOperationAssign) override;

void Clear() override {
std::lock_guard<std::recursive_mutex> lock(m_mutex);
m_current_value.Clear();
m_value_was_set = false;
}

bool IsAggregateValue() const override { return true; }



FileSpecList GetCurrentValue() const {
std::lock_guard<std::recursive_mutex> lock(m_mutex);
return m_current_value;
}

void SetCurrentValue(const FileSpecList &value) {
std::lock_guard<std::recursive_mutex> lock(m_mutex);
m_current_value = value;
}

void AppendCurrentValue(const FileSpec &value) {
std::lock_guard<std::recursive_mutex> lock(m_mutex);
m_current_value.Append(value);
}

protected:
lldb::OptionValueSP Clone() const override;

mutable std::recursive_mutex m_mutex;
FileSpecList m_current_value;
};

}

#endif
