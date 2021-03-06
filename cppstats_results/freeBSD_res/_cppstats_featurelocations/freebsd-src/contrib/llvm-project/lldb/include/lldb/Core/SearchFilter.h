







#if !defined(LLDB_CORE_SEARCHFILTER_H)
#define LLDB_CORE_SEARCHFILTER_H

#include "lldb/Core/FileSpecList.h"
#include "lldb/Utility/StructuredData.h"

#include "lldb/Utility/FileSpec.h"
#include "lldb/lldb-forward.h"

#include <cstdint>

namespace lldb_private {
class Address;
class Breakpoint;
class CompileUnit;
class Status;
class Function;
class ModuleList;
class SearchFilter;
class Stream;
class SymbolContext;
class Target;
}

namespace lldb_private {








class Searcher {
public:
enum CallbackReturn {
eCallbackReturnStop = 0,
eCallbackReturnContinue,
eCallbackReturnPop
};

Searcher();

virtual ~Searcher();

virtual CallbackReturn SearchCallback(SearchFilter &filter,
SymbolContext &context,
Address *addr) = 0;

virtual lldb::SearchDepth GetDepth() = 0;





virtual void GetDescription(Stream *s);
};

















class SearchFilter {
public:




SearchFilter(const lldb::TargetSP &target_sp);

SearchFilter(const lldb::TargetSP &target_sp, unsigned char filterType);

virtual ~SearchFilter();









virtual bool ModulePasses(const FileSpec &spec);










virtual bool ModulePasses(const lldb::ModuleSP &module_sp);










virtual bool AddressPasses(Address &addr);











virtual bool CompUnitPasses(FileSpec &fileSpec);











virtual bool CompUnitPasses(CompileUnit &compUnit);









virtual bool FunctionPasses(Function &function);






virtual void Search(Searcher &searcher);










virtual void SearchInModuleList(Searcher &searcher, ModuleList &modules);











virtual uint32_t GetFilterRequiredItems();





virtual void GetDescription(Stream *s);


virtual void Dump(Stream *s) const;

lldb::SearchFilterSP CreateCopy(lldb::TargetSP& target_sp);

static lldb::SearchFilterSP
CreateFromStructuredData(const lldb::TargetSP& target_sp,
const StructuredData::Dictionary &data_dict,
Status &error);

virtual StructuredData::ObjectSP SerializeToStructuredData() {
return StructuredData::ObjectSP();
}

static const char *GetSerializationKey() { return "SearchFilter"; }

static const char *GetSerializationSubclassKey() { return "Type"; }

static const char *GetSerializationSubclassOptionsKey() { return "Options"; }

enum FilterTy {
Unconstrained = 0,
Exception,
ByModule,
ByModules,
ByModulesAndCU,
LastKnownFilterType = ByModulesAndCU,
UnknownFilter
};

static const char *g_ty_to_name[LastKnownFilterType + 2];

enum FilterTy GetFilterTy() {
if (SubclassID > FilterTy::LastKnownFilterType)
return FilterTy::UnknownFilter;
else
return (enum FilterTy)SubclassID;
}

const char *GetFilterName() { return FilterTyToName(GetFilterTy()); }

static const char *FilterTyToName(enum FilterTy);

static FilterTy NameToFilterTy(llvm::StringRef name);

protected:

enum OptionNames { ModList = 0, CUList, LanguageName, LastOptionName };
static const char *g_option_names[LastOptionName];

static const char *GetKey(enum OptionNames enum_value) {
return g_option_names[enum_value];
}

StructuredData::DictionarySP
WrapOptionsDict(StructuredData::DictionarySP options_dict_sp);

void SerializeFileSpecList(StructuredData::DictionarySP &options_dict_sp,
OptionNames name, FileSpecList &file_list);




Searcher::CallbackReturn DoModuleIteration(const SymbolContext &context,
Searcher &searcher);

Searcher::CallbackReturn DoModuleIteration(const lldb::ModuleSP &module_sp,
Searcher &searcher);

Searcher::CallbackReturn DoCUIteration(const lldb::ModuleSP &module_sp,
const SymbolContext &context,
Searcher &searcher);

Searcher::CallbackReturn DoFunctionIteration(Function *function,
const SymbolContext &context,
Searcher &searcher);

virtual lldb::SearchFilterSP DoCreateCopy() = 0;

void SetTarget(lldb::TargetSP &target_sp) { m_target_sp = target_sp; }

lldb::TargetSP m_target_sp;


private:
unsigned char SubclassID;
};





class SearchFilterForUnconstrainedSearches : public SearchFilter {
public:
SearchFilterForUnconstrainedSearches(const lldb::TargetSP &target_sp)
: SearchFilter(target_sp, FilterTy::Unconstrained) {}

~SearchFilterForUnconstrainedSearches() override = default;

bool ModulePasses(const FileSpec &module_spec) override;

bool ModulePasses(const lldb::ModuleSP &module_sp) override;

static lldb::SearchFilterSP
CreateFromStructuredData(const lldb::TargetSP& target_sp,
const StructuredData::Dictionary &data_dict,
Status &error);

StructuredData::ObjectSP SerializeToStructuredData() override;

protected:
lldb::SearchFilterSP DoCreateCopy() override;
};




class SearchFilterByModule : public SearchFilter {
public:








SearchFilterByModule(const lldb::TargetSP &targetSP, const FileSpec &module);

~SearchFilterByModule() override;

bool ModulePasses(const lldb::ModuleSP &module_sp) override;

bool ModulePasses(const FileSpec &spec) override;

bool AddressPasses(Address &address) override;

void GetDescription(Stream *s) override;

uint32_t GetFilterRequiredItems() override;

void Dump(Stream *s) const override;

void Search(Searcher &searcher) override;

static lldb::SearchFilterSP
CreateFromStructuredData(const lldb::TargetSP& target_sp,
const StructuredData::Dictionary &data_dict,
Status &error);

StructuredData::ObjectSP SerializeToStructuredData() override;

protected:
lldb::SearchFilterSP DoCreateCopy() override;

private:
FileSpec m_module_spec;
};

class SearchFilterByModuleList : public SearchFilter {
public:








SearchFilterByModuleList(const lldb::TargetSP &targetSP,
const FileSpecList &module_list);

SearchFilterByModuleList(const lldb::TargetSP &targetSP,
const FileSpecList &module_list,
enum FilterTy filter_ty);

~SearchFilterByModuleList() override;

bool ModulePasses(const lldb::ModuleSP &module_sp) override;

bool ModulePasses(const FileSpec &spec) override;

bool AddressPasses(Address &address) override;

void GetDescription(Stream *s) override;

uint32_t GetFilterRequiredItems() override;

void Dump(Stream *s) const override;

void Search(Searcher &searcher) override;

static lldb::SearchFilterSP
CreateFromStructuredData(const lldb::TargetSP& target_sp,
const StructuredData::Dictionary &data_dict,
Status &error);

StructuredData::ObjectSP SerializeToStructuredData() override;

void SerializeUnwrapped(StructuredData::DictionarySP &options_dict_sp);

protected:
lldb::SearchFilterSP DoCreateCopy() override;

FileSpecList m_module_spec_list;
};

class SearchFilterByModuleListAndCU : public SearchFilterByModuleList {
public:


SearchFilterByModuleListAndCU(const lldb::TargetSP &targetSP,
const FileSpecList &module_list,
const FileSpecList &cu_list);

~SearchFilterByModuleListAndCU() override;

bool AddressPasses(Address &address) override;

bool CompUnitPasses(FileSpec &fileSpec) override;

bool CompUnitPasses(CompileUnit &compUnit) override;

void GetDescription(Stream *s) override;

uint32_t GetFilterRequiredItems() override;

void Dump(Stream *s) const override;

void Search(Searcher &searcher) override;

static lldb::SearchFilterSP
CreateFromStructuredData(const lldb::TargetSP& target_sp,
const StructuredData::Dictionary &data_dict,
Status &error);

StructuredData::ObjectSP SerializeToStructuredData() override;

protected:
lldb::SearchFilterSP DoCreateCopy() override;

private:
FileSpecList m_cu_spec_list;
};

}

#endif
