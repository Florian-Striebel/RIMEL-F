







#if !defined(LLDB_BREAKPOINT_STOPPOINTCALLBACKCONTEXT_H)
#define LLDB_BREAKPOINT_STOPPOINTCALLBACKCONTEXT_H

#include "lldb/Target/ExecutionContext.h"
#include "lldb/lldb-private.h"

namespace lldb_private {










class StoppointCallbackContext {
public:
StoppointCallbackContext();

StoppointCallbackContext(Event *event, const ExecutionContext &exe_ctx,
bool synchronously = false);





void Clear();


Event *event = nullptr;

ExecutionContextRef
exe_ctx_ref;
bool is_synchronous =
false;


};

}

#endif
