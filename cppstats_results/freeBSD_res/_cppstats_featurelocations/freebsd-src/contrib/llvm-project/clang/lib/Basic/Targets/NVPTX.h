











#if !defined(LLVM_CLANG_LIB_BASIC_TARGETS_NVPTX_H)
#define LLVM_CLANG_LIB_BASIC_TARGETS_NVPTX_H

#include "clang/Basic/Cuda.h"
#include "clang/Basic/TargetInfo.h"
#include "clang/Basic/TargetOptions.h"
#include "llvm/ADT/Triple.h"
#include "llvm/Support/Compiler.h"

namespace clang {
namespace targets {

static const unsigned NVPTXAddrSpaceMap[] = {
0,
1,
3,
4,
0,

0,
1,
1,
1,
4,
3,
1,
1,
1,
3,
0,
0,
0,
0
};



static const int NVPTXDWARFAddrSpaceMap[] = {
-1,
5,
-1,
8,
4,
};

class LLVM_LIBRARY_VISIBILITY NVPTXTargetInfo : public TargetInfo {
static const char *const GCCRegNames[];
static const Builtin::Info BuiltinInfo[];
CudaArch GPU;
uint32_t PTXVersion;
std::unique_ptr<TargetInfo> HostTarget;

public:
NVPTXTargetInfo(const llvm::Triple &Triple, const TargetOptions &Opts,
unsigned TargetPointerWidth);

void getTargetDefines(const LangOptions &Opts,
MacroBuilder &Builder) const override;

ArrayRef<Builtin::Info> getTargetBuiltins() const override;

bool
initFeatureMap(llvm::StringMap<bool> &Features, DiagnosticsEngine &Diags,
StringRef CPU,
const std::vector<std::string> &FeaturesVec) const override {
Features[CudaArchToString(GPU)] = true;
Features["ptx" + std::to_string(PTXVersion)] = true;
return TargetInfo::initFeatureMap(Features, Diags, CPU, FeaturesVec);
}

bool hasFeature(StringRef Feature) const override;

ArrayRef<const char *> getGCCRegNames() const override;

ArrayRef<TargetInfo::GCCRegAlias> getGCCRegAliases() const override {

return None;
}

bool validateAsmConstraint(const char *&Name,
TargetInfo::ConstraintInfo &Info) const override {
switch (*Name) {
default:
return false;
case 'c':
case 'h':
case 'r':
case 'l':
case 'f':
case 'd':
Info.setAllowsRegister();
return true;
}
}

const char *getClobbers() const override {

return "";
}

BuiltinVaListKind getBuiltinVaListKind() const override {

return TargetInfo::CharPtrBuiltinVaList;
}

bool isValidCPUName(StringRef Name) const override {
return StringToCudaArch(Name) != CudaArch::UNKNOWN;
}

void fillValidCPUList(SmallVectorImpl<StringRef> &Values) const override {
for (int i = static_cast<int>(CudaArch::SM_20);
i < static_cast<int>(CudaArch::LAST); ++i)
Values.emplace_back(CudaArchToString(static_cast<CudaArch>(i)));
}

bool setCPU(const std::string &Name) override {
GPU = StringToCudaArch(Name);
return GPU != CudaArch::UNKNOWN;
}

void setSupportedOpenCLOpts() override {
auto &Opts = getSupportedOpenCLOpts();
Opts["cl_clang_storage_class_specifiers"] = true;
Opts["__cl_clang_function_pointers"] = true;
Opts["__cl_clang_variadic_functions"] = true;
Opts["__cl_clang_non_portable_kernel_param_types"] = true;
Opts["__cl_clang_bitfields"] = true;

Opts["cl_khr_fp64"] = true;
Opts["__opencl_c_fp64"] = true;
Opts["cl_khr_byte_addressable_store"] = true;
Opts["cl_khr_global_int32_base_atomics"] = true;
Opts["cl_khr_global_int32_extended_atomics"] = true;
Opts["cl_khr_local_int32_base_atomics"] = true;
Opts["cl_khr_local_int32_extended_atomics"] = true;
}







Optional<unsigned>
getDWARFAddressSpace(unsigned AddressSpace) const override {
if (AddressSpace >= llvm::array_lengthof(NVPTXDWARFAddrSpaceMap) ||
NVPTXDWARFAddrSpaceMap[AddressSpace] < 0)
return llvm::None;
return NVPTXDWARFAddrSpaceMap[AddressSpace];
}

CallingConvCheckResult checkCallingConvention(CallingConv CC) const override {




if (HostTarget)
return HostTarget->checkCallingConvention(CC);
return CCCR_Warning;
}

bool hasExtIntType() const override { return true; }
};
}
}
#endif
