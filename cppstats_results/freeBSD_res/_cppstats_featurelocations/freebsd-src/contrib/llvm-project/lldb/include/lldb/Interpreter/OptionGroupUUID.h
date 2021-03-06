







#if !defined(LLDB_INTERPRETER_OPTIONGROUPUUID_H)
#define LLDB_INTERPRETER_OPTIONGROUPUUID_H

#include "lldb/Interpreter/OptionValueUUID.h"
#include "lldb/Interpreter/Options.h"

namespace lldb_private {



class OptionGroupUUID : public OptionGroup {
public:
OptionGroupUUID() = default;

~OptionGroupUUID() override = default;

llvm::ArrayRef<OptionDefinition> GetDefinitions() override;

Status SetOptionValue(uint32_t option_idx, llvm::StringRef option_value,
ExecutionContext *execution_context) override;

void OptionParsingStarting(ExecutionContext *execution_context) override;

const OptionValueUUID &GetOptionValue() const { return m_uuid; }

protected:
OptionValueUUID m_uuid;
};

}

#endif
