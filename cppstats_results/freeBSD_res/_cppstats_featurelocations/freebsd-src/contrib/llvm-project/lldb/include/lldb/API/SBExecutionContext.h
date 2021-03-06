








#if !defined(LLDB_API_SBEXECUTIONCONTEXT_H)
#define LLDB_API_SBEXECUTIONCONTEXT_H

#include "lldb/API/SBDefines.h"

#include <cstdio>
#include <vector>

namespace lldb {

class LLDB_API SBExecutionContext {
friend class SBCommandInterpreter;

public:
SBExecutionContext();

SBExecutionContext(const lldb::SBExecutionContext &rhs);

SBExecutionContext(lldb::ExecutionContextRefSP exe_ctx_ref_sp);

SBExecutionContext(const lldb::SBTarget &target);

SBExecutionContext(const lldb::SBProcess &process);

SBExecutionContext(lldb::SBThread thread);



SBExecutionContext(const lldb::SBFrame &frame);

~SBExecutionContext();

const SBExecutionContext &operator=(const lldb::SBExecutionContext &rhs);

SBTarget GetTarget() const;

SBProcess GetProcess() const;

SBThread GetThread() const;

SBFrame GetFrame() const;

protected:
void reset(lldb::ExecutionContextRefSP &event_sp);

lldb_private::ExecutionContextRef *get() const;

private:
mutable lldb::ExecutionContextRefSP m_exe_ctx_sp;
};

}

#endif
