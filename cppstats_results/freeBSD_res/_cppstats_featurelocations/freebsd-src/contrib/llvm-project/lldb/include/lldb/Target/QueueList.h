







#if !defined(LLDB_TARGET_QUEUELIST_H)
#define LLDB_TARGET_QUEUELIST_H

#include <mutex>
#include <vector>

#include "lldb/Utility/Iterable.h"
#include "lldb/Utility/UserID.h"
#include "lldb/lldb-private.h"

namespace lldb_private {








class QueueList {
friend class Process;

public:
QueueList(Process *process);

~QueueList();





uint32_t GetSize();







lldb::QueueSP GetQueueAtIndex(uint32_t idx);

typedef std::vector<lldb::QueueSP> collection;
typedef LockingAdaptedIterable<collection, lldb::QueueSP, vector_adapter,
std::mutex>
QueueIterable;






QueueIterable Queues() { return QueueIterable(m_queues, m_mutex); }


void Clear();





void AddQueue(lldb::QueueSP queue);









lldb::QueueSP FindQueueByID(lldb::queue_id_t qid);












lldb::QueueSP FindQueueByIndexID(uint32_t index_id);

std::mutex &GetMutex();

protected:

Process *m_process;
uint32_t
m_stop_id;
collection m_queues;
std::mutex m_mutex;

private:
QueueList() = delete;
};

}

#endif
