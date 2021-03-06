







#if !defined(LLDB_BREAKPOINT_BREAKPOINT_H)
#define LLDB_BREAKPOINT_BREAKPOINT_H

#include <memory>
#include <string>
#include <unordered_set>
#include <vector>

#include "lldb/Breakpoint/BreakpointID.h"
#include "lldb/Breakpoint/BreakpointLocationCollection.h"
#include "lldb/Breakpoint/BreakpointLocationList.h"
#include "lldb/Breakpoint/BreakpointName.h"
#include "lldb/Breakpoint/BreakpointOptions.h"
#include "lldb/Breakpoint/Stoppoint.h"
#include "lldb/Breakpoint/StoppointHitCounter.h"
#include "lldb/Core/SearchFilter.h"
#include "lldb/Utility/Event.h"
#include "lldb/Utility/StringList.h"
#include "lldb/Utility/StructuredData.h"

namespace lldb_private {

















































class Breakpoint : public std::enable_shared_from_this<Breakpoint>,
public Stoppoint {
public:
static ConstString GetEventIdentifier();



enum MatchType { Exact, Regexp, Glob };

private:
enum class OptionNames : uint32_t { Names = 0, Hardware, LastOptionName };

static const char
*g_option_names[static_cast<uint32_t>(OptionNames::LastOptionName)];

static const char *GetKey(OptionNames enum_value) {
return g_option_names[static_cast<uint32_t>(enum_value)];
}

public:
class BreakpointEventData : public EventData {
public:
BreakpointEventData(lldb::BreakpointEventType sub_type,
const lldb::BreakpointSP &new_breakpoint_sp);

~BreakpointEventData() override;

static ConstString GetFlavorString();

ConstString GetFlavor() const override;

lldb::BreakpointEventType GetBreakpointEventType() const;

lldb::BreakpointSP &GetBreakpoint();

BreakpointLocationCollection &GetBreakpointLocationCollection() {
return m_locations;
}

void Dump(Stream *s) const override;

static lldb::BreakpointEventType
GetBreakpointEventTypeFromEvent(const lldb::EventSP &event_sp);

static lldb::BreakpointSP
GetBreakpointFromEvent(const lldb::EventSP &event_sp);

static lldb::BreakpointLocationSP
GetBreakpointLocationAtIndexFromEvent(const lldb::EventSP &event_sp,
uint32_t loc_idx);

static size_t
GetNumBreakpointLocationsFromEvent(const lldb::EventSP &event_sp);

static const BreakpointEventData *
GetEventDataFromEvent(const Event *event_sp);

private:
lldb::BreakpointEventType m_breakpoint_event;
lldb::BreakpointSP m_new_breakpoint_sp;
BreakpointLocationCollection m_locations;

BreakpointEventData(const BreakpointEventData &) = delete;
const BreakpointEventData &operator=(const BreakpointEventData &) = delete;
};


static lldb::BreakpointSP CreateFromStructuredData(
lldb::TargetSP target_sp, StructuredData::ObjectSP &data_object_sp,
Status &error);

static bool
SerializedBreakpointMatchesNames(StructuredData::ObjectSP &bkpt_object_sp,
std::vector<std::string> &names);

virtual StructuredData::ObjectSP SerializeToStructuredData();

static const char *GetSerializationKey() { return "Breakpoint"; }





~Breakpoint() override;





bool IsInternal() const;


void Dump(Stream *s) override;






void ClearAllBreakpointSites();



void ResolveBreakpoint();










void ResolveBreakpointInModules(ModuleList &module_list,
bool send_event = true);









void ResolveBreakpointInModules(ModuleList &module_list,
BreakpointLocationCollection &new_locations);












void ModulesChanged(ModuleList &changed_modules, bool load_event,
bool delete_locations = false);









void ModuleReplaced(lldb::ModuleSP old_module_sp,
lldb::ModuleSP new_module_sp);














lldb::BreakpointLocationSP AddLocation(const Address &addr,
bool *new_location = nullptr);









lldb::BreakpointLocationSP FindLocationByAddress(const Address &addr);








lldb::break_id_t FindLocationIDByAddress(const Address &addr);










lldb::BreakpointLocationSP FindLocationByID(lldb::break_id_t bp_loc_id);










lldb::BreakpointLocationSP GetLocationAtIndex(size_t index);














void RemoveInvalidLocations(const ArchSpec &arch);




void SetEnabled(bool enable) override;




bool IsEnabled() override;




void SetIgnoreCount(uint32_t count);




uint32_t GetIgnoreCount() const;



uint32_t GetHitCount() const;


void SetOneShot(bool one_shot);




bool IsOneShot() const;



void SetAutoContinue(bool auto_continue);




bool IsAutoContinue() const;




void SetThreadID(lldb::tid_t thread_id);





lldb::tid_t GetThreadID() const;

void SetThreadIndex(uint32_t index);

uint32_t GetThreadIndex() const;

void SetThreadName(const char *thread_name);

const char *GetThreadName() const;

void SetQueueName(const char *queue_name);

const char *GetQueueName() const;











void SetCallback(BreakpointHitCallback callback, void *baton,
bool is_synchronous = false);

void SetCallback(BreakpointHitCallback callback,
const lldb::BatonSP &callback_baton_sp,
bool is_synchronous = false);

void ClearCallback();






void SetCondition(const char *condition);






const char *GetConditionText() const;








size_t GetNumResolvedLocations() const;





bool HasResolvedLocations() const;





size_t GetNumLocations() const;











void GetDescription(Stream *s, lldb::DescriptionLevel level,
bool show_locations = false);








void SetBreakpointKind(const char *kind) { m_kind_description.assign(kind); }





const char *GetBreakpointKind() const { return m_kind_description.c_str(); }




Target &GetTarget() { return m_target; }

const Target &GetTarget() const { return m_target; }

const lldb::TargetSP GetTargetSP();

void GetResolverDescription(Stream *s);














bool GetMatchingFileLine(ConstString filename, uint32_t line_number,
BreakpointLocationCollection &loc_coll);

void GetFilterDescription(Stream *s);







BreakpointOptions &GetOptions();







const BreakpointOptions &GetOptions() const;













bool InvokeCallback(StoppointCallbackContext *context,
lldb::break_id_t bp_loc_id);

bool IsHardware() const { return m_hardware; }

lldb::BreakpointResolverSP GetResolver() { return m_resolver_sp; }

lldb::SearchFilterSP GetSearchFilter() { return m_filter_sp; }

private:

bool AddName(llvm::StringRef new_name);

void RemoveName(const char *name_to_remove) {
if (name_to_remove)
m_name_list.erase(name_to_remove);
}

public:
bool MatchesName(const char *name) {
return m_name_list.find(name) != m_name_list.end();
}

void GetNames(std::vector<std::string> &names) {
names.clear();
for (auto name : m_name_list) {
names.push_back(name);
}
}











void SetPrecondition(lldb::BreakpointPreconditionSP precondition_sp) {
m_precondition_sp = std::move(precondition_sp);
}

bool EvaluatePrecondition(StoppointCallbackContext &context);

lldb::BreakpointPreconditionSP GetPrecondition() { return m_precondition_sp; }


const BreakpointName::Permissions &GetPermissions() const {
return m_permissions;
}

BreakpointName::Permissions &GetPermissions() {
return m_permissions;
}

bool AllowList() const {
return GetPermissions().GetAllowList();
}
bool AllowDisable() const {
return GetPermissions().GetAllowDisable();
}
bool AllowDelete() const {
return GetPermissions().GetAllowDelete();
}



static lldb::BreakpointSP CopyFromBreakpoint(lldb::TargetSP new_target,
const Breakpoint &bp_to_copy_from);

protected:
friend class Target;






























Breakpoint(Target &target, lldb::SearchFilterSP &filter_sp,
lldb::BreakpointResolverSP &resolver_sp, bool hardware,
bool resolve_indirect_symbols = true);

friend class BreakpointLocation;


void DecrementIgnoreCount();

private:

Breakpoint(Target &new_target, const Breakpoint &bp_to_copy_from);


bool m_being_created;
bool
m_hardware;
Target &m_target;
std::unordered_set<std::string> m_name_list;



lldb::SearchFilterSP
m_filter_sp;
lldb::BreakpointResolverSP
m_resolver_sp;
lldb::BreakpointPreconditionSP m_precondition_sp;





BreakpointOptions m_options;
BreakpointLocationList
m_locations;
std::string m_kind_description;
bool m_resolve_indirect_symbols;




StoppointHitCounter m_hit_counter;

BreakpointName::Permissions m_permissions;

void SendBreakpointChangedEvent(lldb::BreakpointEventType eventKind);

void SendBreakpointChangedEvent(BreakpointEventData *data);

Breakpoint(const Breakpoint &) = delete;
const Breakpoint &operator=(const Breakpoint &) = delete;
};

}

#endif
