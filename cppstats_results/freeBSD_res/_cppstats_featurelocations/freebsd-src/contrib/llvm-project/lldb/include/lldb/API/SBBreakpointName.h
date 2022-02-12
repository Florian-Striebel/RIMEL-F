







#if !defined(LLDB_API_SBBREAKPOINTNAME_H)
#define LLDB_API_SBBREAKPOINTNAME_H

#include "lldb/API/SBDefines.h"

class SBBreakpointNameImpl;

namespace lldb {

class LLDB_API SBBreakpointName {
public:




SBBreakpointName();

SBBreakpointName(SBTarget &target, const char *name);

SBBreakpointName(SBBreakpoint &bkpt, const char *name);

SBBreakpointName(const lldb::SBBreakpointName &rhs);

~SBBreakpointName();

const lldb::SBBreakpointName &operator=(const lldb::SBBreakpointName &rhs);



bool operator==(const lldb::SBBreakpointName &rhs);

bool operator!=(const lldb::SBBreakpointName &rhs);

explicit operator bool() const;

bool IsValid() const;

const char *GetName() const;

void SetEnabled(bool enable);

bool IsEnabled();

void SetOneShot(bool one_shot);

bool IsOneShot() const;

void SetIgnoreCount(uint32_t count);

uint32_t GetIgnoreCount() const;

void SetCondition(const char *condition);

const char *GetCondition();

void SetAutoContinue(bool auto_continue);

bool GetAutoContinue();

void SetThreadID(lldb::tid_t sb_thread_id);

lldb::tid_t GetThreadID();

void SetThreadIndex(uint32_t index);

uint32_t GetThreadIndex() const;

void SetThreadName(const char *thread_name);

const char *GetThreadName() const;

void SetQueueName(const char *queue_name);

const char *GetQueueName() const;

void SetCallback(SBBreakpointHitCallback callback, void *baton);

void SetScriptCallbackFunction(const char *callback_function_name);

SBError SetScriptCallbackFunction(const char *callback_function_name,
SBStructuredData &extra_args);

void SetCommandLineCommands(lldb::SBStringList &commands);

bool GetCommandLineCommands(lldb::SBStringList &commands);

SBError SetScriptCallbackBody(const char *script_body_text);

const char *GetHelpString() const;
void SetHelpString(const char *help_string);

bool GetAllowList() const;
void SetAllowList(bool value);

bool GetAllowDelete();
void SetAllowDelete(bool value);

bool GetAllowDisable();
void SetAllowDisable(bool value);

bool GetDescription(lldb::SBStream &description);

private:
friend class SBTarget;

lldb_private::BreakpointName *GetBreakpointName() const;
void UpdateName(lldb_private::BreakpointName &bp_name);

std::unique_ptr<SBBreakpointNameImpl> m_impl_up;
};

}

#endif