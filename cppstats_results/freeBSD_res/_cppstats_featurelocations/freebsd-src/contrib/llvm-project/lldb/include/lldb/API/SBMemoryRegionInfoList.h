







#if !defined(LLDB_API_SBMEMORYREGIONINFOLIST_H)
#define LLDB_API_SBMEMORYREGIONINFOLIST_H

#include "lldb/API/SBDefines.h"

class MemoryRegionInfoListImpl;

namespace lldb {

class LLDB_API SBMemoryRegionInfoList {
public:
SBMemoryRegionInfoList();

SBMemoryRegionInfoList(const lldb::SBMemoryRegionInfoList &rhs);

const SBMemoryRegionInfoList &operator=(const SBMemoryRegionInfoList &rhs);

~SBMemoryRegionInfoList();

uint32_t GetSize() const;

bool GetMemoryRegionAtIndex(uint32_t idx, SBMemoryRegionInfo &region_info);

void Append(lldb::SBMemoryRegionInfo &region);

void Append(lldb::SBMemoryRegionInfoList &region_list);

void Clear();

protected:
const MemoryRegionInfoListImpl *operator->() const;

const MemoryRegionInfoListImpl &operator*() const;

private:
friend class SBProcess;

lldb_private::MemoryRegionInfos &ref();

const lldb_private::MemoryRegionInfos &ref() const;

std::unique_ptr<MemoryRegionInfoListImpl> m_opaque_up;
};

}

#endif
