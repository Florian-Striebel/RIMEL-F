







#if !defined(LLDB_TARGET_THREADPLANRUNTOADDRESS_H)
#define LLDB_TARGET_THREADPLANRUNTOADDRESS_H

#include <vector>

#include "lldb/Target/ThreadPlan.h"
#include "lldb/lldb-private.h"

namespace lldb_private {

class ThreadPlanRunToAddress : public ThreadPlan {
public:
ThreadPlanRunToAddress(Thread &thread, Address &address, bool stop_others);

ThreadPlanRunToAddress(Thread &thread, lldb::addr_t address,
bool stop_others);

ThreadPlanRunToAddress(Thread &thread,
const std::vector<lldb::addr_t> &addresses,
bool stop_others);

~ThreadPlanRunToAddress() override;

void GetDescription(Stream *s, lldb::DescriptionLevel level) override;

bool ValidatePlan(Stream *error) override;

bool ShouldStop(Event *event_ptr) override;

bool StopOthers() override;

void SetStopOthers(bool new_value) override;

lldb::StateType GetPlanRunState() override;

bool WillStop() override;

bool MischiefManaged() override;

protected:
bool DoPlanExplainsStop(Event *event_ptr) override;

void SetInitialBreakpoints();
bool AtOurAddress();

private:
bool m_stop_others;
std::vector<lldb::addr_t>
m_addresses;

std::vector<lldb::break_id_t> m_break_ids;


ThreadPlanRunToAddress(const ThreadPlanRunToAddress &) = delete;
const ThreadPlanRunToAddress &
operator=(const ThreadPlanRunToAddress &) = delete;
};

}

#endif
