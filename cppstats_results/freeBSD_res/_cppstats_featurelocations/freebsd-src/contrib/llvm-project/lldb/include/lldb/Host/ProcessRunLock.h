







#if !defined(LLDB_HOST_PROCESSRUNLOCK_H)
#define LLDB_HOST_PROCESSRUNLOCK_H

#include <cstdint>
#include <ctime>

#include "lldb/lldb-defines.h"


namespace lldb_private {






class ProcessRunLock {
public:
ProcessRunLock();
~ProcessRunLock();

bool ReadTryLock();
bool ReadUnlock();
bool SetRunning();
bool TrySetRunning();
bool SetStopped();

class ProcessRunLocker {
public:
ProcessRunLocker() = default;

~ProcessRunLocker() { Unlock(); }


bool TryLock(ProcessRunLock *lock) {
if (m_lock) {
if (m_lock == lock)
return true;
else
Unlock();
}
if (lock) {
if (lock->ReadTryLock()) {
m_lock = lock;
return true;
}
}
return false;
}

protected:
void Unlock() {
if (m_lock) {
m_lock->ReadUnlock();
m_lock = nullptr;
}
}

ProcessRunLock *m_lock = nullptr;

private:
ProcessRunLocker(const ProcessRunLocker &) = delete;
const ProcessRunLocker &operator=(const ProcessRunLocker &) = delete;
};

protected:
lldb::rwlock_t m_rwlock;
bool m_running = false;

private:
ProcessRunLock(const ProcessRunLock &) = delete;
const ProcessRunLock &operator=(const ProcessRunLock &) = delete;
};

}

#endif
