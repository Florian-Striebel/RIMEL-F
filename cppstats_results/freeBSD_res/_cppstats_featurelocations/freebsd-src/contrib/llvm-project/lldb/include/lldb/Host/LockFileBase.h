







#if !defined(LLDB_HOST_LOCKFILEBASE_H)
#define LLDB_HOST_LOCKFILEBASE_H

#include "lldb/Utility/Status.h"

#include <functional>

namespace lldb_private {

class LockFileBase {
public:
virtual ~LockFileBase() = default;

bool IsLocked() const;

Status WriteLock(const uint64_t start, const uint64_t len);
Status TryWriteLock(const uint64_t start, const uint64_t len);

Status ReadLock(const uint64_t start, const uint64_t len);
Status TryReadLock(const uint64_t start, const uint64_t len);

Status Unlock();

protected:
using Locker = std::function<Status(const uint64_t, const uint64_t)>;

LockFileBase(int fd);

virtual bool IsValidFile() const;

virtual Status DoWriteLock(const uint64_t start, const uint64_t len) = 0;
virtual Status DoTryWriteLock(const uint64_t start, const uint64_t len) = 0;

virtual Status DoReadLock(const uint64_t start, const uint64_t len) = 0;
virtual Status DoTryReadLock(const uint64_t start, const uint64_t len) = 0;

virtual Status DoUnlock() = 0;

Status DoLock(const Locker &locker, const uint64_t start, const uint64_t len);

int m_fd;
bool m_locked;
uint64_t m_start;
uint64_t m_len;
};
}

#endif
