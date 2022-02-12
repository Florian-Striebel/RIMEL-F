







#if !defined(LLDB_TARGET_RUNTIME_H)
#define LLDB_TARGET_RUNTIME_H

#include "lldb/Target/Process.h"

namespace lldb_private {
class Runtime {
public:
Runtime(Process *process) : m_process(process) {}
virtual ~Runtime() = default;
Runtime(const Runtime &) = delete;
const Runtime &operator=(const Runtime &) = delete;

Process *GetProcess() { return m_process; }
Target &GetTargetRef() { return m_process->GetTarget(); }


virtual void ModulesDidLoad(const ModuleList &module_list) = 0;

protected:
Process *m_process;
};
}

#endif
