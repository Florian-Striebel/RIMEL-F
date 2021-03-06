







#if !defined(LLDB_CORE_PLUGININTERFACE_H)
#define LLDB_CORE_PLUGININTERFACE_H

#include "lldb/lldb-private.h"

namespace lldb_private {

class PluginInterface {
public:
PluginInterface() = default;
virtual ~PluginInterface() = default;

virtual ConstString GetPluginName() = 0;

virtual uint32_t GetPluginVersion() = 0;

PluginInterface(const PluginInterface &) = delete;
PluginInterface &operator=(const PluginInterface &) = delete;
};

}

#endif
