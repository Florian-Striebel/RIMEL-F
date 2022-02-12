








#if !defined(LLDB_TARGET_MEMORYHISTORY_H)
#define LLDB_TARGET_MEMORYHISTORY_H

#include <vector>

#include "lldb/Core/PluginInterface.h"
#include "lldb/lldb-private.h"
#include "lldb/lldb-types.h"

namespace lldb_private {

typedef std::vector<lldb::ThreadSP> HistoryThreads;

class MemoryHistory : public std::enable_shared_from_this<MemoryHistory>,
public PluginInterface {
public:
static lldb::MemoryHistorySP FindPlugin(const lldb::ProcessSP process);

virtual HistoryThreads GetHistoryThreads(lldb::addr_t address) = 0;
};

}

#endif
