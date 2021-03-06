







#if !defined(LLDB_DATAFORMATTERS_FORMATTERSCONTAINER_H)
#define LLDB_DATAFORMATTERS_FORMATTERSCONTAINER_H

#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <string>

#include "lldb/lldb-public.h"

#include "lldb/Core/ValueObject.h"
#include "lldb/DataFormatters/FormatClasses.h"
#include "lldb/DataFormatters/TypeFormat.h"
#include "lldb/DataFormatters/TypeSummary.h"
#include "lldb/DataFormatters/TypeSynthetic.h"
#include "lldb/Symbol/CompilerType.h"
#include "lldb/Utility/RegularExpression.h"
#include "lldb/Utility/StringLexer.h"

namespace lldb_private {

class IFormatChangeListener {
public:
virtual ~IFormatChangeListener() = default;

virtual void Changed() = 0;

virtual uint32_t GetCurrentRevision() = 0;
};


class TypeMatcher {
RegularExpression m_type_name_regex;
ConstString m_type_name;


bool m_is_regex;





static ConstString StripTypeName(ConstString type) {
if (type.IsEmpty())
return type;

std::string type_cstr(type.AsCString());
StringLexer type_lexer(type_cstr);

type_lexer.AdvanceIf("class ");
type_lexer.AdvanceIf("enum ");
type_lexer.AdvanceIf("struct ");
type_lexer.AdvanceIf("union ");

while (type_lexer.NextIf({' ', '\t', '\v', '\f'}).first)
;

return ConstString(type_lexer.GetUnlexed());
}

public:
TypeMatcher() = delete;

TypeMatcher(ConstString type_name)
: m_type_name(type_name), m_is_regex(false) {}

TypeMatcher(RegularExpression regex)
: m_type_name_regex(std::move(regex)), m_is_regex(true) {}


bool Matches(ConstString type_name) const {
if (m_is_regex)
return m_type_name_regex.Execute(type_name.GetStringRef());
return m_type_name == type_name ||
StripTypeName(m_type_name) == StripTypeName(type_name);
}


ConstString GetMatchString() const {
if (m_is_regex)
return ConstString(m_type_name_regex.GetText());
return StripTypeName(m_type_name);
}









bool CreatedBySameMatchString(TypeMatcher other) const {
return GetMatchString() == other.GetMatchString();
}
};

template <typename ValueType> class FormattersContainer {
public:
typedef typename std::shared_ptr<ValueType> ValueSP;
typedef std::vector<std::pair<TypeMatcher, ValueSP>> MapType;
typedef std::function<bool(const TypeMatcher &, const ValueSP &)>
ForEachCallback;
typedef typename std::shared_ptr<FormattersContainer<ValueType>>
SharedPointer;

friend class TypeCategoryImpl;

FormattersContainer(IFormatChangeListener *lst) : listener(lst) {}

void Add(TypeMatcher matcher, const ValueSP &entry) {
if (listener)
entry->GetRevision() = listener->GetCurrentRevision();
else
entry->GetRevision() = 0;

std::lock_guard<std::recursive_mutex> guard(m_map_mutex);
Delete(matcher);
m_map.emplace_back(std::move(matcher), std::move(entry));
if (listener)
listener->Changed();
}

bool Delete(TypeMatcher matcher) {
std::lock_guard<std::recursive_mutex> guard(m_map_mutex);
for (auto iter = m_map.begin(); iter != m_map.end(); ++iter)
if (iter->first.CreatedBySameMatchString(matcher)) {
m_map.erase(iter);
if (listener)
listener->Changed();
return true;
}
return false;
}

bool Get(ConstString type, ValueSP &entry) {
std::lock_guard<std::recursive_mutex> guard(m_map_mutex);
for (auto &formatter : llvm::reverse(m_map)) {
if (formatter.first.Matches(type)) {
entry = formatter.second;
return true;
}
}
return false;
}

bool GetExact(TypeMatcher matcher, ValueSP &entry) {
std::lock_guard<std::recursive_mutex> guard(m_map_mutex);
for (const auto &pos : m_map)
if (pos.first.CreatedBySameMatchString(matcher)) {
entry = pos.second;
return true;
}
return false;
}

ValueSP GetAtIndex(size_t index) {
std::lock_guard<std::recursive_mutex> guard(m_map_mutex);
if (index >= m_map.size())
return ValueSP();
return m_map[index].second;
}

lldb::TypeNameSpecifierImplSP GetTypeNameSpecifierAtIndex(size_t index) {
std::lock_guard<std::recursive_mutex> guard(m_map_mutex);
if (index >= m_map.size())
return lldb::TypeNameSpecifierImplSP();
TypeMatcher type_matcher = m_map[index].first;
return std::make_shared<TypeNameSpecifierImpl>(
type_matcher.GetMatchString().GetStringRef(), true);
}

void Clear() {
std::lock_guard<std::recursive_mutex> guard(m_map_mutex);
m_map.clear();
if (listener)
listener->Changed();
}

void ForEach(ForEachCallback callback) {
if (callback) {
std::lock_guard<std::recursive_mutex> guard(m_map_mutex);
for (const auto &pos : m_map) {
const TypeMatcher &type = pos.first;
if (!callback(type, pos.second))
break;
}
}
}

uint32_t GetCount() {
std::lock_guard<std::recursive_mutex> guard(m_map_mutex);
return m_map.size();
}

void AutoComplete(CompletionRequest &request) {
ForEach([&request](const TypeMatcher &matcher, const ValueSP &value) {
request.TryCompleteCurrentArg(matcher.GetMatchString().GetStringRef());
return true;
});
}

protected:
FormattersContainer(const FormattersContainer &) = delete;
const FormattersContainer &operator=(const FormattersContainer &) = delete;

bool Get(const FormattersMatchVector &candidates, ValueSP &entry) {
for (const FormattersMatchCandidate &candidate : candidates) {
if (Get(candidate.GetTypeName(), entry)) {
if (candidate.IsMatch(entry) == false) {
entry.reset();
continue;
} else {
return true;
}
}
}
return false;
}

MapType m_map;
std::recursive_mutex m_map_mutex;
IFormatChangeListener *listener;
};

}

#endif
