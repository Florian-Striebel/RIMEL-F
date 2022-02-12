







#if !defined(LLDB_TARGET_DYNAMICLOADER_H)
#define LLDB_TARGET_DYNAMICLOADER_H

#include "lldb/Core/PluginInterface.h"
#include "lldb/Utility/FileSpec.h"
#include "lldb/Utility/Status.h"
#include "lldb/Utility/UUID.h"
#include "lldb/lldb-defines.h"
#include "lldb/lldb-forward.h"
#include "lldb/lldb-private-enumerations.h"
#include "lldb/lldb-types.h"

#include <cstddef>
#include <cstdint>
namespace lldb_private {
class ModuleList;
class Process;
class SectionList;
class Symbol;
class SymbolContext;
class SymbolContextList;
class Thread;
}

namespace lldb_private {


















class DynamicLoader : public PluginInterface {
public:












static DynamicLoader *FindPlugin(Process *process, const char *plugin_name);


DynamicLoader(Process *process);





virtual void DidAttach() = 0;





virtual void DidLaunch() = 0;







virtual bool ProcessDidExec() { return false; }











bool GetStopWhenImagesChange() const;












void SetStopWhenImagesChange(bool stop);











virtual lldb::ThreadPlanSP GetStepThroughTrampolinePlan(Thread &thread,
bool stop_others) = 0;

















virtual void FindEquivalentSymbols(Symbol *original_symbol,
ModuleList &module_list,
SymbolContextList &equivalent_symbols) {}











virtual Status CanLoadImage() = 0;





















virtual bool AlwaysRelyOnEHUnwindInfo(SymbolContext &sym_ctx) {
return false;
}













virtual lldb::addr_t GetThreadLocalData(const lldb::ModuleSP module,
const lldb::ThreadSP thread,
lldb::addr_t tls_file_addr) {
return LLDB_INVALID_ADDRESS;
}



virtual lldb::ModuleSP LoadModuleAtAddress(const lldb_private::FileSpec &file,
lldb::addr_t link_map_addr,
lldb::addr_t base_addr,
bool base_addr_is_offset);



































virtual bool GetSharedCacheInformation(lldb::addr_t &base_address, UUID &uuid,
LazyBool &using_shared_cache,
LazyBool &private_shared_cache) {
base_address = LLDB_INVALID_ADDRESS;
uuid.Clear();
using_shared_cache = eLazyBoolCalculate;
private_shared_cache = eLazyBoolCalculate;
return false;
}







virtual bool IsFullyInitialized() { return true; }

protected:




lldb::ModuleSP GetTargetExecutable();









virtual void UpdateLoadedSections(lldb::ModuleSP module,
lldb::addr_t link_map_addr,
lldb::addr_t base_addr,
bool base_addr_is_offset);



void UpdateLoadedSectionsCommon(lldb::ModuleSP module, lldb::addr_t base_addr,
bool base_addr_is_offset);




virtual void UnloadSections(const lldb::ModuleSP module);


void UnloadSectionsCommon(const lldb::ModuleSP module);

const lldb_private::SectionList *
GetSectionListFromModule(const lldb::ModuleSP module) const;



int64_t ReadUnsignedIntWithSizeInBytes(lldb::addr_t addr, int size_in_bytes);



lldb::addr_t ReadPointer(lldb::addr_t addr);


void LoadOperatingSystemPlugin(bool flush);



Process
*m_process;
};

}

#endif
