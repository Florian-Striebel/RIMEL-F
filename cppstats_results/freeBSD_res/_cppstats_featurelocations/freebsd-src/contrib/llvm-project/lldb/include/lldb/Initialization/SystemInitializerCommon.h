







#if !defined(LLDB_INITIALIZATION_SYSTEMINITIALIZERCOMMON_H)
#define LLDB_INITIALIZATION_SYSTEMINITIALIZERCOMMON_H

#include "SystemInitializer.h"
#include "lldb/Host/HostInfo.h"

namespace lldb_private {








class SystemInitializerCommon : public SystemInitializer {
public:
SystemInitializerCommon(HostInfo::SharedLibraryDirectoryHelper *helper);
~SystemInitializerCommon() override;

llvm::Error Initialize() override;
void Terminate() override;

private:
HostInfo::SharedLibraryDirectoryHelper *m_shlib_dir_helper;
};

}

#endif
