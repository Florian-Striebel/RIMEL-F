







#if !defined(LLDB_DATAFORMATTERS_FORMATMANAGER_H)
#define LLDB_DATAFORMATTERS_FORMATMANAGER_H

#include <atomic>
#include <initializer_list>
#include <map>
#include <mutex>
#include <vector>

#include "lldb/lldb-enumerations.h"
#include "lldb/lldb-public.h"

#include "lldb/DataFormatters/FormatCache.h"
#include "lldb/DataFormatters/FormatClasses.h"
#include "lldb/DataFormatters/FormattersContainer.h"
#include "lldb/DataFormatters/LanguageCategory.h"
#include "lldb/DataFormatters/TypeCategory.h"
#include "lldb/DataFormatters/TypeCategoryMap.h"

namespace lldb_private {







class FormatManager : public IFormatChangeListener {
typedef FormattersContainer<TypeSummaryImpl> NamedSummariesMap;
typedef TypeCategoryMap::MapType::iterator CategoryMapIterator;

public:
typedef std::map<lldb::LanguageType, LanguageCategory::UniquePointer>
LanguageCategories;

FormatManager();

~FormatManager() override = default;

NamedSummariesMap &GetNamedSummaryContainer() {
return m_named_summaries_map;
}

void
EnableCategory(ConstString category_name,
TypeCategoryMap::Position pos = TypeCategoryMap::Default) {
EnableCategory(category_name, pos, {});
}

void EnableCategory(ConstString category_name,
TypeCategoryMap::Position pos, lldb::LanguageType lang) {
TypeCategoryMap::ValueSP category_sp;
if (m_categories_map.Get(category_name, category_sp) && category_sp) {
m_categories_map.Enable(category_sp, pos);
category_sp->AddLanguage(lang);
}
}

void DisableCategory(ConstString category_name) {
m_categories_map.Disable(category_name);
}

void
EnableCategory(const lldb::TypeCategoryImplSP &category,
TypeCategoryMap::Position pos = TypeCategoryMap::Default) {
m_categories_map.Enable(category, pos);
}

void DisableCategory(const lldb::TypeCategoryImplSP &category) {
m_categories_map.Disable(category);
}

void EnableAllCategories();

void DisableAllCategories();

bool DeleteCategory(ConstString category_name) {
return m_categories_map.Delete(category_name);
}

void ClearCategories() { return m_categories_map.Clear(); }

uint32_t GetCategoriesCount() { return m_categories_map.GetCount(); }

lldb::TypeCategoryImplSP GetCategoryAtIndex(size_t index) {
return m_categories_map.GetAtIndex(index);
}

void ForEachCategory(TypeCategoryMap::ForEachCallback callback);

lldb::TypeCategoryImplSP GetCategory(const char *category_name = nullptr,
bool can_create = true) {
if (!category_name)
return GetCategory(m_default_category_name);
return GetCategory(ConstString(category_name));
}

lldb::TypeCategoryImplSP GetCategory(ConstString category_name,
bool can_create = true);

lldb::TypeFormatImplSP
GetFormatForType(lldb::TypeNameSpecifierImplSP type_sp);

lldb::TypeSummaryImplSP
GetSummaryForType(lldb::TypeNameSpecifierImplSP type_sp);

lldb::TypeFilterImplSP
GetFilterForType(lldb::TypeNameSpecifierImplSP type_sp);

lldb::ScriptedSyntheticChildrenSP
GetSyntheticForType(lldb::TypeNameSpecifierImplSP type_sp);

lldb::TypeFormatImplSP GetFormat(ValueObject &valobj,
lldb::DynamicValueType use_dynamic);

lldb::TypeSummaryImplSP GetSummaryFormat(ValueObject &valobj,
lldb::DynamicValueType use_dynamic);

lldb::SyntheticChildrenSP
GetSyntheticChildren(ValueObject &valobj, lldb::DynamicValueType use_dynamic);

bool
AnyMatches(ConstString type_name,
TypeCategoryImpl::FormatCategoryItems items =
TypeCategoryImpl::ALL_ITEM_TYPES,
bool only_enabled = true, const char **matching_category = nullptr,
TypeCategoryImpl::FormatCategoryItems *matching_type = nullptr) {
return m_categories_map.AnyMatches(type_name, items, only_enabled,
matching_category, matching_type);
}

static bool GetFormatFromCString(const char *format_cstr,
bool partial_match_ok, lldb::Format &format);

static char GetFormatAsFormatChar(lldb::Format format);

static const char *GetFormatAsCString(lldb::Format format);




static lldb::Format GetSingleItemFormat(lldb::Format vector_format);





bool ShouldPrintAsOneLiner(ValueObject &valobj);

void Changed() override;

uint32_t GetCurrentRevision() override { return m_last_revision; }

static FormattersMatchVector
GetPossibleMatches(ValueObject &valobj, lldb::DynamicValueType use_dynamic) {
FormattersMatchVector matches;
GetPossibleMatches(valobj, valobj.GetCompilerType(),
use_dynamic, matches, false, false, false, true);
return matches;
}

static ConstString GetTypeForCache(ValueObject &, lldb::DynamicValueType);

LanguageCategory *GetCategoryForLanguage(lldb::LanguageType lang_type);

static std::vector<lldb::LanguageType>
GetCandidateLanguages(lldb::LanguageType lang_type);

private:
static void GetPossibleMatches(ValueObject &valobj,
CompilerType compiler_type,
lldb::DynamicValueType use_dynamic,
FormattersMatchVector &entries,
bool did_strip_ptr, bool did_strip_ref,
bool did_strip_typedef,
bool root_level = false);

std::atomic<uint32_t> m_last_revision;
FormatCache m_format_cache;
std::recursive_mutex m_language_categories_mutex;
LanguageCategories m_language_categories_map;
NamedSummariesMap m_named_summaries_map;
TypeCategoryMap m_categories_map;

ConstString m_default_category_name;
ConstString m_system_category_name;
ConstString m_vectortypes_category_name;

template <typename ImplSP>
ImplSP Get(ValueObject &valobj, lldb::DynamicValueType use_dynamic);
template <typename ImplSP> ImplSP GetCached(FormattersMatchData &match_data);
template <typename ImplSP> ImplSP GetHardcoded(FormattersMatchData &);

TypeCategoryMap &GetCategories() { return m_categories_map; }







void LoadSystemFormatters();

void LoadVectorFormatters();

friend class FormattersMatchData;
};

}

#endif
