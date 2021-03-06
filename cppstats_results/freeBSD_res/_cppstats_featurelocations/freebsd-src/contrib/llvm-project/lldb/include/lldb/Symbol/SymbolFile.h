







#if !defined(LLDB_SYMBOL_SYMBOLFILE_H)
#define LLDB_SYMBOL_SYMBOLFILE_H

#include "lldb/Core/PluginInterface.h"
#include "lldb/Core/SourceLocationSpec.h"
#include "lldb/Symbol/CompilerDecl.h"
#include "lldb/Symbol/CompilerDeclContext.h"
#include "lldb/Symbol/CompilerType.h"
#include "lldb/Symbol/Function.h"
#include "lldb/Symbol/SourceModule.h"
#include "lldb/Symbol/Type.h"
#include "lldb/Symbol/TypeList.h"
#include "lldb/Symbol/TypeSystem.h"
#include "lldb/Utility/XcodeSDK.h"
#include "lldb/lldb-private.h"
#include "llvm/ADT/DenseSet.h"
#include "llvm/Support/Errc.h"

#include <mutex>

#if defined(LLDB_CONFIGURATION_DEBUG)
#define ASSERT_MODULE_LOCK(expr) (expr->AssertModuleLock())
#else
#define ASSERT_MODULE_LOCK(expr) ((void)0)
#endif

