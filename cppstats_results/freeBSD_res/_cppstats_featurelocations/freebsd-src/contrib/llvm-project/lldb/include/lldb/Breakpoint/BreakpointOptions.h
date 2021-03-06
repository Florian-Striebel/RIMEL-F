







#if !defined(LLDB_BREAKPOINT_BREAKPOINTOPTIONS_H)
#define LLDB_BREAKPOINT_BREAKPOINTOPTIONS_H

#include <memory>
#include <string>

#include "lldb/Utility/Baton.h"
#include "lldb/Utility/Flags.h"
#include "lldb/Utility/StringList.h"
#include "lldb/Utility/StructuredData.h"
#include "lldb/lldb-private.h"

namespace lldb_private {





class BreakpointOptions {
friend class BreakpointLocation;
friend class BreakpointName;
friend class lldb_private::BreakpointOptionGroup;
friend class Breakpoint;

public:
enum OptionKind {
eCallback = 1 << 0,
eEnabled = 1 << 1,
eOneShot = 1 << 2,
eIgnoreCount = 1 << 3,
eThreadSpec = 1 << 4,
eCondition = 1 << 5,
eAutoContinue = 1 << 6,
eAllOptions = (eCallback | eEnabled | eOneShot | eIgnoreCount | eThreadSpec
| eCondition | eAutoContinue)
};
struct CommandData {
CommandData() : user_source(), script_source() {}

CommandData(const StringList &user_source, lldb::ScriptLanguage interp)
: user_source(user_source), script_source(), interpreter(interp),
stop_on_error(true) {}

virtual ~CommandData() = default;

static const char *GetSerializationKey() { return "BKPTCMDData"; }

StructuredData::ObjectSP SerializeToStructuredData();

static std::unique_ptr<CommandData>
CreateFromStructuredData(const StructuredData::Dictionary &options_dict,
Status &error);

StringList user_source;
std::string script_source;
enum lldb::ScriptLanguage interpreter =
lldb::eScriptLanguageNone;

bool stop_on_error = true;

private:
enum class OptionNames : uint32_t {
UserSource = 0,
Interpreter,
StopOnError,
LastOptionName
};

static const char
*g_option_names[static_cast<uint32_t>(OptionNames::LastOptionName)];

static const char *GetKey(OptionNames enum_value) {
return g_option_names[static_cast<uint32_t>(enum_value)];
}
};

class CommandBaton : public TypedBaton<CommandData> {
public:
explicit CommandBaton(std::unique_ptr<CommandData> Data)
: TypedBaton(std::move(Data)) {}

void GetDescription(llvm::raw_ostream &s, lldb::DescriptionLevel level,
unsigned indentation) const override;
};

typedef std::shared_ptr<CommandBaton> CommandBatonSP;





















BreakpointOptions(const char *condition, bool enabled = true,
int32_t ignore = 0, bool one_shot = false,
bool auto_continue = false);



BreakpointOptions(bool all_flags_set);
BreakpointOptions(const BreakpointOptions &rhs);

virtual ~BreakpointOptions();

static std::unique_ptr<BreakpointOptions>
CreateFromStructuredData(Target &target,
const StructuredData::Dictionary &data_dict,
Status &error);

virtual StructuredData::ObjectSP SerializeToStructuredData();

static const char *GetSerializationKey() { return "BKPTOptions"; }


const BreakpointOptions &operator=(const BreakpointOptions &rhs);


void CopyOverSetOptions(const BreakpointOptions &rhs);












































void SetCallback(BreakpointHitCallback callback,
const lldb::BatonSP &baton_sp, bool synchronous = false);

void SetCallback(BreakpointHitCallback callback,
const BreakpointOptions::CommandBatonSP &command_baton_sp,
bool synchronous = false);









bool GetCommandLineCallbacks(StringList &command_list);


void ClearCallback();






















bool InvokeCallback(StoppointCallbackContext *context,
lldb::user_id_t break_id, lldb::user_id_t break_loc_id);






bool IsCallbackSynchronous() const { return m_callback_is_synchronous; }





Baton *GetBaton();





const Baton *GetBaton() const;






void SetCondition(const char *condition);






const char *GetConditionText(size_t *hash = nullptr) const;






bool IsEnabled() const { return m_enabled; }


void SetEnabled(bool enabled) {
m_enabled = enabled;
m_set_flags.Set(eEnabled);
}




bool IsAutoContinue() const { return m_auto_continue; }


void SetAutoContinue(bool auto_continue) {
m_auto_continue = auto_continue;
m_set_flags.Set(eAutoContinue);
}




bool IsOneShot() const { return m_one_shot; }


void SetOneShot(bool one_shot) {
m_one_shot = one_shot;
m_set_flags.Set(eOneShot);
}




void SetIgnoreCount(uint32_t n) {
m_ignore_count = n;
m_set_flags.Set(eIgnoreCount);
}




uint32_t GetIgnoreCount() const { return m_ignore_count; }







const ThreadSpec *GetThreadSpecNoCreate() const;




ThreadSpec *GetThreadSpec();

void SetThreadID(lldb::tid_t thread_id);

void GetDescription(Stream *s, lldb::DescriptionLevel level) const;





bool HasCallback() const;


static bool NullCallback(void *baton, StoppointCallbackContext *context,
lldb::user_id_t break_id,
lldb::user_id_t break_loc_id);





void SetCommandDataCallback(std::unique_ptr<CommandData> &cmd_data);

void Clear();

bool AnySet() const {
return m_set_flags.AnySet(eAllOptions);
}

protected:

bool IsOptionSet(OptionKind kind)
{
return m_set_flags.Test(kind);
}

enum class OptionNames {
ConditionText = 0,
IgnoreCount,
EnabledState,
OneShotState,
AutoContinue,
LastOptionName
};
static const char *g_option_names[(size_t)OptionNames::LastOptionName];

static const char *GetKey(OptionNames enum_value) {
return g_option_names[(size_t)enum_value];
}

static bool BreakpointOptionsCallbackFunction(
void *baton, StoppointCallbackContext *context, lldb::user_id_t break_id,
lldb::user_id_t break_loc_id);

void SetThreadSpec(std::unique_ptr<ThreadSpec> &thread_spec_up);

private:



BreakpointHitCallback m_callback;

lldb::BatonSP m_callback_baton_sp;
bool m_baton_is_command_baton;
bool m_callback_is_synchronous;
bool m_enabled;

bool m_one_shot;

uint32_t m_ignore_count;

std::unique_ptr<ThreadSpec> m_thread_spec_up;

std::string m_condition_text;

size_t m_condition_text_hash;

bool m_inject_condition;

bool m_auto_continue;


Flags m_set_flags;
};

}

#endif
