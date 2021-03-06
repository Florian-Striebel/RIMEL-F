







#if !defined(LLDB_INTERPRETER_OPTIONVALUEDICTIONARY_H)
#define LLDB_INTERPRETER_OPTIONVALUEDICTIONARY_H

#include <map>

#include "lldb/Interpreter/OptionValue.h"

namespace lldb_private {

class OptionValueDictionary
: public Cloneable<OptionValueDictionary, OptionValue> {
public:
OptionValueDictionary(uint32_t type_mask = UINT32_MAX,
bool raw_value_dump = true)
: m_type_mask(type_mask), m_raw_value_dump(raw_value_dump) {}

~OptionValueDictionary() override = default;



OptionValue::Type GetType() const override { return eTypeDictionary; }

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

bool IsHomogenous() const {
return ConvertTypeMaskToType(m_type_mask) != eTypeInvalid;
}



size_t GetNumValues() const { return m_values.size(); }

lldb::OptionValueSP GetValueForKey(ConstString key) const;

lldb::OptionValueSP GetSubValue(const ExecutionContext *exe_ctx,
llvm::StringRef name, bool will_modify,
Status &error) const override;

Status SetSubValue(const ExecutionContext *exe_ctx, VarSetOperationType op,
llvm::StringRef name, llvm::StringRef value) override;

bool SetValueForKey(ConstString key,
const lldb::OptionValueSP &value_sp,
bool can_replace = true);

bool DeleteValueForKey(ConstString key);

size_t GetArgs(Args &args) const;

Status SetArgs(const Args &args, VarSetOperationType op);

protected:
typedef std::map<ConstString, lldb::OptionValueSP> collection;
uint32_t m_type_mask;
collection m_values;
bool m_raw_value_dump;
};

}

#endif
