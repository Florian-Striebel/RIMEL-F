







#if !defined(LLDB_TARGET_EXECUTIONCONTEXTSCOPE_H)
#define LLDB_TARGET_EXECUTIONCONTEXTSCOPE_H

#include "lldb/lldb-private.h"

namespace lldb_private {

















class ExecutionContextScope {
public:
virtual ~ExecutionContextScope() = default;

virtual lldb::TargetSP CalculateTarget() = 0;

virtual lldb::ProcessSP CalculateProcess() = 0;

virtual lldb::ThreadSP CalculateThread() = 0;

virtual lldb::StackFrameSP CalculateStackFrame() = 0;










virtual void CalculateExecutionContext(ExecutionContext &exe_ctx) = 0;
};

}

#endif
