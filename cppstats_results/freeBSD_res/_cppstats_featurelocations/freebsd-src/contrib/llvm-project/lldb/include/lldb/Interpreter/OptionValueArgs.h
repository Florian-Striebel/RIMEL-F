







#if !defined(LLDB_INTERPRETER_OPTIONVALUEARGS_H)
#define LLDB_INTERPRETER_OPTIONVALUEARGS_H

#include "lldb/Interpreter/OptionValueArray.h"

namespace lldb_private {

class OptionValueArgs : public Cloneable<OptionValueArgs, OptionValueArray> {
public:
OptionValueArgs()
: Cloneable(OptionValue::ConvertTypeToMask(OptionValue::eTypeString)) {}

~OptionValueArgs() override = default;

size_t GetArgs(Args &args) const;

Type GetType() const override { return eTypeArgs; }
};

}

#endif
