







#if !defined(LLDB_INTERPRETER_COMMANDOPTIONVALIDATORS_H)
#define LLDB_INTERPRETER_COMMANDOPTIONVALIDATORS_H

#include "lldb/lldb-private-types.h"

namespace lldb_private {

class Platform;
class ExecutionContext;

class PosixPlatformCommandOptionValidator : public OptionValidator {
bool IsValid(Platform &platform,
const ExecutionContext &target) const override;
const char *ShortConditionString() const override;
const char *LongConditionString() const override;
};

}

#endif
