







#if !defined(LLDB_INTERPRETER_OPTIONGROUPWATCHPOINT_H)
#define LLDB_INTERPRETER_OPTIONGROUPWATCHPOINT_H

#include "lldb/Interpreter/Options.h"

namespace lldb_private {



class OptionGroupWatchpoint : public OptionGroup {
public:
OptionGroupWatchpoint() = default;

~OptionGroupWatchpoint() override = default;

static bool IsWatchSizeSupported(uint32_t watch_size);

llvm::ArrayRef<OptionDefinition> GetDefinitions() override;

Status SetOptionValue(uint32_t option_idx, llvm::StringRef option_value,
ExecutionContext *execution_context) override;

void OptionParsingStarting(ExecutionContext *execution_context) override;




enum WatchType {
eWatchInvalid = 0,
eWatchRead,
eWatchWrite,
eWatchReadWrite
};

WatchType watch_type;
uint32_t watch_size;
bool watch_type_specified;

private:
OptionGroupWatchpoint(const OptionGroupWatchpoint &) = delete;
const OptionGroupWatchpoint &
operator=(const OptionGroupWatchpoint &) = delete;
};

}

#endif
