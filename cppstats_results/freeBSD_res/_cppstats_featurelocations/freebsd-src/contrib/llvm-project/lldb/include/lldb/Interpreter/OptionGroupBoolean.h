







#if !defined(LLDB_INTERPRETER_OPTIONGROUPBOOLEAN_H)
#define LLDB_INTERPRETER_OPTIONGROUPBOOLEAN_H

#include "lldb/Interpreter/OptionValueBoolean.h"
#include "lldb/Interpreter/Options.h"

namespace lldb_private {


class OptionGroupBoolean : public OptionGroup {
public:



OptionGroupBoolean(uint32_t usage_mask, bool required,
const char *long_option, int short_option,
const char *usage_text, bool default_value,
bool no_argument_toggle_default);

~OptionGroupBoolean() override = default;

llvm::ArrayRef<OptionDefinition> GetDefinitions() override {
return llvm::ArrayRef<OptionDefinition>(&m_option_definition, 1);
}

Status SetOptionValue(uint32_t option_idx, llvm::StringRef option_value,
ExecutionContext *execution_context) override;

void OptionParsingStarting(ExecutionContext *execution_context) override;

OptionValueBoolean &GetOptionValue() { return m_value; }

const OptionValueBoolean &GetOptionValue() const { return m_value; }

protected:
OptionValueBoolean m_value;
OptionDefinition m_option_definition;
};

}

#endif
