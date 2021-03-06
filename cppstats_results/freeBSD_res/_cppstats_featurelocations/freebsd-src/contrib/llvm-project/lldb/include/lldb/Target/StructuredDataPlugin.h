







#if !defined(LLDB_TARGET_STRUCTUREDDATAPLUGIN_H)
#define LLDB_TARGET_STRUCTUREDDATAPLUGIN_H

#include "lldb/Core/PluginInterface.h"
#include "lldb/Utility/StructuredData.h"

namespace lldb_private {

class CommandObjectMultiword;






















class StructuredDataPlugin
: public PluginInterface,
public std::enable_shared_from_this<StructuredDataPlugin> {
public:
~StructuredDataPlugin() override;

lldb::ProcessSP GetProcess() const;




















virtual bool SupportsStructuredDataType(ConstString type_name) = 0;


























virtual void
HandleArrivalOfStructuredData(Process &process, ConstString type_name,
const StructuredData::ObjectSP &object_sp) = 0;


















virtual Status GetDescription(const StructuredData::ObjectSP &object_sp,
lldb_private::Stream &stream) = 0;










virtual bool GetEnabled(ConstString type_name) const;
















virtual void ModulesDidLoad(Process &process, ModuleList &module_list);

protected:

StructuredDataPlugin(const lldb::ProcessWP &process_wp);











static void InitializeBasePluginForDebugger(Debugger &debugger);

private:
lldb::ProcessWP m_process_wp;

StructuredDataPlugin(const StructuredDataPlugin &) = delete;
const StructuredDataPlugin &operator=(const StructuredDataPlugin &) = delete;
};
}

#endif
