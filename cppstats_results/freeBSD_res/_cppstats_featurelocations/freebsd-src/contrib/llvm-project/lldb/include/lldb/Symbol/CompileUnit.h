







#if !defined(LLDB_SYMBOL_COMPILEUNIT_H)
#define LLDB_SYMBOL_COMPILEUNIT_H

#include "lldb/Core/FileSpecList.h"
#include "lldb/Core/ModuleChild.h"
#include "lldb/Core/SourceLocationSpec.h"
#include "lldb/Symbol/DebugMacros.h"
#include "lldb/Symbol/Function.h"
#include "lldb/Symbol/LineTable.h"
#include "lldb/Symbol/SourceModule.h"
#include "lldb/Utility/Stream.h"
#include "lldb/Utility/UserID.h"
#include "lldb/lldb-enumerations.h"

#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/DenseSet.h"

namespace lldb_private {











class CompileUnit : public std::enable_shared_from_this<CompileUnit>,
public ModuleChild,
public UserID,
public SymbolContextScope {
public:


































CompileUnit(const lldb::ModuleSP &module_sp, void *user_data,
const char *pathname, lldb::user_id_t uid,
lldb::LanguageType language, lldb_private::LazyBool is_optimized);




































CompileUnit(const lldb::ModuleSP &module_sp, void *user_data,
const FileSpec &file_spec, lldb::user_id_t uid,
lldb::LanguageType language, lldb_private::LazyBool is_optimized);








void AddFunction(lldb::FunctionSP &function_sp);




void CalculateSymbolContext(SymbolContext *sc) override;

lldb::ModuleSP CalculateSymbolContextModule() override;

CompileUnit *CalculateSymbolContextCompileUnit() override;




void DumpSymbolContext(Stream *s) override;

lldb::LanguageType GetLanguage();

void SetLanguage(lldb::LanguageType language) {
m_flags.Set(flagsParsedLanguage);
m_language = language;
}

void GetDescription(Stream *s, lldb::DescriptionLevel level) const;











void ForeachFunction(
llvm::function_ref<bool(const lldb::FunctionSP &)> lambda) const;










lldb::FunctionSP FindFunction(
llvm::function_ref<bool(const lldb::FunctionSP &)> matching_lambda);









void Dump(Stream *s, bool show_context) const;




































uint32_t FindLineEntry(uint32_t start_idx, uint32_t line,
const FileSpec *file_spec_ptr, bool exact,
LineEntry *line_entry);


const FileSpec &GetPrimaryFile() const { return m_file_spec; }










LineTable *GetLineTable();

DebugMacros *GetDebugMacros();
















virtual bool ForEachExternalModule(
llvm::DenseSet<lldb_private::SymbolFile *> &visited_symbol_files,
llvm::function_ref<bool(Module &)> lambda);








const FileSpecList &GetSupportFiles();








const std::vector<SourceModule> &GetImportedModules();









void *GetUserData() const;


















lldb::VariableListSP GetVariableList(bool can_create);















lldb::FunctionSP FindFunctionByUID(lldb::user_id_t uid);









void SetLineTable(LineTable *line_table);

void SetSupportFiles(const FileSpecList &support_files);

void SetDebugMacros(const DebugMacrosSP &debug_macros);








void SetVariableList(lldb::VariableListSP &variable_list_sp);

































void ResolveSymbolContext(const SourceLocationSpec &src_location_spec,
lldb::SymbolContextItem resolve_scope,
SymbolContextList &sc_list);












bool GetIsOptimized();


size_t GetNumFunctions() const { return m_functions_by_uid.size(); }

protected:

void *m_user_data;

lldb::LanguageType m_language;

Flags m_flags;

llvm::DenseMap<lldb::user_id_t, lldb::FunctionSP> m_functions_by_uid;


std::vector<SourceModule> m_imported_modules;

FileSpec m_file_spec;


FileSpecList m_support_files;

std::unique_ptr<LineTable> m_line_table_up;

DebugMacrosSP m_debug_macros_sp;

lldb::VariableListSP m_variables;


lldb_private::LazyBool m_is_optimized;

private:
enum {
flagsParsedAllFunctions =
(1u << 0),
flagsParsedVariables =
(1u << 1),
flagsParsedSupportFiles = (1u << 2),

flagsParsedLineTable =
(1u << 3),
flagsParsedLanguage = (1u << 4),
flagsParsedImportedModules =
(1u << 5),
flagsParsedDebugMacros =
(1u << 6)
};

CompileUnit(const CompileUnit &) = delete;
const CompileUnit &operator=(const CompileUnit &) = delete;
};

}

#endif
