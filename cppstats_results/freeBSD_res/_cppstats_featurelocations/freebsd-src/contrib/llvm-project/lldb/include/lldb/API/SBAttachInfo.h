







#if !defined(LLDB_API_SBATTACHINFO_H)
#define LLDB_API_SBATTACHINFO_H

#include "lldb/API/SBDefines.h"

namespace lldb {

class SBTarget;

class LLDB_API SBAttachInfo {
public:
SBAttachInfo();

SBAttachInfo(lldb::pid_t pid);












SBAttachInfo(const char *path, bool wait_for);























SBAttachInfo(const char *path, bool wait_for, bool async);

SBAttachInfo(const SBAttachInfo &rhs);

~SBAttachInfo();

SBAttachInfo &operator=(const SBAttachInfo &rhs);

lldb::pid_t GetProcessID();

void SetProcessID(lldb::pid_t pid);

void SetExecutable(const char *path);

void SetExecutable(lldb::SBFileSpec exe_file);

bool GetWaitForLaunch();










void SetWaitForLaunch(bool b);





















void SetWaitForLaunch(bool b, bool async);

bool GetIgnoreExisting();

void SetIgnoreExisting(bool b);

uint32_t GetResumeCount();

void SetResumeCount(uint32_t c);

const char *GetProcessPluginName();

void SetProcessPluginName(const char *plugin_name);

uint32_t GetUserID();

uint32_t GetGroupID();

bool UserIDIsValid();

bool GroupIDIsValid();

void SetUserID(uint32_t uid);

void SetGroupID(uint32_t gid);

uint32_t GetEffectiveUserID();

uint32_t GetEffectiveGroupID();

bool EffectiveUserIDIsValid();

bool EffectiveGroupIDIsValid();

void SetEffectiveUserID(uint32_t uid);

void SetEffectiveGroupID(uint32_t gid);

lldb::pid_t GetParentProcessID();

void SetParentProcessID(lldb::pid_t pid);

bool ParentProcessIDIsValid();







SBListener GetListener();






void SetListener(SBListener &listener);

protected:
friend class SBTarget;

lldb_private::ProcessAttachInfo &ref();

ProcessAttachInfoSP m_opaque_sp;
};

}

#endif
