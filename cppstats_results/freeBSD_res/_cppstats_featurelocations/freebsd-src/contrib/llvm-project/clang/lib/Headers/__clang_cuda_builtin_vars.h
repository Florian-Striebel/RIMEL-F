








#if !defined(__CUDA_BUILTIN_VARS_H)
#define __CUDA_BUILTIN_VARS_H


struct uint3;
struct dim3;














#define __CUDA_DEVICE_BUILTIN(FIELD, INTRINSIC) __declspec(property(get = __fetch_builtin_##FIELD)) unsigned int FIELD; static inline __attribute__((always_inline)) __attribute__((device)) unsigned int __fetch_builtin_##FIELD(void) { return INTRINSIC; }






#if __cplusplus >= 201103L
#define __DELETE =delete
#else
#define __DELETE
#endif




#define __CUDA_DISALLOW_BUILTINVAR_ACCESS(TypeName) __attribute__((device)) TypeName() __DELETE; __attribute__((device)) TypeName(const TypeName &) __DELETE; __attribute__((device)) void operator=(const TypeName &) const __DELETE; __attribute__((device)) TypeName *operator&() const __DELETE





struct __cuda_builtin_threadIdx_t {
__CUDA_DEVICE_BUILTIN(x,__nvvm_read_ptx_sreg_tid_x());
__CUDA_DEVICE_BUILTIN(y,__nvvm_read_ptx_sreg_tid_y());
__CUDA_DEVICE_BUILTIN(z,__nvvm_read_ptx_sreg_tid_z());


__attribute__((device)) operator dim3() const;
__attribute__((device)) operator uint3() const;

private:
__CUDA_DISALLOW_BUILTINVAR_ACCESS(__cuda_builtin_threadIdx_t);
};

struct __cuda_builtin_blockIdx_t {
__CUDA_DEVICE_BUILTIN(x,__nvvm_read_ptx_sreg_ctaid_x());
__CUDA_DEVICE_BUILTIN(y,__nvvm_read_ptx_sreg_ctaid_y());
__CUDA_DEVICE_BUILTIN(z,__nvvm_read_ptx_sreg_ctaid_z());


__attribute__((device)) operator dim3() const;
__attribute__((device)) operator uint3() const;

private:
__CUDA_DISALLOW_BUILTINVAR_ACCESS(__cuda_builtin_blockIdx_t);
};

struct __cuda_builtin_blockDim_t {
__CUDA_DEVICE_BUILTIN(x,__nvvm_read_ptx_sreg_ntid_x());
__CUDA_DEVICE_BUILTIN(y,__nvvm_read_ptx_sreg_ntid_y());
__CUDA_DEVICE_BUILTIN(z,__nvvm_read_ptx_sreg_ntid_z());


__attribute__((device)) operator dim3() const;
__attribute__((device)) operator uint3() const;

private:
__CUDA_DISALLOW_BUILTINVAR_ACCESS(__cuda_builtin_blockDim_t);
};

struct __cuda_builtin_gridDim_t {
__CUDA_DEVICE_BUILTIN(x,__nvvm_read_ptx_sreg_nctaid_x());
__CUDA_DEVICE_BUILTIN(y,__nvvm_read_ptx_sreg_nctaid_y());
__CUDA_DEVICE_BUILTIN(z,__nvvm_read_ptx_sreg_nctaid_z());


__attribute__((device)) operator dim3() const;
__attribute__((device)) operator uint3() const;

private:
__CUDA_DISALLOW_BUILTINVAR_ACCESS(__cuda_builtin_gridDim_t);
};

#define __CUDA_BUILTIN_VAR extern const __attribute__((device)) __attribute__((weak))

__CUDA_BUILTIN_VAR __cuda_builtin_threadIdx_t threadIdx;
__CUDA_BUILTIN_VAR __cuda_builtin_blockIdx_t blockIdx;
__CUDA_BUILTIN_VAR __cuda_builtin_blockDim_t blockDim;
__CUDA_BUILTIN_VAR __cuda_builtin_gridDim_t gridDim;




__attribute__((device)) const int warpSize = 32;

#undef __CUDA_DEVICE_BUILTIN
#undef __CUDA_BUILTIN_VAR
#undef __CUDA_DISALLOW_BUILTINVAR_ACCESS
#undef __DELETE

#endif
