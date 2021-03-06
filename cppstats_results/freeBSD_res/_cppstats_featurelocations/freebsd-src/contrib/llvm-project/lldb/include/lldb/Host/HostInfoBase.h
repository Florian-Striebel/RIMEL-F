







#if !defined(LLDB_HOST_HOSTINFOBASE_H)
#define LLDB_HOST_HOSTINFOBASE_H

#include "lldb/Utility/ArchSpec.h"
#include "lldb/Utility/FileSpec.h"
#include "lldb/Utility/UUID.h"
#include "lldb/Utility/UserIDResolver.h"
#include "lldb/Utility/XcodeSDK.h"
#include "lldb/lldb-enumerations.h"
#include "llvm/ADT/StringRef.h"

#include <cstdint>

#include <string>

namespace lldb_private {

class FileSpec;

struct SharedCacheImageInfo {
UUID uuid;
lldb::DataBufferSP data_sp;
};

class HostInfoBase {
private:

HostInfoBase() = default;
~HostInfoBase() = default;

public:




using SharedLibraryDirectoryHelper = void(FileSpec &this_file);

static void Initialize(SharedLibraryDirectoryHelper *helper = nullptr);
static void Terminate();





static llvm::Triple GetTargetTriple();

enum ArchitectureKind {
eArchKindDefault,

eArchKind32,

eArchKind64

};

static const ArchSpec &
GetArchitecture(ArchitectureKind arch_kind = eArchKindDefault);

static llvm::Optional<ArchitectureKind> ParseArchitectureKind(llvm::StringRef kind);



static FileSpec GetShlibDir();



static FileSpec GetSupportExeDir();



static FileSpec GetHeaderDir();



static FileSpec GetSystemPluginDir();



static FileSpec GetUserPluginDir();




static FileSpec GetProcessTempDir();




static FileSpec GetGlobalTempDir();




static ArchSpec GetAugmentedArchSpec(llvm::StringRef triple);

static bool ComputePathRelativeToLibrary(FileSpec &file_spec,
llvm::StringRef dir);

static FileSpec GetXcodeContentsDirectory() { return {}; }
static FileSpec GetXcodeDeveloperDirectory() { return {}; }


static llvm::StringRef GetXcodeSDKPath(XcodeSDK sdk) { return {}; }



static SharedCacheImageInfo
GetSharedCacheImageInfo(llvm::StringRef image_name) {
return {};
}

protected:
static bool ComputeSharedLibraryDirectory(FileSpec &file_spec);
static bool ComputeSupportExeDirectory(FileSpec &file_spec);
static bool ComputeProcessTempFileDirectory(FileSpec &file_spec);
static bool ComputeGlobalTempFileDirectory(FileSpec &file_spec);
static bool ComputeTempFileBaseDirectory(FileSpec &file_spec);
static bool ComputeHeaderDirectory(FileSpec &file_spec);
static bool ComputeSystemPluginsDirectory(FileSpec &file_spec);
static bool ComputeUserPluginsDirectory(FileSpec &file_spec);

static void ComputeHostArchitectureSupport(ArchSpec &arch_32,
ArchSpec &arch_64);
};
}

#endif
