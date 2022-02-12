








#if !defined(LLDB_TARGET_SECTIONLOADLIST_H)
#define LLDB_TARGET_SECTIONLOADLIST_H

#include <map>
#include <mutex>

#include "llvm/ADT/DenseMap.h"
#include "lldb/Core/Section.h"
#include "lldb/lldb-public.h"

namespace lldb_private {

class SectionLoadList {
public:

SectionLoadList() : m_addr_to_sect(), m_sect_to_addr(), m_mutex() {}

SectionLoadList(const SectionLoadList &rhs);

~SectionLoadList() {


Clear();
}

void operator=(const SectionLoadList &rhs);

bool IsEmpty() const;

void Clear();

lldb::addr_t GetSectionLoadAddress(const lldb::SectionSP &section_sp) const;

bool ResolveLoadAddress(lldb::addr_t load_addr, Address &so_addr,
bool allow_section_end = false) const;

bool SetSectionLoadAddress(const lldb::SectionSP &section_sp,
lldb::addr_t load_addr,
bool warn_multiple = false);




bool SetSectionUnloaded(const lldb::SectionSP &section_sp,
lldb::addr_t load_addr);




size_t SetSectionUnloaded(const lldb::SectionSP &section_sp);

void Dump(Stream &s, Target *target);

protected:
typedef std::map<lldb::addr_t, lldb::SectionSP> addr_to_sect_collection;
typedef llvm::DenseMap<const Section *, lldb::addr_t> sect_to_addr_collection;
addr_to_sect_collection m_addr_to_sect;
sect_to_addr_collection m_sect_to_addr;
mutable std::recursive_mutex m_mutex;
};

}

#endif
