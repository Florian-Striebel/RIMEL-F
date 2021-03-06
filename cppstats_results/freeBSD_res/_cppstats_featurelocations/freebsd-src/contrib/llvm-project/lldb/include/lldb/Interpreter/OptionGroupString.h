







#if !defined(LLDB_INTERPRETER_OPTIONGROUPSTRING_H)
#define LLDB_INTERPRETER_OPTIONGROUPSTRING_H

#include "lldb/Interpreter/OptionValueString.h"
#include "lldb/Interpreter/Options.h"

namespace lldb_private {


class OptionGroupString : public OptionGroup {
public:
OptionGroupString(uint32_t usage_mask, bool required, const char *long_option,
int short_option, uint32_t completion_type,
lldb::CommandArgumentType argument_type,
const char *usage_text, const char *default_value);

~OptionGroupString() override = default;

llvm::ArrayRef<OptionDefinition> GetDefinitions() override {
return llvm::ArrayRef<OptionDefinition>(&m_option_definition, 1);
}

Status SetOptionValue(uint32_t option_idx, llvm::StringRef option_value,
ExecutionContext *execution_context) override;

void OptionParsingStarting(ExecutionContext *execution_context) override;

OptionValueString &GetOptionValue() { return m_value; }

const OptionValueString &GetOptionValue() const { return m_value; }

protected:
OptionValueString m_value;
OptionDefinition m_option_definition;
};

}

#endif
