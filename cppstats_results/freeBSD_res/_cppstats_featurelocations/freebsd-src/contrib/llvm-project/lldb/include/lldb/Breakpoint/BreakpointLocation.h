







#if !defined(LLDB_BREAKPOINT_BREAKPOINTLOCATION_H)
#define LLDB_BREAKPOINT_BREAKPOINTLOCATION_H

#include <memory>
#include <mutex>

#include "lldb/Breakpoint/BreakpointOptions.h"
#include "lldb/Breakpoint/StoppointHitCounter.h"
#include "lldb/Core/Address.h"
#include "lldb/Utility/UserID.h"
#include "lldb/lldb-private.h"

namespace lldb_private {















class BreakpointLocation
: public std::enable_shared_from_this<BreakpointLocation> {
public:
~BreakpointLocation();




lldb::addr_t GetLoadAddress() const;



Address &GetAddress();


Breakpoint &GetBreakpoint();

Target &GetTarget();










bool ShouldStop(StoppointCallbackContext *context);




void SetEnabled(bool enabled);





bool IsEnabled() const;


void SetAutoContinue(bool auto_continue);





bool IsAutoContinue() const;


uint32_t GetHitCount() const { return m_hit_counter.GetValue(); }





uint32_t GetIgnoreCount() const;





void SetIgnoreCount(uint32_t n);














void SetCallback(BreakpointHitCallback callback,
const lldb::BatonSP &callback_baton_sp, bool is_synchronous);

void SetCallback(BreakpointHitCallback callback, void *baton,
bool is_synchronous);

void ClearCallback();





void SetCondition(const char *condition);






const char *GetConditionText(size_t *hash = nullptr) const;

bool ConditionSaysStop(ExecutionContext &exe_ctx, Status &error);





void SetThreadID(lldb::tid_t thread_id);

lldb::tid_t GetThreadID();

void SetThreadIndex(uint32_t index);

uint32_t GetThreadIndex() const;

void SetThreadName(const char *thread_name);

const char *GetThreadName() const;

void SetQueueName(const char *queue_name);

const char *GetQueueName() const;








bool ResolveBreakpointSite();







bool ClearBreakpointSite();




bool IsResolved() const;

lldb::BreakpointSiteSP GetBreakpointSite() const;













void GetDescription(Stream *s, lldb::DescriptionLevel level);


void Dump(Stream *s) const;








BreakpointOptions &GetLocationOptions();










const BreakpointOptions &
GetOptionsSpecifyingKind(BreakpointOptions::OptionKind kind) const;

bool ValidForThisThread(Thread &thread);











bool InvokeCallback(StoppointCallbackContext *context);





bool IsCallbackSynchronous();







bool ShouldResolveIndirectFunctions() {
return m_should_resolve_indirect_functions;
}






bool IsIndirect() { return m_is_indirect; }

void SetIsIndirect(bool is_indirect) { m_is_indirect = is_indirect; }






bool IsReExported() { return m_is_reexported; }

void SetIsReExported(bool is_reexported) { m_is_reexported = is_reexported; }










bool EquivalentToLocation(BreakpointLocation &location);


lldb::break_id_t GetID() const { return m_loc_id; }

protected:
friend class BreakpointSite;
friend class BreakpointLocationList;
friend class Process;
friend class StopInfoBreakpoint;









bool SetBreakpointSite(lldb::BreakpointSiteSP &bp_site_sp);

void DecrementIgnoreCount();





bool IgnoreCountShouldStop();

private:
void SwapLocation(lldb::BreakpointLocationSP swap_from);

void BumpHitCount();

void UndoBumpHitCount();




















BreakpointLocation(lldb::break_id_t bid, Breakpoint &owner,
const Address &addr, lldb::tid_t tid, bool hardware,
bool check_for_resolver = true);


bool m_being_created;
bool m_should_resolve_indirect_functions;
bool m_is_reexported;
bool m_is_indirect;
Address m_address;
Breakpoint &m_owner;
std::unique_ptr<BreakpointOptions> m_options_up;



lldb::BreakpointSiteSP m_bp_site_sp;

lldb::UserExpressionSP m_user_expression_sp;

std::mutex m_condition_mutex;


size_t m_condition_hash;

lldb::break_id_t m_loc_id;
StoppointHitCounter m_hit_counter;


void SetShouldResolveIndirectFunctions(bool do_resolve) {
m_should_resolve_indirect_functions = do_resolve;
}

void SendBreakpointLocationChangedEvent(lldb::BreakpointEventType eventKind);

BreakpointLocation(const BreakpointLocation &) = delete;
const BreakpointLocation &operator=(const BreakpointLocation &) = delete;
};

}

#endif
