







#if !defined(LLDB_TARGET_QUEUE_H)
#define LLDB_TARGET_QUEUE_H

#include <string>
#include <vector>

#include "lldb/Target/QueueItem.h"
#include "lldb/lldb-enumerations.h"
#include "lldb/lldb-forward.h"
#include "lldb/lldb-private.h"

namespace lldb_private {











class Queue : public std::enable_shared_from_this<Queue> {
public:
Queue(lldb::ProcessSP process_sp, lldb::queue_id_t queue_id,
const char *queue_name);

~Queue();











lldb::queue_id_t GetID();






const char *GetName();













uint32_t GetIndexID();









std::vector<lldb::ThreadSP> GetThreads();









const std::vector<lldb::QueueItemSP> &GetPendingItems();

lldb::ProcessSP GetProcess() const { return m_process_wp.lock(); }







uint32_t GetNumRunningWorkItems() const;






uint32_t GetNumPendingWorkItems() const;









lldb::addr_t GetLibdispatchQueueAddress() const;

void SetNumRunningWorkItems(uint32_t count);

void SetNumPendingWorkItems(uint32_t count);

void SetLibdispatchQueueAddress(lldb::addr_t dispatch_queue_t_addr);

void PushPendingQueueItem(lldb::QueueItemSP item) {
m_pending_items.push_back(item);
}


lldb::QueueKind GetKind();

void SetKind(lldb::QueueKind kind);

private:


lldb::ProcessWP m_process_wp;
lldb::queue_id_t m_queue_id;
std::string m_queue_name;
uint32_t m_running_work_items_count;
uint32_t m_pending_work_items_count;
std::vector<lldb::QueueItemSP> m_pending_items;
lldb::addr_t m_dispatch_queue_t_addr;

lldb::QueueKind m_kind;

Queue(const Queue &) = delete;
const Queue &operator=(const Queue &) = delete;
};

}

#endif
