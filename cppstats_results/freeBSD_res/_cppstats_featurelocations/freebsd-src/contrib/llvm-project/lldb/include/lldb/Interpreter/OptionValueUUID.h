







#if !defined(LLDB_INTERPRETER_OPTIONVALUEUUID_H)
#define LLDB_INTERPRETER_OPTIONVALUEUUID_H

#include "lldb/Utility/UUID.h"
#include "lldb/Interpreter/OptionValue.h"

namespace lldb_private {

class OptionValueUUID : public Cloneable<OptionValueUUID, OptionValue> {
public:
OptionValueUUID() = default;

OptionValueUUID(const UUID &uuid) : m_uuid(uuid) {}

~OptionValueUUID() override = default;



OptionValue::Type GetType() const override { return eTypeUUID; }

void DumpValue(const ExecutionContext *exe_ctx, Stream &strm,
uint32_t dump_mask) override;

Status
SetValueFromString(llvm::StringRef value,
VarSetOperationType op = eVarSetOperationAssign) override;

void Clear() override {
m_uuid.Clear();
m_value_was_set = false;
}



UUID &GetCurrentValue() { return m_uuid; }

const UUID &GetCurrentValue() const { return m_uuid; }

void SetCurrentValue(const UUID &value) { m_uuid = value; }

void AutoComplete(CommandInterpreter &interpreter,
CompletionRequest &request) override;

protected:
UUID m_uuid;
};

}

#endif
