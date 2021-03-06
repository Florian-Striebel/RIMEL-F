













#if !defined(LLVM_CLANG_BASIC_ADDRESSSPACES_H)
#define LLVM_CLANG_BASIC_ADDRESSSPACES_H

#include <cassert>

namespace clang {




enum class LangAS : unsigned {


Default = 0,





opencl_global,
opencl_local,
opencl_constant,
opencl_private,
opencl_generic,
opencl_global_device,
opencl_global_host,


cuda_device,
cuda_constant,
cuda_shared,


sycl_global,
sycl_global_device,
sycl_global_host,
sycl_local,
sycl_private,


ptr32_sptr,
ptr32_uptr,
ptr64,




FirstTargetAddressSpace
};



using LangASMap = unsigned[(unsigned)LangAS::FirstTargetAddressSpace];



inline bool isTargetAddressSpace(LangAS AS) {
return (unsigned)AS >= (unsigned)LangAS::FirstTargetAddressSpace;
}

inline unsigned toTargetAddressSpace(LangAS AS) {
assert(isTargetAddressSpace(AS));
return (unsigned)AS - (unsigned)LangAS::FirstTargetAddressSpace;
}

inline LangAS getLangASFromTargetAS(unsigned TargetAS) {
return static_cast<LangAS>((TargetAS) +
(unsigned)LangAS::FirstTargetAddressSpace);
}

inline bool isPtrSizeAddressSpace(LangAS AS) {
return (AS == LangAS::ptr32_sptr || AS == LangAS::ptr32_uptr ||
AS == LangAS::ptr64);
}

}

#endif