namespace lldb_private {

class SymbolFile : public PluginInterface {

static char ID;

public:


virtual bool isA(const void *ClassID) const { return ClassID == &ID; }
static bool classof(const SymbolFile *obj) { return obj->isA(&ID); }








enum Abilities {
CompileUnits = (1u << 0),
LineTables = (1u << 1),
Functions = (1u << 2),
Blocks = (1u << 3),
GlobalVariables = (1u << 4),
LocalVariables = (1u << 5),
VariableTypes = (1u << 6),
kAllAbilities = ((1u << 7) - 1u)
};

static SymbolFile *FindPlugin(lldb::ObjectFileSP objfile_sp);


SymbolFile(lldb::ObjectFileSP objfile_sp)
: m_objfile_sp(std::move(objfile_sp)), m_abilities(0),
m_calculated_abilities(false) {}

~SymbolFile() override = default;




























uint32_t GetAbilities() {
if (!m_calculated_abilities) {
m_abilities = CalculateAbilities();
m_calculated_abilities = true;
}

return m_abilities;
}

virtual uint32_t CalculateAbilities() = 0;



virtual std::recursive_mutex &GetModuleMutex() const;








virtual void InitializeObject() {}



uint32_t GetNumCompileUnits();
lldb::CompUnitSP GetCompileUnitAtIndex(uint32_t idx);

Symtab *GetSymtab();

virtual lldb::LanguageType ParseLanguage(CompileUnit &comp_unit) = 0;

virtual XcodeSDK ParseXcodeSDK(CompileUnit &comp_unit) { return {}; }
virtual size_t ParseFunctions(CompileUnit &comp_unit) = 0;
virtual bool ParseLineTable(CompileUnit &comp_unit) = 0;
virtual bool ParseDebugMacros(CompileUnit &comp_unit) = 0;



























virtual bool ForEachExternalModule(
lldb_private::CompileUnit &comp_unit,
llvm::DenseSet<lldb_private::SymbolFile *> &visited_symbol_files,
llvm::function_ref<bool(Module &)> lambda) {
return false;
}
virtual bool ParseSupportFiles(CompileUnit &comp_unit,
FileSpecList &support_files) = 0;
virtual size_t ParseTypes(CompileUnit &comp_unit) = 0;
virtual bool ParseIsOptimized(CompileUnit &comp_unit) { return false; }

virtual bool
ParseImportedModules(const SymbolContext &sc,
std::vector<SourceModule> &imported_modules) = 0;
virtual size_t ParseBlocksRecursive(Function &func) = 0;
virtual size_t ParseVariablesForContext(const SymbolContext &sc) = 0;
virtual Type *ResolveTypeUID(lldb::user_id_t type_uid) = 0;



struct ArrayInfo {
int64_t first_index = 0;
llvm::SmallVector<uint64_t, 1> element_orders;
uint32_t byte_stride = 0;
uint32_t bit_stride = 0;
};




virtual llvm::Optional<ArrayInfo>
GetDynamicArrayInfoForUID(lldb::user_id_t type_uid,
const lldb_private::ExecutionContext *exe_ctx) = 0;

virtual bool CompleteType(CompilerType &compiler_type) = 0;
virtual void ParseDeclsForContext(CompilerDeclContext decl_ctx) {}
virtual CompilerDecl GetDeclForUID(lldb::user_id_t uid) {
return CompilerDecl();
}
virtual CompilerDeclContext GetDeclContextForUID(lldb::user_id_t uid) {
return CompilerDeclContext();
}
virtual CompilerDeclContext GetDeclContextContainingUID(lldb::user_id_t uid) {
return CompilerDeclContext();
}
virtual uint32_t ResolveSymbolContext(const Address &so_addr,
lldb::SymbolContextItem resolve_scope,
SymbolContext &sc) = 0;
virtual uint32_t
ResolveSymbolContext(const SourceLocationSpec &src_location_spec,
lldb::SymbolContextItem resolve_scope,
SymbolContextList &sc_list);

virtual void DumpClangAST(Stream &s) {}
virtual void FindGlobalVariables(ConstString name,
const CompilerDeclContext &parent_decl_ctx,
uint32_t max_matches,
VariableList &variables);
virtual void FindGlobalVariables(const RegularExpression &regex,
uint32_t max_matches,
VariableList &variables);
virtual void FindFunctions(ConstString name,
const CompilerDeclContext &parent_decl_ctx,
lldb::FunctionNameType name_type_mask,
bool include_inlines, SymbolContextList &sc_list);
virtual void FindFunctions(const RegularExpression &regex,
bool include_inlines, SymbolContextList &sc_list);
virtual void
FindTypes(ConstString name, const CompilerDeclContext &parent_decl_ctx,
uint32_t max_matches,
llvm::DenseSet<lldb_private::SymbolFile *> &searched_symbol_files,
TypeMap &types);






virtual void
FindTypes(llvm::ArrayRef<CompilerContext> pattern, LanguageSet languages,
llvm::DenseSet<lldb_private::SymbolFile *> &searched_symbol_files,
TypeMap &types);

virtual void
GetMangledNamesForFunction(const std::string &scope_qualified_name,
std::vector<ConstString> &mangled_names);

virtual void GetTypes(lldb_private::SymbolContextScope *sc_scope,
lldb::TypeClass type_mask,
lldb_private::TypeList &type_list) = 0;

virtual void PreloadSymbols();

virtual llvm::Expected<lldb_private::TypeSystem &>
GetTypeSystemForLanguage(lldb::LanguageType language);

virtual CompilerDeclContext
FindNamespace(ConstString name, const CompilerDeclContext &parent_decl_ctx) {
return CompilerDeclContext();
}

ObjectFile *GetObjectFile() { return m_objfile_sp.get(); }
const ObjectFile *GetObjectFile() const { return m_objfile_sp.get(); }
ObjectFile *GetMainObjectFile();

virtual std::vector<std::unique_ptr<CallEdge>>
ParseCallEdgesInFunction(UserID func_id) {
return {};
}

virtual void AddSymbols(Symtab &symtab) {}



virtual void SectionFileAddressesChanged();

struct RegisterInfoResolver {
virtual ~RegisterInfoResolver();

virtual const RegisterInfo *ResolveName(llvm::StringRef name) const = 0;
virtual const RegisterInfo *ResolveNumber(lldb::RegisterKind kind,
uint32_t number) const = 0;
};
virtual lldb::UnwindPlanSP
GetUnwindPlan(const Address &address, const RegisterInfoResolver &resolver) {
return nullptr;
}



virtual llvm::Expected<lldb::addr_t> GetParameterStackSize(Symbol &symbol) {
return llvm::createStringError(make_error_code(llvm::errc::not_supported),
"Operation not supported.");
}

virtual void Dump(Stream &s);

protected:
void AssertModuleLock();
virtual uint32_t CalculateNumCompileUnits() = 0;
virtual lldb::CompUnitSP ParseCompileUnitAtIndex(uint32_t idx) = 0;
virtual TypeList &GetTypeList() { return m_type_list; }

void SetCompileUnitAtIndex(uint32_t idx, const lldb::CompUnitSP &cu_sp);

lldb::ObjectFileSP m_objfile_sp;



llvm::Optional<std::vector<lldb::CompUnitSP>> m_compile_units;
TypeList m_type_list;
Symtab *m_symtab = nullptr;
uint32_t m_abilities;
bool m_calculated_abilities;

private:
SymbolFile(const SymbolFile &) = delete;
const SymbolFile &operator=(const SymbolFile &) = delete;
};

}

#endif
