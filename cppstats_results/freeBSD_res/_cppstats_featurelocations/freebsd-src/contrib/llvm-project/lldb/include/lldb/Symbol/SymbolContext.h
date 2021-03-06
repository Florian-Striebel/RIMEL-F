







#if !defined(LLDB_SYMBOL_SYMBOLCONTEXT_H)
#define LLDB_SYMBOL_SYMBOLCONTEXT_H

#include <memory>
#include <string>
#include <vector>

#include "lldb/Core/Address.h"
#include "lldb/Core/Mangled.h"
#include "lldb/Symbol/LineEntry.h"
#include "lldb/Utility/Iterable.h"
#include "lldb/lldb-private.h"

namespace lldb_private {

class SymbolContextScope;








class SymbolContext {
public:




SymbolContext();







explicit SymbolContext(SymbolContextScope *sc_scope);























explicit SymbolContext(const lldb::TargetSP &target_sp,
const lldb::ModuleSP &module_sp,
CompileUnit *comp_unit = nullptr,
Function *function = nullptr, Block *block = nullptr,
LineEntry *line_entry = nullptr,
Symbol *symbol = nullptr);


explicit SymbolContext(const lldb::ModuleSP &module_sp,
CompileUnit *comp_unit = nullptr,
Function *function = nullptr, Block *block = nullptr,
LineEntry *line_entry = nullptr,
Symbol *symbol = nullptr);

~SymbolContext();





void Clear(bool clear_target);








void Dump(Stream *s, Target *target) const;

















































bool DumpStopContext(Stream *s, ExecutionContextScope *exe_scope,
const Address &so_addr, bool show_fullpaths,
bool show_module, bool show_inlined_frames,
bool show_function_arguments,
bool show_function_name) const;






































bool GetAddressRange(uint32_t scope, uint32_t range_idx,
bool use_inline_block_range, AddressRange &range) const;

bool GetAddressRangeFromHereToEndLine(uint32_t end_line, AddressRange &range,
Status &error);




















const Symbol *FindBestGlobalDataSymbol(ConstString name, Status &error);

void GetDescription(Stream *s, lldb::DescriptionLevel level,
Target *target) const;

uint32_t GetResolvedMask() const;

lldb::LanguageType GetLanguage() const;




















Block *GetFunctionBlock();


















bool GetFunctionMethodInfo(lldb::LanguageType &language,
bool &is_instance_method,
ConstString &language_object_name);



void SortTypeList(TypeMap &type_map, TypeList &type_list) const;








ConstString GetFunctionName(
Mangled::NamePreference preference = Mangled::ePreferDemangled) const;










LineEntry GetFunctionStartLineEntry() const;



















bool GetParentOfInlinedScope(const Address &curr_frame_pc,
SymbolContext &next_frame_sc,
Address &inlined_frame_addr) const;


lldb::TargetSP target_sp;
lldb::ModuleSP module_sp;
CompileUnit *comp_unit = nullptr;
Function *function = nullptr;
Block *block = nullptr;
LineEntry line_entry;
Symbol *symbol = nullptr;
Variable *variable =
nullptr;
};

class SymbolContextSpecifier {
public:
enum SpecificationType {
eNothingSpecified = 0,
eModuleSpecified = 1 << 0,
eFileSpecified = 1 << 1,
eLineStartSpecified = 1 << 2,
eLineEndSpecified = 1 << 3,
eFunctionSpecified = 1 << 4,
eClassOrNamespaceSpecified = 1 << 5,
eAddressRangeSpecified = 1 << 6
};


SymbolContextSpecifier(const lldb::TargetSP &target_sp);

~SymbolContextSpecifier();

bool AddSpecification(const char *spec_string, SpecificationType type);

bool AddLineSpecification(uint32_t line_no, SpecificationType type);

void Clear();

bool SymbolContextMatches(const SymbolContext &sc);

bool AddressMatches(lldb::addr_t addr);

void GetDescription(Stream *s, lldb::DescriptionLevel level) const;

private:
lldb::TargetSP m_target_sp;
std::string m_module_spec;
lldb::ModuleSP m_module_sp;
std::unique_ptr<FileSpec> m_file_spec_up;
size_t m_start_line;
size_t m_end_line;
std::string m_function_spec;
std::string m_class_name;
std::unique_ptr<AddressRange> m_address_range_up;
uint32_t m_type;
};









class SymbolContextList {
public:



SymbolContextList();


~SymbolContextList();





void Append(const SymbolContext &sc);

void Append(const SymbolContextList &sc_list);

bool AppendIfUnique(const SymbolContext &sc, bool merge_symbol_into_function);

uint32_t AppendIfUnique(const SymbolContextList &sc_list,
bool merge_symbol_into_function);




void Clear();








void Dump(Stream *s, Target *target) const;
















bool GetContextAtIndex(size_t idx, SymbolContext &sc) const;











SymbolContext &operator[](size_t idx) { return m_symbol_contexts[idx]; }

const SymbolContext &operator[](size_t idx) const {
return m_symbol_contexts[idx];
}

bool RemoveContextAtIndex(size_t idx);





uint32_t GetSize() const;

bool IsEmpty() const;

uint32_t NumLineEntriesWithLine(uint32_t line) const;

void GetDescription(Stream *s, lldb::DescriptionLevel level,
Target *target) const;

protected:
typedef std::vector<SymbolContext>
collection;


collection m_symbol_contexts;

public:
typedef AdaptedIterable<collection, SymbolContext, vector_adapter>
SymbolContextIterable;
SymbolContextIterable SymbolContexts() {
return SymbolContextIterable(m_symbol_contexts);
}
};

bool operator==(const SymbolContext &lhs, const SymbolContext &rhs);
bool operator!=(const SymbolContext &lhs, const SymbolContext &rhs);

bool operator==(const SymbolContextList &lhs, const SymbolContextList &rhs);
bool operator!=(const SymbolContextList &lhs, const SymbolContextList &rhs);

}

#endif
