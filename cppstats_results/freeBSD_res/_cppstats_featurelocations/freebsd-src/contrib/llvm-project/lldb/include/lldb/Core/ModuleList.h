







#if !defined(LLDB_CORE_MODULELIST_H)
#define LLDB_CORE_MODULELIST_H

#include "lldb/Core/Address.h"
#include "lldb/Core/ModuleSpec.h"
#include "lldb/Core/UserSettingsController.h"
#include "lldb/Utility/FileSpec.h"
#include "lldb/Utility/Iterable.h"
#include "lldb/Utility/Status.h"
#include "lldb/lldb-enumerations.h"
#include "lldb/lldb-forward.h"
#include "lldb/lldb-types.h"

#include "llvm/ADT/DenseSet.h"
#include "llvm/Support/RWMutex.h"

#include <functional>
#include <list>
#include <mutex>
#include <vector>

#include <cstddef>
#include <cstdint>

namespace lldb_private {
class ConstString;
class FileSpecList;
class Function;
class Log;
class Module;
class RegularExpression;
class Stream;
class SymbolContext;
class SymbolContextList;
class SymbolFile;
class Target;
class TypeList;
class UUID;
class VariableList;

class ModuleListProperties : public Properties {
mutable llvm::sys::RWMutex m_symlink_paths_mutex;
PathMappingList m_symlink_paths;

void UpdateSymlinkMappings();

public:
ModuleListProperties();

FileSpec GetClangModulesCachePath() const;
bool SetClangModulesCachePath(const FileSpec &path);
bool GetEnableExternalLookup() const;
bool SetEnableExternalLookup(bool new_value);

PathMappingList GetSymlinkMappings() const;
};






class ModuleList {
public:
class Notifier {
public:
virtual ~Notifier() = default;

virtual void NotifyModuleAdded(const ModuleList &module_list,
const lldb::ModuleSP &module_sp) = 0;
virtual void NotifyModuleRemoved(const ModuleList &module_list,
const lldb::ModuleSP &module_sp) = 0;
virtual void NotifyModuleUpdated(const ModuleList &module_list,
const lldb::ModuleSP &old_module_sp,
const lldb::ModuleSP &new_module_sp) = 0;
virtual void NotifyWillClearList(const ModuleList &module_list) = 0;

virtual void NotifyModulesRemoved(lldb_private::ModuleList &module_list) = 0;
};




ModuleList();







ModuleList(const ModuleList &rhs);

ModuleList(ModuleList::Notifier *notifier);


~ModuleList();










const ModuleList &operator=(const ModuleList &rhs);















void Append(const lldb::ModuleSP &module_sp, bool notify = true);













void ReplaceEquivalent(
const lldb::ModuleSP &module_sp,
llvm::SmallVectorImpl<lldb::ModuleSP> *old_modules = nullptr);












bool AppendIfNeeded(const lldb::ModuleSP &module_sp, bool notify = true);

void Append(const ModuleList &module_list);

bool AppendIfNeeded(const ModuleList &module_list);

bool ReplaceModule(const lldb::ModuleSP &old_module_sp,
const lldb::ModuleSP &new_module_sp);






void Clear();







void Destroy();










void Dump(Stream *s) const;

void LogUUIDAndPaths(Log *log, const char *prefix_cstr);

std::recursive_mutex &GetMutex() const { return m_modules_mutex; }

size_t GetIndexForModule(const Module *module) const;











lldb::ModuleSP GetModuleAtIndex(size_t idx) const;













lldb::ModuleSP GetModuleAtIndexUnlocked(size_t idx) const;











Module *GetModulePointerAtIndex(size_t idx) const;












void FindCompileUnits(const FileSpec &path, SymbolContextList &sc_list) const;


void FindFunctions(ConstString name, lldb::FunctionNameType name_type_mask,
bool include_symbols, bool include_inlines,
SymbolContextList &sc_list) const;


void FindFunctionSymbols(ConstString name,
lldb::FunctionNameType name_type_mask,
SymbolContextList &sc_list);


void FindFunctions(const RegularExpression &name, bool include_symbols,
bool include_inlines, SymbolContextList &sc_list);













void FindGlobalVariables(ConstString name, size_t max_matches,
VariableList &variable_list) const;












void FindGlobalVariables(const RegularExpression &regex, size_t max_matches,
VariableList &variable_list) const;














void FindModules(const ModuleSpec &module_spec,
ModuleList &matching_module_list) const;

lldb::ModuleSP FindModule(const Module *module_ptr) const;






lldb::ModuleSP FindModule(const UUID &uuid) const;

lldb::ModuleSP FindFirstModule(const ModuleSpec &module_spec) const;

void FindSymbolsWithNameAndType(ConstString name,
lldb::SymbolType symbol_type,
SymbolContextList &sc_list) const;

void FindSymbolsMatchingRegExAndType(const RegularExpression &regex,
lldb::SymbolType symbol_type,
SymbolContextList &sc_list) const;

















void FindTypes(Module *search_first, ConstString name,
bool name_is_fully_qualified, size_t max_matches,
llvm::DenseSet<SymbolFile *> &searched_symbol_files,
TypeList &types) const;

bool FindSourceFile(const FileSpec &orig_spec, FileSpec &new_spec) const;






















void FindAddressesForLine(const lldb::TargetSP target_sp,
const FileSpec &file, uint32_t line,
Function *function,
std::vector<Address> &output_local,
std::vector<Address> &output_extern);















bool Remove(const lldb::ModuleSP &module_sp, bool notify = true);

size_t Remove(ModuleList &module_list);

bool RemoveIfOrphaned(const Module *module_ptr);

size_t RemoveOrphans(bool mandatory);

bool ResolveFileAddress(lldb::addr_t vm_addr, Address &so_addr) const;



uint32_t ResolveSymbolContextForAddress(const Address &so_addr,
lldb::SymbolContextItem resolve_scope,
SymbolContext &sc) const;



uint32_t ResolveSymbolContextForFilePath(
const char *file_path, uint32_t line, bool check_inlines,
lldb::SymbolContextItem resolve_scope, SymbolContextList &sc_list) const;



uint32_t ResolveSymbolContextsForFileSpec(
const FileSpec &file_spec, uint32_t line, bool check_inlines,
lldb::SymbolContextItem resolve_scope, SymbolContextList &sc_list) const;





size_t GetSize() const;
bool IsEmpty() const { return !GetSize(); }

bool LoadScriptingResourcesInTarget(Target *target, std::list<Status> &errors,
Stream *feedback_stream = nullptr,
bool continue_on_error = true);

static ModuleListProperties &GetGlobalModuleListProperties();

static bool ModuleIsInCache(const Module *module_ptr);

static Status
GetSharedModule(const ModuleSpec &module_spec, lldb::ModuleSP &module_sp,
const FileSpecList *module_search_paths_ptr,
llvm::SmallVectorImpl<lldb::ModuleSP> *old_modules,
bool *did_create_ptr, bool always_create = false);

static bool RemoveSharedModule(lldb::ModuleSP &module_sp);

static void FindSharedModules(const ModuleSpec &module_spec,
ModuleList &matching_module_list);

static size_t RemoveOrphanSharedModules(bool mandatory);

static bool RemoveSharedModuleIfOrphaned(const Module *module_ptr);

void ForEach(std::function<bool(const lldb::ModuleSP &module_sp)> const
&callback) const;

protected:

typedef std::vector<lldb::ModuleSP>
collection;

void AppendImpl(const lldb::ModuleSP &module_sp, bool use_notifier = true);

bool RemoveImpl(const lldb::ModuleSP &module_sp, bool use_notifier = true);

collection::iterator RemoveImpl(collection::iterator pos,
bool use_notifier = true);

void ClearImpl(bool use_notifier = true);


collection m_modules;
mutable std::recursive_mutex m_modules_mutex;

Notifier *m_notifier = nullptr;

public:
typedef LockingAdaptedIterable<collection, lldb::ModuleSP, vector_adapter,
std::recursive_mutex>
ModuleIterable;
ModuleIterable Modules() const {
return ModuleIterable(m_modules, GetMutex());
}

typedef AdaptedIterable<collection, lldb::ModuleSP, vector_adapter>
ModuleIterableNoLocking;
ModuleIterableNoLocking ModulesNoLocking() const {
return ModuleIterableNoLocking(m_modules);
}
};

}

#endif
