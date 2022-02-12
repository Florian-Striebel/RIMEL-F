







#if !defined(LLVM_CLANG_BASIC_DARWIN_SDK_INFO_H)
#define LLVM_CLANG_BASIC_DARWIN_SDK_INFO_H

#include "clang/Basic/LLVM.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/Triple.h"
#include "llvm/Support/Error.h"
#include "llvm/Support/VersionTuple.h"
#include "llvm/Support/VirtualFileSystem.h"

namespace llvm {
namespace json {
class Object;
}
}

namespace clang {


class DarwinSDKInfo {
public:


struct OSEnvPair {
public:
using StorageType = uint64_t;

constexpr OSEnvPair(llvm::Triple::OSType FromOS,
llvm::Triple::EnvironmentType FromEnv,
llvm::Triple::OSType ToOS,
llvm::Triple::EnvironmentType ToEnv)
: Value(((StorageType(FromOS) * StorageType(llvm::Triple::LastOSType) +
StorageType(FromEnv))
<< 32ull) |
(StorageType(ToOS) * StorageType(llvm::Triple::LastOSType) +
StorageType(ToEnv))) {}



static inline constexpr OSEnvPair macOStoMacCatalystPair() {
return OSEnvPair(llvm::Triple::MacOSX, llvm::Triple::UnknownEnvironment,
llvm::Triple::IOS, llvm::Triple::MacABI);
}



static inline constexpr OSEnvPair macCatalystToMacOSPair() {
return OSEnvPair(llvm::Triple::IOS, llvm::Triple::MacABI,
llvm::Triple::MacOSX, llvm::Triple::UnknownEnvironment);
}

private:
StorageType Value;

friend class DarwinSDKInfo;
};






class RelatedTargetVersionMapping {
public:
RelatedTargetVersionMapping(
VersionTuple MinimumKeyVersion, VersionTuple MaximumKeyVersion,
VersionTuple MinimumValue, VersionTuple MaximumValue,
llvm::DenseMap<VersionTuple, VersionTuple> Mapping)
: MinimumKeyVersion(MinimumKeyVersion),
MaximumKeyVersion(MaximumKeyVersion), MinimumValue(MinimumValue),
MaximumValue(MaximumValue), Mapping(Mapping) {
assert(!this->Mapping.empty() && "unexpected empty mapping");
}


const VersionTuple &getMinimumValue() const { return MinimumValue; }




Optional<VersionTuple> map(const VersionTuple &Key,
const VersionTuple &MinimumValue,
Optional<VersionTuple> MaximumValue) const;

static Optional<RelatedTargetVersionMapping>
parseJSON(const llvm::json::Object &Obj,
VersionTuple MaximumDeploymentTarget);

private:
VersionTuple MinimumKeyVersion;
VersionTuple MaximumKeyVersion;
VersionTuple MinimumValue;
VersionTuple MaximumValue;
llvm::DenseMap<VersionTuple, VersionTuple> Mapping;
};

DarwinSDKInfo(VersionTuple Version, VersionTuple MaximumDeploymentTarget,
llvm::DenseMap<OSEnvPair::StorageType,
Optional<RelatedTargetVersionMapping>>
VersionMappings =
llvm::DenseMap<OSEnvPair::StorageType,
Optional<RelatedTargetVersionMapping>>())
: Version(Version), MaximumDeploymentTarget(MaximumDeploymentTarget),
VersionMappings(std::move(VersionMappings)) {}

const llvm::VersionTuple &getVersion() const { return Version; }












const RelatedTargetVersionMapping *getVersionMapping(OSEnvPair Kind) const {
auto Mapping = VersionMappings.find(Kind.Value);
if (Mapping == VersionMappings.end())
return nullptr;
return Mapping->getSecond().hasValue() ? Mapping->getSecond().getPointer()
: nullptr;
}

static Optional<DarwinSDKInfo>
parseDarwinSDKSettingsJSON(const llvm::json::Object *Obj);

private:
VersionTuple Version;
VersionTuple MaximumDeploymentTarget;



llvm::DenseMap<OSEnvPair::StorageType, Optional<RelatedTargetVersionMapping>>
VersionMappings;
};





Expected<Optional<DarwinSDKInfo>> parseDarwinSDKInfo(llvm::vfs::FileSystem &VFS,
StringRef SDKRootPath);

}

#endif
