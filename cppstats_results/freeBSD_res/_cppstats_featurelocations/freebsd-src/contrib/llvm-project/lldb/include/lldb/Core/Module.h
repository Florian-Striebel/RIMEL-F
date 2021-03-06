







#if !defined(LLDB_CORE_MODULE_H)
#define LLDB_CORE_MODULE_H

#include "lldb/Core/Address.h"
#include "lldb/Core/ModuleList.h"
#include "lldb/Core/ModuleSpec.h"
#include "lldb/Symbol/ObjectFile.h"
#include "lldb/Symbol/SymbolContextScope.h"
#include "lldb/Symbol/TypeSystem.h"
#include "lldb/Target/PathMappingList.h"
#include "lldb/Utility/ArchSpec.h"
#include "lldb/Utility/ConstString.h"
#include "lldb/Utility/FileSpec.h"
#include "lldb/Utility/Status.h"
#include "lldb/Utility/XcodeSDK.h"
#include "lldb/Utility/UUID.h"
#include "lldb/lldb-defines.h"
#include "lldb/lldb-enumerations.h"
#include "lldb/lldb-forward.h"
#include "lldb/lldb-types.h"

#include "llvm/ADT/DenseSet.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/Chrono.h"

#include <atomic>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

namespace lldb_private {
class CompilerDeclContext;
class Function;
class Log;
class ObjectFile;
class RegularExpression;
class SectionList;
class Stream;
class Symbol;
class SymbolContext;
class SymbolContextList;
class SymbolFile;
class Symtab;
class Target;
class TypeList;
class TypeMap;
class VariableList;
















class Module : public std::enable_shared_from_this<Module>,
public SymbolContextScope {
public:






static size_t GetNumberAllocatedModules();

static Module *GetAllocatedModuleAtIndex(size_t idx);

static std::recursive_mutex &GetAllocationModuleCollectionMutex();























Module(
const FileSpec &file_spec, const ArchSpec &arch,
const ConstString *object_name = nullptr,
lldb::offset_t object_offset = 0,
const llvm::sys::TimePoint<> &object_mod_time = llvm::sys::TimePoint<>());

Module(const ModuleSpec &module_spec);

template <typename ObjFilePlugin, typename... Args>
static lldb::ModuleSP CreateModuleFromObjectFile(Args &&... args) {



lldb::ModuleSP module_sp(new Module());
module_sp->m_objfile_sp =
std::make_shared<ObjFilePlugin>(module_sp, std::forward<Args>(args)...);
module_sp->m_did_load_objfile.store(true, std::memory_order_relaxed);




ArchSpec arch = module_sp->m_objfile_sp->GetArchitecture();
if (!arch)
return nullptr;
module_sp->m_arch = arch;


module_sp->m_file = module_sp->m_objfile_sp->GetFileSpec();
return module_sp;
}


~Module() override;

bool MatchesModuleSpec(const ModuleSpec &module_ref);





































bool SetLoadAddress(Target &target, lldb::addr_t value, bool value_is_offset,
bool &changed);




void CalculateSymbolContext(SymbolContext *sc) override;

lldb::ModuleSP CalculateSymbolContextModule() override;

void
GetDescription(llvm::raw_ostream &s,
lldb::DescriptionLevel level = lldb::eDescriptionLevelFull);


















std::string GetSpecificationDescription() const;










void Dump(Stream *s);




void DumpSymbolContext(Stream *s) override;















const Symbol *FindFirstSymbolWithNameAndType(
ConstString name,
lldb::SymbolType symbol_type = lldb::eSymbolTypeAny);

void FindSymbolsWithNameAndType(ConstString name,
lldb::SymbolType symbol_type,
SymbolContextList &sc_list);

void FindSymbolsMatchingRegExAndType(const RegularExpression &regex,
lldb::SymbolType symbol_type,
SymbolContextList &sc_list);













void FindFunctionSymbols(ConstString name, uint32_t name_type_mask,
SymbolContextList &sc_list);












void FindCompileUnits(const FileSpec &path, SymbolContextList &sc_list);



















void FindFunctions(ConstString name,
const CompilerDeclContext &parent_decl_ctx,
lldb::FunctionNameType name_type_mask, bool symbols_ok,
bool inlines_ok, SymbolContextList &sc_list);













void FindFunctions(const RegularExpression &regex, bool symbols_ok,
bool inlines_ok, SymbolContextList &sc_list);






















void FindAddressesForLine(const lldb::TargetSP target_sp,
const FileSpec &file, uint32_t line,
Function *function,
std::vector<Address> &output_local,
std::vector<Address> &output_extern);

















void FindGlobalVariables(ConstString name,
const CompilerDeclContext &parent_decl_ctx,
size_t max_matches, VariableList &variable_list);













void FindGlobalVariables(const RegularExpression &regex, size_t max_matches,
VariableList &variable_list);































void
FindTypes(ConstString type_name, bool exact_match, size_t max_matches,
llvm::DenseSet<lldb_private::SymbolFile *> &searched_symbol_files,
TypeList &types);









void FindTypes(llvm::ArrayRef<CompilerContext> pattern, LanguageSet languages,
llvm::DenseSet<lldb_private::SymbolFile *> &searched_symbol_files,
TypeMap &types);

lldb::TypeSP FindFirstType(const SymbolContext &sc,
ConstString type_name, bool exact_match);











void FindTypesInNamespace(ConstString type_name,
const CompilerDeclContext &parent_decl_ctx,
size_t max_matches, TypeList &type_list);





const ArchSpec &GetArchitecture() const;









const FileSpec &GetFileSpec() const { return m_file; }













const FileSpec &GetPlatformFileSpec() const {
if (m_platform_file)
return m_platform_file;
return m_file;
}

void SetPlatformFileSpec(const FileSpec &file) { m_platform_file = file; }

const FileSpec &GetRemoteInstallFileSpec() const {
return m_remote_install_file;
}

void SetRemoteInstallFileSpec(const FileSpec &file) {
m_remote_install_file = file;
}

const FileSpec &GetSymbolFileFileSpec() const { return m_symfile_spec; }

void PreloadSymbols();

void SetSymbolFileFileSpec(const FileSpec &file);

const llvm::sys::TimePoint<> &GetModificationTime() const {
return m_mod_time;
}

const llvm::sys::TimePoint<> &GetObjectModificationTime() const {
return m_object_mod_time;
}




void RegisterXcodeSDK(llvm::StringRef sdk, llvm::StringRef sysroot);






bool IsExecutable();










bool IsLoadedInTarget(Target *target);

bool LoadScriptingResourceInTarget(Target *target, Status &error,
Stream *feedback_stream = nullptr);






size_t GetNumCompileUnits();

lldb::CompUnitSP GetCompileUnitAtIndex(size_t idx);

ConstString GetObjectName() const;

uint64_t GetObjectOffset() const { return m_object_offset; }













virtual ObjectFile *GetObjectFile();










virtual SectionList *GetSectionList();








virtual void SectionFileAddressesChanged();












UnwindTable &GetUnwindTable();

llvm::VersionTuple GetVersion();












ObjectFile *GetMemoryObjectFile(const lldb::ProcessSP &process_sp,
lldb::addr_t header_addr, Status &error,
size_t size_to_read = 512);









virtual SymbolFile *GetSymbolFile(bool can_create = true,
Stream *feedback_strm = nullptr);

Symtab *GetSymtab();











const lldb_private::UUID &GetUUID();











void ParseAllDebugSymbols();

bool ResolveFileAddress(lldb::addr_t vm_addr, Address &so_addr);






































uint32_t ResolveSymbolContextForAddress(
const Address &so_addr, lldb::SymbolContextItem resolve_scope,
SymbolContext &sc, bool resolve_tail_call_address = false);








































uint32_t ResolveSymbolContextForFilePath(
const char *file_path, uint32_t line, bool check_inlines,
lldb::SymbolContextItem resolve_scope, SymbolContextList &sc_list);









































uint32_t ResolveSymbolContextsForFileSpec(
const FileSpec &file_spec, uint32_t line, bool check_inlines,
lldb::SymbolContextItem resolve_scope, SymbolContextList &sc_list);

void SetFileSpecAndObjectName(const FileSpec &file,
ConstString object_name);

bool GetIsDynamicLinkEditor();

llvm::Expected<TypeSystem &>
GetTypeSystemForLanguage(lldb::LanguageType language);






void LogMessage(Log *log, const char *format, ...)
__attribute__((format(printf, 3, 4)));

void LogMessageVerboseBacktrace(Log *log, const char *format, ...)
__attribute__((format(printf, 3, 4)));

void ReportWarning(const char *format, ...)
__attribute__((format(printf, 2, 3)));

void ReportError(const char *format, ...)
__attribute__((format(printf, 2, 3)));



void ReportErrorIfModifyDetected(const char *format, ...)
__attribute__((format(printf, 2, 3)));




bool FileHasChanged() const;



std::recursive_mutex &GetMutex() const { return m_mutex; }

PathMappingList &GetSourceMappingList() { return m_source_mappings; }

const PathMappingList &GetSourceMappingList() const {
return m_source_mappings;
}



















bool FindSourceFile(const FileSpec &orig_spec, FileSpec &new_spec) const;













llvm::Optional<std::string> RemapSourceFile(llvm::StringRef path) const;
bool RemapSourceFile(const char *, std::string &) const = delete;


bool MergeArchitecture(const ArchSpec &arch_spec);






















class LookupInfo {
public:
LookupInfo() : m_name(), m_lookup_name() {}

LookupInfo(ConstString name, lldb::FunctionNameType name_type_mask,
lldb::LanguageType language);

ConstString GetName() const { return m_name; }

void SetName(ConstString name) { m_name = name; }

ConstString GetLookupName() const { return m_lookup_name; }

void SetLookupName(ConstString name) { m_lookup_name = name; }

lldb::FunctionNameType GetNameTypeMask() const { return m_name_type_mask; }

void SetNameTypeMask(lldb::FunctionNameType mask) {
m_name_type_mask = mask;
}

void Prune(SymbolContextList &sc_list, size_t start_idx) const;

protected:

ConstString m_name;


ConstString m_lookup_name;


lldb::LanguageType m_language = lldb::eLanguageTypeUnknown;



lldb::FunctionNameType m_name_type_mask = lldb::eFunctionNameTypeNone;



bool m_match_name_after_lookup = false;
};

protected:

mutable std::recursive_mutex m_mutex;



llvm::sys::TimePoint<> m_mod_time;

ArchSpec m_arch;
UUID m_uuid;

FileSpec m_file;

FileSpec m_platform_file;

FileSpec m_remote_install_file;


FileSpec m_symfile_spec;


ConstString m_object_name;


uint64_t m_object_offset = 0;
llvm::sys::TimePoint<> m_object_mod_time;




lldb::DataBufferSP m_data_sp;

lldb::ObjectFileSP m_objfile_sp;


llvm::Optional<UnwindTable> m_unwind_table;


lldb::SymbolVendorUP
m_symfile_up;
std::vector<lldb::SymbolVendorUP>
m_old_symfiles;



TypeSystemMap m_type_system_map;



PathMappingList m_source_mappings =
ModuleList::GetGlobalModuleListProperties().GetSymlinkMappings();

lldb::SectionListUP m_sections_up;



std::atomic<bool> m_did_load_objfile{false};
std::atomic<bool> m_did_load_symfile{false};
std::atomic<bool> m_did_set_uuid{false};
mutable bool m_file_has_changed : 1,
m_first_file_changed_log : 1;


































uint32_t ResolveSymbolContextForAddress(lldb::addr_t vm_addr,
bool vm_addr_is_file_addr,
lldb::SymbolContextItem resolve_scope,
Address &so_addr, SymbolContext &sc);

void SymbolIndicesToSymbolContextList(Symtab *symtab,
std::vector<uint32_t> &symbol_indexes,
SymbolContextList &sc_list);

bool SetArchitecture(const ArchSpec &new_arch);

void SetUUID(const lldb_private::UUID &uuid);

SectionList *GetUnifiedSectionList();

friend class ModuleList;
friend class ObjectFile;
friend class SymbolFile;

private:
Module();

void FindTypes_Impl(
ConstString name, const CompilerDeclContext &parent_decl_ctx,
size_t max_matches,
llvm::DenseSet<lldb_private::SymbolFile *> &searched_symbol_files,
TypeMap &types);

Module(const Module &) = delete;
const Module &operator=(const Module &) = delete;
};

}

#endif
