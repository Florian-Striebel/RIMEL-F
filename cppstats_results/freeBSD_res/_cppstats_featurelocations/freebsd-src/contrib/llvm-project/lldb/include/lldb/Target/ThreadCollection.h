







#if !defined(LLDB_TARGET_THREADCOLLECTION_H)
#define LLDB_TARGET_THREADCOLLECTION_H

#include <mutex>
#include <vector>

#include "lldb/Utility/Iterable.h"
#include "lldb/lldb-private.h"

namespace lldb_private {

class ThreadCollection {
public:
typedef std::vector<lldb::ThreadSP> collection;
typedef LockingAdaptedIterable<collection, lldb::ThreadSP, vector_adapter,
std::recursive_mutex>
ThreadIterable;

ThreadCollection();

ThreadCollection(collection threads);

virtual ~ThreadCollection() = default;

uint32_t GetSize();

void AddThread(const lldb::ThreadSP &thread_sp);

void AddThreadSortedByIndexID(const lldb::ThreadSP &thread_sp);

void InsertThread(const lldb::ThreadSP &thread_sp, uint32_t idx);




lldb::ThreadSP GetThreadAtIndex(uint32_t idx);

virtual ThreadIterable Threads() {
return ThreadIterable(m_threads, GetMutex());
}

virtual std::recursive_mutex &GetMutex() const { return m_mutex; }

protected:
collection m_threads;
mutable std::recursive_mutex m_mutex;
};

}

#endif
