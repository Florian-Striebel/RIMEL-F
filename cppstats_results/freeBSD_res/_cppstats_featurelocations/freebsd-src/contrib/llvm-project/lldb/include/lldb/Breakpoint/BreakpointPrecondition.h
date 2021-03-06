







#if !defined(LLDB_BREAKPOINT_BREAKPOINTPRECONDITION_H)
#define LLDB_BREAKPOINT_BREAKPOINTPRECONDITION_H

#include "lldb/lldb-enumerations.h"

namespace lldb_private {

class Args;
class Status;
class StoppointCallbackContext;
class Stream;

class BreakpointPrecondition {
public:
virtual ~BreakpointPrecondition() = default;
virtual bool EvaluatePrecondition(StoppointCallbackContext &context);
virtual Status ConfigurePrecondition(Args &args);
virtual void GetDescription(Stream &stream, lldb::DescriptionLevel level);
};
}

#endif
