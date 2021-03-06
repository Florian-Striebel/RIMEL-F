







#if !defined(LLDB_SYMBOL_UNWINDTABLE_H)
#define LLDB_SYMBOL_UNWINDTABLE_H

#include <map>
#include <mutex>

#include "lldb/lldb-private.h"

namespace lldb_private {





class UnwindTable {
public:

explicit UnwindTable(Module &module);

~UnwindTable();

lldb_private::CallFrameInfo *GetObjectFileUnwindInfo();

lldb_private::DWARFCallFrameInfo *GetEHFrameInfo();
lldb_private::DWARFCallFrameInfo *GetDebugFrameInfo();

lldb_private::CompactUnwindInfo *GetCompactUnwindInfo();

ArmUnwindInfo *GetArmUnwindInfo();
SymbolFile *GetSymbolFile();

lldb::FuncUnwindersSP GetFuncUnwindersContainingAddress(const Address &addr,
SymbolContext &sc);

bool GetAllowAssemblyEmulationUnwindPlans();









lldb::FuncUnwindersSP
GetUncachedFuncUnwindersContainingAddress(const Address &addr,
SymbolContext &sc);

ArchSpec GetArchitecture();

private:
void Dump(Stream &s);

void Initialize();
llvm::Optional<AddressRange> GetAddressRange(const Address &addr,
SymbolContext &sc);

typedef std::map<lldb::addr_t, lldb::FuncUnwindersSP> collection;
typedef collection::iterator iterator;
typedef collection::const_iterator const_iterator;

Module &m_module;
collection m_unwinds;

bool m_initialized;
std::mutex m_mutex;

std::unique_ptr<CallFrameInfo> m_object_file_unwind_up;
std::unique_ptr<DWARFCallFrameInfo> m_eh_frame_up;
std::unique_ptr<DWARFCallFrameInfo> m_debug_frame_up;
std::unique_ptr<CompactUnwindInfo> m_compact_unwind_up;
std::unique_ptr<ArmUnwindInfo> m_arm_unwind_up;

UnwindTable(const UnwindTable &) = delete;
const UnwindTable &operator=(const UnwindTable &) = delete;
};

}

#endif
