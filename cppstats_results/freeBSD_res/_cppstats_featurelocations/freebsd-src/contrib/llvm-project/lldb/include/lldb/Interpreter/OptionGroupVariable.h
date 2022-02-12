







#if !defined(LLDB_INTERPRETER_OPTIONGROUPVARIABLE_H)
#define LLDB_INTERPRETER_OPTIONGROUPVARIABLE_H

#include "lldb/Interpreter/OptionValueString.h"
#include "lldb/Interpreter/Options.h"

namespace lldb_private {



class OptionGroupVariable : public OptionGroup {
public:
OptionGroupVariable(bool show_frame_options);

~OptionGroupVariable() override = default;

llvm::ArrayRef<OptionDefinition> GetDefinitions() override;

Status SetOptionValue(uint32_t option_idx, llvm::StringRef option_value,
ExecutionContext *execution_context) override;

void OptionParsingStarting(ExecutionContext *execution_context) override;

bool include_frame_options : 1,
show_args : 1,
show_recognized_args : 1,

show_locals : 1,
show_globals : 1,
use_regex : 1, show_scope : 1, show_decl : 1;
OptionValueString summary;
OptionValueString summary_string;

private:
OptionGroupVariable(const OptionGroupVariable &) = delete;
const OptionGroupVariable &operator=(const OptionGroupVariable &) = delete;
};

}

#endif
