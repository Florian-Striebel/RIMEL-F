







#if !defined(LLDB_BREAKPOINT_BREAKPOINTLIST_H)
#define LLDB_BREAKPOINT_BREAKPOINTLIST_H

#include <list>
#include <mutex>

#include "lldb/Breakpoint/Breakpoint.h"

namespace lldb_private {







class BreakpointList {
public:
BreakpointList(bool is_internal);

~BreakpointList();








lldb::break_id_t Add(lldb::BreakpointSP &bp_sp, bool notify);


void Dump(Stream *s) const;










lldb::BreakpointSP FindBreakpointByID(lldb::break_id_t breakID) const;









lldb::BreakpointSP GetBreakpointAtIndex(size_t i) const;









llvm::Expected<std::vector<lldb::BreakpointSP>>
FindBreakpointsByName(const char *name);





size_t GetSize() const {
std::lock_guard<std::recursive_mutex> guard(m_mutex);
return m_breakpoints.size();
}








bool Remove(lldb::break_id_t breakID, bool notify);














void RemoveInvalidLocations(const ArchSpec &arch);

void SetEnabledAll(bool enabled);

void SetEnabledAllowed(bool enabled);


void RemoveAll(bool notify);




void RemoveAllowed(bool notify);














void UpdateBreakpoints(ModuleList &module_list, bool load,
bool delete_locations);

void UpdateBreakpointsWhenModuleIsReplaced(lldb::ModuleSP old_module_sp,
lldb::ModuleSP new_module_sp);

void ClearAllBreakpointSites();





void GetListMutex(std::unique_lock<std::recursive_mutex> &lock);

protected:
typedef std::vector<lldb::BreakpointSP> bp_collection;

bp_collection::iterator GetBreakpointIDIterator(lldb::break_id_t breakID);

bp_collection::const_iterator
GetBreakpointIDConstIterator(lldb::break_id_t breakID) const;

std::recursive_mutex &GetMutex() const { return m_mutex; }

mutable std::recursive_mutex m_mutex;
bp_collection m_breakpoints;
lldb::break_id_t m_next_break_id;
bool m_is_internal;

public:
typedef LockingAdaptedIterable<bp_collection, lldb::BreakpointSP,
list_adapter, std::recursive_mutex>
BreakpointIterable;
BreakpointIterable Breakpoints() {
return BreakpointIterable(m_breakpoints, GetMutex());
}

private:
BreakpointList(const BreakpointList &) = delete;
const BreakpointList &operator=(const BreakpointList &) = delete;
};

}

#endif
