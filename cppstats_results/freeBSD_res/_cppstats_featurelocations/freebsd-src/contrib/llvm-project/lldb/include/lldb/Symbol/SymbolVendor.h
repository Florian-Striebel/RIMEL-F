







#if !defined(LLDB_SYMBOL_SYMBOLVENDOR_H)
#define LLDB_SYMBOL_SYMBOLVENDOR_H

#include <vector>

#include "lldb/Core/ModuleChild.h"
#include "lldb/Core/PluginInterface.h"
#include "lldb/Symbol/SourceModule.h"
#include "lldb/Symbol/SymbolFile.h"
#include "lldb/Symbol/TypeMap.h"
#include "lldb/lldb-private.h"
#include "llvm/ADT/DenseSet.h"

namespace lldb_private {








class SymbolVendor : public ModuleChild, public PluginInterface {
public:
static SymbolVendor *FindPlugin(const lldb::ModuleSP &module_sp,
Stream *feedback_strm);


SymbolVendor(const lldb::ModuleSP &module_sp);

void AddSymbolFileRepresentation(const lldb::ObjectFileSP &objfile_sp);

SymbolFile *GetSymbolFile() { return m_sym_file_up.get(); }


ConstString GetPluginName() override;

uint32_t GetPluginVersion() override;

protected:
std::unique_ptr<SymbolFile> m_sym_file_up;

};

}

#endif
