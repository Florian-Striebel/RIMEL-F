








#if !defined(LLDB_TARGET_OPERATINGSYSTEM_H)
#define LLDB_TARGET_OPERATINGSYSTEM_H

#include "lldb/Core/PluginInterface.h"
#include "lldb/lldb-private.h"

namespace lldb_private {










class OperatingSystem : public PluginInterface {
public:












static OperatingSystem *FindPlugin(Process *process, const char *plugin_name);

OperatingSystem(Process *process);


virtual bool UpdateThreadList(ThreadList &old_thread_list,
ThreadList &real_thread_list,
ThreadList &new_thread_list) = 0;

virtual void ThreadWasSelected(Thread *thread) = 0;

virtual lldb::RegisterContextSP
CreateRegisterContextForThread(Thread *thread,
lldb::addr_t reg_data_addr) = 0;

virtual lldb::StopInfoSP CreateThreadStopReason(Thread *thread) = 0;

virtual lldb::ThreadSP CreateThread(lldb::tid_t tid, lldb::addr_t context) {
return lldb::ThreadSP();
}

virtual bool IsOperatingSystemPluginThread(const lldb::ThreadSP &thread_sp);

protected:

Process
*m_process;
};

}

#endif
