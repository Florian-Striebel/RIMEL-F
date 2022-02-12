







#if !defined(LLVM_CLANG_BASIC_CUDA_H)
#define LLVM_CLANG_BASIC_CUDA_H

namespace llvm {
class StringRef;
class Twine;
class VersionTuple;
}

namespace clang {

enum class CudaVersion {
UNKNOWN,
CUDA_70,
CUDA_75,
CUDA_80,
CUDA_90,
CUDA_91,
CUDA_92,
CUDA_100,
CUDA_101,
CUDA_102,
CUDA_110,
CUDA_111,
CUDA_112,
LATEST = CUDA_112,
LATEST_SUPPORTED = CUDA_101,
};
const char *CudaVersionToString(CudaVersion V);

CudaVersion CudaStringToVersion(const llvm::Twine &S);

enum class CudaArch {
UNUSED,
UNKNOWN,
SM_20,
SM_21,
SM_30,
SM_32,
SM_35,
SM_37,
SM_50,
SM_52,
SM_53,
SM_60,
SM_61,
SM_62,
SM_70,
SM_72,
SM_75,
SM_80,
SM_86,
GFX600,
GFX601,
GFX602,
GFX700,
GFX701,
GFX702,
GFX703,
GFX704,
GFX705,
GFX801,
GFX802,
GFX803,
GFX805,
GFX810,
GFX900,
GFX902,
GFX904,
GFX906,
GFX908,
GFX909,
GFX90a,
GFX90c,
GFX1010,
GFX1011,
GFX1012,
GFX1013,
GFX1030,
GFX1031,
GFX1032,
GFX1033,
GFX1034,
GFX1035,
LAST,
};

static inline bool IsNVIDIAGpuArch(CudaArch A) {
return A >= CudaArch::SM_20 && A < CudaArch::GFX600;
}

static inline bool IsAMDGpuArch(CudaArch A) {
return A >= CudaArch::GFX600 && A < CudaArch::LAST;
}

const char *CudaArchToString(CudaArch A);
const char *CudaArchToVirtualArchString(CudaArch A);


CudaArch StringToCudaArch(llvm::StringRef S);


CudaVersion MinVersionForCudaArch(CudaArch A);


CudaVersion MaxVersionForCudaArch(CudaArch A);


enum class CudaFeature {

CUDA_USES_NEW_LAUNCH,

CUDA_USES_FATBIN_REGISTER_END,
};

CudaVersion ToCudaVersion(llvm::VersionTuple);
bool CudaFeatureEnabled(llvm::VersionTuple, CudaFeature);
bool CudaFeatureEnabled(CudaVersion, CudaFeature);

}

#endif
