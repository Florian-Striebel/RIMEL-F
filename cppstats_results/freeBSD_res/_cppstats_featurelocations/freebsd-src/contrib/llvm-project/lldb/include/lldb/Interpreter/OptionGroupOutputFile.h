







#if !defined(LLDB_INTERPRETER_OPTIONGROUPOUTPUTFILE_H)
#define LLDB_INTERPRETER_OPTIONGROUPOUTPUTFILE_H

#include "lldb/Interpreter/OptionValueBoolean.h"
#include "lldb/Interpreter/OptionValueFileSpec.h"
#include "lldb/Interpreter/Options.h"

namespace lldb_private {


class OptionGroupOutputFile : public OptionGroup {
public:
OptionGroupOutputFile();

~OptionGroupOutputFile() override = default;

llvm::ArrayRef<OptionDefinition> GetDefinitions() override;

Status SetOptionValue(uint32_t option_idx, llvm::StringRef option_value,
ExecutionContext *execution_context) override;

void OptionParsingStarting(ExecutionContext *execution_context) override;

const OptionValueFileSpec &GetFile() { return m_file; }

const OptionValueBoolean &GetAppend() { return m_append; }

bool AnyOptionWasSet() const {
return m_file.OptionWasSet() || m_append.OptionWasSet();
}

protected:
OptionValueFileSpec m_file;
OptionValueBoolean m_append;
};

}

#endif
