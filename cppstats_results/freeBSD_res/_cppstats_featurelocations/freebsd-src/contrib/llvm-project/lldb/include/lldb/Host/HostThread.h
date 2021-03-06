







#if !defined(LLDB_HOST_HOSTTHREAD_H)
#define LLDB_HOST_HOSTTHREAD_H

#include "lldb/Host/HostNativeThreadForward.h"
#include "lldb/Utility/Status.h"
#include "lldb/lldb-types.h"

#include <memory>

namespace lldb_private {

class HostNativeThreadBase;








class HostThread {
public:
HostThread();
HostThread(lldb::thread_t thread);

Status Join(lldb::thread_result_t *result);
Status Cancel();
void Reset();
lldb::thread_t Release();

bool IsJoinable() const;
HostNativeThread &GetNativeThread();
const HostNativeThread &GetNativeThread() const;
lldb::thread_result_t GetResult() const;

bool EqualsThread(lldb::thread_t thread) const;

private:
std::shared_ptr<HostNativeThreadBase> m_native_thread;
};
}

#endif
