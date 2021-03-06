







#if !defined(LLDB_BREAKPOINT_BREAKPOINTSITELIST_H)
#define LLDB_BREAKPOINT_BREAKPOINTSITELIST_H

#include <functional>
#include <map>
#include <mutex>

#include "lldb/Breakpoint/BreakpointSite.h"

namespace lldb_private {




class BreakpointSiteList {



friend class Process;

public:

BreakpointSiteList();


~BreakpointSiteList();








lldb::break_id_t Add(const lldb::BreakpointSiteSP &bp_site_sp);



void Dump(Stream *s) const;









lldb::BreakpointSiteSP FindByAddress(lldb::addr_t addr);










lldb::BreakpointSiteSP FindByID(lldb::break_id_t breakID);











const lldb::BreakpointSiteSP FindByID(lldb::break_id_t breakID) const;









lldb::break_id_t FindIDByAddress(lldb::addr_t addr);













bool BreakpointSiteContainsBreakpoint(lldb::break_id_t bp_site_id,
lldb::break_id_t bp_id);

void ForEach(std::function<void(BreakpointSite *)> const &callback);








bool Remove(lldb::break_id_t breakID);








bool RemoveByAddress(lldb::addr_t addr);

bool FindInRange(lldb::addr_t lower_bound, lldb::addr_t upper_bound,
BreakpointSiteList &bp_site_list) const;

typedef void (*BreakpointSiteSPMapFunc)(lldb::BreakpointSiteSP &bp,
void *baton);












bool ShouldStop(StoppointCallbackContext *context, lldb::break_id_t breakID);





size_t GetSize() const {
std::lock_guard<std::recursive_mutex> guard(m_mutex);
return m_bp_site_list.size();
}

bool IsEmpty() const {
std::lock_guard<std::recursive_mutex> guard(m_mutex);
return m_bp_site_list.empty();
}

protected:
typedef std::map<lldb::addr_t, lldb::BreakpointSiteSP> collection;

collection::iterator GetIDIterator(lldb::break_id_t breakID);

collection::const_iterator GetIDConstIterator(lldb::break_id_t breakID) const;

mutable std::recursive_mutex m_mutex;
collection m_bp_site_list;
};

}

#endif
