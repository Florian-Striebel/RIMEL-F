







#if !defined(LLDB_TARGET_INSTRUMENTATIONRUNTIME_H)
#define LLDB_TARGET_INSTRUMENTATIONRUNTIME_H

#include <map>
#include <vector>

#include "lldb/Core/PluginInterface.h"
#include "lldb/Utility/StructuredData.h"
#include "lldb/lldb-forward.h"
#include "lldb/lldb-private.h"
#include "lldb/lldb-types.h"

namespace lldb_private {

typedef std::map<lldb::InstrumentationRuntimeType,
lldb::InstrumentationRuntimeSP>
InstrumentationRuntimeCollection;

class InstrumentationRuntime
: public std::enable_shared_from_this<InstrumentationRuntime>,
public PluginInterface {

lldb::ProcessWP m_process_wp;


lldb::ModuleSP m_runtime_module;


lldb::user_id_t m_breakpoint_id;



bool m_is_active;

protected:
InstrumentationRuntime(const lldb::ProcessSP &process_sp)
: m_process_wp(), m_runtime_module(), m_breakpoint_id(0),
m_is_active(false) {
if (process_sp)
m_process_wp = process_sp;
}

lldb::ProcessSP GetProcessSP() { return m_process_wp.lock(); }

lldb::ModuleSP GetRuntimeModuleSP() { return m_runtime_module; }

void SetRuntimeModuleSP(lldb::ModuleSP module_sp) {
m_runtime_module = std::move(module_sp);
}

lldb::user_id_t GetBreakpointID() const { return m_breakpoint_id; }

void SetBreakpointID(lldb::user_id_t ID) { m_breakpoint_id = ID; }

void SetActive(bool IsActive) { m_is_active = IsActive; }



virtual const RegularExpression &GetPatternForRuntimeLibrary() = 0;


virtual bool CheckIfRuntimeIsValid(const lldb::ModuleSP module_sp) = 0;




virtual void Activate() = 0;

public:
static void ModulesDidLoad(lldb_private::ModuleList &module_list,
Process *process,
InstrumentationRuntimeCollection &runtimes);




void ModulesDidLoad(lldb_private::ModuleList &module_list);

bool IsActive() const { return m_is_active; }

virtual lldb::ThreadCollectionSP
GetBacktracesFromExtendedStopInfo(StructuredData::ObjectSP info);
};

}

#endif
