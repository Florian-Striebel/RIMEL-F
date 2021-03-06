







#if !defined(LLDB_BREAKPOINT_WATCHPOINT_H)
#define LLDB_BREAKPOINT_WATCHPOINT_H

#include <memory>
#include <string>

#include "lldb/Breakpoint/StoppointSite.h"
#include "lldb/Breakpoint/WatchpointOptions.h"
#include "lldb/Symbol/CompilerType.h"
#include "lldb/Target/Target.h"
#include "lldb/Utility/UserID.h"
#include "lldb/lldb-private.h"

namespace lldb_private {

class Watchpoint : public std::enable_shared_from_this<Watchpoint>,
public StoppointSite {
public:
class WatchpointEventData : public EventData {
public:
WatchpointEventData(lldb::WatchpointEventType sub_type,
const lldb::WatchpointSP &new_watchpoint_sp);

~WatchpointEventData() override;

static ConstString GetFlavorString();

ConstString GetFlavor() const override;

lldb::WatchpointEventType GetWatchpointEventType() const;

lldb::WatchpointSP &GetWatchpoint();

void Dump(Stream *s) const override;

static lldb::WatchpointEventType
GetWatchpointEventTypeFromEvent(const lldb::EventSP &event_sp);

static lldb::WatchpointSP
GetWatchpointFromEvent(const lldb::EventSP &event_sp);

static const WatchpointEventData *
GetEventDataFromEvent(const Event *event_sp);

private:
lldb::WatchpointEventType m_watchpoint_event;
lldb::WatchpointSP m_new_watchpoint_sp;

WatchpointEventData(const WatchpointEventData &) = delete;
const WatchpointEventData &operator=(const WatchpointEventData &) = delete;
};

Watchpoint(Target &target, lldb::addr_t addr, uint32_t size,
const CompilerType *type, bool hardware = true);

~Watchpoint() override;

void IncrementFalseAlarmsAndReviseHitCount();

bool IsEnabled() const;




void SetEnabled(bool enabled, bool notify = true);

bool IsHardware() const override;

bool ShouldStop(StoppointCallbackContext *context) override;

bool WatchpointRead() const;
bool WatchpointWrite() const;
uint32_t GetIgnoreCount() const;
void SetIgnoreCount(uint32_t n);
void SetWatchpointType(uint32_t type, bool notify = true);
void SetDeclInfo(const std::string &str);
std::string GetWatchSpec();
void SetWatchSpec(const std::string &str);


bool IsWatchVariable() const;
void SetWatchVariable(bool val);
bool CaptureWatchedValue(const ExecutionContext &exe_ctx);

void GetDescription(Stream *s, lldb::DescriptionLevel level);
void Dump(Stream *s) const override;
void DumpSnapshots(Stream *s, const char *prefix = nullptr) const;
void DumpWithLevel(Stream *s, lldb::DescriptionLevel description_level) const;
Target &GetTarget() { return m_target; }
const Status &GetError() { return m_error; }





WatchpointOptions *GetOptions() { return &m_options; }











void SetCallback(WatchpointHitCallback callback, void *callback_baton,
bool is_synchronous = false);

void SetCallback(WatchpointHitCallback callback,
const lldb::BatonSP &callback_baton_sp,
bool is_synchronous = false);

void ClearCallback();








bool InvokeCallback(StoppointCallbackContext *context);







void SetCondition(const char *condition);






const char *GetConditionText() const;

void TurnOnEphemeralMode();

void TurnOffEphemeralMode();

bool IsDisabledDuringEphemeralMode();

const CompilerType &GetCompilerType() { return m_type; }

private:
friend class Target;
friend class WatchpointList;

void ResetHistoricValues() {
m_old_value_sp.reset();
m_new_value_sp.reset();
}

Target &m_target;
bool m_enabled;
bool m_is_hardware;
bool m_is_watch_variable;
bool m_is_ephemeral;



uint32_t m_disabled_count;




uint32_t m_watch_read : 1,
m_watch_write : 1,
m_watch_was_read : 1,
m_watch_was_written : 1;

uint32_t m_ignore_count;
uint32_t m_false_alarms;
std::string m_decl_str;
std::string m_watch_spec_str;
lldb::ValueObjectSP m_old_value_sp;
lldb::ValueObjectSP m_new_value_sp;
CompilerType m_type;
Status m_error;

WatchpointOptions
m_options;

bool m_being_created;

std::unique_ptr<UserExpression> m_condition_up;

void SetID(lldb::watch_id_t id) { m_id = id; }

void SendWatchpointChangedEvent(lldb::WatchpointEventType eventKind);

void SendWatchpointChangedEvent(WatchpointEventData *data);

Watchpoint(const Watchpoint &) = delete;
const Watchpoint &operator=(const Watchpoint &) = delete;
};

}

#endif
