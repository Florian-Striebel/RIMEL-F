







#if !defined(LLDB_CORE_UNIQUECSTRINGMAP_H)
#define LLDB_CORE_UNIQUECSTRINGMAP_H

#include <algorithm>
#include <vector>

#include "lldb/Utility/ConstString.h"
#include "lldb/Utility/RegularExpression.h"

namespace lldb_private {







template <typename T> class UniqueCStringMap {
public:
struct Entry {
Entry(ConstString cstr, const T &v) : cstring(cstr), value(v) {}

ConstString cstring;
T value;
};

typedef std::vector<Entry> collection;
typedef typename collection::iterator iterator;
typedef typename collection::const_iterator const_iterator;




void Append(ConstString unique_cstr, const T &value) {
m_map.push_back(typename UniqueCStringMap<T>::Entry(unique_cstr, value));
}

void Append(const Entry &e) { m_map.push_back(e); }

void Clear() { m_map.clear(); }





bool GetValueAtIndex(uint32_t idx, T &value) const {
if (idx < m_map.size()) {
value = m_map[idx].value;
return true;
}
return false;
}

ConstString GetCStringAtIndexUnchecked(uint32_t idx) const {
return m_map[idx].cstring;
}



T GetValueAtIndexUnchecked(uint32_t idx) const { return m_map[idx].value; }



const T &GetValueRefAtIndexUnchecked(uint32_t idx) const {
return m_map[idx].value;
}

ConstString GetCStringAtIndex(uint32_t idx) const {
return ((idx < m_map.size()) ? m_map[idx].cstring : ConstString());
}







T Find(ConstString unique_cstr, T fail_value) const {
auto pos = llvm::lower_bound(m_map, unique_cstr, Compare());
if (pos != m_map.end() && pos->cstring == unique_cstr)
return pos->value;
return fail_value;
}






const Entry *FindFirstValueForName(ConstString unique_cstr) const {
auto pos = llvm::lower_bound(m_map, unique_cstr, Compare());
if (pos != m_map.end() && pos->cstring == unique_cstr)
return &(*pos);
return nullptr;
}







const Entry *FindNextValueForName(const Entry *entry_ptr) const {
if (!m_map.empty()) {
const Entry *first_entry = &m_map[0];
const Entry *after_last_entry = first_entry + m_map.size();
const Entry *next_entry = entry_ptr + 1;
if (first_entry <= next_entry && next_entry < after_last_entry) {
if (next_entry->cstring == entry_ptr->cstring)
return next_entry;
}
}
return nullptr;
}

size_t GetValues(ConstString unique_cstr, std::vector<T> &values) const {
const size_t start_size = values.size();

for (const Entry &entry : llvm::make_range(std::equal_range(
m_map.begin(), m_map.end(), unique_cstr, Compare())))
values.push_back(entry.value);

return values.size() - start_size;
}

size_t GetValues(const RegularExpression &regex,
std::vector<T> &values) const {
const size_t start_size = values.size();

const_iterator pos, end = m_map.end();
for (pos = m_map.begin(); pos != end; ++pos) {
if (regex.Execute(pos->cstring.GetCString()))
values.push_back(pos->value);
}

return values.size() - start_size;
}


size_t GetSize() const { return m_map.size(); }


bool IsEmpty() const { return m_map.empty(); }





void Reserve(size_t n) { m_map.reserve(n); }










void Sort() { llvm::sort(m_map.begin(), m_map.end(), Compare()); }






void SizeToFit() {
if (m_map.size() < m_map.capacity()) {
collection temp(m_map.begin(), m_map.end());
m_map.swap(temp);
}
}

iterator begin() { return m_map.begin(); }
iterator end() { return m_map.end(); }
const_iterator begin() const { return m_map.begin(); }
const_iterator end() const { return m_map.end(); }


llvm::iterator_range<const_iterator>
equal_range(ConstString unique_cstr) const {
return llvm::make_range(
std::equal_range(m_map.begin(), m_map.end(), unique_cstr, Compare()));
};

protected:
struct Compare {
bool operator()(const Entry &lhs, const Entry &rhs) {
return operator()(lhs.cstring, rhs.cstring);
}

bool operator()(const Entry &lhs, ConstString rhs) {
return operator()(lhs.cstring, rhs);
}

bool operator()(ConstString lhs, const Entry &rhs) {
return operator()(lhs, rhs.cstring);
}




bool operator()(ConstString lhs, ConstString rhs) {
return uintptr_t(lhs.GetCString()) < uintptr_t(rhs.GetCString());
}
};
collection m_map;
};

}

#endif
