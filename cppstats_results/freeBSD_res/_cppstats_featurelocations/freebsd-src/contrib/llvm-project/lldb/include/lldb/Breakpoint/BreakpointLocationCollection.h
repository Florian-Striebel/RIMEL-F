







#if !defined(LLDB_BREAKPOINT_BREAKPOINTLOCATIONCOLLECTION_H)
#define LLDB_BREAKPOINT_BREAKPOINTLOCATIONCOLLECTION_H

#include <mutex>
#include <vector>

#include "lldb/Utility/Iterable.h"
#include "lldb/lldb-private.h"

namespace lldb_private {

class BreakpointLocationCollection {
public:
BreakpointLocationCollection();

~BreakpointLocationCollection();

BreakpointLocationCollection &operator=(const BreakpointLocationCollection &rhs);






void Add(const lldb::BreakpointLocationSP &bp_loc_sp);












bool Remove(lldb::break_id_t break_id, lldb::break_id_t break_loc_id);













lldb::BreakpointLocationSP FindByIDPair(lldb::break_id_t break_id,
lldb::break_id_t break_loc_id);













const lldb::BreakpointLocationSP
FindByIDPair(lldb::break_id_t break_id, lldb::break_id_t break_loc_id) const;










lldb::BreakpointLocationSP GetByIndex(size_t i);










const lldb::BreakpointLocationSP GetByIndex(size_t i) const;





size_t GetSize() const { return m_break_loc_collection.size(); }









bool ShouldStop(StoppointCallbackContext *context);












void GetDescription(Stream *s, lldb::DescriptionLevel level);











bool ValidForThisThread(Thread &thread);






bool IsInternal() const;

protected:



private:


typedef std::vector<lldb::BreakpointLocationSP> collection;

collection::iterator GetIDPairIterator(lldb::break_id_t break_id,
lldb::break_id_t break_loc_id);

collection::const_iterator
GetIDPairConstIterator(lldb::break_id_t break_id,
lldb::break_id_t break_loc_id) const;

collection m_break_loc_collection;
mutable std::mutex m_collection_mutex;

public:
typedef AdaptedIterable<collection, lldb::BreakpointLocationSP,
vector_adapter>
BreakpointLocationCollectionIterable;
BreakpointLocationCollectionIterable BreakpointLocations() {
return BreakpointLocationCollectionIterable(m_break_loc_collection);
}
};

}

#endif
