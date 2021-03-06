







#if !defined(LLDB_INTERPRETER_OPTIONVALUEARRAY_H)
#define LLDB_INTERPRETER_OPTIONVALUEARRAY_H

#include <vector>

#include "lldb/Interpreter/OptionValue.h"

namespace lldb_private {

class OptionValueArray : public Cloneable<OptionValueArray, OptionValue> {
public:
OptionValueArray(uint32_t type_mask = UINT32_MAX, bool raw_value_dump = false)
: m_type_mask(type_mask), m_values(), m_raw_value_dump(raw_value_dump) {}

~OptionValueArray() override = default;



OptionValue::Type GetType() const override { return eTypeArray; }

void DumpValue(const ExecutionContext *exe_ctx, Stream &strm,
uint32_t dump_mask) override;

Status
SetValueFromString(llvm::StringRef value,
VarSetOperationType op = eVarSetOperationAssign) override;

void Clear() override {
m_values.clear();
m_value_was_set = false;
}

lldb::OptionValueSP
DeepCopy(const lldb::OptionValueSP &new_parent) const override;

bool IsAggregateValue() const override { return true; }

lldb::OptionValueSP GetSubValue(const ExecutionContext *exe_ctx,
llvm::StringRef name, bool will_modify,
Status &error) const override;



size_t GetSize() const { return m_values.size(); }

lldb::OptionValueSP operator[](size_t idx) const {
lldb::OptionValueSP value_sp;
if (idx < m_values.size())
value_sp = m_values[idx];
return value_sp;
}

lldb::OptionValueSP GetValueAtIndex(size_t idx) const {
lldb::OptionValueSP value_sp;
if (idx < m_values.size())
value_sp = m_values[idx];
return value_sp;
}

bool AppendValue(const lldb::OptionValueSP &value_sp) {


if (value_sp && (m_type_mask & value_sp->GetTypeAsMask())) {
m_values.push_back(value_sp);
return true;
}
return false;
}

bool InsertValue(size_t idx, const lldb::OptionValueSP &value_sp) {


if (value_sp && (m_type_mask & value_sp->GetTypeAsMask())) {
if (idx < m_values.size())
m_values.insert(m_values.begin() + idx, value_sp);
else
m_values.push_back(value_sp);
return true;
}
return false;
}

bool ReplaceValue(size_t idx, const lldb::OptionValueSP &value_sp) {


if (value_sp && (m_type_mask & value_sp->GetTypeAsMask())) {
if (idx < m_values.size()) {
m_values[idx] = value_sp;
return true;
}
}
return false;
}

bool DeleteValue(size_t idx) {
if (idx < m_values.size()) {
m_values.erase(m_values.begin() + idx);
return true;
}
return false;
}

size_t GetArgs(Args &args) const;

Status SetArgs(const Args &args, VarSetOperationType op);

protected:
typedef std::vector<lldb::OptionValueSP> collection;

uint32_t m_type_mask;
collection m_values;
bool m_raw_value_dump;
};

}

#endif
