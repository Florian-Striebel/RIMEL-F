







#if !defined(LLDB_TARGET_SECTIONLOADHISTORY_H)
#define LLDB_TARGET_SECTIONLOADHISTORY_H

#include <map>
#include <mutex>

#include "lldb/lldb-public.h"

namespace lldb_private {

class SectionLoadHistory {
public:
enum : unsigned {


eStopIDNow = UINT32_MAX
};

SectionLoadHistory() : m_stop_id_to_section_load_list(), m_mutex() {}

~SectionLoadHistory() {


Clear();
}

SectionLoadList &GetCurrentSectionLoadList();

bool IsEmpty() const;

void Clear();

uint32_t GetLastStopID() const;


lldb::addr_t GetSectionLoadAddress(uint32_t stop_id,
const lldb::SectionSP &section_sp);

bool ResolveLoadAddress(uint32_t stop_id, lldb::addr_t load_addr,
Address &so_addr);

bool SetSectionLoadAddress(uint32_t stop_id,
const lldb::SectionSP &section_sp,
lldb::addr_t load_addr,
bool warn_multiple = false);




bool SetSectionUnloaded(uint32_t stop_id, const lldb::SectionSP &section_sp,
lldb::addr_t load_addr);




size_t SetSectionUnloaded(uint32_t stop_id,
const lldb::SectionSP &section_sp);

void Dump(Stream &s, Target *target);

protected:
SectionLoadList *GetSectionLoadListForStopID(uint32_t stop_id,
bool read_only);

typedef std::map<uint32_t, lldb::SectionLoadListSP> StopIDToSectionLoadList;
StopIDToSectionLoadList m_stop_id_to_section_load_list;
mutable std::recursive_mutex m_mutex;

private:
SectionLoadHistory(const SectionLoadHistory &) = delete;
const SectionLoadHistory &operator=(const SectionLoadHistory &) = delete;
};

}

#endif
