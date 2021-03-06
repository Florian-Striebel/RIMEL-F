







#if !defined(LLDB_TARGET_SYSTEMRUNTIME_H)
#define LLDB_TARGET_SYSTEMRUNTIME_H

#include <vector>

#include "lldb/Core/ModuleList.h"
#include "lldb/Core/PluginInterface.h"
#include "lldb/Target/QueueItem.h"
#include "lldb/Target/QueueList.h"
#include "lldb/Target/Runtime.h"
#include "lldb/Utility/ConstString.h"
#include "lldb/Utility/StructuredData.h"
#include "lldb/lldb-private.h"
#include "lldb/lldb-public.h"

namespace lldb_private {


















class SystemRuntime : public Runtime, public PluginInterface {
public:








static SystemRuntime *FindPlugin(Process *process);


SystemRuntime(Process *process);





~SystemRuntime() override;





virtual void DidAttach();





virtual void DidLaunch();





virtual void ModulesDidLoad(const ModuleList &module_list) override;





virtual void Detach();






















virtual const std::vector<ConstString> &GetExtendedBacktraceTypes();

























virtual lldb::ThreadSP GetExtendedBacktraceThread(lldb::ThreadSP thread,
ConstString type);




















virtual lldb::ThreadSP
GetExtendedBacktraceForQueueItem(lldb::QueueItemSP queue_item_sp,
ConstString type) {
return lldb::ThreadSP();
}











virtual void PopulateQueueList(lldb_private::QueueList &queue_list) {}















virtual std::string
GetQueueNameFromThreadQAddress(lldb::addr_t dispatch_qaddr) {
return "";
}















virtual lldb::queue_id_t
GetQueueIDFromThreadQAddress(lldb::addr_t dispatch_qaddr) {
return LLDB_INVALID_QUEUE_ID;
}















virtual lldb::addr_t
GetLibdispatchQueueAddressFromThreadQAddress(lldb::addr_t dispatch_qaddr) {
return LLDB_INVALID_ADDRESS;
}









virtual lldb::QueueKind GetQueueKind(lldb::addr_t dispatch_qaddr) {
return lldb::eQueueKindUnknown;
}









virtual void PopulatePendingItemsForQueue(lldb_private::Queue *queue) {}













virtual void CompleteQueueItem(lldb_private::QueueItem *queue_item,
lldb::addr_t item_ref) {}









virtual void AddThreadExtendedInfoPacketHints(
lldb_private::StructuredData::ObjectSP dict) {}














virtual bool SafeToCallFunctionsOnThisThread(lldb::ThreadSP thread_sp) {
return true;
}

protected:
std::vector<ConstString> m_types;

private:
SystemRuntime(const SystemRuntime &) = delete;
const SystemRuntime &operator=(const SystemRuntime &) = delete;
};

}

#endif
