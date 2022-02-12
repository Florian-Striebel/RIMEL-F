







#if !defined(LLDB_BREAKPOINT_WATCHPOINTLIST_H)
#define LLDB_BREAKPOINT_WATCHPOINTLIST_H

#include <list>
#include <mutex>
#include <vector>

#include "lldb/Core/Address.h"
#include "lldb/Utility/Iterable.h"
#include "lldb/lldb-private.h"

namespace lldb_private {






class WatchpointList {



friend class Watchpoint;
friend class Target;

public:

WatchpointList();


~WatchpointList();

typedef std::list<lldb::WatchpointSP> wp_collection;
typedef LockingAdaptedIterable<wp_collection, lldb::WatchpointSP,
vector_adapter, std::recursive_mutex>
WatchpointIterable;








lldb::watch_id_t Add(const lldb::WatchpointSP &wp_sp, bool notify);


void Dump(Stream *s) const;


void DumpWithLevel(Stream *s, lldb::DescriptionLevel description_level) const;










const lldb::WatchpointSP FindByAddress(lldb::addr_t addr) const;










const lldb::WatchpointSP FindBySpec(std::string spec) const;










lldb::WatchpointSP FindByID(lldb::watch_id_t watchID) const;








lldb::watch_id_t FindIDByAddress(lldb::addr_t addr);









lldb::watch_id_t FindIDBySpec(std::string spec);









lldb::WatchpointSP GetByIndex(uint32_t i);










const lldb::WatchpointSP GetByIndex(uint32_t i) const;








bool Remove(lldb::watch_id_t watchID, bool notify);





uint32_t GetHitCount() const;












bool ShouldStop(StoppointCallbackContext *context, lldb::watch_id_t watchID);





size_t GetSize() const {
std::lock_guard<std::recursive_mutex> guard(m_mutex);
return m_watchpoints.size();
}











void GetDescription(Stream *s, lldb::DescriptionLevel level);

void SetEnabledAll(bool enabled);

void RemoveAll(bool notify);





void GetListMutex(std::unique_lock<std::recursive_mutex> &lock);

WatchpointIterable Watchpoints() const {
return WatchpointIterable(m_watchpoints, m_mutex);
}

protected:
typedef std::vector<lldb::watch_id_t> id_vector;

id_vector GetWatchpointIDs() const;

wp_collection::iterator GetIDIterator(lldb::watch_id_t watchID);

wp_collection::const_iterator
GetIDConstIterator(lldb::watch_id_t watchID) const;

wp_collection m_watchpoints;
mutable std::recursive_mutex m_mutex;

lldb::watch_id_t m_next_wp_id = 0;
};

}

#endif
