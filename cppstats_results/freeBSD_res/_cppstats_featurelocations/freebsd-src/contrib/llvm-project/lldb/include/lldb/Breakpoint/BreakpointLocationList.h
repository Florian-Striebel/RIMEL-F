







#if !defined(LLDB_BREAKPOINT_BREAKPOINTLOCATIONLIST_H)
#define LLDB_BREAKPOINT_BREAKPOINTLOCATIONLIST_H

#include <map>
#include <mutex>
#include <vector>

#include "lldb/Core/Address.h"
#include "lldb/Utility/Iterable.h"
#include "lldb/lldb-private.h"

namespace lldb_private {





class BreakpointLocationList {





friend class Breakpoint;

public:
virtual ~BreakpointLocationList();


void Dump(Stream *s) const;










const lldb::BreakpointLocationSP FindByAddress(const Address &addr) const;










lldb::BreakpointLocationSP FindByID(lldb::break_id_t breakID) const;









lldb::break_id_t FindIDByAddress(const Address &addr);













size_t FindInModule(Module *module,
BreakpointLocationCollection &bp_loc_list);









lldb::BreakpointLocationSP GetByIndex(size_t i);










const lldb::BreakpointLocationSP GetByIndex(size_t i) const;



void ClearAllBreakpointSites();



void ResolveAllBreakpointSites();






size_t GetNumResolvedLocations() const;





uint32_t GetHitCount() const;












bool ShouldStop(StoppointCallbackContext *context, lldb::break_id_t breakID);





size_t GetSize() const { return m_locations.size(); }












void GetDescription(Stream *s, lldb::DescriptionLevel level);

protected:





BreakpointLocationList(Breakpoint &owner);

lldb::BreakpointLocationSP Create(const Address &addr,
bool resolve_indirect_symbols);

void StartRecordingNewLocations(BreakpointLocationCollection &new_locations);

void StopRecordingNewLocations();

lldb::BreakpointLocationSP AddLocation(const Address &addr,
bool resolve_indirect_symbols,
bool *new_location = nullptr);

void SwapLocation(lldb::BreakpointLocationSP to_location_sp,
lldb::BreakpointLocationSP from_location_sp);

bool RemoveLocation(const lldb::BreakpointLocationSP &bp_loc_sp);

void RemoveLocationByIndex(size_t idx);

void RemoveInvalidLocations(const ArchSpec &arch);

void Compact();

typedef std::vector<lldb::BreakpointLocationSP> collection;
typedef std::map<lldb_private::Address, lldb::BreakpointLocationSP,
Address::ModulePointerAndOffsetLessThanFunctionObject>
addr_map;

Breakpoint &m_owner;
collection m_locations;
addr_map m_address_to_location;
mutable std::recursive_mutex m_mutex;
lldb::break_id_t m_next_id;
BreakpointLocationCollection *m_new_location_recorder;

public:
typedef AdaptedIterable<collection, lldb::BreakpointLocationSP,
vector_adapter>
BreakpointLocationIterable;

BreakpointLocationIterable BreakpointLocations() {
return BreakpointLocationIterable(m_locations);
}
};

}

#endif
