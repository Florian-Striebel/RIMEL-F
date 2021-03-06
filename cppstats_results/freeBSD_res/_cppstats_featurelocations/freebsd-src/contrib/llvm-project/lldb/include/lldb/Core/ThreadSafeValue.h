







#if !defined(LLDB_CORE_THREADSAFEVALUE_H)
#define LLDB_CORE_THREADSAFEVALUE_H

#include <mutex>

#include "lldb/lldb-defines.h"

namespace lldb_private {

template <class T> class ThreadSafeValue {
public:
ThreadSafeValue() = default;
ThreadSafeValue(const T &value) : m_value(value), m_mutex() {}

~ThreadSafeValue() = default;

T GetValue() const {
T value;
{
std::lock_guard<std::recursive_mutex> guard(m_mutex);
value = m_value;
}
return value;
}



const T &GetValueNoLock() const { return m_value; }

void SetValue(const T &value) {
std::lock_guard<std::recursive_mutex> guard(m_mutex);
m_value = value;
}



void SetValueNoLock(const T &value) { m_value = value; }

std::recursive_mutex &GetMutex() { return m_mutex; }

private:
T m_value;
mutable std::recursive_mutex m_mutex;


ThreadSafeValue(const ThreadSafeValue &) = delete;
const ThreadSafeValue &operator=(const ThreadSafeValue &) = delete;
};

}
#endif
