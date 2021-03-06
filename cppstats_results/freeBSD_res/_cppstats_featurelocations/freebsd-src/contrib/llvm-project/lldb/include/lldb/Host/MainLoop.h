







#if !defined(LLDB_HOST_MAINLOOP_H)
#define LLDB_HOST_MAINLOOP_H

#include "lldb/Host/Config.h"
#include "lldb/Host/MainLoopBase.h"
#include "llvm/ADT/DenseMap.h"
#include <csignal>
#include <list>

#if !HAVE_PPOLL && !HAVE_SYS_EVENT_H && !defined(__ANDROID__)
#define SIGNAL_POLLING_UNSUPPORTED 1
#endif

namespace lldb_private {















class MainLoop : public MainLoopBase {
private:
class SignalHandle;

public:
typedef std::unique_ptr<SignalHandle> SignalHandleUP;

MainLoop();
~MainLoop() override;

ReadHandleUP RegisterReadObject(const lldb::IOObjectSP &object_sp,
const Callback &callback,
Status &error) override;








SignalHandleUP RegisterSignal(int signo, const Callback &callback,
Status &error);

Status Run() override;




void RequestTermination() override { m_terminate_request = true; }

protected:
void UnregisterReadObject(IOObject::WaitableHandle handle) override;

void UnregisterSignal(int signo, std::list<Callback>::iterator callback_it);

private:
void ProcessReadObject(IOObject::WaitableHandle handle);
void ProcessSignal(int signo);

class SignalHandle {
public:
~SignalHandle() { m_mainloop.UnregisterSignal(m_signo, m_callback_it); }

private:
SignalHandle(MainLoop &mainloop, int signo,
std::list<Callback>::iterator callback_it)
: m_mainloop(mainloop), m_signo(signo), m_callback_it(callback_it) {}

MainLoop &m_mainloop;
int m_signo;
std::list<Callback>::iterator m_callback_it;

friend class MainLoop;
SignalHandle(const SignalHandle &) = delete;
const SignalHandle &operator=(const SignalHandle &) = delete;
};

struct SignalInfo {
std::list<Callback> callbacks;
#if HAVE_SIGACTION
struct sigaction old_action;
#endif
bool was_blocked : 1;
};
class RunImpl;

llvm::DenseMap<IOObject::WaitableHandle, Callback> m_read_fds;
llvm::DenseMap<int, SignalInfo> m_signals;
#if HAVE_SYS_EVENT_H
int m_kqueue;
#endif
bool m_terminate_request : 1;
};

}

#endif
