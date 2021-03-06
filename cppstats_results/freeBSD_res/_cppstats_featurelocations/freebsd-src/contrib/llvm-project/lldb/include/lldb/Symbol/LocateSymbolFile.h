







#if !defined(LLDB_SYMBOL_LOCATESYMBOLFILE_H)
#define LLDB_SYMBOL_LOCATESYMBOLFILE_H

#include <cstdint>

#include "lldb/Core/FileSpecList.h"
#include "lldb/Utility/FileSpec.h"

namespace lldb_private {

class ArchSpec;
class ModuleSpec;
class UUID;

class Symbols {
public:




static ModuleSpec LocateExecutableObjectFile(const ModuleSpec &module_spec);





static FileSpec
LocateExecutableSymbolFile(const ModuleSpec &module_spec,
const FileSpecList &default_search_paths);

static FileSpec FindSymbolFileInBundle(const FileSpec &dsym_bundle_fspec,
const lldb_private::UUID *uuid,
const ArchSpec *arch);










static bool DownloadObjectAndSymbolFile(ModuleSpec &module_spec,
bool force_lookup = true);
};

}

#endif
