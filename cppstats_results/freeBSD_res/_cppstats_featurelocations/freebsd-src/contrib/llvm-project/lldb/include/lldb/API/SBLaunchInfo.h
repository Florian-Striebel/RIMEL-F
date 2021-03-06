







#if !defined(LLDB_API_SBLAUNCHINFO_H)
#define LLDB_API_SBLAUNCHINFO_H

#include "lldb/API/SBDefines.h"

namespace lldb_private {
class SBLaunchInfoImpl;
}

namespace lldb {

class SBPlatform;
class SBTarget;

class LLDB_API SBLaunchInfo {
public:
SBLaunchInfo(const char **argv);

~SBLaunchInfo();

SBLaunchInfo(const SBLaunchInfo &rhs);

SBLaunchInfo &operator=(const SBLaunchInfo &rhs);

lldb::pid_t GetProcessID();

uint32_t GetUserID();

uint32_t GetGroupID();

bool UserIDIsValid();

bool GroupIDIsValid();

void SetUserID(uint32_t uid);

void SetGroupID(uint32_t gid);

SBFileSpec GetExecutableFile();






















void SetExecutableFile(SBFileSpec exe_file, bool add_as_first_arg);







SBListener GetListener();






void SetListener(SBListener &listener);

uint32_t GetNumArguments();

const char *GetArgumentAtIndex(uint32_t idx);

void SetArguments(const char **argv, bool append);

uint32_t GetNumEnvironmentEntries();

const char *GetEnvironmentEntryAtIndex(uint32_t idx);














void SetEnvironmentEntries(const char **envp, bool append);












void SetEnvironment(const SBEnvironment &env, bool append);






SBEnvironment GetEnvironment();

void Clear();

const char *GetWorkingDirectory() const;

void SetWorkingDirectory(const char *working_dir);

uint32_t GetLaunchFlags();

void SetLaunchFlags(uint32_t flags);

const char *GetProcessPluginName();

void SetProcessPluginName(const char *plugin_name);

const char *GetShell();

void SetShell(const char *path);

bool GetShellExpandArguments();

void SetShellExpandArguments(bool expand);

uint32_t GetResumeCount();

void SetResumeCount(uint32_t c);

bool AddCloseFileAction(int fd);

bool AddDuplicateFileAction(int fd, int dup_fd);

bool AddOpenFileAction(int fd, const char *path, bool read, bool write);

bool AddSuppressFileAction(int fd, bool read, bool write);

void SetLaunchEventData(const char *data);

const char *GetLaunchEventData() const;

bool GetDetachOnError() const;

void SetDetachOnError(bool enable);

const char *GetScriptedProcessClassName() const;

void SetScriptedProcessClassName(const char *class_name);

lldb::SBStructuredData GetScriptedProcessDictionary() const;

void SetScriptedProcessDictionary(lldb::SBStructuredData dict);

protected:
friend class SBPlatform;
friend class SBTarget;

const lldb_private::ProcessLaunchInfo &ref() const;
void set_ref(const lldb_private::ProcessLaunchInfo &info);

std::shared_ptr<lldb_private::SBLaunchInfoImpl> m_opaque_sp;
};

}

#endif
