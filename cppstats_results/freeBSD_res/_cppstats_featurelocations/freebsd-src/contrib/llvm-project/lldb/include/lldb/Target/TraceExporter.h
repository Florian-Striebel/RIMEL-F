







#if !defined(LLDB_TARGET_TRACE_EXPORTER_H)
#define LLDB_TARGET_TRACE_EXPORTER_H

#include "lldb/Core/PluginInterface.h"

namespace lldb_private {











class TraceExporter : public PluginInterface {
public:








static llvm::Expected<lldb::TraceExporterUP>
FindPlugin(llvm::StringRef plugin_name);
};

}

#endif
