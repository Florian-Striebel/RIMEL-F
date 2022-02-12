







#if !defined(LLDB_HOST_MAINLOOPBASE_H)
#define LLDB_HOST_MAINLOOPBASE_H

#include "lldb/Utility/IOObject.h"
#include "lldb/Utility/Status.h"
#include "llvm/Support/ErrorHandling.h"
#include <functional>

namespace lldb_private {













class MainLoopBase {
private:
class ReadHandle;

public:
MainLoopBase() = default;
virtual ~MainLoopBase() = default;

typedef std::unique_ptr<ReadHandle> ReadHandleUP;

typedef std::function<void(MainLoopBase &)> Callback;

virtual ReadHandleUP RegisterReadObject(const lldb::IOObjectSP &object_sp,
const Callback &callback,
Status &error) {
llvm_unreachable("Not implemented");
}



virtual Status Run() { llvm_unreachable("Not implemented"); }


virtual void RequestTermination() { llvm_unreachable("Not implemented"); }

protected:
ReadHandleUP CreateReadHandle(const lldb::IOObjectSP &object_sp) {
return ReadHandleUP(new ReadHandle(*this, object_sp->GetWaitableHandle()));
}

virtual void UnregisterReadObject(IOObject::WaitableHandle handle) {
llvm_unreachable("Not implemented");
}

private:
class ReadHandle {
public:
~ReadHandle() { m_mainloop.UnregisterReadObject(m_handle); }

private:
ReadHandle(MainLoopBase &mainloop, IOObject::WaitableHandle handle)
: m_mainloop(mainloop), m_handle(handle) {}

MainLoopBase &m_mainloop;
IOObject::WaitableHandle m_handle;

friend class MainLoopBase;
ReadHandle(const ReadHandle &) = delete;
const ReadHandle &operator=(const ReadHandle &) = delete;
};

MainLoopBase(const MainLoopBase &) = delete;
const MainLoopBase &operator=(const MainLoopBase &) = delete;
};

}

#endif
