







#if !defined(LLDB_INTERPRETER_OPTIONGROUPUINT64_H)
#define LLDB_INTERPRETER_OPTIONGROUPUINT64_H

#include "lldb/Interpreter/OptionValueUInt64.h"
#include "lldb/Interpreter/Options.h"

namespace lldb_private {



class OptionGroupUInt64 : public OptionGroup {
public:
OptionGroupUInt64(uint32_t usage_mask, bool required, const char *long_option,
int short_option, uint32_t completion_type,
lldb::CommandArgumentType argument_type,
const char *usage_text, uint64_t default_value);

~OptionGroupUInt64() override = default;

llvm::ArrayRef<OptionDefinition> GetDefinitions() override {
return llvm::ArrayRef<OptionDefinition>(&m_option_definition, 1);
}

Status SetOptionValue(uint32_t option_idx, llvm::StringRef option_value,
ExecutionContext *execution_context) override;

void OptionParsingStarting(ExecutionContext *execution_context) override;

OptionValueUInt64 &GetOptionValue() { return m_value; }

const OptionValueUInt64 &GetOptionValue() const { return m_value; }

protected:
OptionValueUInt64 m_value;
OptionDefinition m_option_definition;
};

}

#endif