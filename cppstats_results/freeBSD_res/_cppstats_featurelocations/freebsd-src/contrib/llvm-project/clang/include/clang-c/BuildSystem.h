












#if !defined(LLVM_CLANG_C_BUILDSYSTEM_H)
#define LLVM_CLANG_C_BUILDSYSTEM_H

#include "clang-c/CXErrorCode.h"
#include "clang-c/CXString.h"
#include "clang-c/ExternC.h"
#include "clang-c/Platform.h"

LLVM_CLANG_C_EXTERN_C_BEGIN










CINDEX_LINKAGE unsigned long long clang_getBuildSessionTimestamp(void);





typedef struct CXVirtualFileOverlayImpl *CXVirtualFileOverlay;







CINDEX_LINKAGE CXVirtualFileOverlay
clang_VirtualFileOverlay_create(unsigned options);






CINDEX_LINKAGE enum CXErrorCode
clang_VirtualFileOverlay_addFileMapping(CXVirtualFileOverlay,
const char *virtualPath,
const char *realPath);







CINDEX_LINKAGE enum CXErrorCode
clang_VirtualFileOverlay_setCaseSensitivity(CXVirtualFileOverlay,
int caseSensitive);










CINDEX_LINKAGE enum CXErrorCode
clang_VirtualFileOverlay_writeToBuffer(CXVirtualFileOverlay, unsigned options,
char **out_buffer_ptr,
unsigned *out_buffer_size);







CINDEX_LINKAGE void clang_free(void *buffer);




CINDEX_LINKAGE void clang_VirtualFileOverlay_dispose(CXVirtualFileOverlay);




typedef struct CXModuleMapDescriptorImpl *CXModuleMapDescriptor;







CINDEX_LINKAGE CXModuleMapDescriptor
clang_ModuleMapDescriptor_create(unsigned options);





CINDEX_LINKAGE enum CXErrorCode
clang_ModuleMapDescriptor_setFrameworkModuleName(CXModuleMapDescriptor,
const char *name);





CINDEX_LINKAGE enum CXErrorCode
clang_ModuleMapDescriptor_setUmbrellaHeader(CXModuleMapDescriptor,
const char *name);










CINDEX_LINKAGE enum CXErrorCode
clang_ModuleMapDescriptor_writeToBuffer(CXModuleMapDescriptor, unsigned options,
char **out_buffer_ptr,
unsigned *out_buffer_size);




CINDEX_LINKAGE void clang_ModuleMapDescriptor_dispose(CXModuleMapDescriptor);





LLVM_CLANG_C_EXTERN_C_END

#endif

