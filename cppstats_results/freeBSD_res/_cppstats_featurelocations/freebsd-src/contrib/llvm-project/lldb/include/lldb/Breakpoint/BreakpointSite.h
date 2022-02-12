







#if !defined(LLDB_BREAKPOINT_BREAKPOINTSITE_H)
#define LLDB_BREAKPOINT_BREAKPOINTSITE_H

#include <list>
#include <mutex>


#include "lldb/Breakpoint/BreakpointLocationCollection.h"
#include "lldb/Breakpoint/StoppointSite.h"
#include "lldb/Utility/LLDBAssert.h"
#include "lldb/Utility/UserID.h"
#include "lldb/lldb-forward.h"

namespace lldb_private {












class BreakpointSite : public std::enable_shared_from_this<BreakpointSite>,
public StoppointSite {
public:
enum Type {
eSoftware,


eHardware,
eExternal


};

~BreakpointSite() override;




uint8_t *GetTrapOpcodeBytes();


const uint8_t *GetTrapOpcodeBytes() const;


size_t GetTrapOpcodeMaxByteSize() const;


bool SetTrapOpcode(const uint8_t *trap_opcode, uint32_t trap_opcode_size);


uint8_t *GetSavedOpcodeBytes();



const uint8_t *GetSavedOpcodeBytes() const;



bool IntersectsRange(lldb::addr_t addr, size_t size,
lldb::addr_t *intersect_addr, size_t *intersect_size,
size_t *opcode_offset) const;







bool IsEnabled() const;





void SetEnabled(bool enabled);









bool ShouldStop(StoppointCallbackContext *context) override;


void Dump(Stream *s) const override;






void AddOwner(const lldb::BreakpointLocationSP &owner);






size_t GetNumberOfOwners();











lldb::BreakpointLocationSP GetOwnerAtIndex(size_t idx);










size_t CopyOwnersList(BreakpointLocationCollection &out_collection);











bool ValidForThisThread(Thread &thread);














void GetDescription(Stream *s, lldb::DescriptionLevel level);









bool IsBreakpointAtThisSite(lldb::break_id_t bp_id);







bool IsInternal() const;

bool IsHardware() const override {
lldbassert(BreakpointSite::Type::eHardware == GetType() ||
!HardwareRequired());
return BreakpointSite::Type::eHardware == GetType();
}

BreakpointSite::Type GetType() const { return m_type; }

void SetType(BreakpointSite::Type type) { m_type = type; }

private:
friend class Process;
friend class BreakpointLocation;



friend class StopInfoBreakpoint;

void BumpHitCounts();



size_t RemoveOwner(lldb::break_id_t break_id, lldb::break_id_t break_loc_id);

BreakpointSite::Type m_type;
uint8_t m_saved_opcode[8];

uint8_t m_trap_opcode[8];

bool
m_enabled;



BreakpointLocationCollection m_owners;

std::recursive_mutex
m_owners_mutex;

static lldb::break_id_t GetNextID();



BreakpointSite(BreakpointSiteList *list,
const lldb::BreakpointLocationSP &owner, lldb::addr_t m_addr,
bool use_hardware);

BreakpointSite(const BreakpointSite &) = delete;
const BreakpointSite &operator=(const BreakpointSite &) = delete;
};

}

#endif
