







#if !defined(LLDB_TARGET_MODULECACHE_H)
#define LLDB_TARGET_MODULECACHE_H

#include "lldb/lldb-forward.h"
#include "lldb/lldb-types.h"

#include "lldb/Host/File.h"
#include "lldb/Utility/FileSpec.h"
#include "lldb/Utility/Status.h"

#include <functional>
#include <string>
#include <unordered_map>

namespace lldb_private {

class Module;
class UUID;




















class ModuleCache {
public:
using ModuleDownloader =
std::function<Status(const ModuleSpec &, const FileSpec &)>;
using SymfileDownloader =
std::function<Status(const lldb::ModuleSP &, const FileSpec &)>;

Status GetAndPut(const FileSpec &root_dir_spec, const char *hostname,
const ModuleSpec &module_spec,
const ModuleDownloader &module_downloader,
const SymfileDownloader &symfile_downloader,
lldb::ModuleSP &cached_module_sp, bool *did_create_ptr);

private:
Status Put(const FileSpec &root_dir_spec, const char *hostname,
const ModuleSpec &module_spec, const FileSpec &tmp_file,
const FileSpec &target_file);

Status Get(const FileSpec &root_dir_spec, const char *hostname,
const ModuleSpec &module_spec, lldb::ModuleSP &cached_module_sp,
bool *did_create_ptr);

std::unordered_map<std::string, lldb::ModuleWP> m_loaded_modules;
};

}

#endif
