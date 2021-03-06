







#if !defined(lldb_Host_netbsd_HostInfoNetBSD_h_)
#define lldb_Host_netbsd_HostInfoNetBSD_h_

#include "lldb/Host/posix/HostInfoPosix.h"
#include "lldb/Utility/FileSpec.h"
#include "llvm/Support/VersionTuple.h"

namespace lldb_private {

class HostInfoNetBSD : public HostInfoPosix {
public:
static llvm::VersionTuple GetOSVersion();
static bool GetOSBuildString(std::string &s);
static bool GetOSKernelDescription(std::string &s);
static FileSpec GetProgramFileSpec();
};
}

#endif
