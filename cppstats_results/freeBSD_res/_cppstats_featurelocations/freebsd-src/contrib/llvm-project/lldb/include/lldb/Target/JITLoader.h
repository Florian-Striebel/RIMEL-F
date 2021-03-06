







#if !defined(LLDB_TARGET_JITLOADER_H)
#define LLDB_TARGET_JITLOADER_H

#include <vector>

#include "lldb/Core/PluginInterface.h"
#include "lldb/Target/JITLoaderList.h"

namespace lldb_private {









class JITLoader : public PluginInterface {
public:









static void LoadPlugins(Process *process, lldb_private::JITLoaderList &list);


JITLoader(Process *process);

~JITLoader() override;





virtual void DidAttach() = 0;





virtual void DidLaunch() = 0;



virtual void ModulesDidLoad(lldb_private::ModuleList &module_list) = 0;

protected:

Process *m_process;
};

}

#endif
