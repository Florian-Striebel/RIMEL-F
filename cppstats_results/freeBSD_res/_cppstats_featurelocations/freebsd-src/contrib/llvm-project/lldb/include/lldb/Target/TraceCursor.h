







#if !defined(LLDB_TARGET_TRACE_CURSOR_H)
#define LLDB_TARGET_TRACE_CURSOR_H

#include "lldb/lldb-private.h"

#include "lldb/Target/ExecutionContext.h"

namespace lldb_private {
























































class TraceCursor {
public:


enum class SeekType {

Set = 0,

Current,

End
};



TraceCursor(lldb::ThreadSP thread_sp);

virtual ~TraceCursor() = default;


void SetGranularity(lldb::TraceInstructionControlFlowType granularity);


void SetIgnoreErrors(bool ignore_errors);





void SetForwards(bool forwards);






bool IsForwards() const;
























virtual bool Next() = 0;




















virtual size_t Seek(ssize_t offset, SeekType origin) = 0;




ExecutionContextRef &GetExecutionContextRef();






virtual bool IsError() = 0;







virtual llvm::Error GetError() = 0;





virtual lldb::addr_t GetLoadAddress() = 0;








virtual llvm::Optional<uint64_t> GetTimestampCounter() = 0;





virtual lldb::TraceInstructionControlFlowType
GetInstructionControlFlowType() = 0;


protected:
ExecutionContextRef m_exe_ctx_ref;

lldb::TraceInstructionControlFlowType m_granularity =
lldb::eTraceInstructionControlFlowTypeInstruction;
bool m_ignore_errors = false;
bool m_forwards = false;
};

}

#endif
