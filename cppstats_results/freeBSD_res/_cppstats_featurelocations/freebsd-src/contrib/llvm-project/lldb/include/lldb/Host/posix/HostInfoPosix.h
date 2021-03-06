







#if !defined(LLDB_HOST_POSIX_HOSTINFOPOSIX_H)
#define LLDB_HOST_POSIX_HOSTINFOPOSIX_H

#include "lldb/Host/HostInfoBase.h"
#include "lldb/Utility/FileSpec.h"

namespace lldb_private {

class UserIDResolver;

class HostInfoPosix : public HostInfoBase {
friend class HostInfoBase;

public:
static size_t GetPageSize();
static bool GetHostname(std::string &s);

static uint32_t GetUserID();
static uint32_t GetGroupID();
static uint32_t GetEffectiveUserID();
static uint32_t GetEffectiveGroupID();

static FileSpec GetDefaultShell();

static bool GetEnvironmentVar(const std::string &var_name, std::string &var);

static UserIDResolver &GetUserIDResolver();

protected:
static bool ComputeSupportExeDirectory(FileSpec &file_spec);
static bool ComputeHeaderDirectory(FileSpec &file_spec);
};
}

#endif
